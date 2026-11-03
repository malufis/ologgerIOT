/**
 * @file sensor.c
 * @brief Implementação da média por segundo e da conversão do sinal 4-20 mA.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#include "sensor.h"

#include <stddef.h>

/** Fator de conversão de ampères para miliampères. */
#define SENSOR_MA_POR_A 1000.0f

int32_t sensor_media_inicia(sensor_media_t *media, uint16_t amostras_por_seg)
{
  if (media == NULL)
  {
    return SENSOR_ERRO_PARAMETRO;
  }
  if ((amostras_por_seg < SENSOR_TAXA_MIN_SPS) || (amostras_por_seg > SENSOR_TAXA_MAX_SPS))
  {
    return SENSOR_ERRO_TAXA;
  }

  media->soma = 0U;
  media->amostras = 0U;
  media->alvo = amostras_por_seg;

  return SENSOR_OK;
}

int32_t sensor_media_acumula(sensor_media_t *media, uint16_t contagens, bool *pronta,
                             float *resultado)
{
  if ((media == NULL) || (pronta == NULL) || (resultado == NULL) || (media->alvo == 0U))
  {
    return SENSOR_ERRO_PARAMETRO;
  }

  media->soma += contagens;
  media->amostras++;
  *pronta = false;

  if (media->amostras >= media->alvo)
  {
    *resultado = (float)media->soma / (float)media->amostras;
    *pronta = true;
    media->soma = 0U;
    media->amostras = 0U;
  }

  return SENSOR_OK;
}

int32_t sensor_contagens_para_ma(float contagens, float vref_v, float shunt_ohm, float *ma)
{
  if ((ma == NULL) || (vref_v <= 0.0f) || (shunt_ohm <= 0.0f) || (contagens < 0.0f) ||
      (contagens > SENSOR_ADC_MAX))
  {
    return SENSOR_ERRO_PARAMETRO;
  }

  *ma = (contagens / SENSOR_ADC_MAX) * vref_v / shunt_ohm * SENSOR_MA_POR_A;

  return SENSOR_OK;
}

int32_t sensor_ma_para_valor(float ma, float zero, float span, float offset, float *valor)
{
  float fracao;

  if (valor == NULL)
  {
    return SENSOR_ERRO_PARAMETRO;
  }

  fracao = (ma - SENSOR_MA_INICIO) / (SENSOR_MA_FIM - SENSOR_MA_INICIO);
  *valor = zero + (fracao * (span - zero)) + offset;

  return SENSOR_OK;
}
