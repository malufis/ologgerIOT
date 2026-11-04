/**
 * @file io_frequencia.c
 * @brief Implementação do tipo das saídas no firmware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "io_frequencia.h"

#include "FreeRTOS.h"
#include "task.h"

/** Tipos das saídas; começam todas discretas (memória zerada = TIPO_SAIDA_DISCRETO). */
static tipos_saida_t tipos;

int32_t io_frequencia_define_tipo(uint8_t canal, uint16_t tipo, bool sleep_ativo)
{
  int32_t resultado;

  taskENTER_CRITICAL();
  resultado = tipos_saida_define(&tipos, canal, tipo, sleep_ativo);
  taskEXIT_CRITICAL();
  return resultado;
}

int32_t io_frequencia_tipo(uint8_t canal, uint16_t *tipo)
{
  int32_t resultado;

  taskENTER_CRITICAL();
  resultado = tipos_saida_tipo(&tipos, canal, tipo);
  taskEXIT_CRITICAL();
  return resultado;
}

bool io_frequencia_sleep_permitido(void)
{
  bool permitido;

  taskENTER_CRITICAL();
  permitido = !tipos_saida_algum_em_frequencia(&tipos);
  taskEXIT_CRITICAL();
  return permitido;
}
