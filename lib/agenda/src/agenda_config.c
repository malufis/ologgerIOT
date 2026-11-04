/**
 * @file agenda_config.c
 * @brief Implementação do formato da configuração persistente da agenda.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "agenda_config.h"

#include <string.h>

/** Bytes de cada saída na carga: o indicador de definida e os bits. */
#define BYTES_POR_SAIDA (1U + AGENDA_BYTES)
/** Início das saídas na carga. */
#define INICIO_SAIDAS   4U

int32_t agenda_config_empacota(const agenda_aplicador_t *ap, int16_t fuso_min, uint8_t *carga,
                               size_t capacidade, size_t *tamanho)
{
  uint8_t i;
  uint8_t *p;
  uint16_t fuso = (uint16_t)fuso_min;

  if ((ap == NULL) || (carga == NULL) || (tamanho == NULL) || (capacidade < AGENDA_CONFIG_TAMANHO))
  {
    return AGENDA_CONFIG_ERRO_PARAMETRO;
  }
  (void)memset(carga, 0, AGENDA_CONFIG_TAMANHO);
  carga[0] = (uint8_t)AGENDA_CONFIG_VERSAO;
  carga[2] = (uint8_t)(fuso & 0xFFU);
  carga[3] = (uint8_t)(fuso >> 8U);
  for (i = 0U; i < AGENDA_APLICADOR_SAIDAS; i++)
  {
    p = &carga[INICIO_SAIDAS + ((size_t)i * BYTES_POR_SAIDA)];
    p[0] = ap->agenda[i].definida ? 1U : 0U;
    (void)memcpy(&p[1], ap->agenda[i].bits, AGENDA_BYTES);
  }
  *tamanho = AGENDA_CONFIG_TAMANHO;
  return AGENDA_CONFIG_OK;
}

int32_t agenda_config_desempacota(agenda_aplicador_t *ap, int16_t *fuso_min, const uint8_t *carga,
                                  size_t tamanho)
{
  agenda_aplicador_t novo;
  const uint8_t *p;
  uint8_t i;

  if ((ap == NULL) || (fuso_min == NULL) || (carga == NULL))
  {
    return AGENDA_CONFIG_ERRO_PARAMETRO;
  }
  if ((tamanho != AGENDA_CONFIG_TAMANHO) || (carga[0] != AGENDA_CONFIG_VERSAO))
  {
    return AGENDA_CONFIG_ERRO_FORMATO;
  }

  /* Valida tudo numa cópia, para não alterar nada se algum campo estiver errado. */
  (void)agenda_aplicador_limpa(&novo);
  for (i = 0U; i < AGENDA_APLICADOR_SAIDAS; i++)
  {
    p = &carga[INICIO_SAIDAS + ((size_t)i * BYTES_POR_SAIDA)];
    if (p[0] > 1U)
    {
      return AGENDA_CONFIG_ERRO_CAMPO;
    }
    novo.agenda[i].definida = (p[0] == 1U);
    (void)memcpy(novo.agenda[i].bits, &p[1], AGENDA_BYTES);
  }
  (void)agenda_aplicador_reavalia(&novo);

  *ap = novo;
  *fuso_min = (int16_t)((uint16_t)carga[2] | ((uint16_t)carga[3] << 8U));
  return AGENDA_CONFIG_OK;
}
