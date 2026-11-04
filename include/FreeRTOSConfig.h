/**
 * @file FreeRTOSConfig.h
 * @brief Configuração do FreeRTOS do firmware oLoggerIOT (STM32L4 e STM32L0).
 *
 * Alocação estática, sem heap: tasks, filas e mutex são criados com as
 * variantes *Static (documentacao/padrao_codigo_c.md, seção FreeRTOS).
 * Valores de prioridade de interrupção conforme os exemplos da ST para
 * Cortex-M4F (4 bits) e Cortex-M0+ (2 bits).
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#ifndef __ASSEMBLER__
  #include <stdint.h>
  #include "hal_stm32.h"
#endif

/* Escalonador. */
#define configUSE_PREEMPTION                     1
#define configUSE_TIME_SLICING                   1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  0
#define configCPU_CLOCK_HZ                       ( SystemCoreClock )
#define configTICK_RATE_HZ                       ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                     ( 7 )
#define configMINIMAL_STACK_SIZE                 ( ( uint16_t ) 128 )
#define configMAX_TASK_NAME_LEN                  ( 16 )
#define configUSE_16_BIT_TICKS                   0
#define configIDLE_SHOULD_YIELD                  1

/* Memória: somente alocação estática. */
#define configSUPPORT_STATIC_ALLOCATION          1
#define configSUPPORT_DYNAMIC_ALLOCATION         0

/* Recursos do kernel. */
#define configUSE_MUTEXES                        1
#define configUSE_RECURSIVE_MUTEXES              0
#define configUSE_COUNTING_SEMAPHORES            1
#define configUSE_TASK_NOTIFICATIONS             1
#define configUSE_TIMERS                         0
#define configQUEUE_REGISTRY_SIZE                0

/* Diagnóstico. */
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configCHECK_FOR_STACK_OVERFLOW           2
#define configUSE_TRACE_FACILITY                 0
#define configGENERATE_RUN_TIME_STATS            0

/* Funções da API incluídas. */
#define INCLUDE_vTaskDelay                       1
#define INCLUDE_vTaskDelayUntil                  1
#define INCLUDE_vTaskDelete                      0
#define INCLUDE_vTaskSuspend                     1
#define INCLUDE_xTaskGetSchedulerState           1
#define INCLUDE_uxTaskGetStackHighWaterMark      1

/* Prioridades de interrupção: o Cortex-M4F tem 4 bits, o Cortex-M0+ tem 2. */
#ifdef __NVIC_PRIO_BITS
  #define configPRIO_BITS                        __NVIC_PRIO_BITS
#else
  #define configPRIO_BITS                        4
#endif

#if ( configPRIO_BITS == 4 )
  #define configLIBRARY_LOWEST_INTERRUPT_PRIORITY      0xf
  #define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#else
  #define configLIBRARY_LOWEST_INTERRUPT_PRIORITY      3
  #define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 3
#endif

#define configKERNEL_INTERRUPT_PRIORITY \
  ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
  ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/* Falha de asserção: desliga as interrupções e trava, para o depurador ver. */
#define configASSERT( x ) \
  do { if ( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for ( ;; ) { } } } while ( 0 )

/* Os handlers de SVC e PendSV são os do vetor. O SysTick é compartilhado com o
   HAL e chamado por rtos_tick_handler (lib/rtos). */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler

#endif /* FREERTOS_CONFIG_H */
