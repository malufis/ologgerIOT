/**
 * @file vida.h
 * @brief Tarefa de vida: prova que o escalonador roda e mede a folga de pilha.
 *
 * Uma tarefa de baixa prioridade que conta e registra a menor folga de pilha
 * medida. Serve de teste do FreeRTOS em cada alvo (FW-046) e de base para o
 * diagnóstico em campo.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef VIDA_H
#define VIDA_H

#include <stdint.h>

/** Período da tarefa de vida, em milissegundos. */
#define VIDA_PERIODO_MS     100U
/** Tamanho da pilha da tarefa de vida, em palavras de 32 bits. */
#define VIDA_PILHA_PALAVRAS 128U

/**
 * @brief Cria a tarefa de vida com memória estática.
 *
 * @return 0 em sucesso, código negativo em erro.
 * @note Chamar antes de vTaskStartScheduler.
 */
int32_t vida_inicia(void);

/**
 * @brief Lê o contador de ciclos da tarefa de vida.
 *
 * @return Quantidade de ciclos executados desde a partida.
 */
uint32_t vida_contador(void);

/**
 * @brief Lê a menor folga de pilha medida na tarefa de vida.
 *
 * @return Folga mínima, em palavras de 32 bits. Zero antes do primeiro ciclo.
 */
uint32_t vida_pilha_livre_palavras(void);

#endif /* VIDA_H */
