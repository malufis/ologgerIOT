/**
 * @file freq_saida.c
 * @brief Implementação da conversão de corrente 4–20 mA em frequência.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "freq_saida.h"

#include <math.h>
#include <stddef.h>

/**
 * @brief Diz se a frequência é aceita pelo cadastro.
 *
 * @param hz Frequência em Hz.
 * @return true se for finita e estiver entre 0 e FREQ_SAIDA_MAX_HZ.
 */
static bool frequencia_valida(float hz)
{
  return (isfinite(hz) != 0) && (hz >= 0.0F) && (hz <= FREQ_SAIDA_MAX_HZ);
}

int32_t freq_saida_limpa(freq_saida_t *freq)
{
  if (freq == NULL)
  {
    return FREQ_SAIDA_ERRO_PARAMETRO;
  }
  freq->definida = false;
  freq->hz_4ma = 0.0F;
  freq->hz_20ma = 0.0F;
  return FREQ_SAIDA_OK;
}

int32_t freq_saida_cadastra(freq_saida_t *freq, float hz_4ma, float hz_20ma)
{
  if (freq == NULL)
  {
    return FREQ_SAIDA_ERRO_PARAMETRO;
  }
  if (!frequencia_valida(hz_4ma) || !frequencia_valida(hz_20ma))
  {
    return FREQ_SAIDA_ERRO_FREQUENCIA;
  }
  freq->definida = true;
  freq->hz_4ma = hz_4ma;
  freq->hz_20ma = hz_20ma;
  return FREQ_SAIDA_OK;
}

int32_t freq_saida_calcula(const freq_saida_t *freq, float ma, bool falha, float *hz)
{
  float corrente;
  float fracao;

  if ((freq == NULL) || (hz == NULL) || (isnan(ma) != 0))
  {
    return FREQ_SAIDA_ERRO_PARAMETRO;
  }
  *hz = 0.0F;
  if (!freq->definida)
  {
    return FREQ_SAIDA_ERRO_SEM_CADASTRO;
  }
  if (falha)
  {
    return FREQ_SAIDA_OK;
  }
  corrente = ma;
  if (corrente < FREQ_SAIDA_MA_MINIMO)
  {
    corrente = FREQ_SAIDA_MA_MINIMO;
  }
  if (corrente > FREQ_SAIDA_MA_MAXIMO)
  {
    corrente = FREQ_SAIDA_MA_MAXIMO;
  }
  fracao = (corrente - FREQ_SAIDA_MA_MINIMO) / (FREQ_SAIDA_MA_MAXIMO - FREQ_SAIDA_MA_MINIMO);
  *hz = freq->hz_4ma + ((freq->hz_20ma - freq->hz_4ma) * fracao);
  return FREQ_SAIDA_OK;
}
