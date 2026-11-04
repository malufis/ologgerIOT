/**
 * @file config.c
 * @brief Implementação da tabela de parâmetros de configuração do device.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "config.h"

#include <math.h>
#include <string.h>

/** Descrição de um parâmetro no mapa Modbus. */
typedef struct
{
  uint16_t base;       /**< Endereço do primeiro registrador da instância 0 (5 dígitos). */
  uint8_t instancias;  /**< Quantidade de entradas ou saídas instaladas. */
  bool flutuante;      /**< true se for float de 2 registradores. */
  float minimo;        /**< Menor valor aceito. */
  float maximo;        /**< Maior valor aceito. */
  float padrao;        /**< Valor de fábrica. */
} param_t;

/* Na mesma ordem de config_id_t. */
static const param_t tabela[CFG_QTD] = {
  /* CFG_ENT_SPAN */ {40001U, 4U, true, -1.0e9F, 1.0e9F, 100.0F},
  /* CFG_ENT_ZERO */ {40031U, 4U, true, -1.0e9F, 1.0e9F, 0.0F},
  /* CFG_ENT_OFFSET */ {40061U, 4U, true, -100.0F, 100.0F, 0.0F},
  /* CFG_ENT_MODO_DIG */ {40151U, 4U, false, 0.0F, 1.0F, 0.0F},
  /* CFG_SAI_TIPO */ {40166U, 4U, false, 0.0F, 1.0F, 0.0F},
  /* CFG_SAI_F4MA */ {40181U, 4U, true, 0.0F, 20000.0F, 0.0F},
  /* CFG_SAI_F20MA */ {40211U, 4U, true, 0.0F, 20000.0F, 0.0F},
  /* CFG_SAI_ENTRADA */ {40241U, 4U, false, 0.0F, 4.0F, 0.0F},
  /* CFG_SAI_RETARDO */ {40256U, 4U, false, 0.0F, 65535.0F, 0.0F},
  /* CFG_SAI_LIM_MAX */ {40307U, 4U, true, -1.0e9F, 1.0e9F, 0.0F},
  /* CFG_SAI_LIM_MIN */ {40337U, 4U, true, -1.0e9F, 1.0e9F, 0.0F},
  /* CFG_SAI_SENTIDO */ {40367U, 4U, false, 0.0F, 1.0F, 0.0F},
  /* CFG_SAI_ESTAB */ {40382U, 4U, false, 0.0F, 65535.0F, 300.0F},
  /* CFG_VEL_RS485 */ {40400U, 1U, false, 0.0F, 3.0F, 1.0F},
  /* CFG_MODO_RS485 */ {40401U, 1U, false, 0.0F, 1.0F, 1.0F},
  /* CFG_ENDERECO_ESCRAVO */ {40402U, 1U, false, 1.0F, 247.0F, 1.0F},
  /* CFG_FUSO */ {40403U, 1U, false, -720.0F, 840.0F, -180.0F},
  /* CFG_INTERVALO_COLETA */ {40410U, 1U, false, 1.0F, 65535.0F, 60.0F},
  /* CFG_INTERVALO_ENVIO */ {40411U, 1U, false, 1.0F, 65535.0F, 60.0F},
  /* CFG_MODO_OPERACAO */ {40412U, 1U, false, 0.0F, 1.0F, 0.0F},
  /* CFG_INATIVIDADE */ {40413U, 1U, false, 1.0F, 65535.0F, 60.0F},
};

/** Resultado da busca de um registrador na tabela. */
typedef struct
{
  const param_t *param; /**< Parâmetro dono do registrador. */
  uint16_t indice;      /**< Posição da instância no vetor de valores. */
  uint8_t palavra;      /**< 0 = primeira palavra (ou única), 1 = segunda palavra do float. */
} achado_t;

/**
 * @brief Posição, no vetor de valores, da primeira instância de um parâmetro.
 *
 * @param id Parâmetro.
 * @return Índice da instância 0.
 */
static uint16_t primeiro_indice(config_id_t id)
{
  uint16_t soma = 0U;
  uint8_t i;

  for (i = 0U; i < (uint8_t)id; i++)
  {
    soma = (uint16_t)(soma + tabela[i].instancias);
  }
  return soma;
}

/**
 * @brief Procura o parâmetro dono de um registrador.
 *
 * @param endereco Endereço do registrador (5 dígitos).
 * @param achado Recebe o parâmetro, a posição e a palavra.
 * @return true se o endereço é de configuração.
 */
