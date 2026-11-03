/**
 * @file sensor.h
 * @brief Média por segundo e conversão do sinal 4-20 mA em valor de engenharia.
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/sensor). O driver do
 * ADC entrega as amostras; este módulo calcula a média simples de cada segundo
 * e converte contagens do ADC em corrente e em grandeza do sensor.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <stdbool.h>
#include <stdint.h>

/** Taxa mínima de amostragem por entrada, em amostras por segundo. */
#define SENSOR_TAXA_MIN_SPS      10U
/** Taxa máxima de amostragem por entrada, em amostras por segundo. */
#define SENSOR_TAXA_MAX_SPS      1000U
/** Maior contagem do ADC de 12 bits. */
#define SENSOR_ADC_MAX           4095.0f
/** Corrente no início da escala, em mA. */
#define SENSOR_MA_INICIO         4.0f
/** Corrente no fim da escala, em mA. */
#define SENSOR_MA_FIM            20.0f

/** Código de retorno: sucesso. */
#define SENSOR_OK                0
/** Código de retorno: parâmetro nulo ou fora da faixa. */
#define SENSOR_ERRO_PARAMETRO    (-1)
/** Código de retorno: taxa de amostragem fora de 10 a 1000 amostras por segundo. */
#define SENSOR_ERRO_TAXA         (-2)

/** Acumulador da média simples de um segundo, de uma entrada. */
typedef struct
{
  uint32_t soma;     /**< Soma das contagens acumuladas no segundo. */
  uint16_t amostras; /**< Quantidade de amostras já acumuladas. */
  uint16_t alvo;     /**< Quantidade de amostras que fecham um segundo. */
} sensor_media_t;

/**
 * @brief Inicia o acumulador para uma taxa de amostragem.
 *
 * @param[out] media             Acumulador a iniciar.
 * @param[in]  amostras_por_seg  Taxa por entrada, de 10 a 1000 amostras por segundo.
 * @return     SENSOR_OK, SENSOR_ERRO_PARAMETRO ou SENSOR_ERRO_TAXA.
 */
int32_t sensor_media_inicia(sensor_media_t *media, uint16_t amostras_por_seg);

/**
 * @brief Acumula uma amostra e, ao fechar um segundo, entrega a média.
 *
 * @param[in,out] media     Acumulador da entrada.
 * @param[in]     contagens Contagem do ADC, de 0 a 4095.
 * @param[out]    pronta    Verdadeiro quando a amostra fechou um segundo.
 * @param[out]    resultado Média das contagens do segundo, válida só se pronta for verdadeiro.
 * @return        SENSOR_OK ou SENSOR_ERRO_PARAMETRO.
 * @note          Ao fechar o segundo, o acumulador reinicia sozinho.
 */
int32_t sensor_media_acumula(sensor_media_t *media, uint16_t contagens, bool *pronta,
                             float *resultado);

/**
 * @brief Converte contagens do ADC em corrente, pela tensão no resistor shunt.
 *
 * @param[in]  contagens Contagem do ADC, de 0 a 4095. Pode ser média e fracionária.
 * @param[in]  vref_v    Tensão de referência do ADC, em volts. Maior que zero.
 * @param[in]  shunt_ohm Resistência do shunt, em ohms. Maior que zero.
 * @param[out] ma        Corrente em miliampères.
 * @return     SENSOR_OK ou SENSOR_ERRO_PARAMETRO.
 * @note       Não limita o resultado: corrente fora de 4 a 20 mA é falha, tratada em outro módulo.
 */
int32_t sensor_contagens_para_ma(float contagens, float vref_v, float shunt_ohm, float *ma);

/**
 * @brief Converte corrente 4-20 mA em valor de engenharia, com zero, span e offset.
 *
 * valor = zero + (ma - 4) / 16 * (span - zero) + offset
 *
 * @param[in]  ma     Corrente em miliampères.
 * @param[in]  zero   Valor de engenharia em 4 mA.
 * @param[in]  span   Valor de engenharia em 20 mA.
 * @param[in]  offset Ajuste de calibração somado ao resultado.
 * @param[out] valor  Valor na unidade do sensor.
 * @return     SENSOR_OK ou SENSOR_ERRO_PARAMETRO.
 * @note       Não limita o resultado.
 */
int32_t sensor_ma_para_valor(float ma, float zero, float span, float offset, float *valor);

#endif /* SENSOR_H */
