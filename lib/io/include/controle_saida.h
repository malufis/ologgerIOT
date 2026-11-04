/**
 * @file controle_saida.h
 * @brief Modo manual e automático de uma saída digital e a prioridade entre as fontes.
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/io). Envolve o estado da saída
 * (saida.h, com o retardo de acionamento) e decide quem manda nela, conforme o B4:
 *
 * - **Manual:** um comando manual direto prevalece sobre qualquer regra automática. A saída
 *   passa a operar em modo manual e fica assim até receber o comando de modo automático. Em
 *   manual, os pedidos automáticos são guardados, mas não são aplicados.
 * - **Automático local:** quando a saída tem uma **regra local cadastrada** (entrada analógica,
 *   máximo, mínimo e, para a saída em frequência, a frequência), a saída segue só o pedido
 *   dessa regra, e os comandos remotos são ignorados.
 * - **Automático remoto:** quando **não há regra local definida**, a saída segue os comandos
 *   automáticos do CCO (regra da plataforma ou agenda offline, que tem origem no CCO).
 * - **Falha do sensor:** em automático, a saída desliga. Depois que a falha termina, a saída só
 *   volta a operar pelo sensor quando ele ficar **estável por 5 minutos** (configurável); até lá
 *   continua desligada. Em manual, o comando manual se mantém.
 * - **Volta ao automático:** a saída avalia e executa a regra válida naquele momento. Sem
 *   nenhum pedido válido, mantém o último estado válido do comando.
 *
 * Todo comando que liga a saída, manual ou automático, respeita o retardo de acionamento
 * (relé de tempo): ver saida.h. O módulo não gera o alerta ao CCO que acompanha a chegada de um
 * comando manual: quem chama usa o valor devolvido por controle_saida_comando_manual().
 * Depois de um reinício a saída volta desligada e em modo automático: nada disto é guardado.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.2.0
 */

#ifndef CONTROLE_SAIDA_H
#define CONTROLE_SAIDA_H

#include <stdbool.h>
#include <stdint.h>

#include "saida.h"

/** Código de retorno: sucesso. */
#define CONTROLE_SAIDA_OK             0
/** Código de retorno: parâmetro nulo. */
#define CONTROLE_SAIDA_ERRO_PARAMETRO (-1)

/** Tempo padrão de estabilidade do sensor depois de uma falha, em milissegundos (5 minutos). */
#define CONTROLE_ESTABILIDADE_PADRAO_MS 300000U

/** Quem manda na saída. */
typedef enum
{
  CONTROLE_MODO_AUTOMATICO = 0, /**< A saída segue os pedidos automáticos. */
  CONTROLE_MODO_MANUAL = 1      /**< A saída segue o último comando manual. */
} controle_modo_t;

/** O que uma fonte automática pede para a saída. */
typedef enum
{
  CONTROLE_PEDIDO_NENHUM = 0,  /**< A fonte não pede nada. */
  CONTROLE_PEDIDO_LIGA = 1,    /**< A fonte pede para ligar. */
  CONTROLE_PEDIDO_DESLIGA = 2  /**< A fonte pede para desligar. */
} controle_pedido_t;

/** Fontes de pedidos automáticos. */
typedef enum
{
  CONTROLE_FONTE_LOCAL = 0, /**< Regra local cadastrada: entrada analógica para a saída digital. */
  CONTROLE_FONTE_REMOTA = 1 /**< Comando automático do CCO: regra da plataforma ou agenda offline. */
} controle_fonte_t;

/** Quantidade de fontes automáticas. */
#define CONTROLE_FONTES 2U

/** Controle de uma saída. */
typedef struct
{
  saida_t saida;                              /**< Estado da saída, com o retardo. */
  controle_modo_t modo;                       /**< Quem manda na saída. */
  controle_pedido_t pedido[CONTROLE_FONTES];  /**< Último pedido de cada fonte automática. */
  bool regra_local;                           /**< true se a saída tem regra local cadastrada. */
  bool sensor_falha;                          /**< true se o sensor que controla a saída falhou. */
  bool recuperando;                           /**< true na espera de estabilidade depois da falha. */
  uint32_t fim_falha_ms;                      /**< Instante em que a falha terminou. */
  uint32_t estabilidade_ms;                   /**< Tempo de estabilidade exigido depois da falha. */
} controle_saida_t;

/**
 * @brief Inicia o controle: saída desligada, em modo automático, sem regra local, sem pedidos e
 *        sem falha, com a estabilidade padrão de 5 minutos.
 *
 * @param controle Controle a iniciar.
 * @return CONTROLE_SAIDA_OK ou CONTROLE_SAIDA_ERRO_PARAMETRO.
 */
