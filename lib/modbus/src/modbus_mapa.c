/**
 * @file modbus_mapa.c
 * @brief Implementação do mapa Modbus do OloggerMulti.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "modbus_mapa.h"

#include <string.h>

#include "config.h"
#include "config_tarefa.h"
#include "io_entrada.h"
#include "io_frequencia.h"
#include "io_saidas.h"
#include "regras_tarefa.h"
#include "rtc.h"
#include "tempo.h"

/** Bases das tabelas (número do mapa do endereço de protocolo 0). */
#define BASE_BOBINAS   1UL
#define BASE_DISCRETAS 10001UL
#define BASE_ENTRADA   30001UL
#define BASE_RETENCAO  40001UL
/** Faixa dos registradores de retenção que são estado (modo manual ou automático das saídas). */
#define MODO_PRIMEIRO  40286UL
#define MODO_ULTIMO    40300UL
/** Quantidade de entradas analógicas e de saídas com endereço. */
#define N_ENTRADAS     4UL
#define N_SAIDAS       4UL

/** Traduz um erro de config.h em exceção Modbus. */
static int32_t excecao_de_config(int32_t erro)
{
  if (erro == CONFIG_OK)
  {
    return MODBUS_OK;
  }
  if (erro == CONFIG_ERRO_ENDERECO)
  {
    return MODBUS_EXC_ENDERECO;
  }
  return ((erro == CONFIG_ERRO_VALOR) || (erro == CONFIG_ERRO_CONFLITO)) ? MODBUS_EXC_VALOR : MODBUS_EXC_FALHA;
}

/** Bits de um float, para devolvê-lo em dois registradores. */
static uint32_t bits_do_float(float f)
{
  uint32_t b;

  (void)memcpy(&b, &f, sizeof(b));
  return b;
}

/* ------------------------------------------------------------------ bobinas */

static int32_t le_bobinas(void *c, uint16_t e, uint16_t q, bool *v)
{
  uint16_t i;
  bool ligada = false;

  (void)c;
  for (i = 0U; i < q; i++)
  {
    uint32_t n = BASE_BOBINAS + e + i;

    if ((n < 1UL) || (n > N_SAIDAS))
    {
      return MODBUS_EXC_ENDERECO;
    }
    (void)io_saidas_estado((uint8_t)n, &ligada);
    v[i] = ligada;
  }
  return MODBUS_OK;
}

static int32_t escreve_bobinas(void *c, uint16_t e, uint16_t q, const bool *v)
{
  uint16_t i;

  (void)c;
  if (((BASE_BOBINAS + e + q - 1UL) > N_SAIDAS))
  {
    return MODBUS_EXC_ENDERECO;
  }
  for (i = 0U; i < q; i++)
  {
    (void)io_saidas_comando_manual((uint8_t)(BASE_BOBINAS + e + i), v[i], NULL);
  }
  return MODBUS_OK;
}

/* ---------------------------------------------------------------- discretas */

/**
 * @brief Valor de uma entrada discreta.
 *
 * @param n Número do mapa (10001 em diante).
 * @param valor Recebe o valor.
 * @return true se o número existe.
 */
static bool discreta(uint32_t n, bool *valor)
{
  regras_leitura_t l;
  bool ativa = false;

  if ((n >= 10001UL) && (n <= (10000UL + N_ENTRADAS)))
  {
    (void)io_entrada_le((uint8_t)(n - 10000UL), &ativa);
    *valor = ativa;
    return true;
  }
  if ((n >= 10016UL) && (n <= (10015UL + N_ENTRADAS)))
  {
    (void)regras_tarefa_leitura((uint8_t)(n - 10015UL), &l);
    *valor = l.valida && l.falha;
    return true;
  }
  if (n == 10031UL)
  {
    *valor = rtc_hora_valida();
    return true;
  }
  return false;
}

static int32_t le_discretas(void *c, uint16_t e, uint16_t q, bool *v)
{
  uint16_t i;

  (void)c;
  for (i = 0U; i < q; i++)
  {
    if (!discreta(BASE_DISCRETAS + e + i, &v[i]))
    {
      return MODBUS_EXC_ENDERECO;
    }
  }
  return MODBUS_OK;
}

/* ----------------------------------------------------------------- entrada */

/**
 * @brief Palavra de um float de 2 registradores, dado o número do mapa.
 *
 * @param n Número do mapa.
 * @param base Primeiro número do bloco.
 * @param instancias Quantidade de instâncias do bloco.
 * @param indice Recebe a instância (0 em diante).
 * @param palavra Recebe a palavra (0 ou 1).
 * @return true se o número está no bloco.
 */
static bool no_bloco_float(uint32_t n, uint32_t base, uint32_t instancias, uint8_t *indice, uint8_t *palavra)
{
  if ((n < base) || (n >= (base + (2UL * instancias))))
  {
    return false;
  }
  *indice = (uint8_t)((n - base) / 2UL);
  *palavra = (uint8_t)((n - base) % 2UL);
  return true;
}

/**
 * @brief Data e hora local em registradores (ano, mês, dia, hora, minuto, segundo).
 *
 * @param posicao Posição de 0 a 5.
 * @return O valor; zero sem hora válida.
 */
static uint16_t data_hora(uint32_t posicao)
{
  uint32_t epoch_s = 0U;
  uint16_t fuso = 0U;
  tempo_local_t t;
  uint16_t valores[6];

  if ((rtc_le(&epoch_s) != RTC_OK) || (config_tarefa_le(40403U, 1U, &fuso) != CONFIG_OK) ||
      (tempo_para_local(epoch_s, (int16_t)fuso, &t) != TEMPO_OK))
  {
    return 0U;
  }
  valores[0] = t.ano;
  valores[1] = t.mes;
  valores[2] = t.dia_mes;
  valores[3] = t.hora;
  valores[4] = t.minuto;
  valores[5] = t.segundo;
  return valores[posicao];
}

