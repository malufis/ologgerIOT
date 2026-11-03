/**
 * @file saida.h
 * @brief Estado de uma saída digital discreta, com retardo de acionamento (relé de tempo).
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/io). Uma saída está desligada,
 * aguardando o retardo ou ligada. Se a saída tem retardo de acionamento configurado, o
 * comando de ligar espera esse retardo antes de ligar; com retardo zero liga na hora. O
 * "acionamento por tempo" do requisito é esse mesmo retardo (decisão de 03/11/2026): não há
 * um acionamento que desliga sozinho depois de um tempo.
 *
 * Regras do comando de ligar:
 * - com a saída desligada, começa o retardo (ou liga na hora, se o retardo for zero);
 * - com um retardo em curso, o comando é descartado e a contagem em curso continua;
 * - com a saída já ligada, o comando é descartado.
 *
 * Um comando de desligar cancela o retardo em curso (a partida) e desliga a saída.
 *
 * Quem usa o módulo chama saida_atualiza() periodicamente com o instante atual e aplica o
 * estado devolvido ao pino. O tempo é medido em milissegundos por diferença sem sinal, então
 * a volta do contador de milissegundos não atrapalha. O módulo não decide quem manda na saída
 * (manual, automático, sensor ou agenda): isso fica no item FW-025.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.3.0
 */

#ifndef SAIDA_H
#define SAIDA_H

#include <stdbool.h>
#include <stdint.h>

/** Código de retorno: sucesso. */
#define SAIDA_OK               0
/** Código de retorno: parâmetro nulo. */
#define SAIDA_ERRO_PARAMETRO   (-1)

/** Estado de uma saída digital discreta. */
typedef struct
{
  bool ligada;        /**< Estado atual: true = ligada. */
  bool aguardando;    /**< true durante o retardo de acionamento: ainda desligada. */
  uint32_t atraso_ms; /**< Retardo de acionamento configurado; zero = liga na hora. */
  uint32_t espera_ms; /**< Instante em que o retardo em curso começou. */
} saida_t;

/**
 * @brief Inicia a saída desligada e sem retardo.
 *
 * @param saida Saída a iniciar.
 * @return SAIDA_OK ou SAIDA_ERRO_PARAMETRO.
 */
int32_t saida_inicia(saida_t *saida);

/**
 * @brief Define o retardo de acionamento da saída.
 *
 * Vale para os próximos comandos de ligar. Um retardo já em curso não muda.
 *
 * @param saida Saída iniciada com saida_inicia().
 * @param atraso_ms Retardo em milissegundos; zero liga na hora.
 */
void saida_define_atraso(saida_t *saida, uint32_t atraso_ms);

/**
 * @brief Comando de ligar a saída, respeitando o retardo de acionamento.
 *
 * Descartado se a saída já está ligada ou se já há um retardo em curso.
 *
 * @param saida Saída iniciada com saida_inicia().
 * @param agora_ms Instante atual, em milissegundos.
 */
void saida_liga(saida_t *saida, uint32_t agora_ms);

/**
 * @brief Comando de desligar. Cancela o retardo em curso e desliga a saída.
 *
 * @param saida Saída iniciada com saida_inicia().
 */
void saida_desliga(saida_t *saida);

/**
 * @brief Atualiza o estado com o instante atual e devolve se a saída está ligada.
 *
 * Liga a saída se o retardo acabou.
 *
 * @param saida Saída iniciada com saida_inicia().
 * @param agora_ms Instante atual, em milissegundos.
 * @return true se a saída está ligada depois da atualização.
 */
bool saida_atualiza(saida_t *saida, uint32_t agora_ms);

/**
 * @brief Lê o estado da saída sem atualizar o retardo.
 *
 * @param saida Saída iniciada com saida_inicia().
 * @return true se a saída está ligada.
 */
bool saida_ligada(const saida_t *saida);

/**
 * @brief Informa se a saída está esperando o retardo de acionamento.
 *
 * @param saida Saída iniciada com saida_inicia().
 * @return true se há um retardo em curso; a saída continua desligada.
 */
bool saida_aguardando(const saida_t *saida);

#endif /* SAIDA_H */
