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
 * **Sem hora válida ou sem fuso configurado, nada é pedido** e as saídas mantêm o estado. O fuso
 * é configuração enviada pelo CCO (40403, proposta): enquanto não chegar, a agenda não é aplicada
 * (proposta a confirmar; sem o fuso, a hora local seria um palpite).
 *
 * Limites desta versão: a tarefa depende do tick do FreeRTOS, que para em Stop 2, então o
 * despertar no limite do intervalo, com o alarme do RTC, é um passo futuro. A agenda e o fuso
 * ainda não são guardados em memória persistente e se perdem no reinício. Sem teste em hardware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef AGENDA_TAREFA_H
#define AGENDA_TAREFA_H

#include <stddef.h>
#include <stdint.h>

/** Período da tarefa, em milissegundos. */
#define AGENDA_TAREFA_PERIODO_MS     1000U
/** Tamanho da pilha da tarefa, em palavras de 32 bits. */
#define AGENDA_TAREFA_PILHA_PALAVRAS 192U

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
