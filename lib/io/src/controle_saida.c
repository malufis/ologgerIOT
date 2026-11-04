/**
 * @file controle_saida.c
 * @brief Implementação do modo manual e automático de uma saída digital.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "controle_saida.h"

#include <stddef.h>

/**
 * @brief Pedido vencedor entre as fontes automáticas, na ordem de prioridade.
 *
 * A falha do sensor vence tudo e pede para desligar. Sem falha, vale o pedido do sensor local
 * e, se ele não pedir nada, o da fonte externa.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @return O pedido a aplicar, ou CONTROLE_PEDIDO_NENHUM se ninguém pede nada.
 */
static controle_pedido_t pedido_vencedor(const controle_saida_t *controle)
{
  uint32_t fonte;

  if (controle->sensor_falha)
  {
    return CONTROLE_PEDIDO_DESLIGA;
  }
  for (fonte = 0U; fonte < CONTROLE_FONTES; fonte++)
  {
    if (controle->pedido[fonte] != CONTROLE_PEDIDO_NENHUM)
    {
      return controle->pedido[fonte];
    }
  }
  return CONTROLE_PEDIDO_NENHUM;
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
    /* Nenhum pedido: a saída mantém o estado em que está. */
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
  controle->sensor_falha = false;
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
  controle->sensor_falha = falha;
  reavalia(controle, agora_ms);
}

bool controle_saida_atualiza(controle_saida_t *controle, uint32_t agora_ms)
{
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
