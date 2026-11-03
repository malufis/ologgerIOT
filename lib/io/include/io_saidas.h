/**
 * @file io_saidas.h
 * @brief Controle das quatro saídas digitais discretas, com retardo de acionamento (relé de tempo).
 *
 * Reúne o estado de cada saída (saida.h) e o driver dos pinos (io_saida.h). Os comandos
 * ligar e desligar valem na hora, e uma tarefa de baixa prioridade liga as saídas cujo retardo
 * de acionamento chegou ao fim. As saídas começam desligadas.
 *
 * Toda saída aceita o comando direto e pode ter retardo de acionamento (relé de tempo, FW-024);
 * o "acionamento por tempo" do requisito é esse retardo. Quem manda na saída (modo manual ou
 * automático, sensor, agenda) entra no item FW-025, e a saída em frequência (pulsada), no
 * FW-023. Com o STM32 em Stop 2 o pino mantém o estado, mas o retardo não corre, porque o tick
 * do FreeRTOS para.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.2.0
 */

#ifndef IO_SAIDAS_H
#define IO_SAIDAS_H

#include <stdbool.h>
#include <stdint.h>

#include "io_saida.h"

/** Período da tarefa que liga as saídas ao fim do retardo, em milissegundos. */
#define IO_SAIDAS_PERIODO_MS     100U
/** Tamanho da pilha da tarefa, em palavras de 32 bits. */
#define IO_SAIDAS_PILHA_PALAVRAS 128U

/** Código de retorno: sucesso. */
#define IO_SAIDAS_OK             0
/** Código de retorno: canal inválido, retardo grande demais ou parâmetro nulo. */
#define IO_SAIDAS_ERRO_PARAMETRO (-1)
/** Código de retorno: a tarefa não foi criada. */
#define IO_SAIDAS_ERRO_TAREFA    (-2)

/**
 * @brief Leva os pinos das saídas ao estado desligado e zera os estados.
 *
 * @return IO_SAIDAS_OK.
 * @note Chamar o quanto antes na partida, antes de criar tarefas.
 */
int32_t io_saidas_prepara(void);

/**
 * @brief Cria a tarefa que liga as saídas ao fim do retardo, com memória estática.
 *
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_TAREFA.
 * @note Chamar antes de vTaskStartScheduler e depois de io_saidas_prepara().
 */
int32_t io_saidas_inicia(void);

/**
 * @brief Liga uma saída sem prazo, respeitando o retardo de acionamento configurado.
 *
 * Com retardo, a saída continua desligada durante a espera e liga ao fim dela; um comando de
 * desligar durante a espera cancela a partida.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_liga(uint8_t canal);

/**
 * @brief Define o retardo de acionamento (relé de tempo) de uma saída.
 *
 * Vale para os próximos comandos de ligar. Um retardo já em curso não
 * muda. Corresponde ao atraso de partida por saída do mapa Modbus (40256 a 40270).
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param segundos Retardo em segundos, de 0 (liga na hora) a 4 294 967.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_define_atraso(uint8_t canal, uint32_t segundos);

/**
 * @brief Informa se uma saída está esperando o retardo de acionamento.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param aguardando Saída: true se há um retardo em curso; a saída continua desligada.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_aguardando(uint8_t canal, bool *aguardando);

/**
 * @brief Desliga uma saída.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_desliga(uint8_t canal);

/**
 * @brief Lê o estado de uma saída.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param ligada Saída: true se a saída está ligada.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_estado(uint8_t canal, bool *ligada);

#endif /* IO_SAIDAS_H */