int32_t controle_saida_inicia(controle_saida_t *controle);

/**
 * @brief Define o retardo de acionamento da saída, em milissegundos.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @param atraso_ms Retardo; zero liga na hora.
 */
void controle_saida_define_atraso(controle_saida_t *controle, uint32_t atraso_ms);

/**
 * @brief Define o tempo de estabilidade do sensor depois de uma falha, em milissegundos.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @param estabilidade_ms Tempo exigido sem falha antes de voltar a operar pelo sensor.
 */
void controle_saida_define_estabilidade(controle_saida_t *controle, uint32_t estabilidade_ms);

/**
 * @brief Informa se a saída tem uma regra local cadastrada.
 *
 * Com regra local, a saída segue só o pedido dela e ignora os comandos remotos. Sem regra local,
 * vale o comando automático remoto do CCO.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @param definida true se há regra local cadastrada para a saída.
 * @param agora_ms Instante atual, em milissegundos.
 */
void controle_saida_define_regra_local(controle_saida_t *controle, bool definida, uint32_t agora_ms);

/**
 * @brief Comando manual direto: passa a saída ao modo manual e liga ou desliga.
 *
 * Prevalece sobre qualquer pedido automático. Desligar cancela um retardo em curso; ligar
 * respeita o retardo e é descartado se a saída já está ligada ou aguardando.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @param liga true para ligar, false para desligar.
 * @param agora_ms Instante atual, em milissegundos.
 * @return true se a saída estava em modo automático e passou a manual. A chegada de todo comando
 *         manual gera um alerta ao CCO; o valor devolvido diz se houve a mudança de modo.
 */
bool controle_saida_comando_manual(controle_saida_t *controle, bool liga, uint32_t agora_ms);

/**
 * @brief Comando de modo automático: a saída avalia e executa a regra válida na hora.
 *
 * Sem nenhum pedido válido, a saída mantém o último estado válido do comando.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @param agora_ms Instante atual, em milissegundos.
 */
void controle_saida_modo_automatico(controle_saida_t *controle, uint32_t agora_ms);

/**
 * @brief Registra o pedido de uma fonte automática e, em modo automático, o aplica.
 *
 * Em modo manual o pedido é só guardado, para valer na volta ao automático. O pedido continua
 * valendo até a fonte mudá-lo; a fonte pede "nenhum" quando não tem opinião. Com regra local
 * definida, o pedido remoto fica guardado e não é aplicado.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @param fonte Fonte do pedido.
 * @param pedido O que a fonte pede.
 * @param agora_ms Instante atual, em milissegundos.
 */
void controle_saida_pedido(controle_saida_t *controle, controle_fonte_t fonte,
                           controle_pedido_t pedido, uint32_t agora_ms);

/**
 * @brief Informa se o sensor que controla a saída falhou.
 *
 * Em modo automático, a falha desliga a saída. Quando ela termina, a saída só volta a operar
 * pelo sensor depois do tempo de estabilidade (5 minutos por padrão) sem nova falha; uma falha
 * dentro desse tempo reinicia a contagem. Em modo manual, a saída mantém o comando manual.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @param falha true se o sensor falhou.
 * @param agora_ms Instante atual, em milissegundos.
 */
void controle_saida_sensor_falha(controle_saida_t *controle, bool falha, uint32_t agora_ms);

/**
 * @brief Atualiza o estado com o instante atual e devolve se a saída está ligada.
 *
 * Encerra a espera de estabilidade do sensor, se ela acabou, e liga a saída ao fim do retardo.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @param agora_ms Instante atual, em milissegundos.
 * @return true se a saída está ligada depois da atualização.
 */
bool controle_saida_atualiza(controle_saida_t *controle, uint32_t agora_ms);

/**
 * @brief Lê o modo da saída.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @return Modo manual ou automático.
 */
controle_modo_t controle_saida_modo(const controle_saida_t *controle);

/**
 * @brief Lê se a saída está ligada.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @return true se a saída está ligada.
 */
bool controle_saida_ligada(const controle_saida_t *controle);

/**
 * @brief Lê se a saída espera o retardo de acionamento.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @return true se há um retardo em curso.
 */
bool controle_saida_aguardando(const controle_saida_t *controle);

/**
 * @brief Lê se o sensor ainda está em falha ou esperando a estabilidade depois dela.
 *
 * @param controle Controle iniciado com controle_saida_inicia().
 * @return true se a saída ainda não pode operar pelo sensor.
 */
bool controle_saida_sensor_indisponivel(const controle_saida_t *controle);

#endif /* CONTROLE_SAIDA_H */
