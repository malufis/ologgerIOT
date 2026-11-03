/**
 * @file io_tarefa.h
 * @brief Tarefa que lê as entradas digitais D1 a D4 e aplica o filtro de ruído.
 *
 * Lê as quatro entradas num período fixo, passa cada uma pelo filtro de ruído
 * (tempo padrão em io_filtro.h) e guarda o estado bruto e o filtrado. Os dois
 * valores são lidos por SWD na bancada e pelos módulos de regra.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#ifndef IO_TAREFA_H
#define IO_TAREFA_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Período de leitura das entradas, em milissegundos. Com o filtro de pulso de 20 ms,
 * 2 ms dão 10 amostras por janela e deixam folga para um pulso pouco acima de 20 ms.
 */
#define IO_TAREFA_PERIODO_MS     2U
/** Tamanho da pilha da tarefa, em palavras de 32 bits. */
#define IO_TAREFA_PILHA_PALAVRAS 128U

/** Código de retorno: sucesso. */
#define IO_TAREFA_OK             0
/** Código de retorno: falha ao iniciar as entradas, o filtro ou a tarefa. */
#define IO_TAREFA_ERRO           (-1)

/**
 * @brief Inicia as entradas, o filtro e cria a tarefa com memória estática.
 *
 * O filtro de cada entrada começa no estado lido na partida, sem gerar mudança.
 *
 * @return IO_TAREFA_OK ou IO_TAREFA_ERRO.
 * @note Chamar antes de vTaskStartScheduler.
 */
int32_t io_tarefa_inicia(void);

/**
 * @brief Lê o estado filtrado das entradas.
 *
 * @return Máscara de bits: o bit 0 é a entrada D1 e o bit 3 é a D4; 1 = ativa (há tensão).
 */
uint8_t io_tarefa_estado(void);

/**
 * @brief Lê o total de pulsos contados em uma entrada desde a partida.
 *
 * @param canal Entrada de 1 a 4, como nos rótulos D1 a D4.
 * @return Pulsos contados; zero se o canal for inválido.
 */
uint32_t io_tarefa_pulsos(uint8_t canal);

/**
 * @brief Lê o totalizador (hodômetro) de uma entrada.
 *
 * O total só soma, guardado nos registradores de backup, e volta a zero ao passar de
 * 32 bits. Quem o lê calcula a diferença entre duas leituras.
 *
 * @param canal Entrada de 1 a 4.
 * @return Total acumulado de pulsos; zero se o canal for inválido.
 */
uint32_t io_tarefa_total(uint8_t canal);

/**
 * @brief Define o totalizador de uma entrada, por exemplo para igualar ao medidor.
 *
 * @param canal Entrada de 1 a 4.
 * @param valor Novo total.
 * @return IO_TAREFA_OK ou IO_TAREFA_ERRO.
 */
int32_t io_tarefa_define_total(uint8_t canal, uint32_t valor);

/**
 * @brief Prepara as entradas para dormir: liga a contagem pela interrupção externa.
 *
 * @note Chamar logo antes de entrar em sleep.
 */
void io_tarefa_prepara_sleep(void);

/**
 * @brief Retoma a leitura periódica depois do sleep.
 *
 * Desliga a contagem pela interrupção, soma os pulsos contados dormindo e ressincroniza
 * os filtros.
 *
 * @note Chamar logo depois de acordar, antes de a tarefa de leitura voltar a correr.
 */
void io_tarefa_retoma(void);

/**
 * @brief Altera o tempo sem pulso necessário para poder dormir, em todas as entradas.
 *
 * O padrão é IO_PULSOS_TIMEOUT_PADRAO_MS. A gravação do valor entre reinícios fica para
 * o módulo de configuração.
 *
 * @param timeout_ms Novo tempo, em milissegundos; deve ser maior que zero.
 * @return IO_TAREFA_OK ou IO_TAREFA_ERRO.
 */
int32_t io_tarefa_define_inatividade(uint32_t timeout_ms);

/**
 * @brief Informa se todas as entradas ficaram sem pulso pelo tempo de inatividade.
 *
 * @return true se nenhuma entrada recebeu pulso no tempo configurado.
 */
bool io_tarefa_pode_dormir(void);

/**
 * @brief Soma os pulsos contados dormindo e ressincroniza os filtros com o estado atual.
 *
 * Chamar logo depois de acordar do sleep. Os pulsos vêm da interrupção externa
 * (io_entrada_coleta_pulsos). O estado lido agora evita contar de novo um pulso que ainda
 * esteja ativo.
 */
void io_tarefa_trata_despertar(void);

/**
 * @brief Lê o estado das entradas sem filtro.
 *
 * @return Máscara de bits, igual a io_tarefa_estado(), mas da última leitura direta.
 */
uint8_t io_tarefa_bruto(void);

#endif /* IO_TAREFA_H */
