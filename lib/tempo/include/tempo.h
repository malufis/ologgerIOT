/**
 * @file tempo.h
 * @brief Conversão do tempo UTC em segundos para a data e hora locais da instalação.
 *
 * Lógica pura, sem acesso a hardware nem ao RTC, testada no PC (tests/tempo). Recebe o instante
 * em segundos desde 01/01/1970 UTC e o **fuso horário da instalação**, em minutos em relação ao
 * UTC (configurado pelo CCO, endereço Modbus 40403 proposto), e devolve a data, a hora e o dia da
 * semana locais. O dia da semana segue a agenda: 0 = segunda-feira a 6 = domingo.
 *
 * O módulo não decide se a hora é válida: isso é do FW-031.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.2.0
 */

#ifndef TEMPO_H
#define TEMPO_H

#include <stdbool.h>
#include <stdint.h>

/** Código de retorno: sucesso. */
#define TEMPO_OK              0
/** Código de retorno: parâmetro nulo. */
#define TEMPO_ERRO_PARAMETRO  (-1)
/** Código de retorno: fuso fora de -720 a +840 minutos. */
#define TEMPO_ERRO_FUSO       (-2)
/** Código de retorno: data ou hora inexistente, ou fora de 1970 a 2105. */
#define TEMPO_ERRO_DATA       (-3)

/** Menor fuso aceito, em minutos (UTC-12:00). */
#define TEMPO_FUSO_MIN        (-720)
/** Maior fuso aceito, em minutos (UTC+14:00). */
#define TEMPO_FUSO_MAX        840

/** Data e hora locais. */
typedef struct
{
  uint16_t ano;        /**< Ano, por exemplo 2026. */
  uint8_t mes;         /**< Mês, de 1 a 12. */
  uint8_t dia_mes;     /**< Dia do mês, de 1 a 31. */
  uint8_t dia_semana;  /**< 0 = segunda-feira a 6 = domingo. */
  uint8_t hora;        /**< Hora, de 0 a 23. */
  uint8_t minuto;      /**< Minuto, de 0 a 59. */
  uint8_t segundo;     /**< Segundo, de 0 a 59. */
} tempo_local_t;

/**
 * @brief Converte o tempo UTC na data e hora locais.
 *
 * @param epoch_s Segundos desde 01/01/1970 00:00:00 UTC.
 * @param fuso_min Fuso da instalação em minutos em relação ao UTC (−180 para Brasília).
 * @param local Recebe a data e hora locais.
 * @return TEMPO_OK, TEMPO_ERRO_PARAMETRO ou TEMPO_ERRO_FUSO.
 */
int32_t tempo_para_local(uint32_t epoch_s, int16_t fuso_min, tempo_local_t *local);

/**
 * @brief Converte data e hora em UTC nos segundos desde 01/01/1970 (o inverso com fuso zero).
 *
 * Valida a data: mês de 1 a 12, dia existente no mês (com ano bissexto), hora, minuto e segundo
 * dentro da faixa, e o resultado dentro de 32 bits sem sinal. O campo `dia_semana` é ignorado.
 *
 * @param utc Data e hora em UTC.
 * @param epoch_s Recebe os segundos desde 01/01/1970 00:00:00 UTC.
 * @return TEMPO_OK, TEMPO_ERRO_PARAMETRO ou TEMPO_ERRO_DATA.
 */
int32_t tempo_de_utc(const tempo_local_t *utc, uint32_t *epoch_s);

/**
 * @brief Diz se o instante local é o início de um intervalo de 30 minutos da agenda.
 *
 * @param local Data e hora locais.
 * @return true se o minuto é 0 ou 30. Falso se @p local for nulo.
 */
bool tempo_inicio_de_intervalo(const tempo_local_t *local);

#endif /* TEMPO_H */
