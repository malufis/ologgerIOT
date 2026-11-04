/**
 * @file config_carga.c
 * @brief Empacotamento e leitura da configuração gravada (formato da carga do armazém).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "config.h"

/** Lê uma palavra de 32 bits little-endian. */
static uint32_t le32(const uint8_t *p)
{
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8U) | ((uint32_t)p[2] << 16U) | ((uint32_t)p[3] << 24U);
}

int32_t config_empacota(const config_t *cfg, uint8_t *carga, size_t capacidade, size_t *tamanho)
{
  size_t i;
  uint32_t v;

  if ((cfg == NULL) || (carga == NULL) || (tamanho == NULL))
  {
    return CONFIG_ERRO_PARAMETRO;
  }
  if (capacidade < CONFIG_TAMANHO_CARGA)
  {
    return CONFIG_ERRO_TAMANHO;
  }
  carga[0] = (uint8_t)(CONFIG_VERSAO & 0xFFU);
  carga[1] = (uint8_t)(CONFIG_VERSAO >> 8U);
  carga[2] = (uint8_t)(CONFIG_VALORES & 0xFFU);
  carga[3] = (uint8_t)(CONFIG_VALORES >> 8U);
  for (i = 0U; i < CONFIG_VALORES; i++)
  {
    v = cfg->valor[i];
    carga[4U + (i * 4U)] = (uint8_t)v;
    carga[5U + (i * 4U)] = (uint8_t)(v >> 8U);
    carga[6U + (i * 4U)] = (uint8_t)(v >> 16U);
    carga[7U + (i * 4U)] = (uint8_t)(v >> 24U);
  }
  *tamanho = CONFIG_TAMANHO_CARGA;
  return CONFIG_OK;
}

/**
 * @brief Carrega os valores gravados que são válidos; os demais ficam como estão.
 *
 * @param nova Configuração a completar (já com os padrões).
 * @param carga Carga gravada.
 * @param quantidade Quantidade de valores gravados.
 */
static void carrega_valores(config_t *nova, const uint8_t *carga, uint16_t quantidade)
{
  uint8_t id;
  uint8_t instancia;
  uint16_t i;
  uint32_t bits;

  for (id = 0U; id < (uint8_t)CFG_QTD; id++)
  {
    for (instancia = 0U; instancia < config_instancias((config_id_t)id); instancia++)
    {
      i = config_indice((config_id_t)id, instancia);
      bits = le32(&carga[4U + ((size_t)i * 4U)]);
      if ((i < quantidade) && config_bits_validos((config_id_t)id, bits))
      {
        nova->valor[i] = bits;
      }
    }
  }
}

/**
 * @brief Desfaz um conflito da gravação: o sleep perde; se persistir, descarta tudo.
 *
 * @param nova Configuração a corrigir.
 */
static void resolve_conflito(config_t *nova)
{
  if (!config_conflito(nova))
  {
    return;
  }
  nova->valor[config_indice(CFG_MODO_OPERACAO, 0U)] = 0U; /* Desligado é o estado seguro. */
  if (config_conflito(nova))
  {
    (void)config_padroes(nova); /* Saída 1 em frequência: descarta a gravação inteira. */
  }
}

int32_t config_desempacota(config_t *cfg, const uint8_t *carga, size_t tamanho)
{
  config_t nova;
  uint16_t versao;
  uint16_t quantidade;

  if ((cfg == NULL) || (carga == NULL) || (tamanho < 4U))
  {
    return CONFIG_ERRO_PARAMETRO;
  }
  versao = (uint16_t)((uint16_t)carga[0] | ((uint16_t)carga[1] << 8U));
  quantidade = (uint16_t)((uint16_t)carga[2] | ((uint16_t)carga[3] << 8U));
  if (versao != CONFIG_VERSAO)
  {
    return CONFIG_ERRO_VERSAO;
  }
  if (tamanho != (4U + ((size_t)quantidade * 4U)))
  {
    return CONFIG_ERRO_TAMANHO;
  }

  /* Parte dos padrões; cada valor gravado e válido os substitui. */
  (void)config_padroes(&nova);
  carrega_valores(&nova, carga, quantidade);
  resolve_conflito(&nova);
  *cfg = nova;
  return CONFIG_OK;
}
