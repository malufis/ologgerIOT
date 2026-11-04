/**
 * @file io_saida.h
 * @brief Driver dos pinos das saídas digitais O1 a O4 (OloggerMulti).
 *
 * Aplica ao pino o estado ligada ou desligada. O pino aciona um transistor NPN em coletor
 * aberto, e a polaridade vem de pinos.h (PINO_SAIDA_DIGITAL_NIVEL_LIGADO). Este módulo só
 * escreve nos pinos; a regra de quando ligar fica em io_saidas.h e saida.h.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#ifndef IO_SAIDA_H
#define IO_SAIDA_H

#include <stdbool.h>
#include <stdint.h>

/** Quantidade de saídas digitais da placa. */
#define IO_SAIDAS_QTD          4U

/** Código de retorno: sucesso. */
#define IO_SAIDA_OK            0
/** Código de retorno: parâmetro nulo ou canal fora de 1 a IO_SAIDAS_QTD. */
#define IO_SAIDA_ERRO_PARAMETRO (-1)

/**
 * @brief Configura os pinos das saídas, todos desligados.
 *
 * Escreve o nível desligado antes de configurar cada pino como saída, para não haver pulso
 * na ligação. Chamar o quanto antes na partida, porque o transistor de saída não tem resistor
 * da base para o terra e um pino solto pode deixá-lo em estado indefinido.
 *
 * @return IO_SAIDA_OK.
 */
int32_t io_saida_inicia(void);

/**
 * @brief Liga ou desliga uma saída no pino.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD, como nos rótulos do conector J12.
 * @param ligada true para ligar.
 * @return IO_SAIDA_OK ou IO_SAIDA_ERRO_PARAMETRO.
 */
int32_t io_saida_define(uint8_t canal, bool ligada);

/**
 * @brief Lê o estado que o pino de uma saída está comandando.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param ligada Saída: true se o pino comanda a saída ligada.
 * @return IO_SAIDA_OK ou IO_SAIDA_ERRO_PARAMETRO.
 */
int32_t io_saida_le(uint8_t canal, bool *ligada);

#endif /* IO_SAIDA_H */
