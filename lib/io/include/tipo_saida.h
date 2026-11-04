/**
 * @file tipo_saida.h
 * @brief Tipo de cada saída digital (discreto ou frequência) e a exclusão com o modo sleep.
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/io). Guarda o tipo das quatro saídas
 * (mapa Modbus 40166 a 40180: 0 = discreto, 1 = frequência, 2 = reservado) e aplica as regras
 * de exclusão do FW-061:
 *
 * - **Só as saídas 2, 3 e 4 têm modo frequência** (decidido em 04/11/2026). Elas ficam em PB13,
 *   PB14 e PB15, os pinos com canal de timer. A saída 1 (PB12) não tem canal de timer e fica
 *   só discreta.
 * - **A saída em frequência só pode ser habilitada com o sleep desligado**, porque o timer de
 *   software não roda em Stop 2.
 * - **O sleep só pode ligar se nenhuma saída estiver em frequência**: quem liga o sleep consulta
 *   tipos_saida_algum_em_frequencia().
 *
 * Voltar uma saída ao tipo discreto é sempre permitido.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef TIPO_SAIDA_H
#define TIPO_SAIDA_H

#include <stdbool.h>
#include <stdint.h>

/** Número de saídas. */
#define TIPO_SAIDA_QTD                 4U
/** Primeira saída (de 1 a 4) com modo frequência: a saída 1 não tem canal de timer. */
#define TIPO_SAIDA_PRIMEIRA_FREQUENCIA 2U

/** Tipo discreto no mapa Modbus. */
#define TIPO_SAIDA_DISCRETO            0U
/** Tipo frequência no mapa Modbus. */
#define TIPO_SAIDA_FREQUENCIA          1U

/** Código de retorno: sucesso. */
#define TIPO_SAIDA_OK                  0
/** Código de retorno: parâmetro nulo ou canal fora de 1 a TIPO_SAIDA_QTD. */
#define TIPO_SAIDA_ERRO_PARAMETRO      (-1)
/** Código de retorno: tipo diferente de 0 e 1 (o 2 é reservado). */
#define TIPO_SAIDA_ERRO_TIPO           (-2)
/** Código de retorno: a saída não tem canal de timer, então não tem modo frequência. */
#define TIPO_SAIDA_ERRO_SEM_TIMER      (-3)
/** Código de retorno: o sleep está ligado, então a frequência não pode ser habilitada. */
#define TIPO_SAIDA_ERRO_SLEEP_ATIVO    (-4)

/** Tipos das saídas. */
typedef struct
{
  uint16_t tipo[TIPO_SAIDA_QTD]; /**< Tipo de cada saída: TIPO_SAIDA_DISCRETO ou TIPO_SAIDA_FREQUENCIA. */
} tipos_saida_t;

/**
 * @brief Põe todas as saídas no tipo discreto.
 *
 * @param tipos Tipos a limpar.
 * @return TIPO_SAIDA_OK, ou TIPO_SAIDA_ERRO_PARAMETRO se @p tipos for nulo.
 */
int32_t tipos_saida_limpa(tipos_saida_t *tipos);

/**
 * @brief Define o tipo de uma saída, respeitando a exclusão com o sleep.
 *
 * @param tipos Tipos das saídas.
 * @param canal Saída de 1 a TIPO_SAIDA_QTD.
 * @param tipo TIPO_SAIDA_DISCRETO ou TIPO_SAIDA_FREQUENCIA.
 * @param sleep_ativo true se o modo sleep está ligado.
 * @return TIPO_SAIDA_OK, ou o código de erro (o tipo anterior fica como estava).
 */
int32_t tipos_saida_define(tipos_saida_t *tipos, uint8_t canal, uint16_t tipo, bool sleep_ativo);

/**
 * @brief Lê o tipo de uma saída.
 *
 * @param tipos Tipos das saídas.
 * @param canal Saída de 1 a TIPO_SAIDA_QTD.
 * @param tipo Recebe o tipo.
 * @return TIPO_SAIDA_OK ou TIPO_SAIDA_ERRO_PARAMETRO.
 */
int32_t tipos_saida_tipo(const tipos_saida_t *tipos, uint8_t canal, uint16_t *tipo);

/**
 * @brief Diz se alguma saída está em frequência.
 *
 * @param tipos Tipos das saídas.
 * @return true se há uma saída em frequência: o sleep não pode ligar. Falso se @p tipos for nulo.
 */
bool tipos_saida_algum_em_frequencia(const tipos_saida_t *tipos);

#endif /* TIPO_SAIDA_H */
