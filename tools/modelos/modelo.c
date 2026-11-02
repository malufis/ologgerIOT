/**
 * @file modelo.c
 * @brief Implementação do módulo de exemplo.
 *
 * @date criacao DD/MM/AAAA
 * @date modificacao DD/MM/AAAA
 * @version 0.1.0
 */

#include <stddef.h>

#include "modelo.h"

/** Código de erro: parâmetro de saída nulo. */
#define MODELO_ERRO_PARAMETRO (-1)

/* Estado do módulo: sempre static, sem variável global compartilhada. */
static uint32_t ultimo_valor;

int32_t modelo_executa(uint32_t entrada, uint32_t *saida)
{
  if (saida == NULL)
  {
    return MODELO_ERRO_PARAMETRO;
  }

  ultimo_valor = entrada;
  *saida = ultimo_valor;

  return 0;
}
