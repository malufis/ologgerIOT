/**
 * @file aquisicao.c
 * @brief Implementação da tarefa de aquisição das entradas 4-20 mA.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#include "aquisicao.h"

#include <stdbool.h>
#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"

#include "adc.h"
#include "rtos.h"
#include "sensor.h"

/** Tamanho da pilha da tarefa de aquisição, em palavras de 32 bits. */
#define AQUISICAO_PILHA_PALAVRAS 256U

/* Estado do módulo. As variáveis de espelho são lidas por depurador na bancada. */
static StaticTask_t tcb_aquisicao;
static StackType_t pilha_aquisicao[AQUISICAO_PILHA_PALAVRAS];
static TaskHandle_t tarefa;
static sensor_media_t media[ADC_CANAIS];
static float contagens_1s[ADC_CANAIS];
static float ma_1s[ADC_CANAIS];
static bool valido[ADC_CANAIS];
static volatile const uint16_t *meia_pendente;
static volatile uint16_t quadros_pendentes;
static volatile uint32_t segundos;
static uint16_t taxa_guardada_sps;
static volatile uint32_t meias_perdidas;
static volatile uint32_t espelho_contagens_x100[ADC_CANAIS];
static volatile uint32_t espelho_microamperes[ADC_CANAIS];
/** Última contagem de VREFINT do quadro, para calcular a alimentação real do ADC na bancada. */
static volatile uint32_t espelho_vrefint;

/**
 * @brief Guarda o resultado de 1 segundo de uma entrada e converte em corrente.
 *
 * @param[in] canal     Entrada de 0 a 3.
 * @param[in] contagens Média das contagens do segundo.
 */
static void fecha_segundo_do_canal(uint8_t canal, float contagens)
{
  float ma = 0.0f;

  contagens_1s[canal] = contagens;
  espelho_contagens_x100[canal] = (uint32_t)(contagens * 100.0f);

  if (sensor_contagens_para_ma(contagens, AQUISICAO_VREF_V, AQUISICAO_SHUNT_OHM, &ma) == SENSOR_OK)
  {
    ma_1s[canal] = ma;
    espelho_microamperes[canal] = (uint32_t)(ma * 1000.0f);
    valido[canal] = true;
  }
}

/**
 * @brief Acumula um quadro, com uma contagem de cada entrada.
 *
 * @param[in] quadro Contagens das entradas 1 a 4.
 */
static void processa_quadro(const volatile uint16_t *quadro)
{
  bool pronta = false;
  float resultado = 0.0f;
  uint8_t canal;

  espelho_vrefint = quadro[ADC_CANAIS];
  for (canal = 0U; canal < ADC_CANAIS; canal++)
  {
    (void)sensor_media_acumula(&media[canal], quadro[canal], &pronta, &resultado);
    if (pronta)
    {
      fecha_segundo_do_canal(canal, resultado);
      if (canal == (ADC_CANAIS - 1U))
      {
        segundos++;
      }
    }
  }
}

/**
 * @brief Corpo da tarefa: espera uma metade do buffer e processa seus quadros.
 *
 * @param[in] argumento Não usado.
 */
static void tarefa_aquisicao(void *argumento)
{
  uint16_t quadro;
  uint16_t quantidade;
  const volatile uint16_t *inicio;

  (void)argumento;

  for (;;)
  {
    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    inicio = meia_pendente;
    quantidade = quadros_pendentes;
    meia_pendente = NULL;

    for (quadro = 0U; quadro < quantidade; quadro++)
    {
      processa_quadro(&inicio[(uint32_t)quadro * ADC_SEQ_CANAIS]);
    }
  }
}

/**
 * @brief Recebe do driver uma metade cheia e acorda a tarefa de aquisição.
 *
 * Executa em interrupção. Se a metade anterior ainda não foi tratada, conta a perda.
 *
 * @param[in] quadros    Início da metade cheia.
 * @param[in] quantidade Quantidade de quadros.
 */
static void ao_receber_meia(const uint16_t *quadros, uint16_t quantidade)
{
  BaseType_t acordou = pdFALSE;

  if (meia_pendente != NULL)
  {
    meias_perdidas++;
  }
  meia_pendente = quadros;
  quadros_pendentes = quantidade;

  vTaskNotifyGiveFromISR(tarefa, &acordou);
  portYIELD_FROM_ISR(acordou);
}

int32_t aquisicao_inicia(uint16_t taxa_sps)
{
  uint8_t canal;

  taxa_guardada_sps = taxa_sps;
  for (canal = 0U; canal < ADC_CANAIS; canal++)
  {
    if (sensor_media_inicia(&media[canal], taxa_sps) != SENSOR_OK)
    {
      return AQUISICAO_ERRO_PARAMETRO;
    }
  }

  tarefa = xTaskCreateStatic(tarefa_aquisicao, "aquis", AQUISICAO_PILHA_PALAVRAS, NULL,
                             RTOS_PRIO_AQUISICAO, pilha_aquisicao, &tcb_aquisicao);
  if (tarefa == NULL)
  {
    return AQUISICAO_ERRO_INICIO;
  }

  return (adc_inicia(taxa_sps, ao_receber_meia) == ADC_OK) ? AQUISICAO_OK : AQUISICAO_ERRO_INICIO;
}

int32_t aquisicao_para(void)
{
  return (adc_para() == ADC_OK) ? AQUISICAO_OK : AQUISICAO_ERRO_INICIO;
}

int32_t aquisicao_retoma(void)
{
  uint8_t canal;

  /* Descarta o segundo parcial e os valores de antes do sleep: o dado antigo não vale mais. */
  for (canal = 0U; canal < ADC_CANAIS; canal++)
  {
    (void)sensor_media_inicia(&media[canal], taxa_guardada_sps);
    valido[canal] = false;
  }
  meia_pendente = NULL;

  return (adc_retoma() == ADC_OK) ? AQUISICAO_OK : AQUISICAO_ERRO_INICIO;
}

int32_t aquisicao_ma(uint8_t canal, float *ma)
{
  if ((canal >= ADC_CANAIS) || (ma == NULL))
  {
    return AQUISICAO_ERRO_PARAMETRO;
  }
  if (!valido[canal])
  {
    return AQUISICAO_ERRO_SEM_DADO;
  }
  *ma = ma_1s[canal];
  return AQUISICAO_OK;
}

int32_t aquisicao_contagens(uint8_t canal, float *contagens)
{
  if ((canal >= ADC_CANAIS) || (contagens == NULL))
  {
    return AQUISICAO_ERRO_PARAMETRO;
  }
  if (!valido[canal])
  {
    return AQUISICAO_ERRO_SEM_DADO;
  }
  *contagens = contagens_1s[canal];
  return AQUISICAO_OK;
}

uint32_t aquisicao_segundos(void)
{
  return segundos;
}
