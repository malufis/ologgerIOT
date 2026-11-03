/**
 * @file vida.c
 * @brief Implementação da tarefa de vida.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#include "vida.h"

#include "FreeRTOS.h"
#include "task.h"
#include "rtos.h"

/** Código de erro: a tarefa não foi criada. */
#define VIDA_ERRO_CRIACAO (-1)

/* Estado do módulo. O contador e a folga são lidos por SWD na bancada. */
static StaticTask_t tcb_vida;
static StackType_t pilha_vida[VIDA_PILHA_PALAVRAS];
static volatile uint32_t ciclos;
static volatile uint32_t pilha_livre;

/**
 * @brief Corpo da tarefa de vida: conta, mede a pilha e espera o período.
 *
 * @param[in] argumento Não usado.
 */
static void tarefa_vida(void *argumento)
{
  (void)argumento;

  for (;;)
  {
    ciclos++;
    pilha_livre = (uint32_t)uxTaskGetStackHighWaterMark(NULL);
    vTaskDelay(pdMS_TO_TICKS(VIDA_PERIODO_MS));
  }
}

int32_t vida_inicia(void)
{
  TaskHandle_t tarefa = xTaskCreateStatic(tarefa_vida, "vida", VIDA_PILHA_PALAVRAS, NULL,
                                          RTOS_PRIO_SEGUNDO_PLANO, pilha_vida, &tcb_vida);

  return (tarefa == NULL) ? VIDA_ERRO_CRIACAO : 0;
}

uint32_t vida_contador(void)
{
  return ciclos;
}

uint32_t vida_pilha_livre_palavras(void)
{
  return pilha_livre;
}
