/**
 * @file io_totalizador.c
 * @brief Implementação do totalizador de pulsos com duas cópias e complemento.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#include "io_totalizador.h"

#include <stddef.h>

/** Metade do espaço de 32 bits, usada para comparar totais que voltam a zero. */
#define METADE_32BITS 0x80000000UL

/* Estado do módulo. */
static const io_totalizador_armazem_t *armazem_atual;
static uint32_t totais[IO_TOTALIZADOR_CANAIS];
static bool corrompido[IO_TOTALIZADOR_CANAIS];

/**
 * @brief Lê uma cópia (valor e complemento) e diz se é válida.
 *
 * @param base Índice da primeira palavra da cópia no armazém.
 * @param[out] valor Valor da cópia, se válida.
 * @return true se o complemento confere com o valor.
 */
static bool le_copia(uint8_t base, uint32_t *valor)
{
  uint32_t v = armazem_atual->le(base);
  uint32_t c = armazem_atual->le((uint8_t)(base + 1U));

  *valor = v;
  return v == (uint32_t)~c;
}

/**
 * @brief Grava as duas cópias de um total, uma depois da outra.
 *
 * @param indice Índice do canal, de 0 a IO_TOTALIZADOR_CANAIS - 1.
 */
static void grava_canal(uint8_t indice)
{
  uint8_t base = (uint8_t)(indice * IO_TOTALIZADOR_PALAVRAS_CANAL);
  uint32_t v = totais[indice];

  armazem_atual->grava(base, v);
  armazem_atual->grava((uint8_t)(base + 1U), (uint32_t)~v);
  armazem_atual->grava((uint8_t)(base + 2U), v);
  armazem_atual->grava((uint8_t)(base + 3U), (uint32_t)~v);
}

/**
 * @brief Carrega o total de um canal escolhendo a cópia válida mais adiantada.
 *
 * @param indice Índice do canal, de 0 a IO_TOTALIZADOR_CANAIS - 1.
 */
static void carrega_canal(uint8_t indice)
{
  uint8_t base = (uint8_t)(indice * IO_TOTALIZADOR_PALAVRAS_CANAL);
  uint32_t a = 0U;
  uint32_t b = 0U;
  bool a_valida = le_copia(base, &a);
  bool b_valida = le_copia((uint8_t)(base + 2U), &b);

  corrompido[indice] = false;
  if (a_valida && b_valida)
  {
    /* Se a gravação foi interrompida entre as cópias, a mais nova está à frente. */
    totais[indice] = ((uint32_t)(a - b) < METADE_32BITS) ? a : b;
  }
  else if (a_valida || b_valida)
  {
    totais[indice] = a_valida ? a : b;
  }
  else
  {
    totais[indice] = 0U;
    corrompido[indice] = true;
    grava_canal(indice);
  }
}

int32_t io_totalizador_inicia(const io_totalizador_armazem_t *armazem)
{
  uint8_t i;

  if ((armazem == NULL) || (armazem->le == NULL) || (armazem->grava == NULL))
  {
    return IO_TOTALIZADOR_ERRO_PARAMETRO;
  }

  armazem_atual = armazem;
  for (i = 0U; i < IO_TOTALIZADOR_CANAIS; i++)
  {
    carrega_canal(i);
  }
  return IO_TOTALIZADOR_OK;
}

int32_t io_totalizador_soma(uint8_t canal, uint32_t quantidade)
{
  if ((armazem_atual == NULL) || (canal < 1U) || (canal > IO_TOTALIZADOR_CANAIS))
  {
    return IO_TOTALIZADOR_ERRO_PARAMETRO;
  }

  totais[canal - 1U] += quantidade; /* Volta a zero em 32 bits, como um hodômetro. */
  grava_canal((uint8_t)(canal - 1U));
  return IO_TOTALIZADOR_OK;
}

int32_t io_totalizador_define(uint8_t canal, uint32_t valor)
{
  if ((armazem_atual == NULL) || (canal < 1U) || (canal > IO_TOTALIZADOR_CANAIS))
  {
    return IO_TOTALIZADOR_ERRO_PARAMETRO;
  }

  totais[canal - 1U] = valor;
  corrompido[canal - 1U] = false;
  grava_canal((uint8_t)(canal - 1U));
  return IO_TOTALIZADOR_OK;
}

uint32_t io_totalizador_le(uint8_t canal)
{
  if ((canal < 1U) || (canal > IO_TOTALIZADOR_CANAIS))
  {
    return 0U;
  }
  return totais[canal - 1U];
}

bool io_totalizador_corrompido(uint8_t canal)
{
  if ((canal < 1U) || (canal > IO_TOTALIZADOR_CANAIS))
  {
    return false;
  }
  return corrompido[canal - 1U];
}
