/**
 * @file interrupcoes.c
 * @brief Tratadores de interrupção do núcleo que o firmware precisa definir.
 *
 * @date criacao 02/11/2026
 * @date modificacao 04/11/2026
 * @version 0.3.0
 */

#include "hal_stm32.h"
#include "rtos.h"

#ifdef OLOG_TEM_ENTRADA_DIGITAL
  #include "io_entrada.h"
#endif

#ifdef OLOG_TEM_SAIDA_FREQUENCIA
  #include "io_frequencia.h"
#endif

#ifdef OLOG_TEM_ENERGIA
  #include "energia.h"
#endif

/**
 * @brief Tratador do SysTick: avança o tick do HAL e o do FreeRTOS.
 *
 * O SysTick é um só e atende os dois. Os dois ticks têm período de 1 ms.
 */
void SysTick_Handler(void)
{
  HAL_IncTick();
  rtos_tick_handler();
}

#ifdef OLOG_TEM_ENTRADA_DIGITAL
/**
 * @brief Tratador das linhas externas 5 a 9: atende as entradas digitais D1 a D4.
 *
 * Fica aqui, e não na biblioteca, porque o vetor do startup é fraco e o linker não
 * extrai um objeto de uma biblioteca estática só para resolver uma referência fraca.
 */
void EXTI9_5_IRQHandler(void)
{
  io_entrada_irq();
#ifdef OLOG_TEM_ENERGIA
  /* Um pulso tira o STM32 do sleep. Ele fica acordado e só dorme de novo depois do tempo de
   * inatividade sem pulso, então um pulso constante não fica acordando e dormindo. */
  energia_solicita_acordar();
#endif
}
#endif

#ifdef OLOG_TEM_SAIDA_FREQUENCIA
/**
 * @brief Tratador do TIM6: avança os geradores das saídas em frequência.
 *
 * Fica aqui pelo mesmo motivo do EXTI9_5_IRQHandler: o vetor do startup é fraco e o linker não
 * extrai um objeto de uma biblioteca estática só para resolver uma referência fraca.
 */
void TIM6_IRQHandler(void)
{
  io_frequencia_irq();
}
#endif
