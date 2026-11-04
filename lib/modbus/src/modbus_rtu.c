/**
 * @file modbus_rtu.c
 * @brief Implementação do escravo Modbus RTU.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "modbus_rtu.h"

#include <string.h>

/** Funções do Modbus. */
#define FN_LER_BOBINAS      0x01U
#define FN_LER_DISCRETAS    0x02U
#define FN_LER_RETENCAO     0x03U
#define FN_LER_ENTRADA      0x04U
#define FN_ESCREVER_BOBINA  0x05U
#define FN_ESCREVER_REG     0x06U
#define FN_ESCREVER_BOBINAS 0x0FU
#define FN_ESCREVER_REGS    0x10U

/** Tamanho mínimo de um quadro: endereço, função e CRC. */
#define QUADRO_MIN 4U

uint16_t modbus_crc16(const uint8_t *dados, size_t tamanho)
{
  uint16_t crc = 0xFFFFU;
  size_t i;
  uint8_t bit;

  for (i = 0U; i < tamanho; i++)
  {
    crc ^= dados[i];
    for (bit = 0U; bit < 8U; bit++)
    {
      crc = ((crc & 1U) != 0U) ? (uint16_t)((crc >> 1U) ^ 0xA001U) : (uint16_t)(crc >> 1U);
    }
  }
  return crc;
}

/** Lê um inteiro de 16 bits big-endian. */
static uint16_t le16(const uint8_t *p)
{
  return (uint16_t)(((uint16_t)p[0] << 8U) | p[1]);
}

/** Escreve um inteiro de 16 bits big-endian. */
static void grava16(uint8_t *p, uint16_t v)
{
  p[0] = (uint8_t)(v >> 8U);
  p[1] = (uint8_t)(v & 0xFFU);
}

/**
 * @brief Fecha uma resposta acrescentando o CRC.
 *
 * @param resposta Quadro sem o CRC.
 * @param tamanho Tamanho sem o CRC.
 * @return Tamanho com o CRC.
 */
static size_t fecha(uint8_t *resposta, size_t tamanho)
{
  uint16_t crc = modbus_crc16(resposta, tamanho);

  resposta[tamanho] = (uint8_t)(crc & 0xFFU);
  resposta[tamanho + 1U] = (uint8_t)(crc >> 8U);
  return tamanho + 2U;
}

/**
 * @brief Monta uma resposta de exceção.
 *
 * @param resposta Quadro de resposta (o endereço já está no byte 0).
 * @param funcao Função pedida.
 * @param codigo Código da exceção.
 * @return Tamanho da resposta.
 */
static size_t excecao(uint8_t *resposta, uint8_t funcao, int32_t codigo)
{
  resposta[1] = (uint8_t)(funcao | 0x80U);
  resposta[2] = (uint8_t)codigo;
  return fecha(resposta, 3U);
}

/** Empacota bobinas em bytes, a primeira no bit 0 do primeiro byte. */
static void empacota_bits(const bool *bits, uint16_t quantidade, uint8_t *bytes)
{
  uint16_t i;

  (void)memset(bytes, 0, (size_t)((quantidade + 7U) / 8U));
  for (i = 0U; i < quantidade; i++)
  {
    if (bits[i])
    {
      bytes[i / 8U] = (uint8_t)(bytes[i / 8U] | (uint8_t)(1U << (i % 8U)));
    }
  }
}

/**
 * @brief Funções 01 e 02: leitura de bits.
 *
 * @param ops Operações.
 * @param p Pedido (função, endereço e quantidade).
 * @param resposta Quadro de resposta.
 * @return Tamanho da resposta.
 */
static size_t le_bits(const modbus_ops_t *ops, const uint8_t *p, uint8_t *resposta)
{
  uint16_t endereco = le16(&p[2]);
  uint16_t quantidade = le16(&p[4]);
  bool bits[MODBUS_LER_BIT_MAX];
  int32_t (*op)(void *, uint16_t, uint16_t, bool *) = (p[1] == FN_LER_BOBINAS) ? ops->le_bobinas : ops->le_discretas;
  int32_t r;

  if (op == NULL)
  {
    return excecao(resposta, p[1], MODBUS_EXC_FUNCAO);
  }
  if ((quantidade == 0U) || (quantidade > MODBUS_LER_BIT_MAX) ||
      (((size_t)endereco + quantidade) > 0x10000UL))
  {
    return excecao(resposta, p[1], MODBUS_EXC_VALOR);
  }
  r = op(ops->contexto, endereco, quantidade, bits);
  if (r != MODBUS_OK)
  {
    return excecao(resposta, p[1], r);
  }
  resposta[1] = p[1];
  resposta[2] = (uint8_t)((quantidade + 7U) / 8U);
  empacota_bits(bits, quantidade, &resposta[3]);
  return fecha(resposta, 3U + resposta[2]);
}

