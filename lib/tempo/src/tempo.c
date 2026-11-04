/**
 * @file tempo.c
 * @brief Implementação da conversão de tempo UTC em data e hora locais.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "tempo.h"

#include <stddef.h>

/** Segundos em um dia. */
#define SEGUNDOS_POR_DIA   86400
/** Segundos em uma hora. */
#define SEGUNDOS_POR_HORA  3600
/** Segundos em um minuto. */
#define SEGUNDOS_POR_MINUTO 60
/** Dias do início da era (01/03/0000) até 01/01/1970. */
#define DIAS_ATE_1970      719468
/** Dias em um ciclo de 400 anos. */
#define DIAS_EM_400_ANOS   146097
/** Dia da semana (0 = segunda) de 01/01/1970, que foi uma quinta-feira. */
#define SEMANA_EM_1970     3

/**
 * @brief Divisão inteira que arredonda para baixo, também para numeradores negativos.
 *
 * @param a Numerador.
 * @param b Denominador positivo.
 * @return O maior inteiro menor ou igual a a / b.
 */
static int64_t divide_para_baixo(int64_t a, int64_t b)
{
  int64_t q = a / b;

  if (((a % b) != 0) && (a < 0))
  {
    q--;
  }
  return q;
}

/**
 * @brief Converte dias desde 01/01/1970 em ano, mês e dia do calendário gregoriano.
 *
 * @param dias Dias desde 01/01/1970 (pode ser negativo).
 * @param local Recebe ano, mês e dia do mês.
 */
static void data_dos_dias(int64_t dias, tempo_local_t *local)
{
  int64_t z = dias + DIAS_ATE_1970;
  int64_t era = divide_para_baixo(z, DIAS_EM_400_ANOS);
  int64_t doe = z - (era * DIAS_EM_400_ANOS);
  int64_t aoe = (doe - (doe / 1460) + (doe / 36524) - (doe / 146096)) / 365;
  int64_t doa = doe - ((365 * aoe) + (aoe / 4) - (aoe / 100));
  int64_t mp = ((5 * doa) + 2) / 153;
  int64_t dia = doa - (((153 * mp) + 2) / 5) + 1;
  int64_t mes = (mp < 10) ? (mp + 3) : (mp - 9);
  int64_t ano = aoe + (era * 400) + ((mes <= 2) ? 1 : 0);

  local->ano = (uint16_t)ano;
  local->mes = (uint8_t)mes;
  local->dia_mes = (uint8_t)dia;
}

int32_t tempo_para_local(uint32_t epoch_s, int16_t fuso_min, tempo_local_t *local)
{
  int64_t segundos;
  int64_t dias;
  int64_t resto;

  if (local == NULL)
  {
    return TEMPO_ERRO_PARAMETRO;
  }
  if ((fuso_min < TEMPO_FUSO_MIN) || (fuso_min > TEMPO_FUSO_MAX))
  {
    return TEMPO_ERRO_FUSO;
  }

  segundos = (int64_t)epoch_s + ((int64_t)fuso_min * SEGUNDOS_POR_MINUTO);
  dias = divide_para_baixo(segundos, SEGUNDOS_POR_DIA);
  resto = segundos - (dias * SEGUNDOS_POR_DIA);

  data_dos_dias(dias, local);
  local->dia_semana = (uint8_t)(((dias % 7) + 7 + SEMANA_EM_1970) % 7);
  local->hora = (uint8_t)(resto / SEGUNDOS_POR_HORA);
  local->minuto = (uint8_t)((resto % SEGUNDOS_POR_HORA) / SEGUNDOS_POR_MINUTO);
  local->segundo = (uint8_t)(resto % SEGUNDOS_POR_MINUTO);
  return TEMPO_OK;
}

bool tempo_inicio_de_intervalo(const tempo_local_t *local)
{
  if (local == NULL)
  {
    return false;
  }
  return ((local->minuto == 0U) || (local->minuto == 30U)) && (local->segundo == 0U);
}
