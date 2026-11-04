/**
 * @file io_filtro.c
 * @brief Implementação do filtro de ruído de uma entrada digital discreta.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#include "io_filtro.h"

#include <stddef.h>

int32_t io_filtro_inicia(io_filtro_t *filtro, uint16_t amostras_estaveis, bool estado_inicial)
{
  if ((filtro == NULL) || (amostras_estaveis == 0U))
  {
    return IO_FILTRO_ERRO_PARAMETRO;
  }

  filtro->amostras_estaveis = amostras_estaveis;
  filtro->contagem = 0U;
  filtro->estado = estado_inicial;
  return IO_FILTRO_OK;
}

int32_t io_filtro_amostras_do_tempo(uint32_t tempo_ms, uint32_t periodo_us, uint16_t *amostras_estaveis)
{
  uint64_t tempo_us;
  uint64_t amostras;

  if ((amostras_estaveis == NULL) || (tempo_ms == 0U) || (periodo_us == 0U))
  {
    return IO_FILTRO_ERRO_PARAMETRO;
  }

  /* 64 bits evitam estouro na soma do arredondamento para qualquer entrada de 32 bits. */
  tempo_us = (uint64_t)tempo_ms * 1000U;
  amostras = (tempo_us + periodo_us - 1U) / periodo_us;
  if (amostras > UINT16_MAX)
  {
    return IO_FILTRO_ERRO_PARAMETRO;
  }

  *amostras_estaveis = (uint16_t)amostras;
  return IO_FILTRO_OK;
}

bool io_filtro_atualiza(io_filtro_t *filtro, bool nivel)
{
  if (nivel == filtro->estado)
  {
    /* Nível igual ao aceito: qualquer contagem de mudança é descartada. */
    filtro->contagem = 0U;
    return filtro->estado;
  }

  filtro->contagem++;
  if (filtro->contagem >= filtro->amostras_estaveis)
  {
    filtro->estado = nivel;
    filtro->contagem = 0U;
  }
  return filtro->estado;
}
