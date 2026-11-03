/**
 * @file aquisicao.h
 * @brief Tarefa de aquisição das entradas 4-20 mA: média de 1 segundo em mA.
 *
 * Recebe as amostras do driver do ADC, acumula a média simples de cada segundo
 * por entrada (módulo sensor) e converte em corrente. Os valores de 1 segundo
 * são lidos pelas funções abaixo. Também mantém um espelho em inteiros, para
 * leitura por depurador na bancada.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef AQUISICAO_H
#define AQUISICAO_H

#include <stdint.h>

/** Taxa de amostragem usada na bancada, em amostras por segundo por entrada. */
#define AQUISICAO_TAXA_PADRAO_SPS 100U
/**
 * Referência efetiva do ADC, em volts, usada na conversão para corrente.
 * Calibração de bancada em 02/11/2026, por dois pontos (4 mA e 8 mA medidos com
 * multímetro em série): o chip converte como se a referência fosse ~2,367 V,
 * embora a alimentação medida seja 3,25 V. A causa do fator ~1,37 segue em aberto.
 */
#define AQUISICAO_VREF_V          2.367f
/** Shunt de cada entrada, em ohms. Confirmado em 150 pelo responsável técnico em 02/11/2026. */
#define AQUISICAO_SHUNT_OHM       150.0f

/** Código de retorno: sucesso. */
#define AQUISICAO_OK              0
/** Código de retorno: parâmetro inválido, como canal inexistente. */
#define AQUISICAO_ERRO_PARAMETRO  (-1)
/** Código de retorno: falha ao criar a tarefa ou iniciar o ADC. */
#define AQUISICAO_ERRO_INICIO     (-2)
/** Código de retorno: ainda não fechou o primeiro segundo. */
#define AQUISICAO_ERRO_SEM_DADO   (-3)

/**
 * @brief Cria a tarefa de aquisição e inicia o ADC.
 *
 * @param[in] taxa_sps Amostras por segundo por entrada, de 10 a 1000.
 * @return    AQUISICAO_OK, AQUISICAO_ERRO_PARAMETRO ou AQUISICAO_ERRO_INICIO.
 * @note      Chamar antes de vTaskStartScheduler.
 */
int32_t aquisicao_inicia(uint16_t taxa_sps);

/**
 * @brief Para a aquisição, o timer e o ADC, para o equipamento dormir.
 *
 * @return AQUISICAO_OK ou AQUISICAO_ERRO_INICIO.
 */
int32_t aquisicao_para(void);

/**
 * @brief Retoma a aquisição depois do sleep, com a mesma taxa.
 *
 * Descarta o segundo parcial e invalida as leituras anteriores; a leitura volta a ficar
 * disponível quando fechar o primeiro segundo depois de retomar.
 *
 * @return AQUISICAO_OK ou AQUISICAO_ERRO_INICIO.
 */
int32_t aquisicao_retoma(void);

/**
 * @brief Lê a corrente média do último segundo de uma entrada.
 *
 * @param[in]  canal Entrada de 0 a 3 (entradas 1 a 4 da placa).
 * @param[out] ma    Corrente média, em miliampères.
 * @return     AQUISICAO_OK, AQUISICAO_ERRO_PARAMETRO ou AQUISICAO_ERRO_SEM_DADO.
 */
int32_t aquisicao_ma(uint8_t canal, float *ma);

/**
 * @brief Lê a média de contagens do ADC do último segundo de uma entrada.
 *
 * @param[in]  canal      Entrada de 0 a 3.
 * @param[out] contagens  Média das contagens, de 0 a 4095.
 * @return     AQUISICAO_OK, AQUISICAO_ERRO_PARAMETRO ou AQUISICAO_ERRO_SEM_DADO.
 */
int32_t aquisicao_contagens(uint8_t canal, float *contagens);

/**
 * @brief Lê a quantidade de segundos fechados desde a partida.
 *
 * @return Segundos fechados.
 */
uint32_t aquisicao_segundos(void);

#endif /* AQUISICAO_H */