/**
 * @brief Registrador de entrada, dado o número do mapa.
 *
 * @param n Número do mapa (30001 em diante).
 * @param valor Recebe o registrador.
 * @return true se o número existe.
 */
static bool registrador_entrada(uint32_t n, uint16_t *valor)
{
  uint8_t i;
  uint8_t w;
  regras_leitura_t l = {0.0F, 0.0F, false, false};
  float f;

  if (no_bloco_float(n, 30001UL, N_ENTRADAS, &i, &w) || no_bloco_float(n, 30031UL, N_ENTRADAS, &i, &w))
  {
    (void)regras_tarefa_leitura((uint8_t)(i + 1U), &l);
    f = (n < 30031UL) ? l.valor : l.ma;
  }
  else if (no_bloco_float(n, 30091UL, N_SAIDAS, &i, &w))
  {
    f = io_frequencia_hz_atual((uint8_t)(i + 1U));
  }
  else if ((n >= 30121UL) && (n <= 30126UL))
  {
    *valor = data_hora(n - 30121UL);
    return true;
  }
  else
  {
    return false;
  }
  *valor = (w == 0U) ? (uint16_t)(bits_do_float(f) >> 16U) : (uint16_t)(bits_do_float(f) & 0xFFFFU);
  return true;
}

static int32_t le_entrada(void *c, uint16_t e, uint16_t q, uint16_t *r)
{
  uint16_t i;

  (void)c;
  for (i = 0U; i < q; i++)
  {
    if (!registrador_entrada(BASE_ENTRADA + e + i, &r[i]))
    {
      return MODBUS_EXC_ENDERECO;
    }
  }
  return MODBUS_OK;
}

/* ---------------------------------------------------------------- retenção */

/** Diz se o pedido toca só o bloco de modo manual ou automático, só fora dele, ou mistura. */
static int32_t classifica_retencao(uint32_t inicio, uint32_t fim, bool *so_modo)
{
  bool dentro_ini = (inicio >= MODO_PRIMEIRO) && (inicio <= MODO_ULTIMO);
  bool dentro_fim = (fim >= MODO_PRIMEIRO) && (fim <= MODO_ULTIMO);
  bool cobre = (inicio < MODO_PRIMEIRO) && (fim > MODO_ULTIMO);

  *so_modo = dentro_ini && dentro_fim;
  if ((dentro_ini != dentro_fim) || cobre)
  {
    return MODBUS_EXC_ENDERECO; /* Mistura estado com configuração. */
  }
  return MODBUS_OK;
}

static int32_t le_retencao(void *c, uint16_t e, uint16_t q, uint16_t *r)
{
  uint32_t inicio = BASE_RETENCAO + e;
  bool so_modo = false;
  uint16_t i;
  controle_modo_t modo = CONTROLE_MODO_AUTOMATICO;

  (void)c;
  if (((inicio + q - 1UL) > 0xFFFFUL) || (classifica_retencao(inicio, inicio + q - 1UL, &so_modo) != MODBUS_OK))
  {
    return MODBUS_EXC_ENDERECO;
  }
  if (!so_modo)
  {
    return excecao_de_config(config_tarefa_le((uint16_t)inicio, q, r));
  }
  for (i = 0U; i < q; i++)
  {
    uint32_t canal = inicio + i - (MODO_PRIMEIRO - 1UL);

    if (canal > N_SAIDAS)
    {
      return MODBUS_EXC_ENDERECO;
    }
    (void)io_saidas_modo((uint8_t)canal, &modo);
    r[i] = (modo == CONTROLE_MODO_MANUAL) ? 1U : 0U;
  }
  return MODBUS_OK;
}

/** Aplica o modo manual ou automático de uma saída. */
static void aplica_modo(uint8_t canal, uint16_t valor)
{
  bool ligada = false;

  if (valor == 0U)
  {
    (void)io_saidas_modo_automatico(canal);
  }
  else
  {
    (void)io_saidas_estado(canal, &ligada);
    (void)io_saidas_comando_manual(canal, ligada, NULL); /* Passa a manual mantendo o estado. */
  }
}

static int32_t escreve_modo(uint32_t inicio, uint16_t q, const uint16_t *r)
{
  uint16_t i;

  for (i = 0U; i < q; i++)
  {
    if ((inicio + i - (MODO_PRIMEIRO - 1UL)) > N_SAIDAS)
    {
      return MODBUS_EXC_ENDERECO;
    }
    if (r[i] > 1U)
    {
      return MODBUS_EXC_VALOR;
    }
  }
  for (i = 0U; i < q; i++)
  {
    aplica_modo((uint8_t)(inicio + i - (MODO_PRIMEIRO - 1UL)), r[i]);
  }
  return MODBUS_OK;
}

static int32_t escreve_retencao(void *c, uint16_t e, uint16_t q, const uint16_t *r)
{
  uint32_t inicio = BASE_RETENCAO + e;
  bool so_modo = false;

  (void)c;
  if (((inicio + q - 1UL) > 0xFFFFUL) || (classifica_retencao(inicio, inicio + q - 1UL, &so_modo) != MODBUS_OK))
  {
    return MODBUS_EXC_ENDERECO;
  }
  if (so_modo)
  {
    return escreve_modo(inicio, q, r);
  }
  return excecao_de_config(config_tarefa_escreve((uint16_t)inicio, q, r));
}

const modbus_ops_t *modbus_mapa_ops(void)
{
  static const modbus_ops_t ops = {NULL, le_bobinas, escreve_bobinas, le_discretas, le_entrada, le_retencao,
                                   escreve_retencao};

  return &ops;
}
