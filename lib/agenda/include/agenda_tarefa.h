/**
 * @file agenda_tarefa.h
 * @brief Tarefa que aplica as agendas das saídas pelo horário do RTC.
 *
 * Liga o relógio (rtc.h), a conversão para a hora local (tempo.h), o aplicador das agendas
 * (agenda_aplicador.h) e as saídas (io_saidas.h). Uma tarefa de baixa prioridade lê o RTC a
 * cada segundo, aplica o fuso horário e entrega às saídas, como pedido da fonte agenda, o estado
 * do intervalo atual: na primeira avaliação depois da partida, a cada virada de intervalo de 30
 * minutos e depois de carregar a agenda ou de mudar o fuso. A saída só aplica o pedido da agenda
 * com o device offline, em modo automático e sem regra do sensor (controle_saida.h).
 *
 * **A agenda só funciona com o modo sleep desabilitado** (decidido em 04/11/2026): com o sleep
 * habilitado nada é pedido. Assim a tarefa não precisa acordar o STM32 no limite do intervalo.
 * **Sem hora válida, nada é pedido** e as saídas mantêm o estado. O fuso é configuração enviada
 * pelo CCO (40403, proposta); até chegar, vale o de Brasília (UTC−3), que é o padrão da partida.
 *
 * **Persistência (decidido em 04/11/2026):** a agenda e o fuso gravados persistem ao reinício, mas
 * quem os grava é o módulo de configuração (config_tarefa.h), junto com todos os outros parâmetros:
 * ele lê a agenda por agenda_tarefa_exporta(), a devolve por agenda_tarefa_importa() e percebe as
 * mudanças por agenda_tarefa_alteracoes(). O fuso vem da configuração (40403). Sem teste em
 * hardware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.2.0
 */

#ifndef AGENDA_TAREFA_H
#define AGENDA_TAREFA_H

#include <stddef.h>
#include <stdint.h>

/** Período da tarefa, em milissegundos. */
#define AGENDA_TAREFA_PERIODO_MS     1000U
/** Tamanho da pilha da tarefa, em palavras de 32 bits. */
#define AGENDA_TAREFA_PILHA_PALAVRAS 192U

/** Fuso horário inicial, em minutos em relação ao UTC: Brasília (decidido em 04/11/2026). */
#define AGENDA_TAREFA_FUSO_PADRAO_MIN (-180)

/** Código de retorno: sucesso. */
#define AGENDA_TAREFA_OK             0
/** Código de retorno: a tarefa não foi criada. */
#define AGENDA_TAREFA_ERRO_TAREFA    (-1)
/** Código de retorno: fuso fora de -720 a +840 minutos. */
#define AGENDA_TAREFA_ERRO_FUSO      (-2)

/**
 * @brief Cria a tarefa que aplica as agendas, com memória estática.
 *
 * @return AGENDA_TAREFA_OK ou AGENDA_TAREFA_ERRO_TAREFA.
 * @note Chamar antes de vTaskStartScheduler, depois de io_saidas_prepara() e rtc_inicia().
 */
int32_t agenda_tarefa_inicia(void);

/**
 * @brief Define o fuso horário da instalação, enviado pelo CCO.
 *
 * Um fuso novo faz as saídas serem avaliadas de novo na próxima passada.
 *
 * @param fuso_min Minutos em relação ao UTC (−180 para Brasília), de −720 a +840.
 * @return AGENDA_TAREFA_OK ou AGENDA_TAREFA_ERRO_FUSO.
 */
int32_t agenda_tarefa_define_fuso(int16_t fuso_min);

/**
 * @brief Quantas vezes uma agenda foi alterada desde a partida.
 *
 * @return Contador de alterações; o módulo de configuração o compara para saber quando gravar.
 */
uint32_t agenda_tarefa_alteracoes(void);

/**
 * @brief Empacota as agendas para a gravação (formato de agenda_config.h).
 *
 * @param carga Recebe a carga.
 * @param capacidade Tamanho de @p carga; pelo menos AGENDA_CONFIG_TAMANHO.
 * @param tamanho Recebe o tamanho da carga.
 * @return AGENDA_CONFIG_OK ou AGENDA_CONFIG_ERRO_PARAMETRO.
 */
int32_t agenda_tarefa_exporta(uint8_t *carga, size_t capacidade, size_t *tamanho);

/**
 * @brief Restaura as agendas gravadas, só se a carga for válida por inteiro.
 *
 * @param carga Carga lida do armazém.
 * @param tamanho Tamanho da carga.
 * @return AGENDA_CONFIG_OK ou o erro de agenda_config.h (as agendas ficam como estavam).
 */
int32_t agenda_tarefa_importa(const uint8_t *carga, size_t tamanho);

/**
 * @brief Carrega a agenda de uma saída, recebida do CCO.
 *
 * Só troca a agenda se a cadeia for válida por inteiro; tamanho zero apaga a agenda. A saída é
 * avaliada de novo na próxima passada.
 *
 * @param canal Saída de 1 a 4.
 * @param hex Cadeia de 84 caracteres hexadecimais, ou vazia.
 * @param tamanho Número de caracteres de @p hex.
 * @return Os códigos de agenda.h: AGENDA_OK ou o erro de parâmetro, tamanho ou caractere.
 */
int32_t agenda_tarefa_carrega(uint8_t canal, const char *hex, size_t tamanho);

#endif /* AGENDA_TAREFA_H */
