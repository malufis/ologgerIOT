/**
 * @file rtos.c
 * @brief Memória estática da tarefa ociosa, gancho de pilha e tick do FreeRTOS.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#include "rtos.h"

#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Tratador do SysTick do FreeRTOS, definido no port.c do kernel.
 *
 * O FreeRTOS não o declara em cabeçalho, por isso a declaração fica aqui.
 */
extern void xPortSysTickHandler(void);

/* Estado do módulo: memória da tarefa ociosa, exigida na alocação estática. */
static StaticTask_t tcb_ociosa;
static StackType_t pilha_ociosa[configMINIMAL_STACK_SIZE];

/**
 * @brief Fornece ao FreeRTOS a memória estática da tarefa ociosa.
 *
 * @param[out] tcb     Recebe o endereço do bloco de controle da tarefa.
 * @param[out] pilha   Recebe o endereço da pilha da tarefa.
 * @param[out] tamanho Recebe o tamanho da pilha, em palavras.
 * @note Chamada pelo FreeRTOS ao criar a tarefa ociosa, na partida do escalonador.
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **tcb, StackType_t **pilha, uint32_t *tamanho)
{
  *tcb = &tcb_ociosa;
  *pilha = pilha_ociosa;
  *tamanho = configMINIMAL_STACK_SIZE;
}

/**
 * @brief Trata o estouro de pilha de uma tarefa.
 *
 * @param[in] tarefa Tarefa que estourou a pilha.
 * @param[in] nome   Nome da tarefa.
 * @warning Não retorna. Desliga as interrupções e trava, para o depurador
 *          encontrar a tarefa pelo estado do núcleo.
 */
void vApplicationStackOverflowHook(TaskHandle_t tarefa, char *nome)
{
  (void)tarefa;
  (void)nome;
  taskDISABLE_INTERRUPTS();
  for (;;)
  {
  }
}

void rtos_tick_handler(void)
{
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
    xPortSysTickHandler();
  }
}
