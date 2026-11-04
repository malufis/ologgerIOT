/**
 * @file saida.c
 * @brief Implementação do estado de uma saída digital discreta, com retardo de acionamento.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.3.0
 */

#include "saida.h"

#include <stddef.h>

int32_t saida_inicia(saida_t *saida)
{
  if (saida == NULL)
  {
    return SAIDA_ERRO_PARAMETRO;
  }

  saida->ligada = false;
  saida->aguardando = false;
  saida->atraso_ms = 0U;
  saida->espera_ms = 0U;
  return SAIDA_OK;
}

void saida_define_atraso(saida_t *saida, uint32_t atraso_ms)
{
  saida->atraso_ms = atraso_ms;
}

void saida_liga(saida_t *saida, uint32_t agora_ms)
{
  if (saida->ligada || saida->aguardando)
  {
    return; /* Já ligada, ou com a partida em curso: o comando é descartado. */
  }

  if (saida->atraso_ms == 0U)
  {
    saida->ligada = true;
  }
  else
  {
    saida->aguardando = true;
    saida->espera_ms = agora_ms;
  }
}

void saida_desliga(saida_t *saida)
{
  saida->ligada = false;
  saida->aguardando = false;
}

bool saida_atualiza(saida_t *saida, uint32_t agora_ms)
{
  /* A subtração sem sinal resolve a volta do contador de milissegundos. */
  if (saida->aguardando && ((uint32_t)(agora_ms - saida->espera_ms) >= saida->atraso_ms))
  {
    saida->aguardando = false;
    saida->ligada = true;
  }
  return saida->ligada;
}

bool saida_ligada(const saida_t *saida)
{
  return saida->ligada;
}

bool saida_aguardando(const saida_t *saida)
{
  return saida->aguardando;
}
