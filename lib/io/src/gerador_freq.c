/**
 * @file gerador_freq.c
 * @brief Implementação do gerador de onda quadrada por acumulador de fase.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "gerador_freq.h"

#include <math.h>
#include <stddef.h>

/** Metade da volta do acumulador: abaixo dela o nível é alto (duty de 50%). */
#define METADE_DA_VOLTA 0x80000000UL
/** Passo do acumulador por hertz: 2^32 / GERADOR_FREQ_TICK_HZ (100 kHz). */
#define PASSOS_POR_HZ   42949.67296F

int32_t gerador_freq_limpa(gerador_freq_t *gerador)
{
  if (gerador == NULL)
  {
    return GERADOR_FREQ_ERRO_PARAMETRO;
  }
  gerador->fase = 0U;
  gerador->passo = 0U;
  return GERADOR_FREQ_OK;
}

int32_t gerador_freq_define(gerador_freq_t *gerador, float hz)
{
  float passo;

  if (gerador == NULL)
  {
    return GERADOR_FREQ_ERRO_PARAMETRO;
  }
  if ((isfinite(hz) == 0) || (hz < 0.0F) || (hz > GERADOR_FREQ_MAX_HZ))
  {
    return GERADOR_FREQ_ERRO_FREQUENCIA;
  }
  /* Passo = hz * 2^32 / tick, em ponto flutuante de 32 bits: evita a aritmética de 64 bits, que
   * custa vários quilobytes de flash. Em 20 kHz o passo cabe em 32 bits (8,6e8). */
  passo = (hz * PASSOS_POR_HZ) + 0.5F;
  if (passo < 1.0F)
  {
    return gerador_freq_limpa(gerador);
  }
  gerador->passo = (uint32_t)passo;
  return GERADOR_FREQ_OK;
}

bool gerador_freq_tick(gerador_freq_t *gerador)
{
  if ((gerador == NULL) || (gerador->passo == 0U))
  {
    return false;
  }
  gerador->fase += gerador->passo;
  return gerador->fase < METADE_DA_VOLTA;
}
