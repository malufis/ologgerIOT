/**
 * @file regra_sensor.h
 * @brief Regra do sensor: limiares de uma entrada analógica que ligam e desligam uma saída.
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/io). A regra tem dois limiares, um
 * **mínimo** e um **máximo**, na unidade de engenharia da entrada (por exemplo, bar), e um
 * **sentido**:
 *
 * - **Sentido 0:** liga quando o valor chega ao mínimo (ou abaixo) e desliga quando chega ao
 *   máximo (ou acima).
 * - **Sentido 1:** liga quando o valor chega ao máximo (ou acima) e desliga quando chega ao
 *   mínimo (ou abaixo). Exemplo do B4: liga a 5 bar e desliga a 4 bar.
 *
 * Entre os dois limiares vale a histerese: o pedido anterior é mantido. Antes de o valor
 * alcançar algum limiar, a regra não pede nada. O resultado é o pedido da fonte
 * CONTROLE_FONTE_SENSOR de controle_saida.h. Com a regra cadastrada, a agenda e o CCO são
 * ignorados pela saída (controle_saida.h): a agenda não tem prioridade sobre o sensor.
 *
 * Falta a camada que lê e grava o cadastro pelos endereços do mapa Modbus e por MQTT.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef REGRA_SENSOR_H
#define REGRA_SENSOR_H

#include <stdbool.h>
#include <stdint.h>

#include "controle_saida.h"

/** Código de retorno: sucesso. */
#define REGRA_SENSOR_OK              0
/** Código de retorno: parâmetro nulo. */
#define REGRA_SENSOR_ERRO_PARAMETRO  (-1)
/** Código de retorno: sentido diferente de 0 e 1. */
#define REGRA_SENSOR_ERRO_SENTIDO    (-2)
/** Código de retorno: limiar inválido (não numérico) ou mínimo não menor que o máximo. */
#define REGRA_SENSOR_ERRO_LIMIARES   (-3)

/** Sentido da comparação. */
typedef enum
{
  REGRA_SENTIDO_LIGA_NO_MINIMO = 0, /**< Liga no mínimo e desliga no máximo. */
  REGRA_SENTIDO_LIGA_NO_MAXIMO = 1  /**< Liga no máximo e desliga no mínimo. */
} regra_sentido_t;

/** Estado de uma regra do sensor. */
typedef struct
{
  bool definida;              /**< true depois de um cadastro válido. */
  regra_sentido_t sentido;    /**< Sentido da comparação. */
  float minimo;               /**< Limiar mínimo, na unidade da entrada. */
  float maximo;               /**< Limiar máximo, na unidade da entrada. */
  controle_pedido_t pedido;   /**< Último pedido gerado. */
} regra_sensor_t;

/**
 * @brief Deixa a regra sem cadastro e sem pedido.
 *
 * @param regra Regra a limpar.
 * @return REGRA_SENSOR_OK, ou REGRA_SENSOR_ERRO_PARAMETRO se @p regra for nula.
 */
int32_t regra_sensor_limpa(regra_sensor_t *regra);

/**
 * @brief Cadastra a regra. Só troca a regra se todos os campos forem válidos.
 *
 * Um novo cadastro apaga o pedido anterior: a regra volta a esperar o valor alcançar um limiar.
 *
 * @param regra Regra a cadastrar.
 * @param sentido 0 (liga no mínimo) ou 1 (liga no máximo).
 * @param minimo Limiar mínimo.
 * @param maximo Limiar máximo, maior que o mínimo.
 * @return REGRA_SENSOR_OK, ou o código de erro (a regra anterior fica como estava).
 */
int32_t regra_sensor_cadastra(regra_sensor_t *regra, uint16_t sentido, float minimo, float maximo);

/**
 * @brief Avalia um valor da entrada e devolve o pedido da regra.
 *
 * @param regra Regra cadastrada.
 * @param valor Valor da entrada, na mesma unidade dos limiares.
 * @return O pedido atual: LIGA, DESLIGA, ou NENHUM se a regra não está definida ou ainda não
 *         alcançou nenhum limiar. Um valor não numérico mantém o pedido anterior.
 */
controle_pedido_t regra_sensor_avalia(regra_sensor_t *regra, float valor);

#endif /* REGRA_SENSOR_H */
