/**
 * @file tipo_saida.c
 * @brief Implementação do tipo das saídas e da exclusão com o sleep.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "tipo_saida.h"

#include <stddef.h>

/**
 * @brief Diz se o canal é válido.
 *
 * @param canal Saída de 1 a TIPO_SAIDA_QTD.
 * @return true se estiver dentro da faixa.
 */
static bool canal_valido(uint8_t canal)
{
  return (canal >= 1U) && (canal <= TIPO_SAIDA_QTD);
}

int32_t tipos_saida_limpa(tipos_saida_t *tipos)
{
  uint8_t i;

  if (tipos == NULL)
  {
    return TIPO_SAIDA_ERRO_PARAMETRO;
  }
  for (i = 0U; i < TIPO_SAIDA_QTD; i++)
  {
    tipos->tipo[i] = TIPO_SAIDA_DISCRETO;
  }
  return TIPO_SAIDA_OK;
}

int32_t tipos_saida_define(tipos_saida_t *tipos, uint8_t canal, uint16_t tipo, bool sleep_ativo)
{
  if ((tipos == NULL) || !canal_valido(canal))
  {
    return TIPO_SAIDA_ERRO_PARAMETRO;
  }
  if (tipo > TIPO_SAIDA_FREQUENCIA)
  {
    return TIPO_SAIDA_ERRO_TIPO;
  }
  if (tipo == TIPO_SAIDA_FREQUENCIA)
  {
    if (canal < TIPO_SAIDA_PRIMEIRA_FREQUENCIA)
    {
      return TIPO_SAIDA_ERRO_SEM_TIMER;
    }
    if (sleep_ativo)
    {
      return TIPO_SAIDA_ERRO_SLEEP_ATIVO;
    }
  }
  tipos->tipo[canal - 1U] = tipo;
  return TIPO_SAIDA_OK;
}

int32_t tipos_saida_tipo(const tipos_saida_t *tipos, uint8_t canal, uint16_t *tipo)
{
  if ((tipos == NULL) || (tipo == NULL) || !canal_valido(canal))
  {
    return TIPO_SAIDA_ERRO_PARAMETRO;
  }
  *tipo = tipos->tipo[canal - 1U];
  return TIPO_SAIDA_OK;
}

bool tipos_saida_algum_em_frequencia(const tipos_saida_t *tipos)
{
  uint8_t i;

  if (tipos == NULL)
  {
    return false;
  }
  for (i = 0U; i < TIPO_SAIDA_QTD; i++)
  {
    if (tipos->tipo[i] == TIPO_SAIDA_FREQUENCIA)
    {
      return true;
    }
  }
  return false;
}
