/**
 * @file regras_tarefa.c
 * @brief Implementação da tarefa que liga as entradas analógicas às saídas.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "regras_tarefa.h"

#include "FreeRTOS.h"
#include "task.h"

#include "aquisicao.h"
#include "config_tarefa.h"
#include "io_frequencia.h"
#include "io_saidas.h"
#include "regra_sensor.h"
#include "rtos.h"
#include "sensor.h"

static StaticTask_t tcb_regras;
static StackType_t pilha_regras[REGRAS_TAREFA_PILHA_PALAVRAS];

/** Regra do sensor de cada saída. */
static regra_sensor_t regras[CONFIG_QTD_SAIDAS];
/** Parâmetros com que cada regra foi cadastrada, para recadastrar só quando mudarem. */
typedef struct
{
  uint16_t sentido;
  float minimo;
  float maximo;
  bool ativa;
} cadastro_t;
static cadastro_t cadastrados[CONFIG_QTD_SAIDAS];
/** true se a saída estava em frequência com sensor na passada anterior. */
static bool em_frequencia[CONFIG_QTD_SAIDAS];

/**
 * @brief Diz se a corrente indica falha do sensor.
 *
 * @param ma Corrente em mA.
 * @return true se abaixo de REGRAS_FALHA_ABAIXO_MA ou acima de REGRAS_FALHA_ACIMA_MA.
 */
static bool corrente_em_falha(float ma)
{
  return (ma < REGRAS_FALHA_ABAIXO_MA) || (ma > REGRAS_FALHA_ACIMA_MA);
}

/**
 * @brief Solta a saída do sensor: sem regra, sem falha, e frequência a zero se havia.
 *
 * @param i Índice da saída, de 0 a 3.
 */
static void solta_saida(uint8_t i)
{
  uint8_t canal = (uint8_t)(i + 1U);

  if (cadastrados[i].ativa)
  {
    (void)io_saidas_define_regra_sensor(canal, false);
    (void)io_saidas_sensor_falha(canal, false);
    cadastrados[i].ativa = false;
  }
  if (em_frequencia[i])
  {
    (void)io_frequencia_define_hz(canal, 0.0F);
    em_frequencia[i] = false;
  }
}

/**
 * @brief Aplica a regra do sensor de uma saída discreta.
 *
 * @param cfg Configuração.
 * @param i Índice da saída.
 * @param falha true se o sensor está em falha.
 * @param valor Valor de engenharia da entrada.
 */
static void aplica_regra(const config_t *cfg, uint8_t i, bool falha, float valor)
{
  uint8_t canal = (uint8_t)(i + 1U);
  uint16_t sentido = (uint16_t)config_inteiro(cfg, CFG_SAI_SENTIDO, i);
  float minimo = config_float(cfg, CFG_SAI_LIM_MIN, i);
  float maximo = config_float(cfg, CFG_SAI_LIM_MAX, i);

  if (!cadastrados[i].ativa || (cadastrados[i].sentido != sentido) || (cadastrados[i].minimo != minimo) ||
      (cadastrados[i].maximo != maximo))
  {
    if (regra_sensor_cadastra(&regras[i], sentido, minimo, maximo) != REGRA_SENSOR_OK)
    {
      solta_saida(i); /* Limiares inválidos: sem regra. */
      return;
    }
    cadastrados[i].sentido = sentido;
    cadastrados[i].minimo = minimo;
    cadastrados[i].maximo = maximo;
    cadastrados[i].ativa = true;
    (void)io_saidas_define_regra_sensor(canal, true);
  }
  (void)io_saidas_sensor_falha(canal, falha);
  if (!falha)
  {
    (void)io_saidas_pedido(canal, CONTROLE_FONTE_SENSOR, regra_sensor_avalia(&regras[i], valor));
  }
}

/**
 * @brief Trata uma saída: lê a entrada associada e aplica a frequência ou a regra.
 *
 * @param cfg Configuração.
 * @param i Índice da saída, de 0 a 3.
 */
static void trata_saida(const config_t *cfg, uint8_t i)
{
  uint8_t entrada = (uint8_t)config_inteiro(cfg, CFG_SAI_ENTRADA, i);
  uint8_t e;
  float ma = 0.0F;
  float valor = 0.0F;
  bool falha;

  if ((entrada == 0U) || (entrada > CONFIG_QTD_ENTRADAS_ANALOGICAS))
  {
    solta_saida(i);
    return;
  }
  e = (uint8_t)(entrada - 1U);
  if (aquisicao_ma(e, &ma) != AQUISICAO_OK)
  {
    return; /* Ainda sem leitura: não declara falha. */
  }
  falha = corrente_em_falha(ma);
  (void)sensor_ma_para_valor(ma, config_float(cfg, CFG_ENT_ZERO, e), config_float(cfg, CFG_ENT_SPAN, e),
                             config_float(cfg, CFG_ENT_OFFSET, e), &valor);

  if (config_inteiro(cfg, CFG_SAI_TIPO, i) == 1)
  {
    em_frequencia[i] = true;
    (void)io_frequencia_atualiza((uint8_t)(i + 1U), ma, falha);
  }
  else
  {
    aplica_regra(cfg, i, falha, valor);
  }
}

/**
 * @brief Corpo da tarefa: avalia as quatro saídas a cada período.
 *
 * @param[in] argumento Não usado.
 */
static void tarefa_regras(void *argumento)
{
  TickType_t ultimo = xTaskGetTickCount();
  config_t cfg;
  uint8_t i;

  (void)argumento;
  for (;;)
  {
    config_tarefa_copia(&cfg);
    for (i = 0U; i < CONFIG_QTD_SAIDAS; i++)
    {
      trata_saida(&cfg, i);
    }
    vTaskDelayUntil(&ultimo, pdMS_TO_TICKS(REGRAS_TAREFA_PERIODO_MS));
  }
}

int32_t regras_tarefa_inicia(void)
{
  uint8_t i;
  TaskHandle_t tarefa;

  for (i = 0U; i < CONFIG_QTD_SAIDAS; i++)
  {
    (void)regra_sensor_limpa(&regras[i]);
  }
  tarefa = xTaskCreateStatic(tarefa_regras, "regras", REGRAS_TAREFA_PILHA_PALAVRAS, NULL,
                             RTOS_PRIO_REGRAS, pilha_regras, &tcb_regras);
  return (tarefa == NULL) ? REGRAS_TAREFA_ERRO_TAREFA : REGRAS_TAREFA_OK;
}
