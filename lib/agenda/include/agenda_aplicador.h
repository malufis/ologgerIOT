/**
 * @file agenda_aplicador.h
 * @brief Aplicação das agendas das saídas pelo horário atual.
 *
 * Lógica pura, sem relógio, sem MQTT e sem armazenamento, testada no PC (tests/agenda). Guarda a
 * agenda de cada saída (agenda.h) e, a cada chamada de agenda_aplicador_atualiza() com o horário
 * local, diz que saídas têm um novo estado de agenda a pedir. O estado vale para o **intervalo
 * atual**, e não só para as viradas de intervalo: depois de um reinício, a agenda assume o estado
 * do intervalo do horário atual (decidido em 04/11/2026; se reiniciou às 9h30, vale o que está
 * agendado para as 9h30).
 *
 * Uma saída tem um estado a pedir quando:
 *
 * - é a **primeira avaliação** depois da partida com a hora válida;
 * - o **intervalo de 30 minutos mudou** desde a última avaliação;
 * - a **agenda foi carregada ou apagada** (reavaliação imediata, sem esperar o próximo intervalo).
 *
 * **Hora inválida:** nada é pedido e as saídas mantêm o estado (FW-031). Assim que a hora for
 * válida, a agenda é avaliada de novo.
 *
 * O módulo só informa o estado da agenda. Quem o chama o entrega à saída como pedido da fonte
 * agenda (controle_saida.h), que só o aplica com o device offline, em modo automático e sem
 * regra do sensor. Por isso o pedido pode ser repetido sem efeito: ao ficar offline, a saída já
 * tem o pedido do intervalo atual.
 *
 * Só a última agenda enviada pelo CCO é guardada: carregar de novo substitui a anterior. Uma
 * cadeia vazia apaga a agenda da saída (P-043, proposta mantida); uma cadeia inválida é recusada
 * por inteiro e mantém a agenda anterior.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef AGENDA_APLICADOR_H
#define AGENDA_APLICADOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "agenda.h"

/** Número de saídas com agenda. */
#define AGENDA_APLICADOR_SAIDAS 4U

/** Estado do aplicador: a agenda de cada saída e o último intervalo avaliado. */
typedef struct
{
  agenda_t agenda[AGENDA_APLICADOR_SAIDAS];    /**< Agenda de cada saída. */
  uint16_t indice[AGENDA_APLICADOR_SAIDAS];    /**< Último intervalo avaliado, ou AGENDA_INDICE_INVALIDO. */
  bool reavalia[AGENDA_APLICADOR_SAIDAS];      /**< true se a agenda mudou e deve ser avaliada de novo. */
} agenda_aplicador_t;

/**
 * @brief Apaga todas as agendas e volta ao estado de partida.
 *
 * @param ap Aplicador.
 * @return AGENDA_OK, ou AGENDA_ERRO_PARAMETRO se @p ap for nulo.
 */
int32_t agenda_aplicador_limpa(agenda_aplicador_t *ap);

/**
 * @brief Carrega a agenda de uma saída; só troca a agenda se a cadeia for válida por inteiro.
 *
 * Tamanho zero apaga a agenda da saída. A saída será avaliada de novo na próxima chamada de
 * agenda_aplicador_atualiza().
 *
 * @param ap Aplicador.
 * @param canal Saída de 1 a AGENDA_APLICADOR_SAIDAS.
 * @param hex Cadeia de AGENDA_HEX_TAMANHO caracteres hexadecimais, ou vazia.
 * @param tamanho Número de caracteres de @p hex.
 * @return AGENDA_OK, AGENDA_ERRO_PARAMETRO (nulo ou canal inválido), AGENDA_ERRO_TAMANHO ou
 *         AGENDA_ERRO_CARACTERE.
 */
int32_t agenda_aplicador_carrega(agenda_aplicador_t *ap, uint8_t canal, const char *hex,
                                 size_t tamanho);

/**
 * @brief Avalia as agendas no horário atual e informa as saídas com um estado a pedir.
 *
 * @param ap Aplicador.
 * @param hora_valida true se o relógio tem hora válida; falso não pede nada.
 * @param dia_semana Dia da semana local, de 0 (segunda) a 6 (domingo).
 * @param hora Hora local, de 0 a 23.
 * @param minuto Minuto local, de 0 a 59.
 * @param estados Saída de AGENDA_APLICADOR_SAIDAS posições: o estado a pedir de cada saída
 *                (AGENDA_SEM_AGENDA se a agenda foi apagada), válido onde @p pedir for true.
 * @param pedir Saída de AGENDA_APLICADOR_SAIDAS posições: true onde há um estado a pedir.
 * @return Quantidade de saídas com um estado a pedir, ou zero se algum ponteiro for nulo.
 */
uint8_t agenda_aplicador_atualiza(agenda_aplicador_t *ap, bool hora_valida, uint8_t dia_semana,
                                  uint8_t hora, uint8_t minuto,
                                  agenda_estado_t estados[AGENDA_APLICADOR_SAIDAS],
                                  bool pedir[AGENDA_APLICADOR_SAIDAS]);

#endif /* AGENDA_APLICADOR_H */
