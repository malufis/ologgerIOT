/**
 * @file agenda_config.h
 * @brief Formato da configuração persistente da agenda: o fuso horário e as quatro agendas.
 *
 * Lógica pura, testada no PC (tests/agenda). Empacota o que precisa sobreviver ao reinício do
 * device (decidido em 04/11/2026: a agenda e o fuso gravados persistem) numa carga de bytes para o
 * armazém (armazem.h), e a desempacota na partida.
 *
 * Formato da carga, com 176 bytes:
 *
 * | Posição | Tamanho | Conteúdo |
 * |---|---|---|
 * | 0 | 1 | Versão do formato (AGENDA_CONFIG_VERSAO) |
 * | 1 | 1 | Reservado, zero |
 * | 2 | 2 | Fuso horário em minutos em relação ao UTC, inteiro com sinal, little-endian |
 * | 4 + 43 * (n − 1) | 1 | Agenda da saída n definida (0 ou 1) |
 * | 5 + 43 * (n − 1) | 42 | Bits da agenda da saída n (agenda.h) |
 *
 * A agenda não guarda versão nem histórico: só a última enviada pelo CCO (P-043).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef AGENDA_CONFIG_H
#define AGENDA_CONFIG_H

#include <stddef.h>
#include <stdint.h>

#include "agenda_aplicador.h"

/** Versão do formato da carga. */
#define AGENDA_CONFIG_VERSAO   1U
/** Tamanho da carga, em bytes. */
#define AGENDA_CONFIG_TAMANHO  176U

/** Código de retorno: sucesso. */
#define AGENDA_CONFIG_OK              0
/** Código de retorno: parâmetro nulo ou buffer pequeno. */
#define AGENDA_CONFIG_ERRO_PARAMETRO  (-1)
/** Código de retorno: tamanho ou versão da carga diferentes do esperado. */
#define AGENDA_CONFIG_ERRO_FORMATO    (-2)
/** Código de retorno: um campo da carga tem valor inválido. */
#define AGENDA_CONFIG_ERRO_CAMPO      (-3)

/**
 * @brief Empacota o fuso e as agendas numa carga.
 *
 * @param ap Aplicador com as agendas.
 * @param fuso_min Fuso horário em minutos em relação ao UTC.
 * @param carga Recebe a carga.
 * @param capacidade Tamanho de @p carga; pelo menos AGENDA_CONFIG_TAMANHO.
 * @param tamanho Recebe o tamanho da carga (AGENDA_CONFIG_TAMANHO).
 * @return AGENDA_CONFIG_OK ou AGENDA_CONFIG_ERRO_PARAMETRO.
 */
int32_t agenda_config_empacota(const agenda_aplicador_t *ap, int16_t fuso_min, uint8_t *carga,
                               size_t capacidade, size_t *tamanho);

/**
 * @brief Desempacota uma carga nas agendas e no fuso, só se ela for válida por inteiro.
 *
 * Depois de carregar, todas as saídas ficam marcadas para serem avaliadas de novo. Com erro, as
 * agendas e o fuso ficam como estavam.
 *
 * @param ap Aplicador que recebe as agendas.
 * @param fuso_min Recebe o fuso horário.
 * @param carga Carga lida do armazém.
 * @param tamanho Tamanho da carga.
 * @return AGENDA_CONFIG_OK, AGENDA_CONFIG_ERRO_PARAMETRO, AGENDA_CONFIG_ERRO_FORMATO ou
 *         AGENDA_CONFIG_ERRO_CAMPO.
 */
int32_t agenda_config_desempacota(agenda_aplicador_t *ap, int16_t *fuso_min, const uint8_t *carga,
                                  size_t tamanho);

#endif /* AGENDA_CONFIG_H */
