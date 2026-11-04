/**
 * @file modbus_tarefa.c
 * @brief Implementação da tarefa do escravo Modbus RTU.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "modbus_tarefa.h"

#include "FreeRTOS.h"
#include "task.h"

#include "config.h"
#include "config_tarefa.h"
#include "modbus_mapa.h"
#include "modbus_rtu.h"
#include "rs485.h"
#include "rtos.h"

static StaticTask_t tcb_modbus;
static StackType_t pilha_modbus[MODBUS_TAREFA_PILHA_PALAVRAS];

/** Registrador de retenção com a velocidade, o modo e o endereço (40400, 40401 e 40402). */
#define REG_VELOCIDADE 40400U

/**
 * @brief Lê a velocidade, o modo e o endereço da configuração.
 *
 * @param velocidade Recebe o código da velocidade.
 * @param modo Recebe o modo (1 = escravo).
 * @param endereco Recebe o endereço do escravo.
 */
static void le_configuracao(uint16_t *velocidade, uint16_t *modo, uint16_t *endereco)
{
  uint16_t r[3] = {1U, 1U, 1U};

  (void)config_tarefa_le(REG_VELOCIDADE, 3U, r);
  *velocidade = r[0];
  *modo = r[1];
  *endereco = r[2];
}

/**
 * @brief Corpo da tarefa: recebe um quadro, processa e responde.
 *
 * @param[in] argumento Não usado.
 */
static void tarefa_modbus(void *argumento)
{
  uint8_t pedido[MODBUS_QUADRO_MAX];
  uint8_t resposta[MODBUS_QUADRO_MAX];
  uint16_t velocidade = 0U;
  uint16_t velocidade_atual = 0xFFFFU;
  uint16_t modo = 1U;
  uint16_t endereco = 1U;
  size_t n;
  size_t r;

  (void)argumento;
  le_configuracao(&velocidade, &modo, &endereco);
  velocidade_atual = velocidade;
  for (;;)
  {
    le_configuracao(&velocidade, &modo, &endereco);
    if (velocidade != velocidade_atual)
    {
      (void)rs485_define_velocidade(velocidade);
      velocidade_atual = velocidade;
    }
    if (modo != 1U)
    {
      vTaskDelay(pdMS_TO_TICKS(MODBUS_TAREFA_ESPERA_MS)); /* Modo mestre ainda não existe. */
      continue;
    }
    n = rs485_recebe(pedido, sizeof(pedido), MODBUS_TAREFA_ESPERA_MS);
    if (n > 0U)
    {
      r = modbus_rtu_processa(modbus_mapa_ops(), (uint8_t)endereco, pedido, n, resposta);
      if (r > 0U)
      {
        (void)rs485_envia(resposta, r);
      }
    }
  }
}

int32_t modbus_tarefa_inicia(void)
{
  uint16_t velocidade = 0U;
  uint16_t modo = 1U;
  uint16_t endereco = 1U;
  TaskHandle_t tarefa;

  le_configuracao(&velocidade, &modo, &endereco);
  if (rs485_inicia(velocidade) != RS485_OK)
  {
    return MODBUS_TAREFA_ERRO;
  }
  tarefa = xTaskCreateStatic(tarefa_modbus, "modbus", MODBUS_TAREFA_PILHA_PALAVRAS, NULL, RTOS_PRIO_COMUNICACAO,
                             pilha_modbus, &tcb_modbus);
  return (tarefa == NULL) ? MODBUS_TAREFA_ERRO : MODBUS_TAREFA_OK;
}
