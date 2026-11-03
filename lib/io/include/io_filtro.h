/**
 * @file io_filtro.h
 * @brief Filtro de ruído de uma entrada digital discreta.
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/io). A mudança de
 * estado só é aceita depois de um número de amostras seguidas com o mesmo nível
 * oposto ao estado atual. O tempo do filtro é esse número multiplicado pelo
 * período de leitura, que quem chama define.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef IO_FILTRO_H
#define IO_FILTRO_H

#include <stdbool.h>
#include <stdint.h>

/** Código de retorno: sucesso. */
#define IO_FILTRO_OK               0
/** Código de retorno: parâmetro nulo ou fora da faixa. */
#define IO_FILTRO_ERRO_PARAMETRO   (-1)
/** Tempo padrão de estabilidade de uma entrada digital, em milissegundos. */
#define IO_FILTRO_TEMPO_PADRAO_MS  50U
/** Tempo de estabilidade usado na contagem de pulsos, em milissegundos. */
#define IO_FILTRO_TEMPO_PULSO_MS   20U

/** Filtro de ruído de uma entrada digital. */
typedef struct
{
  uint16_t amostras_estaveis; /**< Amostras seguidas do nível oposto para aceitar a mudança. */
  uint16_t contagem;          /**< Amostras seguidas do nível oposto já lidas. */
  bool estado;                /**< Estado aceito da entrada: true = ativa (nível alto). */
} io_filtro_t;

/**
 * @brief Inicia o filtro com o estado aceito e o número de amostras de estabilidade.
 *
 * @param filtro Filtro a iniciar.
 * @param amostras_estaveis Amostras seguidas para aceitar uma mudança; mínimo 1.
 * @param estado_inicial Estado aceito no início.
 * @return IO_FILTRO_OK ou IO_FILTRO_ERRO_PARAMETRO.
 */
int32_t io_filtro_inicia(io_filtro_t *filtro, uint16_t amostras_estaveis, bool estado_inicial);

/**
 * @brief Converte um tempo de estabilidade em número de amostras, arredondando para cima.
 *
 * Arredondar para cima garante que o filtro nunca fique mais curto que o tempo pedido.
 *
 * @param tempo_ms Tempo de estabilidade em milissegundos; deve ser maior que zero.
 * @param periodo_us Período entre duas leituras da entrada, em microssegundos; deve ser maior que zero.
 * @param amostras_estaveis Saída: número de amostras, pelo menos 1.
 * @return IO_FILTRO_OK ou IO_FILTRO_ERRO_PARAMETRO (nulo, zero ou número fora de uint16_t).
 */
int32_t io_filtro_amostras_do_tempo(uint32_t tempo_ms, uint32_t periodo_us, uint16_t *amostras_estaveis);

/**
 * @brief Recebe uma amostra do nível da entrada e devolve o estado aceito.
 *
 * @param filtro Filtro iniciado com io_filtro_inicia().
 * @param nivel Nível lido nesta amostra: true = nível alto.
 * @return Estado aceito após esta amostra.
 */
bool io_filtro_atualiza(io_filtro_t *filtro, bool nivel);

#endif /* IO_FILTRO_H */
