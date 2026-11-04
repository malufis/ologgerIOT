/**
 * @file io_saidas.h
 * @brief Controle das quatro saídas digitais discretas: modo manual e automático e retardo.
 *
 * Reúne o controle de cada saída (controle_saida.h), que decide quem manda nela e aplica o
 * retardo de acionamento (relé de tempo), e o driver dos pinos (io_saida.h). Uma tarefa de baixa
 * prioridade liga as saídas cujo retardo de acionamento chegou ao fim. As saídas começam
 * desligadas e em modo automático.
 *
 * Os comandos que chegam do sistema são **manuais** (io_saidas_comando_manual) ou de **modo
 * automático** (io_saidas_modo_automatico). Os pedidos da regra do sensor local, da agenda
 * offline e da regra da plataforma entram por io_saidas_pedido(). O "acionamento por tempo" do
 * requisito é o retardo de acionamento (FW-024). A saída em frequência (pulsada) entra no
 * FW-023. Com o STM32 em Stop 2 o pino mantém o estado, mas o retardo não corre, porque o tick
 * do FreeRTOS para.
 *
 * @date criacao 03/11/2026
 * @date modificacao 04/11/2026
 * @version 0.4.0
 */

#ifndef IO_SAIDAS_H
#define IO_SAIDAS_H

#include <stdbool.h>
#include <stdint.h>

#include "controle_saida.h"
#include "io_saida.h"

/** Período da tarefa que liga as saídas ao fim do retardo, em milissegundos. */
#define IO_SAIDAS_PERIODO_MS     100U
/** Tamanho da pilha da tarefa, em palavras de 32 bits. */
#define IO_SAIDAS_PILHA_PALAVRAS 128U

/** Código de retorno: sucesso. */
#define IO_SAIDAS_OK             0
/** Código de retorno: canal inválido, retardo grande demais ou parâmetro nulo. */
#define IO_SAIDAS_ERRO_PARAMETRO (-1)
/** Código de retorno: a tarefa não foi criada. */
#define IO_SAIDAS_ERRO_TAREFA    (-2)

/**
 * @brief Leva os pinos das saídas ao estado desligado e zera os estados.
 *
 * @return IO_SAIDAS_OK.
 * @note Chamar o quanto antes na partida, antes de criar tarefas.
 */
int32_t io_saidas_prepara(void);

/**
 * @brief Cria a tarefa que liga as saídas ao fim do retardo, com memória estática.
 *
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_TAREFA.
 * @note Chamar antes de vTaskStartScheduler e depois de io_saidas_prepara().
 */
int32_t io_saidas_inicia(void);

/**
 * @brief Comando manual direto: passa a saída ao modo manual e a liga ou desliga.
 *
 * Prevalece sobre qualquer pedido automático e respeita o retardo de acionamento. A chegada de
 * todo comando manual gera um alerta ao CCO, que ainda não existe: ver `mudou_de_modo`.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param liga true para ligar, false para desligar.
 * @param mudou_de_modo Saída opcional (pode ser nula): true se a saída estava em automático.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_comando_manual(uint8_t canal, bool liga, bool *mudou_de_modo);

/**
 * @brief Comando de modo automático: a saída avalia e executa a regra válida na hora.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_modo_automatico(uint8_t canal);

/**
 * @brief Registra o pedido de uma fonte automática: sensor, agenda ou CCO.
 *
 * Em modo automático é aplicado na hora; em manual fica guardado para a volta ao automático. Com
 * regra do sensor definida (io_saidas_define_regra_sensor), vale só o pedido do sensor. Sem ela,
 * vale o pedido do CCO com o device online e o da agenda com o device offline.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param fonte Fonte do pedido.
 * @param pedido O que a fonte pede; "nenhum" quando não tem opinião.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_pedido(uint8_t canal, controle_fonte_t fonte, controle_pedido_t pedido);

/**
 * @brief Informa se o sensor que controla a saída falhou.
 *
 * Em automático a falha desliga a saída; em manual o comando manual se mantém.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param falha true se o sensor falhou.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_sensor_falha(uint8_t canal, bool falha);

/**
 * @brief Informa se a saída tem uma regra do sensor cadastrada (entrada analógica para a saída).
 *
 * Com regra do sensor, a saída segue só o pedido dela e ignora a agenda e o CCO. Sem regra do
 * sensor, vale a agenda (offline) ou o comando do CCO (online).
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param definida true se há regra do sensor cadastrada para a saída.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_define_regra_sensor(uint8_t canal, bool definida);

/**
 * @brief Informa se o device tem conectividade com o CCO, para todas as saídas.
 *
 * Online, vale o comando automático do CCO; offline, vale a agenda local. Ao ficar online, as
 * saídas mantêm o último estado e esperam um novo comando do CCO.
 *
 * @param online true se há conectividade.
 * @return IO_SAIDAS_OK.
 */
int32_t io_saidas_define_conectividade(bool online);

/**
 * @brief Define o tempo de estabilidade do sensor depois de uma falha.
 *
 * Depois que a falha termina, a saída só volta a operar pelo sensor depois deste tempo sem nova
 * falha. O padrão é de 5 minutos.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param segundos Tempo em segundos, de 0 a 4 294 967.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_define_estabilidade(uint8_t canal, uint32_t segundos);

/**
 * @brief Define o retardo de acionamento (relé de tempo) de uma saída.
 *
 * Vale para os próximos comandos que ligam a saída. Um retardo já em curso não muda. Corresponde
 * ao retardo de acionamento do mapa Modbus (40256 a 40270).
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param segundos Retardo em segundos, de 0 (liga na hora) a 4 294 967.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_define_atraso(uint8_t canal, uint32_t segundos);

/**
 * @brief Lê o estado de uma saída.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param ligada Saída: true se a saída está ligada.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_estado(uint8_t canal, bool *ligada);

/**
 * @brief Informa se uma saída está esperando o retardo de acionamento.
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param aguardando Saída: true se há um retardo em curso; a saída continua desligada.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_aguardando(uint8_t canal, bool *aguardando);

/**
 * @brief Lê o modo de uma saída (manual ou automático).
 *
 * @param canal Saída de 1 a IO_SAIDAS_QTD.
 * @param modo Saída: modo atual da saída.
 * @return IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO.
 */
int32_t io_saidas_modo(uint8_t canal, controle_modo_t *modo);

#endif /* IO_SAIDAS_H */
