/**
 * @file adc.h
 * @brief Driver do ADC das entradas 4-20 mA, com timer, DMA e callback (STM32L412).
 *
 * Um timer dispara o ADC na taxa pedida (10 a 1000 amostras por segundo por
 * entrada). O ADC varre as entradas em sequência e o DMA circular grava os
 * quadros em um buffer de duas metades. Ao encher cada metade, o callback é
 * chamado, em contexto de interrupção.
 *
 * @note Só o STM32L412 (OloggerMulti). O ADC do STM32L0 tem outra API e fica
 *       para o y7080.
 *
 * @date criacao 02/11/2026
 * @date modificacao 03/11/2026
 * @version 0.2.0
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/** Quantidade de entradas 4-20 mA varridas por quadro. */
#define ADC_CANAIS              4U
/** Posições por quadro na sequência do ADC: as entradas e a referência interna VREFINT. */
#define ADC_SEQ_CANAIS          5U
/** Maior quantidade de quadros em uma metade do buffer (taxa de 1000 amostras por segundo). */
#define ADC_QUADROS_MEIA_MAX    100U

/** Código de retorno: sucesso. */
#define ADC_OK                  0
/** Código de retorno: parâmetro inválido, como taxa fora de 10 a 1000. */
#define ADC_ERRO_PARAMETRO      (-1)
/** Código de retorno: falha no HAL do ADC, do DMA ou do timer. */
#define ADC_ERRO_HAL            (-2)

/**
 * @brief Tipo do callback chamado quando uma metade do buffer enche.
 *
 * Executa em contexto de interrupção: deve ser curto e só usar funções FromISR.
 *
 * @param[in] quadros    Início da metade cheia. Cada quadro tem ADC_CANAIS contagens, na ordem dos canais 1 a 4.
 * @param[in] quantidade Quantidade de quadros na metade.
 */
typedef void (*adc_callback_t)(const uint16_t *quadros, uint16_t quantidade);

/**
 * @brief Diagnóstico de bancada: mede VREFINT e a entrada 4 com conversões simples.
 *
 * Disparadas por software, sem timer nem DMA, para separar o comportamento do ADC
 * do comportamento do driver contínuo. Os resultados ficam em uma variável lida por SWD.
 *
 * @note Chamar antes de criar qualquer tarefa do FreeRTOS: ela usa HAL_Delay, que
 *       depende do tick, e o FreeRTOS mascara o SysTick entre a criação da primeira
 *       tarefa e a partida do escalonador.
 */
void adc_diagnostico(void);

/**
 * @brief Inicia a aquisição contínua.
 *
 * @param[in] taxa_sps  Amostras por segundo por entrada, de 10 a 1000.
 * @param[in] callback  Função chamada a cada metade do buffer cheia.
 * @return    ADC_OK, ADC_ERRO_PARAMETRO ou ADC_ERRO_HAL.
 * @note      A frequência do timer é arredondada para um tick de 100 kHz.
 *            Taxas que não dividem 100000 têm erro menor que 1%.
 */
int32_t adc_inicia(uint16_t taxa_sps, adc_callback_t callback);

/**
 * @brief Para a aquisição para o equipamento dormir: timer, DMA, ADC e referência interna.
 *
 * @return ADC_OK ou ADC_ERRO_HAL.
 * @note      Só vale depois de adc_inicia().
 */
int32_t adc_para(void);

/**
 * @brief Retoma a aquisição com a taxa e o callback de adc_inicia().
 *
 * @return ADC_OK, ADC_ERRO_PARAMETRO (se adc_inicia() nunca foi chamado) ou ADC_ERRO_HAL.
 */
int32_t adc_retoma(void);

#endif /* ADC_H */
