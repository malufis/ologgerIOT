/**
 * @file test_tempo.c
 * @brief Testes de unidade da conversão de tempo UTC em data e hora locais (FW-030).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "tempo.h"
#include "teste.h"

#include <stddef.h>

/* Confere todos os campos de uma conversão. */
static bool igual(uint32_t epoch, int16_t fuso, uint16_t ano, uint8_t mes, uint8_t dia,
                  uint8_t semana, uint8_t hora, uint8_t minuto, uint8_t segundo)
{
  tempo_local_t t;

  if (tempo_para_local(epoch, fuso, &t) != TEMPO_OK)
  {
    return false;
  }
  return (t.ano == ano) && (t.mes == mes) && (t.dia_mes == dia) && (t.dia_semana == semana) &&
         (t.hora == hora) && (t.minuto == minuto) && (t.segundo == segundo);
}

static void rejeita_nulo_e_fuso_invalido(void)
{
  tempo_local_t t;

  VERIFICA(tempo_para_local(0U, 0, NULL) == TEMPO_ERRO_PARAMETRO);
  VERIFICA(tempo_para_local(0U, -721, &t) == TEMPO_ERRO_FUSO);
  VERIFICA(tempo_para_local(0U, 841, &t) == TEMPO_ERRO_FUSO);
  VERIFICA(tempo_para_local(0U, -720, &t) == TEMPO_OK);
  VERIFICA(tempo_para_local(0U, 840, &t) == TEMPO_OK);
}

static void origem_da_era_unix_foi_quinta_feira(void)
{
  VERIFICA(igual(0U, 0, 1970U, 1U, 1U, 3U, 0U, 0U, 0U));
}

static void ano_bissexto_de_2000(void)
{
  VERIFICA(igual(951868799U, 0, 2000U, 2U, 29U, 1U, 23U, 59U, 59U));
}

static void ano_bissexto_de_2024(void)
{
  VERIFICA(igual(1709164800U, 0, 2024U, 2U, 29U, 3U, 0U, 0U, 0U));
}

static void ano_de_2100_nao_e_bissexto(void)
{
  VERIFICA(igual(4107542400U, 0, 2100U, 3U, 1U, 0U, 0U, 0U, 0U));
}

static void limite_de_32_bits_sem_sinal(void)
{
  VERIFICA(igual(4294967295U, 0, 2106U, 2U, 7U, 6U, 6U, 28U, 15U));
  VERIFICA(igual(2147483647U, 0, 2038U, 1U, 19U, 1U, 3U, 14U, 7U));
}

static void dia_de_hoje_em_utc_e_em_brasilia(void)
{
  /* 04/11/2026 é quarta-feira. 12h30 UTC são 9h30 em Brasília (-180). */
  VERIFICA(igual(1793795400U, 0, 2026U, 11U, 4U, 2U, 12U, 30U, 0U));
  VERIFICA(igual(1793795400U, -180, 2026U, 11U, 4U, 2U, 9U, 30U, 0U));
}

static void fuso_negativo_volta_para_o_dia_anterior(void)
{
  /* 01h UTC de quarta são 22h de terça em Brasília. */
  VERIFICA(igual(1793754000U, -180, 2026U, 11U, 3U, 1U, 22U, 0U, 0U));
  VERIFICA(igual(1794106799U, -180, 2026U, 11U, 7U, 5U, 23U, 59U, 59U));
}

static void fuso_positivo_passa_para_o_ano_seguinte(void)
{
  /* 23h UTC de 31/12/2026 são 1h de 01/01/2027 (sexta) em UTC+2. */
  VERIFICA(igual(1798758000U, 120, 2027U, 1U, 1U, 4U, 1U, 0U, 0U));
}

static void fuso_com_meia_hora(void)
{
  /* Índia, UTC+5:30: 12h30 UTC são 18h00. */
  VERIFICA(igual(1793795400U, 330, 2026U, 11U, 4U, 2U, 18U, 0U, 0U));
}

static void fuso_negativo_antes_de_1970_nao_quebra(void)
{
  /* 1970-01-01 00:00 UTC em UTC-3 é 31/12/1969, quarta-feira, 21h. */
  VERIFICA(igual(0U, -180, 1969U, 12U, 31U, 2U, 21U, 0U, 0U));
}

static void inicio_de_intervalo_so_nos_minutos_0_e_30_com_segundo_zero(void)
{
  tempo_local_t t;

  VERIFICA(tempo_inicio_de_intervalo(NULL) == false);
  (void)tempo_para_local(1793795400U, -180, &t); /* 09:30:00 */
  VERIFICA(tempo_inicio_de_intervalo(&t) == true);
  (void)tempo_para_local(1793795401U, -180, &t); /* 09:30:01 */
  VERIFICA(tempo_inicio_de_intervalo(&t) == false);
  (void)tempo_para_local(1793794800U, -180, &t); /* 09:20:00 */
  VERIFICA(tempo_inicio_de_intervalo(&t) == false);
  (void)tempo_para_local(1793793600U, -180, &t); /* 09:00:00 */
  VERIFICA(tempo_inicio_de_intervalo(&t) == true);
}

int main(void)
{
  EXECUTA(rejeita_nulo_e_fuso_invalido);
  EXECUTA(origem_da_era_unix_foi_quinta_feira);
  EXECUTA(ano_bissexto_de_2000);
  EXECUTA(ano_bissexto_de_2024);
  EXECUTA(ano_de_2100_nao_e_bissexto);
  EXECUTA(limite_de_32_bits_sem_sinal);
  EXECUTA(dia_de_hoje_em_utc_e_em_brasilia);
  EXECUTA(fuso_negativo_volta_para_o_dia_anterior);
  EXECUTA(fuso_positivo_passa_para_o_ano_seguinte);
  EXECUTA(fuso_com_meia_hora);
  EXECUTA(fuso_negativo_antes_de_1970_nao_quebra);
  EXECUTA(inicio_de_intervalo_so_nos_minutos_0_e_30_com_segundo_zero);
  return RESULTADO();
}
