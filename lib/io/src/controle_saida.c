/**
 * @file controle_saida.c
 * @brief Implementação do modo manual e automático de uma saída digital.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.3.0
 */

#include "controle_saida.h"

#include <stddef.h>

/**
 * @brief Pedido vencedor, na ordem de prioridade.
 *
 * O sensor indisponível (em falha ou esperando a estabilidade) vence tudo e pede para desligar.
 * Com regra do sensor definida, vale só o pedido dela. Sem regra do sensor, vale o pedido do CCO
 * com o device online e o da agenda com o device offline.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @return O pedido a aplicar, ou CONTROLE_PEDIDO_NENHUM se ninguém pede nada.
 */
static controle_pedido_t pedido_vencedor(const controle_saida_t *controle)
{
  if (controle_saida_sensor_indisponivel(controle))
  {
    return CONTROLE_PEDIDO_DESLIGA;
  }
  if (controle->regra_sensor)
  {
    return controle->pedido[CONTROLE_FONTE_SENSOR];
  }
  return controle->online ? controle->pedido[CONTROLE_FONTE_CCO]
                          : controle->pedido[CONTROLE_FONTE_AGENDA];
}

/**
 * @brief Aplica o pedido vencedor, se o modo for automático.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @param agora_ms Instante atual, em milissegundos.
 */
static void reavalia(controle_saida_t *controle, uint32_t agora_ms)
{
  controle_pedido_t pedido;

  if (controle->modo != CONTROLE_MODO_AUTOMATICO)
  {
    return;
  }

  pedido = pedido_vencedor(controle);
  if (pedido == CONTROLE_PEDIDO_LIGA)
  {
    saida_liga(&controle->saida, agora_ms);
  }
  else if (pedido == CONTROLE_PEDIDO_DESLIGA)
  {
    saida_desliga(&controle->saida);
  }
  else
  {
    /* Nenhum pedido válido: a saída mantém o último estado válido do comando. */
  }
}

int32_t controle_saida_inicia(controle_saida_t *controle)
{
  uint32_t fonte;

  if (controle == NULL)
  {
    return CONTROLE_SAIDA_ERRO_PARAMETRO;
  }

  (void)saida_inicia(&controle->saida);
  controle->modo = CONTROLE_MODO_AUTOMATICO;
  controle->regra_sensor = false;
  controle->online = false;
  controle->sensor_falha = false;
  controle->recuperando = false;
  controle->fim_falha_ms = 0U;
  controle->estabilidade_ms = CONTROLE_ESTABILIDADE_PADRAO_MS;
  for (fonte = 0U; fonte < CONTROLE_FONTES; fonte++)
  {
    controle->pedido[fonte] = CONTROLE_PEDIDO_NENHUM;
  }
  return CONTROLE_SAIDA_OK;
}

void controle_saida_define_atraso(controle_saida_t *controle, uint32_t atraso_ms)
{
  saida_define_atraso(&controle->saida, atraso_ms);
}

void controle_saida_define_estabilidade(controle_saida_t *controle, uint32_t estabilidade_ms)
{
  controle->estabilidade_ms = estabilidade_ms;
}

void controle_saida_define_regra_sensor(controle_saida_t *controle, bool definida, uint32_t agora_ms)
{
  controle->regra_sensor = definida;
  reavalia(controle, agora_ms);
}

void controle_saida_define_conectividade(controle_saida_t *controle, bool online, uint32_t agora_ms)
{
  if (online && !controle->online)
  {
    /* Reconexão: mantém o último estado e espera um novo comando do CCO. */
    controle->pedido[CONTROLE_FONTE_CCO] = CONTROLE_PEDIDO_NENHUM;
  }
  controle->online = online;
  reavalia(controle, agora_ms);
}

bool controle_saida_comando_manual(controle_saida_t *controle, bool liga, uint32_t agora_ms)
{
  bool mudou = (controle->modo != CONTROLE_MODO_MANUAL);

  controle->modo = CONTROLE_MODO_MANUAL;
  if (liga)
  {
    saida_liga(&controle->saida, agora_ms);
  }
  else
  {
    saida_desliga(&controle->saida);
  }
  return mudou;
}

void controle_saida_modo_automatico(controle_saida_t *controle, uint32_t agora_ms)
{
  controle->modo = CONTROLE_MODO_AUTOMATICO;
  reavalia(controle, agora_ms);
}

void controle_saida_pedido(controle_saida_t *controle, controle_fonte_t fonte,
                           controle_pedido_t pedido, uint32_t agora_ms)
{
  if ((uint32_t)fonte >= CONTROLE_FONTES)
  {
    return;
  }

  controle->pedido[fonte] = pedido;
  reavalia(controle, agora_ms);
}

void controle_saida_sensor_falha(controle_saida_t *controle, bool falha, uint32_t agora_ms)
{
  if (falha)
  {
    controle->recuperando = false; /* Nova falha: a contagem de estabilidade recomeça depois. */
  }
  else if (controle->sensor_falha)
  {
    controle->recuperando = true; /* A falha terminou agora: começa a espera de estabilidade. */
    controle->fim_falha_ms = agora_ms;
  }
  else
  {
    /* Sem falha antes e sem falha agora: nada muda. */
  }

  controle->sensor_falha = falha;
  reavalia(controle, agora_ms);
}

bool controle_saida_atualiza(controle_saida_t *controle, uint32_t agora_ms)
{
  /* A subtração sem sinal resolve a volta do contador de milissegundos. */
  if (controle->recuperando &&
      ((uint32_t)(agora_ms - controle->fim_falha_ms) >= controle->estabilidade_ms))
  {
    controle->recuperando = false;
    reavalia(controle, agora_ms);
  }
  return saida_atualiza(&controle->saida, agora_ms);
}

controle_modo_t controle_saida_modo(const controle_saida_t *controle)
{
  return controle->modo;
}

bool controle_saida_ligada(const controle_saida_t *controle)
{
  return saida_ligada(&controle->saida);
}

bool controle_saida_aguardando(const controle_saida_t *controle)
{
  return saida_aguardando(&controle->saida);
}

bool controle_saida_sensor_indisponivel(const controle_saida_t *controle)
{
  return controle->sensor_falha || controle->recuperando;
}
