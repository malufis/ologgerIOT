/**
 * @file io_saidas.c
 * @brief Implementação do controle das quatro saídas digitais discretas.
 *
 * @date criacao 03/11/2026
 * @date modificacao 04/11/2026
 * @version 0.4.0
 */

#include "io_saidas.h"

#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "rtos.h"

/** Maior retardo aceito em segundos, para o valor em milissegundos caber em 32 bits. */
#define DURACAO_MAX_S (UINT32_MAX / 1000U)

/* Estado do módulo. */
static StaticTask_t tcb_saidas;
static StackType_t pilha_saidas[IO_SAIDAS_PILHA_PALAVRAS];
static controle_saida_t saidas[IO_SAIDAS_QTD];
/** true se o pino da saída foi cedido ao gerador de frequência (io_frequencia). */
static volatile bool cedidas[IO_SAIDAS_QTD];

/**
 * @brief Instante atual em milissegundos, a partir do tick do FreeRTOS.
 *
 * @return Milissegundos desde o início do escalonador.
 */
static uint32_t agora_ms(void)
{
  return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

/**
 * @brief Escreve no pino o estado atual de uma saída.
 *
 * @param indice Índice da saída, de 0 a IO_SAIDAS_QTD - 1.
 */
static void aplica_no_pino(uint8_t indice)
{
  if (cedidas[indice])
  {
    return;
  }
  (void)io_saida_define((uint8_t)(indice + 1U), controle_saida_ligada(&saidas[indice]));
}

/**
 * @brief Indica se o canal é válido.
 *
 * @param canal Saída pedida.
 * @return true se o canal está entre 1 e IO_SAIDAS_QTD.
 */
static bool canal_valido(uint8_t canal)
{
  return (canal >= 1U) && (canal <= IO_SAIDAS_QTD);
}

/**
 * @brief Corpo da tarefa: liga as saídas cujo retardo de acionamento chegou ao fim.
 *
 * @param[in] argumento Não usado.
 */
static void tarefa_saidas(void *argumento)
{
  TickType_t ultimo = xTaskGetTickCount();
  uint8_t i;

  (void)argumento;
  for (;;)
  {
    for (i = 0U; i < IO_SAIDAS_QTD; i++)
    {
      taskENTER_CRITICAL();
      (void)controle_saida_atualiza(&saidas[i], agora_ms());
      aplica_no_pino(i);
      taskEXIT_CRITICAL();
    }
    vTaskDelayUntil(&ultimo, pdMS_TO_TICKS(IO_SAIDAS_PERIODO_MS));
  }
}

int32_t io_saidas_prepara(void)
{
  uint8_t i;

  for (i = 0U; i < IO_SAIDAS_QTD; i++)
  {
    (void)controle_saida_inicia(&saidas[i]);
  }
  (void)io_saida_inicia(); /* Pinos já desligados antes de virarem saída. */
  return IO_SAIDAS_OK;
}

int32_t io_saidas_inicia(void)
{
  TaskHandle_t tarefa = xTaskCreateStatic(tarefa_saidas, "saidas", IO_SAIDAS_PILHA_PALAVRAS, NULL,
                                          RTOS_PRIO_REGRAS, pilha_saidas, &tcb_saidas);

  return (tarefa == NULL) ? IO_SAIDAS_ERRO_TAREFA : IO_SAIDAS_OK;
}

int32_t io_saidas_comando_manual(uint8_t canal, bool liga, bool *mudou_de_modo)
{
  bool mudou;

  if (!canal_valido(canal))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  mudou = controle_saida_comando_manual(&saidas[canal - 1U], liga, agora_ms());
  aplica_no_pino((uint8_t)(canal - 1U));
  taskEXIT_CRITICAL();

  if (mudou_de_modo != NULL)
  {
    *mudou_de_modo = mudou;
  }
  return IO_SAIDAS_OK;
}

int32_t io_saidas_modo_automatico(uint8_t canal)
{
  if (!canal_valido(canal))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  controle_saida_modo_automatico(&saidas[canal - 1U], agora_ms());
  aplica_no_pino((uint8_t)(canal - 1U));
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_pedido(uint8_t canal, controle_fonte_t fonte, controle_pedido_t pedido)
{
  if (!canal_valido(canal) || ((uint32_t)fonte >= CONTROLE_FONTES))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  controle_saida_pedido(&saidas[canal - 1U], fonte, pedido, agora_ms());
  aplica_no_pino((uint8_t)(canal - 1U));
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_sensor_falha(uint8_t canal, bool falha)
{
  if (!canal_valido(canal))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  controle_saida_sensor_falha(&saidas[canal - 1U], falha, agora_ms());
  aplica_no_pino((uint8_t)(canal - 1U));
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_define_regra_sensor(uint8_t canal, bool definida)
{
  if (!canal_valido(canal))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  controle_saida_define_regra_sensor(&saidas[canal - 1U], definida, agora_ms());
  aplica_no_pino((uint8_t)(canal - 1U));
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_define_conectividade(bool online)
{
  uint8_t i;

  taskENTER_CRITICAL();
  for (i = 0U; i < IO_SAIDAS_QTD; i++)
  {
    controle_saida_define_conectividade(&saidas[i], online, agora_ms());
    aplica_no_pino(i);
  }
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_define_estabilidade(uint8_t canal, uint32_t segundos)
{
  if (!canal_valido(canal) || (segundos > DURACAO_MAX_S))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  controle_saida_define_estabilidade(&saidas[canal - 1U], segundos * 1000U);
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_cede_pino(uint8_t canal, bool cede)
{
  if (!canal_valido(canal))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  cedidas[canal - 1U] = cede;
  if (!cede)
  {
    aplica_no_pino((uint8_t)(canal - 1U));
  }
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_define_atraso(uint8_t canal, uint32_t segundos)
{
  if (!canal_valido(canal) || (segundos > DURACAO_MAX_S))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  controle_saida_define_atraso(&saidas[canal - 1U], segundos * 1000U);
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_estado(uint8_t canal, bool *ligada)
{
  if ((ligada == NULL) || !canal_valido(canal))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  *ligada = controle_saida_ligada(&saidas[canal - 1U]);
  return IO_SAIDAS_OK;
}

int32_t io_saidas_aguardando(uint8_t canal, bool *aguardando)
{
  if ((aguardando == NULL) || !canal_valido(canal))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  *aguardando = controle_saida_aguardando(&saidas[canal - 1U]);
  return IO_SAIDAS_OK;
}

int32_t io_saidas_modo(uint8_t canal, controle_modo_t *modo)
{
  if ((modo == NULL) || !canal_valido(canal))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  *modo = controle_saida_modo(&saidas[canal - 1U]);
  return IO_SAIDAS_OK;
}
