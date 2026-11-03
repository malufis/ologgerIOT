/**
 * @file io_entrada.h
 * @brief Leitura do estado das entradas digitais D1 a D4.
 *
 * Lê o nível do pino e entrega o estado da entrada de campo: ativa quando há
 * tensão no campo. A polaridade do pino vem de pinos.h
 * (PINO_ENTRADA_DIGITAL_NIVEL_ATIVO), porque depende do circuito de cada placa.
 * Esta camada não filtra ruído; o filtro está em io_filtro.h.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#ifndef IO_ENTRADA_H
#define IO_ENTRADA_H

#include <stdbool.h>
#include <stdint.h>

/** Quantidade de entradas digitais da placa. */
#define IO_ENTRADAS_QTD        4U

/** Código de retorno: sucesso. */
#define IO_OK                  0
/** Código de retorno: parâmetro nulo ou canal fora de 1 a IO_ENTRADAS_QTD. */
#define IO_ERRO_PARAMETRO      (-1)

/**
 * @brief Configura os pinos das entradas digitais como entrada, sem resistor interno.
 *
 * A placa já tem pull-up externo de 10k em cada pino, no lado do optoacoplador.
 *
 * @return IO_OK.
 */
int32_t io_entrada_inicia(void);

/**
 * @brief Lê o estado de uma entrada digital.
 *
 * @param canal Entrada de 1 a IO_ENTRADAS_QTD, como nos rótulos D1 a D4 do esquema.
 * @param ativa Saída: true se a entrada de campo tem tensão.
 * @return IO_OK ou IO_ERRO_PARAMETRO.
 */
int32_t io_entrada_le(uint8_t canal, bool *ativa);

/**
 * @brief Liga a interrupção externa na borda de ativação das entradas, para contar dormindo.
 *
 * Só a borda que ativa a entrada (início do pulso) gera interrupção. Cada interrupção
 * soma um pulso no contador da entrada, que o totalizador já grava, e quem atende a
 * interrupção pede para o STM32 acordar. Acordado, o STM32 conta pela leitura periódica e
 * só dorme de novo depois do tempo de inatividade sem pulso. Não há filtro neste modo, para o
 * pulso que acorda não se perder: ele serve a pulsos limpos, sem repique.
 * Uma entrada que já está ativa ao chamar esta função só conta depois de desativar e
 * ativar de novo. Quem coloca o STM32 em sleep chama esta função antes.
 *
 * @return IO_OK.
 */
int32_t io_entrada_acordar_habilita(void);

/**
 * @brief Desliga a interrupção externa e volta as entradas ao modo de leitura simples.
 */
void io_entrada_acordar_desabilita(void);

/**
 * @brief Lê e zera os pulsos contados em uma entrada pela interrupção externa.
 *
 * @param canal Entrada de 1 a IO_ENTRADAS_QTD.
 * @return Pulsos contados desde a última coleta; zero se o canal for inválido.
 */
uint32_t io_entrada_coleta_pulsos(uint8_t canal);

/**
 * @brief Trata a interrupção externa das entradas: soma um pulso em cada linha disparada.
 *
 * @note Chamar somente de dentro de EXTI9_5_IRQHandler. Não usa a API do FreeRTOS.
 */
void io_entrada_irq(void);

#endif /* IO_ENTRADA_H */
