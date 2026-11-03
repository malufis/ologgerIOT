/**
 * @file rtos.h
 * @brief Base do FreeRTOS: prioridades das tarefas e ligação com o SysTick.
 *
 * Reúne a tabela de prioridades do projeto e o que o FreeRTOS exige da
 * aplicação com alocação estática. Não esconde a API do FreeRTOS: os módulos
 * criam suas tarefas com as variantes *Static.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef RTOS_H
#define RTOS_H

/** Prioridade para diagnóstico e tarefas sem prazo. A tarefa ociosa fica em 0. */
#define RTOS_PRIO_SEGUNDO_PLANO 1U
/** Prioridade das tarefas de comunicação (serial, rádio, MQTT). */
#define RTOS_PRIO_COMUNICACAO   2U
/** Prioridade das regras de sensor, de saída e da agenda. */
#define RTOS_PRIO_REGRAS        3U
/** Prioridade da aquisição analógica, que tem prazo de tempo real. */
#define RTOS_PRIO_AQUISICAO     4U
/** Prioridade das funções críticas: watchdog e tratamento de falha. */
#define RTOS_PRIO_CRITICA       5U

/**
 * @brief Trata o tick do sistema, chamado pela interrupção do SysTick.
 *
 * Só repassa o tick ao FreeRTOS depois que o escalonador foi iniciado, porque o
 * SysTick já corre desde o HAL_Init.
 *
 * @note Chamar somente de dentro de SysTick_Handler.
 */
void rtos_tick_handler(void);

#endif /* RTOS_H */
