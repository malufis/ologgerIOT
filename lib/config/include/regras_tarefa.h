/**
 * @file regras_tarefa.h
 * @brief Tarefa que liga as entradas analógicas às saídas: regra do sensor e saída em frequência.
 *
 * A cada 200 ms lê a corrente das entradas 4–20 mA (aquisicao.h) e, conforme a configuração de cada
 * saída (config.h):
 *
 * - **Entrada associada = 0:** nada a fazer; a saída segue a agenda ou o CCO. Se estava em
 *   frequência, a frequência vai a 0 Hz.
 * - **Saída em frequência:** replica a leitura em Hz entre as frequências de 4 mA e de 20 mA
 *   (io_frequencia_atualiza()); o sensor manda e os comandos não a influenciam.
 * - **Saída discreta com limiares válidos (mínimo menor que o máximo):** aplica a regra do sensor
 *   com histerese (regra_sensor.h) e entrega o pedido à saída como fonte sensor. Com a regra
 *   definida, a saída ignora a agenda e o CCO.
 *
 * **Falha do sensor:** corrente abaixo de 3,6 mA ou acima de 21 mA (faixa de falha da norma NAMUR
 * NE43, **confirmada em 04/11/2026**; o dente de serra continua no FW-013). Em falha, a saída discreta desliga e a de frequência
 * vai a 0 Hz; a volta ao sensor só depois de 5 minutos estáveis (controle_saida.h). Sem leitura
 * ainda (primeiro segundo depois da partida), a passada é pulada sem declarar falha.
 *
 * O valor de engenharia usa o zero, o span e o offset da entrada (sensor.h). A tarefa lê as quatro
 * entradas a cada passada, associadas a uma saída ou não, e guarda a última leitura de cada uma
 * (regras_tarefa_leitura()) para os registradores de entrada do Modbus.
 *
 * Sem teste em hardware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef REGRAS_TAREFA_H
#define REGRAS_TAREFA_H

#include <stdbool.h>
#include <stdint.h>

/** Período da tarefa, em milissegundos. */
#define REGRAS_TAREFA_PERIODO_MS     200U
/** Tamanho da pilha da tarefa, em palavras de 32 bits. */
#define REGRAS_TAREFA_PILHA_PALAVRAS 384U

/** Corrente abaixo da qual o sensor está em falha, em mA (NAMUR NE43, confirmada). */
#define REGRAS_FALHA_ABAIXO_MA       3.6F
/** Corrente acima da qual o sensor está em falha, em mA (NAMUR NE43, confirmada). */
#define REGRAS_FALHA_ACIMA_MA        21.0F

/** Código de retorno: sucesso. */
#define REGRAS_TAREFA_OK             0
/** Código de retorno: a tarefa não foi criada. */
#define REGRAS_TAREFA_ERRO_TAREFA    (-1)

/** Última leitura de uma entrada analógica. */
typedef struct
{
  float ma;      /**< Corrente em mA. */
  float valor;   /**< Valor de engenharia (zero, span e offset da entrada). */
  bool falha;    /**< true se a corrente indica falha do sensor. */
  bool valida;   /**< true se já houve leitura desde a partida. */
} regras_leitura_t;

/**
 * @brief Última leitura de uma entrada analógica, para o Modbus.
 *
 * @param entrada Entrada de 1 a 4.
 * @param leitura Recebe a leitura.
 * @return REGRAS_TAREFA_OK, ou REGRAS_TAREFA_ERRO_TAREFA se a entrada for inválida (com `valida` falso).
 */
int32_t regras_tarefa_leitura(uint8_t entrada, regras_leitura_t *leitura);

/**
 * @brief Cria a tarefa das regras, com memória estática.
 *
 * @return REGRAS_TAREFA_OK ou REGRAS_TAREFA_ERRO_TAREFA.
 * @note Chamar antes de vTaskStartScheduler, depois de iniciar as saídas e a aquisição.
 */
int32_t regras_tarefa_inicia(void);

#endif /* REGRAS_TAREFA_H */
