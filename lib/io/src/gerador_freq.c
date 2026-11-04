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
/** Resolução da frequência: milésimos de hertz. */
#define MILI_POR_HZ     1000U

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
  uint64_t mili_hz;

  if (gerador == NULL)
  {
    return GERADOR_FREQ_ERRO_PARAMETRO;
  }
  if ((isfinite(hz) == 0) || (hz < 0.0F) || (hz > GERADOR_FREQ_MAX_HZ))
  {
    return GERADOR_FREQ_ERRO_FREQUENCIA;
  }
  mili_hz = (uint64_t)((hz * (float)MILI_POR_HZ) + 0.5F);
  if (mili_hz == 0U)
  {
    return gerador_freq_limpa(gerador);
  }
  gerador->passo = (uint32_t)((mili_hz << 32) / ((uint64_t)GERADOR_FREQ_TICK_HZ * MILI_POR_HZ));
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
