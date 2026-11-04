/**
 * @file modbus_tarefa.h
 * @brief Tarefa do escravo Modbus RTU na RS-485.
 *
 * Espera quadros na RS-485 (rs485.h), os processa com o mapa do device (modbus_mapa.h) e responde. O
 * endereço do escravo, a velocidade e o modo vêm da configuração (registradores 40402, 40400 e 40401):
 *
 * - **Endereço:** 1 a 247, padrão 1. A difusão (endereço 0) só vale para escrita e não é respondida.
 * - **Velocidade:** código 0 a 3 (2400, 9600, 57600 e 115200 bps), padrão 9600. Uma mudança vale a partir
 *   do quadro seguinte à resposta que a confirma.
 * - **Modo:** 1 = escravo (padrão). O modo mestre ainda não existe: com 0, a tarefa não atende nada.
 *
 * A tarefa só atende com o sleep desabilitado (o USART não recebe em Stop 2). Sem teste em hardware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef MODBUS_TAREFA_H
#define MODBUS_TAREFA_H

#include <stdint.h>

/** Tamanho da pilha da tarefa, em palavras de 32 bits. */
#define MODBUS_TAREFA_PILHA_PALAVRAS 512U
/** Espera por um quadro a cada volta, em milissegundos (também o período de conferência da configuração). */
#define MODBUS_TAREFA_ESPERA_MS      200U

/** Código de retorno: sucesso. */
#define MODBUS_TAREFA_OK             0
/** Código de retorno: a tarefa ou a RS-485 não foi iniciada. */
#define MODBUS_TAREFA_ERRO           (-1)

/**
 * @brief Inicia a RS-485 com a velocidade da configuração e cria a tarefa do escravo.
 *
 * @return MODBUS_TAREFA_OK ou MODBUS_TAREFA_ERRO.
 * @note Chamar antes de vTaskStartScheduler, depois de config_tarefa_inicia().
 */
int32_t modbus_tarefa_inicia(void);

#endif /* MODBUS_TAREFA_H */
