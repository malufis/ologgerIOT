/**
 * @file config_tarefa.h
 * @brief Configuração do device no firmware: leitura e escrita pelo mapa Modbus, aplicação e gravação.
 *
 * Reúne a tabela de parâmetros (config.h) com o resto do firmware:
 *
 * - **Leitura e escrita:** config_tarefa_le() e config_tarefa_escreve() são as duas funções que a
 *   camada Modbus (registradores de retenção) e a MQTT (chaves iguais aos endereços) chamam. A
 *   escrita é atômica; só o que mudou é aplicado aos módulos (retardo, estabilidade, tipo da
 *   saída, frequências, fuso, sleep, inatividade da entrada digital).
 * - **Persistência (decidido em 04/11/2026):** toda a configuração e a agenda das saídas ficam num
 *   só registro do armazém (armazem.h), nas duas últimas páginas da flash interna
 *   (flash_interna.h). Uma tarefa o grava 5 segundos depois da última mudança, para juntar várias
 *   escritas do CCO numa só gravação e poupar a flash. Na partida, o registro é lido e aplicado;
 *   sem registro válido valem os padrões de fábrica.
 *
 * Uma queda de energia nos 5 segundos depois de uma escrita perde essa escrita; o registro anterior
 * continua válido. Os parâmetros sem módulo ainda (RS-485, intervalos de coleta e envio, modo da
 * entrada digital) ficam guardados e prontos para quando o módulo existir. Sem teste em hardware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef CONFIG_TAREFA_H
#define CONFIG_TAREFA_H

#include <stdint.h>

#include "config.h"

/** Segundos sem novas mudanças antes de gravar na flash. */
#define CONFIG_TAREFA_ESPERA_GRAVAR_S 5U
/** Tamanho da pilha da tarefa de gravação, em palavras de 32 bits (o registro fica nela). */
#define CONFIG_TAREFA_PILHA_PALAVRAS  640U

/** Código de retorno: sucesso. */
#define CONFIG_TAREFA_OK              0
/** Código de retorno: a tarefa não foi criada. */
#define CONFIG_TAREFA_ERRO_TAREFA     (-1)

/**
 * @brief Lê a configuração gravada, a aplica aos módulos e cria a tarefa de gravação.
 *
 * @return CONFIG_TAREFA_OK ou CONFIG_TAREFA_ERRO_TAREFA.
 * @note Chamar antes de vTaskStartScheduler, depois de iniciar as saídas, a agenda, as entradas
 *       digitais e a energia, porque aplica a configuração a eles.
 */
int32_t config_tarefa_inicia(void);

/**
 * @brief Lê registradores de retenção da configuração.
 *
 * @param endereco Primeiro endereço, em 5 dígitos (40001 em diante).
 * @param quantidade Quantidade de registradores.
 * @param registradores Recebe os valores.
 * @return Os códigos de config.h (CONFIG_OK, CONFIG_ERRO_ENDERECO, ...).
 */
int32_t config_tarefa_le(uint16_t endereco, uint16_t quantidade, uint16_t *registradores);

/**
 * @brief Grava registradores de retenção da configuração e aplica o que mudou.
 *
 * @param endereco Primeiro endereço, em 5 dígitos.
 * @param quantidade Quantidade de registradores.
 * @param registradores Valores a gravar.
 * @return Os códigos de config.h; com erro, nada é alterado nem aplicado.
 */
int32_t config_tarefa_escreve(uint16_t endereco, uint16_t quantidade, const uint16_t *registradores);

/**
 * @brief Copia a configuração atual (para os módulos que a leem em laço, como as regras do sensor).
 *
 * @param copia Recebe a configuração.
 */
void config_tarefa_copia(config_t *copia);

#endif /* CONFIG_TAREFA_H */