static bool procura(uint16_t endereco, achado_t *achado)
{
  uint8_t id;
  uint8_t i;
  uint16_t largura;
  uint16_t inicio;

  for (id = 0U; id < (uint8_t)CFG_QTD; id++)
  {
    largura = tabela[id].flutuante ? 2U : 1U;
    for (i = 0U; i < tabela[id].instancias; i++)
    {
      inicio = (uint16_t)(tabela[id].base + (i * largura));
      if ((endereco >= inicio) && (endereco < (uint16_t)(inicio + largura)))
      {
        achado->param = &tabela[id];
        achado->indice = (uint16_t)(primeiro_indice((config_id_t)id) + i);
        achado->palavra = (uint8_t)(endereco - inicio);
        return true;
      }
    }
  }
  return false;
}

/** Converte um float nos seus 32 bits. */
static uint32_t bits_do_float(float f)
{
  uint32_t b;

  (void)memcpy(&b, &f, sizeof(b));
  return b;
}

/** Converte 32 bits em float. */
static float float_dos_bits(uint32_t b)
{
  float f;

  (void)memcpy(&f, &b, sizeof(f));
  return f;
}

/**
 * @brief Valor de fábrica de um parâmetro, nos 32 bits guardados.
 *
 * @param p Parâmetro.
 * @return Valor de fábrica.
 */
static uint32_t bits_do_padrao(const param_t *p)
{
  return p->flutuante ? bits_do_float(p->padrao) : (uint32_t)(int32_t)p->padrao;
}

/**
 * @brief Diz se os 32 bits guardados são um valor aceito pelo parâmetro.
 *
 * @param p Parâmetro.
 * @param bits Valor guardado.
 * @return true se estiver nos limites (e, para float, for finito).
 */
static bool bits_validos(const param_t *p, uint32_t bits)
{
  float v;

  if (p->flutuante)
  {
    v = float_dos_bits(bits);
    return (isfinite(v) != 0) && (v >= p->minimo) && (v <= p->maximo);
  }
  v = (float)(int32_t)bits;
  return (v >= p->minimo) && (v <= p->maximo);
}

/**
 * @brief Interpreta o valor de um registrador de 16 bits de um parâmetro inteiro.
 *
 * @param p Parâmetro.
 * @param registrador Registrador recebido.
 * @return Valor guardado: com sinal se o mínimo do parâmetro for negativo.
 */
static uint32_t bits_do_inteiro(const param_t *p, uint16_t registrador)
{
  return (p->minimo < 0.0F) ? (uint32_t)(int32_t)(int16_t)registrador : (uint32_t)registrador;
}

/**
 * @brief Percorre um pedido de escrita, validando ou aplicando.
 *
 * @param cfg Configuração (alterada só se @p aplica for true).
 * @param endereco Primeiro endereço.
 * @param quantidade Quantidade de registradores.
 * @param registradores Valores.
 * @param aplica true para gravar; false só valida.
 * @return CONFIG_OK, CONFIG_ERRO_ENDERECO ou CONFIG_ERRO_VALOR.
 */
static int32_t percorre_escrita(config_t *cfg, uint16_t endereco, uint16_t quantidade,
                                const uint16_t *registradores, bool aplica)
{
  uint16_t k = 0U;
  achado_t a;
  uint32_t bits;

  while (k < quantidade)
  {
    if (!procura((uint16_t)(endereco + k), &a))
    {
      return CONFIG_ERRO_ENDERECO;
    }
    if (a.param->flutuante)
    {
      if ((a.palavra != 0U) || ((uint16_t)(k + 1U) >= quantidade))
      {
        return CONFIG_ERRO_ENDERECO; /* Float gravado pela metade. */
      }
      bits = ((uint32_t)registradores[k] << 16U) | (uint32_t)registradores[k + 1U];
      k = (uint16_t)(k + 2U);
    }
    else
    {
      bits = bits_do_inteiro(a.param, registradores[k]);
      k = (uint16_t)(k + 1U);
    }
    if (!bits_validos(a.param, bits))
    {
      return CONFIG_ERRO_VALOR;
    }
    if (aplica)
    {
      cfg->valor[a.indice] = bits;
    }
  }
  return CONFIG_OK;
}

int32_t config_padroes(config_t *cfg)
{
  uint8_t id;
  uint8_t i;
  uint16_t base;

  if (cfg == NULL)
  {
    return CONFIG_ERRO_PARAMETRO;
  }
  for (id = 0U; id < (uint8_t)CFG_QTD; id++)
  {
    base = primeiro_indice((config_id_t)id);
    for (i = 0U; i < tabela[id].instancias; i++)
    {
      cfg->valor[base + i] = bits_do_padrao(&tabela[id]);
    }
  }
  return CONFIG_OK;
}