/**
 * @brief Funções 03 e 04: leitura de registradores.
 *
 * @param ops Operações.
 * @param p Pedido.
 * @param resposta Quadro de resposta.
 * @return Tamanho da resposta.
 */
static size_t le_registradores(const modbus_ops_t *ops, const uint8_t *p, uint8_t *resposta)
{
  uint16_t endereco = le16(&p[2]);
  uint16_t quantidade = le16(&p[4]);
  uint16_t regs[MODBUS_LER_REG_MAX];
  int32_t (*op)(void *, uint16_t, uint16_t, uint16_t *) = (p[1] == FN_LER_RETENCAO) ? ops->le_retencao : ops->le_entrada;
  int32_t r;
  uint16_t i;

  if (op == NULL)
  {
    return excecao(resposta, p[1], MODBUS_EXC_FUNCAO);
  }
  if ((quantidade == 0U) || (quantidade > MODBUS_LER_REG_MAX) ||
      (((size_t)endereco + quantidade) > 0x10000UL))
  {
    return excecao(resposta, p[1], MODBUS_EXC_VALOR);
  }
  r = op(ops->contexto, endereco, quantidade, regs);
  if (r != MODBUS_OK)
  {
    return excecao(resposta, p[1], r);
  }
  resposta[1] = p[1];
  resposta[2] = (uint8_t)(quantidade * 2U);
  for (i = 0U; i < quantidade; i++)
  {
    grava16(&resposta[3U + ((size_t)i * 2U)], regs[i]);
  }
  return fecha(resposta, 3U + ((size_t)quantidade * 2U));
}

/**
 * @brief Função 05: escreve uma bobina (0xFF00 liga, 0x0000 desliga).
 *
 * @param ops Operações.
 * @param p Pedido.
 * @param resposta Quadro de resposta.
 * @return Tamanho da resposta.
 */
static size_t escreve_bobina(const modbus_ops_t *ops, const uint8_t *p, uint8_t *resposta)
{
  uint16_t valor = le16(&p[4]);
  bool bit = (valor == 0xFF00U);
  int32_t r;

  if (ops->escreve_bobinas == NULL)
  {
    return excecao(resposta, p[1], MODBUS_EXC_FUNCAO);
  }
  if ((valor != 0xFF00U) && (valor != 0x0000U))
  {
    return excecao(resposta, p[1], MODBUS_EXC_VALOR);
  }
  r = ops->escreve_bobinas(ops->contexto, le16(&p[2]), 1U, &bit);
  if (r != MODBUS_OK)
  {
    return excecao(resposta, p[1], r);
  }
  (void)memcpy(&resposta[1], &p[1], 5U);
  return fecha(resposta, 6U);
}

/**
 * @brief Função 06: escreve um registrador de retenção.
 *
 * @param ops Operações.
 * @param p Pedido.
 * @param resposta Quadro de resposta.
 * @return Tamanho da resposta.
 */
static size_t escreve_registrador(const modbus_ops_t *ops, const uint8_t *p, uint8_t *resposta)
{
  uint16_t valor = le16(&p[4]);
  int32_t r;

  if (ops->escreve_retencao == NULL)
  {
    return excecao(resposta, p[1], MODBUS_EXC_FUNCAO);
  }
  r = ops->escreve_retencao(ops->contexto, le16(&p[2]), 1U, &valor);
  if (r != MODBUS_OK)
  {
    return excecao(resposta, p[1], r);
  }
  (void)memcpy(&resposta[1], &p[1], 5U);
  return fecha(resposta, 6U);
}

/**
 * @brief Função 15: escreve várias bobinas.
 *
 * @param ops Operações.
 * @param p Pedido.
 * @param n Tamanho do pedido, com o CRC.
 * @param resposta Quadro de resposta.
 * @return Tamanho da resposta.
 */
static size_t escreve_bobinas(const modbus_ops_t *ops, const uint8_t *p, size_t n, uint8_t *resposta)
{
  uint16_t endereco = le16(&p[2]);
  uint16_t quantidade = le16(&p[4]);
  bool bits[MODBUS_ESCREVER_BIT_MAX];
  uint16_t i;
  int32_t r;

  if (ops->escreve_bobinas == NULL)
  {
    return excecao(resposta, p[1], MODBUS_EXC_FUNCAO);
  }
  if ((n < 9U) || (quantidade == 0U) || (quantidade > MODBUS_ESCREVER_BIT_MAX) ||
      (p[6] != (uint8_t)((quantidade + 7U) / 8U)) || (n != (9U + p[6])) ||
      (((size_t)endereco + quantidade) > 0x10000UL))
  {
    return excecao(resposta, p[1], MODBUS_EXC_VALOR);
  }
  for (i = 0U; i < quantidade; i++)
  {
    bits[i] = ((p[7U + (i / 8U)] >> (i % 8U)) & 1U) != 0U;
  }
  r = ops->escreve_bobinas(ops->contexto, endereco, quantidade, bits);
  if (r != MODBUS_OK)
  {
    return excecao(resposta, p[1], r);
  }
  (void)memcpy(&resposta[1], &p[1], 5U);
  return fecha(resposta, 6U);
}

