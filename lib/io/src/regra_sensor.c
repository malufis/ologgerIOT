/**
 * @file regra_sensor.c
 * @brief Implementação da regra do sensor.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "regra_sensor.h"

#include <math.h>
#include <stddef.h>

int32_t regra_sensor_limpa(regra_sensor_t *regra)
{
  if (regra == NULL)
  {
    return REGRA_SENSOR_ERRO_PARAMETRO;
  }
  regra->definida = false;
  regra->sentido = REGRA_SENTIDO_LIGA_NO_MINIMO;
  regra->minimo = 0.0F;
  regra->maximo = 0.0F;
  regra->pedido = CONTROLE_PEDIDO_NENHUM;
  return REGRA_SENSOR_OK;
}

int32_t regra_sensor_cadastra(regra_sensor_t *regra, uint16_t sentido, float minimo, float maximo)
{
  if (regra == NULL)
  {
    return REGRA_SENSOR_ERRO_PARAMETRO;
  }
  if (sentido > (uint16_t)REGRA_SENTIDO_LIGA_NO_MAXIMO)
  {
    return REGRA_SENSOR_ERRO_SENTIDO;
  }
  if ((isfinite(minimo) == 0) || (isfinite(maximo) == 0) || !(minimo < maximo))
  {
    return REGRA_SENSOR_ERRO_LIMIARES;
  }
  regra->definida = true;
  regra->sentido = (regra_sentido_t)sentido;
  regra->minimo = minimo;
  regra->maximo = maximo;
  regra->pedido = CONTROLE_PEDIDO_NENHUM;
  return REGRA_SENSOR_OK;
}

controle_pedido_t regra_sensor_avalia(regra_sensor_t *regra, float valor)
{
  if ((regra == NULL) || !regra->definida)
  {
    return CONTROLE_PEDIDO_NENHUM;
  }
  if (isnan(valor) != 0)
  {
    return regra->pedido;
  }
  if (valor >= regra->maximo)
  {
    regra->pedido = (regra->sentido == REGRA_SENTIDO_LIGA_NO_MAXIMO) ? CONTROLE_PEDIDO_LIGA
                                                                      : CONTROLE_PEDIDO_DESLIGA;
  }
  else if (valor <= regra->minimo)
  {
    regra->pedido = (regra->sentido == REGRA_SENTIDO_LIGA_NO_MINIMO) ? CONTROLE_PEDIDO_LIGA
                                                                      : CONTROLE_PEDIDO_DESLIGA;
  }
  else
  {
    /* Entre os limiares: histerese, mantém o pedido anterior. */
  }
  return regra->pedido;
}
