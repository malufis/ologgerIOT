/**
 * @file agenda.h
 * @brief Agenda semanal de uma saída digital: 336 intervalos de 30 minutos, ligada ou desligada.
 *
 * Lógica pura, sem acesso a hardware, relógio nem MQTT, testada no PC (tests/agenda). Guarda o
 * estado desejado de cada intervalo da semana e informa o estado de um instante. A agenda é um
 * comando automático **local**: funciona sem conectividade e só vale com o device offline (ver
 * controle_saida.h).
 *
 * **Formato da agenda recebida do CCO por MQTT** (proposta de 04/11/2026, a confirmar):
 * cada saída recebe uma cadeia de **84 caracteres hexadecimais**, que representam 42 bytes e
 * 336 bits. O bit k é o estado do intervalo k: 1 = ligada, 0 = desligada. O intervalo k vale
 * `dia * 48 + (hora * 2 + minuto / 30)`, com o dia contado de 0 (segunda) a 6 (domingo). O bit k
 * fica no byte `k / 8`, na posição `k % 8`, da menos significativa (bit 0) para a mais
 * significativa. O byte 0 vem primeiro na cadeia, com dois dígitos por byte, em maiúsculas ou
 * minúsculas.
 *
 * Os horários são os da instalação, os mesmos do `timestamp` do JSON (fuso ainda a definir, P-043).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef AGENDA_H
#define AGENDA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Intervalos de 30 minutos em uma semana: 48 por dia, durante sete dias. */
#define AGENDA_INTERVALOS      336U
/** Bytes de uma agenda: um bit por intervalo. */
#define AGENDA_BYTES           42U
/** Caracteres hexadecimais de uma agenda: dois por byte. */
#define AGENDA_HEX_TAMANHO     84U
/** Valor devolvido por agenda_indice() para um instante inválido. */
#define AGENDA_INDICE_INVALIDO 0xFFFFU

/** Código de retorno: sucesso. */
#define AGENDA_OK              0
/** Código de retorno: parâmetro nulo. */
#define AGENDA_ERRO_PARAMETRO  (-1)
/** Código de retorno: a cadeia não tem exatamente 84 caracteres. */
#define AGENDA_ERRO_TAMANHO    (-2)
/** Código de retorno: a cadeia tem um caractere que não é dígito hexadecimal. */
#define AGENDA_ERRO_CARACTERE  (-3)

/** O que a agenda diz para um instante. */
typedef enum
{
  AGENDA_SEM_AGENDA = 0, /**< A saída não tem agenda definida. */
  AGENDA_DESLIGADA = 1,  /**< O intervalo pede a saída desligada. */
  AGENDA_LIGADA = 2      /**< O intervalo pede a saída ligada. */
} agenda_estado_t;

/** Agenda semanal de uma saída. */
typedef struct
{
  uint8_t bits[AGENDA_BYTES]; /**< Um bit por intervalo: 1 = ligada. */
  bool definida;              /**< true se a agenda foi carregada. */
} agenda_t;

/**
 * @brief Deixa a agenda sem definição: o estado de qualquer instante passa a ser "sem agenda".
 *
 * @param agenda Agenda a limpar.
 * @return AGENDA_OK ou AGENDA_ERRO_PARAMETRO.
 */
int32_t agenda_limpa(agenda_t *agenda);

/**
 * @brief Carrega a agenda a partir da cadeia hexadecimal recebida do CCO.
 *
 * A agenda só muda se a cadeia for válida por inteiro: um erro deixa a agenda anterior intacta.
 *
 * @param agenda Agenda a carregar.
 * @param hex Cadeia de 84 caracteres hexadecimais; não precisa terminar em zero.
 * @param tamanho Quantidade de caracteres de `hex`.
 * @return AGENDA_OK, AGENDA_ERRO_PARAMETRO, AGENDA_ERRO_TAMANHO ou AGENDA_ERRO_CARACTERE.
 */
int32_t agenda_carrega_hex(agenda_t *agenda, const char *hex, size_t tamanho);

/**
 * @brief Calcula o índice do intervalo de 30 minutos de um instante da semana.
 *
 * @param dia Dia da semana, de 0 (segunda) a 6 (domingo).
 * @param hora Hora, de 0 a 23.
 * @param minuto Minuto, de 0 a 59.
 * @return Índice de 0 a 335, ou AGENDA_INDICE_INVALIDO se algum argumento estiver fora da faixa.
 */
uint16_t agenda_indice(uint8_t dia, uint8_t hora, uint8_t minuto);

/**
 * @brief Informa o que a agenda pede para um instante da semana.
 *
 * @param agenda Agenda iniciada com agenda_limpa() ou carregada com agenda_carrega_hex().
 * @param dia Dia da semana, de 0 (segunda) a 6 (domingo).
 * @param hora Hora, de 0 a 23.
 * @param minuto Minuto, de 0 a 59.
 * @return AGENDA_SEM_AGENDA se a agenda não está definida ou o instante é inválido; senão, ligada
 *         ou desligada.
 */
agenda_estado_t agenda_estado(const agenda_t *agenda, uint8_t dia, uint8_t hora, uint8_t minuto);

#endif /* AGENDA_H */