/**
 * @brief Função 16: escreve vários registradores de retenção.
 *
 * @param ops Operações.
 * @param p Pedido.
 * @param n Tamanho do pedido, com o CRC.
 * @param resposta Quadro de resposta.
 * @return Tamanho da resposta.
 */
static size_t escreve_registradores(const modbus_ops_t *ops, const uint8_t *p, size_t n, uint8_t *resposta)
{
  uint16_t endereco = le16(&p[2]);
  uint16_t quantidade = le16(&p[4]);
  uint16_t regs[MODBUS_ESCREVER_REG_MAX];
  uint16_t i;
  int32_t r;

  if (ops->escreve_retencao == NULL)
  {
    return excecao(resposta, p[1], MODBUS_EXC_FUNCAO);
  }
  if ((n < 9U) || (quantidade == 0U) || (quantidade > MODBUS_ESCREVER_REG_MAX) ||
      (p[6] != (uint8_t)(quantidade * 2U)) || (n != (9U + p[6])) ||
      (((size_t)endereco + quantidade) > 0x10000UL))
  {
    return excecao(resposta, p[1], MODBUS_EXC_VALOR);
  }
  for (i = 0U; i < quantidade; i++)
  {
    regs[i] = le16(&p[7U + ((size_t)i * 2U)]);
  }
  r = ops->escreve_retencao(ops->contexto, endereco, quantidade, regs);
  if (r != MODBUS_OK)
  {
    return excecao(resposta, p[1], r);
  }
  (void)memcpy(&resposta[1], &p[1], 5U);
  return fecha(resposta, 6U);
}

/**
 * @brief Executa a função do pedido.
 *
 * @param ops Operações.
 * @param p Pedido.
 * @param n Tamanho do pedido, com o CRC.
 * @param resposta Quadro de resposta.
 * @return Tamanho da resposta.
 */
static size_t executa(const modbus_ops_t *ops, const uint8_t *p, size_t n, uint8_t *resposta)
{
  /* Os pedidos de leitura e de escrita simples têm função, endereço e quantidade ou valor: 8 bytes. */
  bool simples = (n == 8U);

  switch (p[1])
  {
    case FN_LER_BOBINAS:
    case FN_LER_DISCRETAS:
      return simples ? le_bits(ops, p, resposta) : excecao(resposta, p[1], MODBUS_EXC_VALOR);
    case FN_LER_RETENCAO:
    case FN_LER_ENTRADA:
      return simples ? le_registradores(ops, p, resposta) : excecao(resposta, p[1], MODBUS_EXC_VALOR);
    case FN_ESCREVER_BOBINA:
      return simples ? escreve_bobina(ops, p, resposta) : excecao(resposta, p[1], MODBUS_EXC_VALOR);
    case FN_ESCREVER_REG:
      return simples ? escreve_registrador(ops, p, resposta) : excecao(resposta, p[1], MODBUS_EXC_VALOR);
    case FN_ESCREVER_BOBINAS:
      return escreve_bobinas(ops, p, n, resposta);
    case FN_ESCREVER_REGS:
      return escreve_registradores(ops, p, n, resposta);
    default:
      return excecao(resposta, p[1], MODBUS_EXC_FUNCAO);
  }
}

size_t modbus_rtu_processa(const modbus_ops_t *ops, uint8_t escravo, const uint8_t *pedido, size_t tamanho,
                           uint8_t *resposta)
{
  size_t r;
  bool difusao;
  uint8_t funcao;

  if ((ops == NULL) || (pedido == NULL) || (resposta == NULL) || (tamanho < QUADRO_MIN) ||
      (tamanho > MODBUS_QUADRO_MAX))
  {
    return 0U;
  }
  if (modbus_crc16(pedido, tamanho) != 0U)
  {
    return 0U; /* O CRC de um quadro íntegro, calculado sobre o CRC, dá zero. */
  }
  difusao = (pedido[0] == 0U);
  if (!difusao && (pedido[0] != escravo))
  {
    return 0U;
  }
  funcao = pedido[1];
  if (difusao && (funcao != FN_ESCREVER_BOBINA) && (funcao != FN_ESCREVER_REG) &&
      (funcao != FN_ESCREVER_BOBINAS) && (funcao != FN_ESCREVER_REGS))
  {
    return 0U; /* Difusão só vale para escrita. */
  }
  resposta[0] = pedido[0];
  r = executa(ops, pedido, tamanho, resposta);
  return difusao ? 0U : r;
}
