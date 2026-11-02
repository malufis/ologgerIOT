/**
 * @file main.c
 * @brief Ponto de entrada do firmware STM32 do oLoggerIOT.
 *
 * Build vazio (FW-006): inicializa o HAL e fica em laço, sem tarefas.
 * O FreeRTOS entra na FW-046.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#include "hal_stm32.h"

/**
 * @brief Função principal do firmware.
 *
 * @return Nunca retorna.
 * @note Chamada pelo código de inicialização do startup, depois de SystemInit.
 */
int main(void)
{
  (void)HAL_Init();

  for (;;)
  {
  }
}
