/**
 * @file armazem.c
 * @brief Implementação do armazém de configuração em dois espaços de flash.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "armazem.h"

#include <string.h>

/** Marca de um registro: "OLOG" em little-endian. */
#define MARCA 0x474F4C4FUL
/** Polinômio do CRC-32 (refletido). */
#define POLINOMIO 0xEDB88320UL

/**
 * @brief CRC-32 por bits, sem tabela (poucos bytes de flash, e a carga é pequena).
 *
 * @param crc Valor inicial (0xFFFFFFFF no começo).
 * @param dados Bytes.
 * @param tamanho Número de bytes.
 * @return CRC acumulado, sem a inversão final.
 */
static uint32_t crc32_acumula(uint32_t crc, const uint8_t *dados, size_t tamanho)
{
  size_t i;
  uint8_t bit;

  for (i = 0U; i < tamanho; i++)
  {
    crc ^= dados[i];
    for (bit = 0U; bit < 8U; bit++)
    {
      crc = ((crc & 1U) != 0U) ? ((crc >> 1U) ^ POLINOMIO) : (crc >> 1U);
    }
  }
  return crc;
}

/** Lê uma palavra de 32 bits little-endian. */
static uint32_t le32(const uint8_t *p)
{
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8U) | ((uint32_t)p[2] << 16U) | ((uint32_t)p[3] << 24U);
}

/** Escreve uma palavra de 32 bits little-endian. */
static void grava32(uint8_t *p, uint32_t v)
{
  p[0] = (uint8_t)v;
  p[1] = (uint8_t)(v >> 8U);
  p[2] = (uint8_t)(v >> 16U);
  p[3] = (uint8_t)(v >> 24U);
}

/**
 * @brief CRC de um registro: sequência, tamanho e carga.
 *
 * @param registro Início do registro.
 * @param tamanho Tamanho da carga.
 * @return CRC-32 do registro.
 */
static uint32_t crc_do_registro(const uint8_t *registro, uint32_t tamanho)
{
  uint32_t crc = crc32_acumula(0xFFFFFFFFUL, &registro[4], 8U);

  crc = crc32_acumula(crc, &registro[ARMAZEM_CABECALHO], tamanho);
  return ~crc;
}

/**
 * @brief Confere se o espaço tem um registro válido.
 *
 * @param armazem Armazém.
 * @param espaco Espaço a conferir.
 * @param sequencia Recebe a sequência do registro, se válido.
 * @return true se há marca, tamanho coerente e CRC certo.
 */
static bool registro_valido(const armazem_t *armazem, uint8_t espaco, uint32_t *sequencia)
{
  const uint8_t *r = armazem->ops->endereco(espaco);
  uint32_t tamanho;

  if ((r == NULL) || (le32(r) != MARCA))
  {
    return false;
  }
  tamanho = le32(&r[8]);
  if ((tamanho == 0U) || (tamanho > ARMAZEM_CARGA_MAX) ||
      ((tamanho + ARMAZEM_CABECALHO) > armazem->ops->tamanho_espaco))
  {
    return false;
  }
  if (le32(&r[12]) != crc_do_registro(r, tamanho))
  {
    return false;
  }
  *sequencia = le32(&r[4]);
  return true;
}

/**
 * @brief Procura o registro válido mais recente nos dois espaços.
 *
 * @param armazem Armazém; atualiza `sequencia` e `ultimo`.
 */
static void procura_mais_recente(armazem_t *armazem)
{
  uint8_t espaco;
  uint32_t sequencia = 0U;

  armazem->ultimo = -1;
  armazem->sequencia = 0U;
  for (espaco = 0U; espaco < ARMAZEM_ESPACOS; espaco++)
  {
    if (registro_valido(armazem, espaco, &sequencia) &&
        ((armazem->ultimo < 0) || (sequencia > armazem->sequencia)))
    {
      armazem->ultimo = (int8_t)espaco;
      armazem->sequencia = sequencia;
    }
  }
}

int32_t armazem_inicia(armazem_t *armazem, const armazem_ops_t *ops)
{
  if ((armazem == NULL) || (ops == NULL) || (ops->endereco == NULL) || (ops->apaga == NULL) ||
      (ops->grava == NULL) || (ops->tamanho_espaco < (ARMAZEM_CABECALHO + ARMAZEM_CARGA_MAX)))
  {
    return ARMAZEM_ERRO_PARAMETRO;
  }
  armazem->ops = ops;
  procura_mais_recente(armazem);
  return ARMAZEM_OK;
}

int32_t armazem_le(const armazem_t *armazem, uint8_t *carga, size_t capacidade, size_t *tamanho)
{
  const uint8_t *r;
  uint32_t n;

  if ((armazem == NULL) || (armazem->ops == NULL) || (carga == NULL) || (tamanho == NULL))
  {
    return ARMAZEM_ERRO_PARAMETRO;
  }
  if (armazem->ultimo < 0)
  {
    return ARMAZEM_ERRO_VAZIO;
  }
  r = armazem->ops->endereco((uint8_t)armazem->ultimo);
  n = le32(&r[8]);
  if (n > capacidade)
  {
    return ARMAZEM_ERRO_TAMANHO;
  }
  (void)memcpy(carga, &r[ARMAZEM_CABECALHO], n);
  *tamanho = n;
  return ARMAZEM_OK;
}

int32_t armazem_grava(armazem_t *armazem, const uint8_t *carga, size_t tamanho)
{
  uint8_t registro[ARMAZEM_CABECALHO + ARMAZEM_CARGA_MAX];
  size_t total;
  uint8_t destino;
  uint32_t sequencia = armazem->sequencia + 1U;
  uint32_t lida = 0U;

  if ((armazem == NULL) || (armazem->ops == NULL) || (carga == NULL))
  {
    return ARMAZEM_ERRO_PARAMETRO;
  }
  if ((tamanho == 0U) || (tamanho > ARMAZEM_CARGA_MAX))
  {
    return ARMAZEM_ERRO_TAMANHO;
  }

  /* Monta o registro, completando com 0xFF até o alinhamento da gravação. */
  total = ARMAZEM_CABECALHO + tamanho;
  total = ((total + ARMAZEM_ALINHAMENTO - 1U) / ARMAZEM_ALINHAMENTO) * ARMAZEM_ALINHAMENTO;
  (void)memset(registro, 0xFF, total);
  grava32(&registro[0], MARCA);
  grava32(&registro[4], sequencia);
  grava32(&registro[8], (uint32_t)tamanho);
  (void)memcpy(&registro[ARMAZEM_CABECALHO], carga, tamanho);
  grava32(&registro[12], crc_do_registro(registro, (uint32_t)tamanho));

  destino = (armazem->ultimo == 0) ? 1U : 0U;
  if ((armazem->ops->apaga(destino) != 0) || (armazem->ops->grava(destino, registro, total) != 0))
  {
    return ARMAZEM_ERRO_FLASH;
  }
  if (!registro_valido(armazem, destino, &lida) || (lida != sequencia))
  {
    return ARMAZEM_ERRO_FLASH;
  }
  armazem->ultimo = (int8_t)destino;
  armazem->sequencia = sequencia;
  return ARMAZEM_OK;
}
