/**
 * @file io_pulsos.h
 * @brief Contador de pulsos de uma entrada digital, com controle de inatividade.
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/io). Conta cada
 * transição do estado filtrado para ativo. Guarda o instante do último pulso
 * para dizer quando a entrada ficou parada por tempo suficiente para o
 * equipamento dormir. Os pulsos contados dormindo entram por io_pulsos_soma().
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#ifndef IO_PULSOS_H
#define IO_PULSOS_H

#include <stdbool.h>
#include <stdint.h>

/** Tempo padrão sem pulso para o equipamento poder dormir, em milissegundos. */
#define IO_PULSOS_TIMEOUT_PADRAO_MS 60000U

/** Código de retorno: sucesso. */
#define IO_PULSOS_OK                0
/** Código de retorno: parâmetro nulo ou tempo de inatividade zero. */
#define IO_PULSOS_ERRO_PARAMETRO    (-1)

/** Contador de pulsos de uma entrada. */
typedef struct
{
  uint32_t contagem;   /**< Pulsos contados desde o início. */
  uint32_t ultimo_ms;  /**< Instante do último pulso, ou do início, em milissegundos. */
  uint32_t timeout_ms; /**< Tempo sem pulso para poder dormir, em milissegundos. */
  bool ativa_antes;    /**< Estado da entrada na atualização anterior. */
} io_pulsos_t;

/**
 * @brief Inicia o contador. O instante inicial conta como atividade.
 *
 * @param pulsos Contador a iniciar.
 * @param timeout_ms Tempo sem pulso para poder dormir; deve ser maior que zero.
 * @param estado_inicial Estado da entrada no início. Uma entrada já ativa não conta pulso.
 * @param agora_ms Instante atual, em milissegundos.
 * @return IO_PULSOS_OK ou IO_PULSOS_ERRO_PARAMETRO.
 */
int32_t io_pulsos_inicia(io_pulsos_t *pulsos, uint32_t timeout_ms, bool estado_inicial,
                         uint32_t agora_ms);

/**
 * @brief Altera o tempo sem pulso necessário para poder dormir.
 *
 * @param pulsos Contador iniciado com io_pulsos_inicia().
 * @param timeout_ms Novo tempo, em milissegundos; deve ser maior que zero.
 * @return IO_PULSOS_OK ou IO_PULSOS_ERRO_PARAMETRO.
 */
int32_t io_pulsos_define_timeout(io_pulsos_t *pulsos, uint32_t timeout_ms);

/**
 * @brief Recebe o estado filtrado da entrada e conta a transição para ativo.
 *
 * @param pulsos Contador iniciado com io_pulsos_inicia().
 * @param ativa Estado filtrado atual da entrada.
 * @param agora_ms Instante atual, em milissegundos.
 */
void io_pulsos_atualiza(io_pulsos_t *pulsos, bool ativa, uint32_t agora_ms);

/**
 * @brief Soma pulsos contados enquanto o equipamento dormia.
 *
 * Também sincroniza o estado da entrada com o valor lido ao acordar, para um pulso ainda
 * ativo nesse momento não ser contado de novo pela leitura periódica.
 *
 * @param pulsos Contador iniciado com io_pulsos_inicia().
 * @param quantidade Pulsos contados dormindo.
 * @param ativa_agora Estado da entrada lido ao acordar.
 * @param agora_ms Instante atual, em milissegundos.
 */
void io_pulsos_soma(io_pulsos_t *pulsos, uint32_t quantidade, bool ativa_agora, uint32_t agora_ms);

/**
 * @brief Informa se a entrada ficou sem pulso pelo tempo configurado.
 *
 * @param pulsos Contador iniciado com io_pulsos_inicia().
 * @param agora_ms Instante atual, em milissegundos.
 * @return true se passou ao menos o tempo de inatividade desde o último pulso.
 */
bool io_pulsos_pode_dormir(const io_pulsos_t *pulsos, uint32_t agora_ms);

/**
 * @brief Lê o total de pulsos contados.
 *
 * @param pulsos Contador iniciado com io_pulsos_inicia().
 * @return Pulsos contados desde o início.
 */
uint32_t io_pulsos_contagem(const io_pulsos_t *pulsos);

#endif /* IO_PULSOS_H */
