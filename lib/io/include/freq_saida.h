/**
 * @file freq_saida.h
 * @brief Frequência da saída "pulsada" a partir da corrente 4–20 mA da entrada analógica.
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/io). A saída em frequência
 * representa em Hz a leitura de uma entrada analógica: o cadastro leva a frequência
 * correspondente a **4 mA** e a correspondente a **20 mA**, e os valores intermediários são
 * proporcionais (interpolação linear). A frequência em 4 mA pode ser maior que a de 20 mA, o
 * que inverte a relação.
 *
 * - **Fora da faixa:** correntes abaixo de 4 mA ou acima de 20 mA ficam presas nos extremos
 *   da faixa. Detectar a falha é trabalho do módulo do sensor.
 * - **Falha do sensor:** a saída vai para 0 Hz.
 * - **Limite:** o cadastro aceita frequências de 0 Hz a FREQ_SAIDA_MAX_HZ (faixa de projeto).
 *
 * Decidido em 04/11/2026: em **0 Hz a saída fica em nível baixo** (transistor desligado) e o
 * sinal tem **duty cycle de 50%** (FREQ_SAIDA_DUTY_PERCENTUAL). Só as saídas 2, 3 e 4 (PB13,
 * PB14 e PB15, com canal de timer) têm modo frequência; a saída 1 (PB12) não (tipo_saida.h).
 *
 * Este módulo só calcula a frequência. A geração do sinal (timer) fica para o próximo passo do
 * FW-023. A exclusão com o modo sleep está em tipo_saida.h (FW-061).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef FREQ_SAIDA_H
#define FREQ_SAIDA_H

#include <stdbool.h>
#include <stdint.h>

/** Código de retorno: sucesso. */
#define FREQ_SAIDA_OK              0
/** Código de retorno: parâmetro nulo. */
#define FREQ_SAIDA_ERRO_PARAMETRO  (-1)
/** Código de retorno: frequência inválida (não numérica, negativa ou acima do limite). */
#define FREQ_SAIDA_ERRO_FREQUENCIA (-2)
/** Código de retorno: o cadastro ainda não foi feito. */
#define FREQ_SAIDA_ERRO_SEM_CADASTRO (-3)

/** Frequência máxima aceita no cadastro, em Hz (faixa de projeto de 0 a 20 kHz). */
#define FREQ_SAIDA_MAX_HZ 20000.0F
/** Duty cycle do sinal gerado, em percentual (decidido em 04/11/2026). */
#define FREQ_SAIDA_DUTY_PERCENTUAL 50U
/** Corrente que corresponde à frequência de 4 mA, em miliampères. */
#define FREQ_SAIDA_MA_MINIMO 4.0F
/** Corrente que corresponde à frequência de 20 mA, em miliampères. */
#define FREQ_SAIDA_MA_MAXIMO 20.0F

/** Cadastro da conversão de uma saída em frequência. */
typedef struct
{
  bool definida;   /**< true depois de um cadastro válido. */
  float hz_4ma;    /**< Frequência em 4 mA, em Hz. */
  float hz_20ma;   /**< Frequência em 20 mA, em Hz. */
} freq_saida_t;

/**
 * @brief Deixa a conversão sem cadastro.
 *
 * @param freq Conversão a limpar.
 * @return FREQ_SAIDA_OK, ou FREQ_SAIDA_ERRO_PARAMETRO se @p freq for nula.
 */
int32_t freq_saida_limpa(freq_saida_t *freq);

/**
 * @brief Cadastra as frequências de 4 mA e de 20 mA. Só troca o cadastro se ambas forem válidas.
 *
 * @param freq Conversão a cadastrar.
 * @param hz_4ma Frequência em 4 mA, de 0 a FREQ_SAIDA_MAX_HZ.
 * @param hz_20ma Frequência em 20 mA, de 0 a FREQ_SAIDA_MAX_HZ.
 * @return FREQ_SAIDA_OK, ou o código de erro (o cadastro anterior fica como estava).
 */
int32_t freq_saida_cadastra(freq_saida_t *freq, float hz_4ma, float hz_20ma);

/**
 * @brief Calcula a frequência da saída para uma corrente.
 *
 * @param freq Conversão cadastrada.
 * @param ma Corrente da entrada analógica, em miliampères.
 * @param falha true se o sensor está em falha: a saída vai para 0 Hz.
 * @param hz Recebe a frequência, em Hz.
 * @return FREQ_SAIDA_OK, FREQ_SAIDA_ERRO_PARAMETRO se algum ponteiro for nulo ou @p ma não for
 *         numérico, ou FREQ_SAIDA_ERRO_SEM_CADASTRO se não houver cadastro (então @p hz recebe 0).
 */
int32_t freq_saida_calcula(const freq_saida_t *freq, float ma, bool falha, float *hz);

#endif /* FREQ_SAIDA_H */
