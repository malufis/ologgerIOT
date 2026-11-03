/**
 * @file io_saidas.c
 * @brief Implementação do controle das quatro saídas digitais discretas.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.2.0
 */

#include "io_saidas.h"

#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "rtos.h"
#include "saida.h"

/** Maior retardo aceito em segundos, para o valor em milissegundos caber em 32 bits. */
#define DURACAO_MAX_S (UINT32_MAX / 1000U)

/* Estado do módulo. */
static StaticTask_t tcb_saidas;
static StackType_t pilha_saidas[IO_SAIDAS_PILHA_PALAVRAS];
static saida_t saidas[IO_SAIDAS_QTD];

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
  (void)io_saida_define((uint8_t)(indice + 1U), saida_ligada(&saidas[indice]));
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
      (void)saida_atualiza(&saidas[i], agora_ms());
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
    (void)saida_inicia(&saidas[i]);
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

int32_t io_saidas_liga(uint8_t canal)
{
  if ((canal < 1U) || (canal > IO_SAIDAS_QTD))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  saida_liga(&saidas[canal - 1U], agora_ms());
  aplica_no_pino((uint8_t)(canal - 1U));
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_define_atraso(uint8_t canal, uint32_t segundos)
{
  if ((canal < 1U) || (canal > IO_SAIDAS_QTD) || (segundos > DURACAO_MAX_S))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  saida_define_atraso(&saidas[canal - 1U], segundos * 1000U);
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_aguardando(uint8_t canal, bool *aguardando)
{
  if ((aguardando == NULL) || (canal < 1U) || (canal > IO_SAIDAS_QTD))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  *aguardando = saida_aguardando(&saidas[canal - 1U]);
  return IO_SAIDAS_OK;
}

int32_t io_saidas_desliga(uint8_t canal)
{
  if ((canal < 1U) || (canal > IO_SAIDAS_QTD))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  taskENTER_CRITICAL();
  saida_desliga(&saidas[canal - 1U]);
  aplica_no_pino((uint8_t)(canal - 1U));
  taskEXIT_CRITICAL();
  return IO_SAIDAS_OK;
}

int32_t io_saidas_estado(uint8_t canal, bool *ligada)
{
  if ((ligada == NULL) || (canal < 1U) || (canal > IO_SAIDAS_QTD))
  {
    return IO_SAIDAS_ERRO_PARAMETRO;
  }

  *ligada = saida_ligada(&saidas[canal - 1U]);
  return IO_SAIDAS_OK;
}
