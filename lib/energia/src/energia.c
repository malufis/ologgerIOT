/**
 * @file energia.c
 * @brief Implementação da gerência de energia com Stop 2 (STM32L4).
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#include "energia.h"

#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "hal_stm32.h"
#include "rtos.h"

/* Estado do módulo. */
static StaticTask_t tcb_energia;
static StackType_t pilha_energia[ENERGIA_PILHA_PALAVRAS];
static const energia_ganchos_t *ganchos_atuais;
static volatile bool habilitada;
static volatile bool pedido_acordar;

/**
 * @brief Fica em Stop 2 até alguém pedir para acordar.
 *
 * As interrupções ficam mascaradas entre a verificação do pedido e o WFI, para um pedido
 * feito nesse intervalo não ser perdido: o WFI sai na hora se houver interrupção pendente.
 * Depois de cada saída, as interrupções são liberadas por um instante para o tratador
 * executar. Se ele pediu para acordar (por exemplo, o de um pulso), o laço termina.
 */
static void dorme_ate_pedido(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();
  __disable_irq();
  while (!pedido_acordar)
  {
    HAL_SuspendTick();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    HAL_ResumeTick();
    __enable_irq();
    __ISB();
    __disable_irq();
  }
  __enable_irq();
}

/**
 * @brief Executa um ciclo de sleep: prepara, dorme até o pedido e retoma.
 */
static void ciclo_de_sleep(void)
{
  if (ganchos_atuais->antes != NULL)
  {
    ganchos_atuais->antes();
  }
  pedido_acordar = false;
  dorme_ate_pedido();
  if (ganchos_atuais->depois != NULL)
  {
    ganchos_atuais->depois();
  }
}

/**
 * @brief Corpo da tarefa: a cada período, dorme se estiver habilitado e puder.
 *
 * @param[in] argumento Não usado.
 */
static void tarefa_energia(void *argumento)
{
  (void)argumento;

  for (;;)
  {
    vTaskDelay(pdMS_TO_TICKS(ENERGIA_PERIODO_MS));
    if (habilitada && ganchos_atuais->pode_dormir())
    {
      ciclo_de_sleep();
    }
  }
}

int32_t energia_inicia(const energia_ganchos_t *ganchos)
{
  TaskHandle_t tarefa;

  if ((ganchos == NULL) || (ganchos->pode_dormir == NULL))
  {
    return ENERGIA_ERRO;
  }

  ganchos_atuais = ganchos;
  tarefa = xTaskCreateStatic(tarefa_energia, "energia", ENERGIA_PILHA_PALAVRAS, NULL,
                             RTOS_PRIO_SEGUNDO_PLANO, pilha_energia, &tcb_energia);
  return (tarefa == NULL) ? ENERGIA_ERRO : ENERGIA_OK;
}

int32_t energia_define_habilitada(bool valor)
{
  if (valor && (ganchos_atuais != NULL) && (ganchos_atuais->sleep_permitido != NULL) &&
      !ganchos_atuais->sleep_permitido())
  {
    return ENERGIA_ERRO_BLOQUEADO;
  }

  habilitada = valor;
  return ENERGIA_OK;
}

bool energia_habilitada(void)
{
  return habilitada;
}

void energia_solicita_acordar(void)
{
  pedido_acordar = true;
}