int32_t config_le_registradores(const config_t *cfg, uint16_t endereco, uint16_t quantidade,
                                uint16_t *registradores)
{
  uint16_t k;
  achado_t a;
  uint32_t bits;

  if ((cfg == NULL) || (registradores == NULL) || (quantidade == 0U))
  {
    return CONFIG_ERRO_PARAMETRO;
  }
  for (k = 0U; k < quantidade; k++)
  {
    if (!procura((uint16_t)(endereco + k), &a))
    {
      return CONFIG_ERRO_ENDERECO;
    }
    bits = cfg->valor[a.indice];
    if (a.param->flutuante)
    {
      registradores[k] = (a.palavra == 0U) ? (uint16_t)(bits >> 16U) : (uint16_t)(bits & 0xFFFFU);
    }
    else
    {
      registradores[k] = (uint16_t)(bits & 0xFFFFU);
    }
  }
  return CONFIG_OK;
}

bool config_conflito(const config_t *cfg)
{
  uint8_t saida;
  bool alguma_em_frequencia = false;

  if (cfg == NULL)
  {
    return false;
  }
  for (saida = 0U; saida < CONFIG_QTD_SAIDAS; saida++)
  {
    if (config_inteiro(cfg, CFG_SAI_TIPO, saida) == 1)
    {
      alguma_em_frequencia = true;
      if (saida == 0U)
      {
        return true; /* A saída 1 não tem canal de timer. */
      }
    }
  }
  return alguma_em_frequencia && (config_inteiro(cfg, CFG_MODO_OPERACAO, 0U) == 1);
}

bool config_mudou(const config_t *a, const config_t *b, config_id_t id, uint8_t instancia)
{
  if ((a == NULL) || (b == NULL) || ((uint8_t)id >= (uint8_t)CFG_QTD) ||
      (instancia >= tabela[id].instancias))
  {
    return false;
  }
  return a->valor[primeiro_indice(id) + instancia] != b->valor[primeiro_indice(id) + instancia];
}

int32_t config_escreve_registradores(config_t *cfg, uint16_t endereco, uint16_t quantidade,
                                     const uint16_t *registradores)
{
  config_t nova;
  int32_t resultado;

  if ((cfg == NULL) || (registradores == NULL) || (quantidade == 0U))
  {
    return CONFIG_ERRO_PARAMETRO;
  }
  resultado = percorre_escrita(cfg, endereco, quantidade, registradores, false);
  if (resultado != CONFIG_OK)
  {
    return resultado;
  }
  nova = *cfg;
  (void)percorre_escrita(&nova, endereco, quantidade, registradores, true);
  if (config_conflito(&nova))
  {
    return CONFIG_ERRO_CONFLITO;
  }
  *cfg = nova;
  return CONFIG_OK;
}

int32_t config_inteiro(const config_t *cfg, config_id_t id, uint8_t instancia)
{
  if ((cfg == NULL) || ((uint8_t)id >= (uint8_t)CFG_QTD) || tabela[id].flutuante ||
      (instancia >= tabela[id].instancias))
  {
    return 0;
  }
  return (int32_t)cfg->valor[primeiro_indice(id) + instancia];
}

float config_float(const config_t *cfg, config_id_t id, uint8_t instancia)
{
  if ((cfg == NULL) || ((uint8_t)id >= (uint8_t)CFG_QTD) || !tabela[id].flutuante ||
      (instancia >= tabela[id].instancias))
  {
    return 0.0F;
  }
  return float_dos_bits(cfg->valor[primeiro_indice(id) + instancia]);
}

uint8_t config_instancias(config_id_t id)
{
  return ((uint8_t)id < (uint8_t)CFG_QTD) ? tabela[id].instancias : 0U;
}

uint16_t config_indice(config_id_t id, uint8_t instancia)
{
  if (((uint8_t)id >= (uint8_t)CFG_QTD) || (instancia >= tabela[id].instancias))
  {
    return (uint16_t)CONFIG_VALORES;
  }
  return (uint16_t)(primeiro_indice(id) + instancia);
}

bool config_bits_validos(config_id_t id, uint32_t bits)
{
  return ((uint8_t)id < (uint8_t)CFG_QTD) && bits_validos(&tabela[id], bits);
}
