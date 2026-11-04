/**
 * @file io_pulsos.c
 * @brief Implementação do contador de pulsos com controle de inatividade.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#include "io_pulsos.h"

#include <stddef.h>

int32_t io_pulsos_inicia(io_pulsos_t *pulsos, uint32_t timeout_ms, bool estado_inicial,
                         uint32_t agora_ms)
{
  if ((pulsos == NULL) || (timeout_ms == 0U))
  {
    return IO_PULSOS_ERRO_PARAMETRO;
  }

  pulsos->contagem = 0U;
  pulsos->ultimo_ms = agora_ms;
  pulsos->timeout_ms = timeout_ms;
  pulsos->ativa_antes = estado_inicial;
  return IO_PULSOS_OK;
}

int32_t io_pulsos_define_timeout(io_pulsos_t *pulsos, uint32_t timeout_ms)
{
  if ((pulsos == NULL) || (timeout_ms == 0U))
  {
    return IO_PULSOS_ERRO_PARAMETRO;
  }

  pulsos->timeout_ms = timeout_ms;
  return IO_PULSOS_OK;
}

void io_pulsos_atualiza(io_pulsos_t *pulsos, bool ativa, uint32_t agora_ms)
{
  if (ativa && !pulsos->ativa_antes)
  {
    pulsos->contagem++;
    pulsos->ultimo_ms = agora_ms;
  }
  pulsos->ativa_antes = ativa;
}

void io_pulsos_soma(io_pulsos_t *pulsos, uint32_t quantidade, bool ativa_agora, uint32_t agora_ms)
{
  if (quantidade > 0U)
  {
    pulsos->contagem += quantidade;
    pulsos->ultimo_ms = agora_ms;
  }
  pulsos->ativa_antes = ativa_agora;
}

bool io_pulsos_pode_dormir(const io_pulsos_t *pulsos, uint32_t agora_ms)
{
  /* A subtração sem sinal resolve a volta do contador de milissegundos. */
  return (uint32_t)(agora_ms - pulsos->ultimo_ms) >= pulsos->timeout_ms;
}

uint32_t io_pulsos_contagem(const io_pulsos_t *pulsos)
{
  return pulsos->contagem;
}
