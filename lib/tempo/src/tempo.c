/**
 * @file tempo.c
 * @brief Implementação da conversão de tempo UTC em data e hora locais.
 *
 * Só usa aritmética de 32 bits nas divisões: a divisão de 64 bits do compilador custa alguns
 * quilobytes de flash no STM32 e não é necessária, porque os dias desde 1970 cabem em 32 bits.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.3.0
 */

#include "tempo.h"

#include <stddef.h>

/** Segundos em um dia. */
#define SEGUNDOS_POR_DIA    86400
/** Segundos em uma hora. */
#define SEGUNDOS_POR_HORA   3600
/** Segundos em um minuto. */
#define SEGUNDOS_POR_MINUTO 60
/** Dias do início da era (01/03/0000) até 01/01/1970. */
#define DIAS_ATE_1970       719468
/** Dias em um ciclo de 400 anos. */
#define DIAS_EM_400_ANOS    146097
/** Dia da semana (0 = segunda) de 01/01/1970, que foi uma quinta-feira. */
#define SEMANA_EM_1970      3

/**
 * @brief Converte dias desde 01/01/1970 em ano, mês e dia do calendário gregoriano.
 *
 * @param dias Dias desde 01/01/1970; deve ser maior ou igual a -719468 (a era começa em 0000).
 * @param local Recebe ano, mês e dia do mês.
 */
static void data_dos_dias(int32_t dias, tempo_local_t *local)
{
  int32_t z = dias + DIAS_ATE_1970;
  int32_t era = z / DIAS_EM_400_ANOS;
  int32_t doe = z - (era * DIAS_EM_400_ANOS);
  int32_t aoe = (doe - (doe / 1460) + (doe / 36524) - (doe / 146096)) / 365;
  int32_t doa = doe - ((365 * aoe) + (aoe / 4) - (aoe / 100));
  int32_t mp = ((5 * doa) + 2) / 153;
  int32_t dia = doa - (((153 * mp) + 2) / 5) + 1;
  int32_t mes = (mp < 10) ? (mp + 3) : (mp - 9);
  int32_t ano = aoe + (era * 400) + ((mes <= 2) ? 1 : 0);

  local->ano = (uint16_t)ano;
  local->mes = (uint8_t)mes;
  local->dia_mes = (uint8_t)dia;
}

int32_t tempo_para_local(uint32_t epoch_s, int16_t fuso_min, tempo_local_t *local)
{
  int32_t dias;
  int32_t resto;

  if (local == NULL)
  {
    return TEMPO_ERRO_PARAMETRO;
  }
  if ((fuso_min < TEMPO_FUSO_MIN) || (fuso_min > TEMPO_FUSO_MAX))
  {
    return TEMPO_ERRO_FUSO;
  }

  /* Os dias e o resto do dia em UTC; o fuso entra no resto, que tem folga em 32 bits. */
  dias = (int32_t)(epoch_s / (uint32_t)SEGUNDOS_POR_DIA);
  resto = (int32_t)(epoch_s % (uint32_t)SEGUNDOS_POR_DIA) + ((int32_t)fuso_min * SEGUNDOS_POR_MINUTO);
  if (resto < 0)
  {
    dias--;
    resto += SEGUNDOS_POR_DIA;
  }
  else if (resto >= SEGUNDOS_POR_DIA)
  {
    dias++;
    resto -= SEGUNDOS_POR_DIA;
  }

  data_dos_dias(dias, local);
  local->dia_semana = (uint8_t)(((dias % 7) + 7 + SEMANA_EM_1970) % 7);
  local->hora = (uint8_t)(resto / SEGUNDOS_POR_HORA);
  local->minuto = (uint8_t)((resto % SEGUNDOS_POR_HORA) / SEGUNDOS_POR_MINUTO);
  local->segundo = (uint8_t)(resto % SEGUNDOS_POR_MINUTO);
  return TEMPO_OK;
}

/**
 * @brief Diz se o ano é bissexto no calendário gregoriano.
 *
 * @param ano Ano.
 * @return true se bissexto.
 */
static bool ano_bissexto(uint16_t ano)
{
  return (((ano % 4U) == 0U) && ((ano % 100U) != 0U)) || ((ano % 400U) == 0U);
}

/**
 * @brief Dias de um mês.
 *
 * @param ano Ano.
 * @param mes Mês de 1 a 12.
 * @return Quantidade de dias.
 */
static uint8_t dias_do_mes(uint16_t ano, uint8_t mes)
{
  static const uint8_t dias[12] = {31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

  if ((mes == 2U) && ano_bissexto(ano))
  {
    return 29U;
  }
  return dias[mes - 1U];
}

int32_t tempo_de_utc(const tempo_local_t *utc, uint32_t *epoch_s)
{
  int32_t y;
  int32_t era;
  int32_t yoe;
  int32_t mp;
  int32_t doy;
  int32_t doe;
  int32_t dias;
  uint64_t segundos;

  if ((utc == NULL) || (epoch_s == NULL))
  {
    return TEMPO_ERRO_PARAMETRO;
  }
  if ((utc->ano < 1970U) || (utc->mes < 1U) || (utc->mes > 12U) || (utc->dia_mes < 1U) ||
      (utc->dia_mes > dias_do_mes(utc->ano, utc->mes)) || (utc->hora > 23U) ||
      (utc->minuto > 59U) || (utc->segundo > 59U))
  {
    return TEMPO_ERRO_DATA;
  }

  y = (int32_t)utc->ano - ((utc->mes <= 2U) ? 1 : 0);
  era = y / 400;
  yoe = y - (era * 400);
  mp = (utc->mes > 2U) ? ((int32_t)utc->mes - 3) : ((int32_t)utc->mes + 9);
  doy = (((153 * mp) + 2) / 5) + (int32_t)utc->dia_mes - 1;
  doe = (yoe * 365) + (yoe / 4) - (yoe / 100) + doy;
  dias = (era * DIAS_EM_400_ANOS) + doe - DIAS_ATE_1970;

  /* O ano mínimo de 1970 garante dias >= 0; a multiplicação é de 32 por 32 bits, sem divisão. */
  segundos = ((uint64_t)(uint32_t)dias * (uint32_t)SEGUNDOS_POR_DIA) +
             ((uint32_t)utc->hora * (uint32_t)SEGUNDOS_POR_HORA) +
             ((uint32_t)utc->minuto * (uint32_t)SEGUNDOS_POR_MINUTO) + utc->segundo;
  if (segundos > (uint64_t)UINT32_MAX)
  {
    return TEMPO_ERRO_DATA;
  }
  *epoch_s = (uint32_t)segundos;
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
