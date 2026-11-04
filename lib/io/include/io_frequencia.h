/**
 * @file io_frequencia.h
 * @brief Saídas em frequência no firmware: tipo, gerador no timer e exclusão com o sleep.
 *
 * Reúne o tipo das saídas (tipo_saida.h), os geradores de onda quadrada (gerador_freq.h) e o
 * timer que os avança:
 *
 * - **Tipo (FW-061):** io_frequencia_define_tipo() é o comando do mapa Modbus 40166 a 40180.
 *   Só as saídas 2, 3 e 4 (PB13, PB14 e PB15) aceitam frequência, e só com o sleep desligado. A
 *   saída em frequência só pode ser usada com o módulo fora do modo sleep.
 *   io_frequencia_sleep_permitido() é o gancho de energia.h que recusa ligar o sleep enquanto
 *   houver saída em frequência.
 * - **Frequência (FW-023):** io_frequencia_define_hz() define a frequência de uma saída em modo
 *   frequência. O TIM6 interrompe a 100 kHz (GERADOR_FREQ_TICK_HZ) e a interrupção avança os
 *   geradores e escreve os pinos. O timer só roda quando alguma saída tem frequência diferente de
 *   zero. Em 0 Hz o pino fica em nível baixo (transistor desligado).
 * - **Pino:** ao entrar em modo frequência a saída cede o pino ao gerador
 *   (io_saidas_cede_pino()) e passa a 0 Hz; ao voltar a discreta, o pino é devolvido e o estado
 *   discreto é aplicado. Os comandos manual e automático seguem calculando o estado da saída em
 *   modo frequência, mas não chegam ao pino. A interação do modo manual com a saída em frequência
 *   segue em aberto.
 *
 * O tratador da interrupção fica em src/interrupcoes.c, que chama io_frequencia_irq().
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.2.0
 */

#ifndef IO_FREQUENCIA_H
#define IO_FREQUENCIA_H

#include <stdbool.h>
#include <stdint.h>

#include "tipo_saida.h"

/** Código de retorno: sucesso. */
#define IO_FREQUENCIA_OK                 0
/** Código de retorno: o timer não pôde ser configurado para 100 kHz. */
#define IO_FREQUENCIA_ERRO_TIMER         (-20)
/** Código de retorno: a saída não está em modo frequência. */
#define IO_FREQUENCIA_ERRO_NAO_FREQUENCIA (-21)
/** Código de retorno: frequência fora de 0 a 20 kHz ou não numérica. */
#define IO_FREQUENCIA_ERRO_FREQUENCIA    (-22)

/**
 * @brief Configura o TIM6 para interromper a 100 kHz, sem iniciá-lo.
 *
 * @return IO_FREQUENCIA_OK ou IO_FREQUENCIA_ERRO_TIMER.
 * @note Chamar uma vez na partida, depois de io_saidas_prepara().
 */
int32_t io_frequencia_inicia(void);

/**
 * @brief Define o tipo de uma saída, respeitando a exclusão com o sleep.
 *
 * Em modo frequência a saída cede o pino ao gerador e fica em 0 Hz. Voltando a discreta, o gerador
 * para e o pino é devolvido ao controle discreto.
 *
 * @param canal Saída de 1 a TIPO_SAIDA_QTD (só 2, 3 e 4 aceitam frequência).
 * @param tipo TIPO_SAIDA_DISCRETO ou TIPO_SAIDA_FREQUENCIA.
 * @param sleep_ativo true se o modo sleep está ligado.
 * @return TIPO_SAIDA_OK ou o código de erro de tipo_saida.h.
 */
int32_t io_frequencia_define_tipo(uint8_t canal, uint16_t tipo, bool sleep_ativo);

/**
 * @brief Lê o tipo de uma saída.
 *
 * @param canal Saída de 1 a TIPO_SAIDA_QTD.
 * @param tipo Recebe o tipo.
 * @return TIPO_SAIDA_OK ou TIPO_SAIDA_ERRO_PARAMETRO.
 */
int32_t io_frequencia_tipo(uint8_t canal, uint16_t *tipo);

/**
 * @brief Define a frequência de uma saída em modo frequência.
 *
 * Zero Hz leva o pino a nível baixo. O timer só roda se alguma saída tiver frequência diferente de
 * zero.
 *
 * @param canal Saída de 1 a TIPO_SAIDA_QTD, em modo frequência.
 * @param hz Frequência em Hz, de 0 a 20 000.
 * @return IO_FREQUENCIA_OK, TIPO_SAIDA_ERRO_PARAMETRO, IO_FREQUENCIA_ERRO_NAO_FREQUENCIA ou
 *         IO_FREQUENCIA_ERRO_FREQUENCIA.
 */
int32_t io_frequencia_define_hz(uint8_t canal, float hz);

/**
 * @brief Gancho `sleep_permitido` de energia.h.
 *
 * @return false se alguma saída está em frequência (o sleep não pode ligar).
 */
bool io_frequencia_sleep_permitido(void);

/**
 * @brief Atende a interrupção do TIM6: avança os geradores e escreve os pinos.
 *
 * Chamada só por src/interrupcoes.c.
 */
void io_frequencia_irq(void);

#endif /* IO_FREQUENCIA_H */
