/**
 * @file rs485.h
 * @brief Driver da RS-485 (USART2) do OloggerMulti: recepção de quadros e envio.
 *
 * A RS-485 usa o transceptor SP3485 na USART2 (PA2 TX e PA3 RX), com DE e /RE ligados ao PB9
 * (nível alto transmite). O formato é **8 bits, sem paridade, 1 stop bit (8N1)**; confirmado em
 * 04/11/2026 (P-048); o mapa Modbus não tem registrador de paridade.
 *
 * - **Fim do quadro:** o silêncio de 3,5 caracteres (40 bits) é medido pelo próprio USART (receiver
 *   timeout, RTOR), que interrompe quando a linha fica quieta. A interrupção entrega o quadro à tarefa.
 * - **Recepção:** byte a byte na interrupção para um buffer de MODBUS_QUADRO_MAX bytes. Um quadro com erro
 *   de paridade, de formato, de ruído ou maior que o buffer é descartado inteiro.
 * - **Envio:** liga o DE, envia por interrupção e desliga o DE quando o último bit sai (TC).
 * - **Velocidade:** código 0 = 2400, 1 = 9600, 2 = 57600, 3 = 115200 bps (registrador 40400). O divisor
 *   sai do clock do APB1 (hoje, o do MSI de 4 MHz); a 115200 o erro é de cerca de 0,8%.
 *
 * Com o sleep (Stop 2) habilitado, o USART não recebe: a RS-485 só funciona com o sleep desabilitado. Sem
 * teste em hardware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef RS485_H
#define RS485_H

#include <stddef.h>
#include <stdint.h>

/** Código de retorno: sucesso. */
#define RS485_OK              0
/** Código de retorno: parâmetro inválido. */
#define RS485_ERRO_PARAMETRO  (-1)
/** Código de retorno: o envio não terminou a tempo. */
#define RS485_ERRO_TEMPO      (-2)

/** Quantidade de velocidades (códigos de 0 a 3). */
#define RS485_VELOCIDADES     4U

/**
 * @brief Liga o USART2, os pinos e a interrupção, com a velocidade pedida.
 *
 * @param codigo Código da velocidade, de 0 a 3.
 * @return RS485_OK ou RS485_ERRO_PARAMETRO.
 */
int32_t rs485_inicia(uint16_t codigo);

/**
 * @brief Muda a velocidade (entre dois quadros).
 *
 * @param codigo Código da velocidade, de 0 a 3.
 * @return RS485_OK ou RS485_ERRO_PARAMETRO.
 */
int32_t rs485_define_velocidade(uint16_t codigo);

/**
 * @brief Espera um quadro completo e o copia.
 *
 * Bloqueia a tarefa que chama (só uma tarefa deve chamar). Devolve zero se o tempo acabou ou se o quadro
 * foi descartado por erro.
 *
 * @param quadro Recebe o quadro.
 * @param capacidade Tamanho de @p quadro.
 * @param espera_ms Tempo máximo de espera, em milissegundos.
 * @return Tamanho do quadro, ou 0.
 */
size_t rs485_recebe(uint8_t *quadro, size_t capacidade, uint32_t espera_ms);

/**
 * @brief Envia um quadro e espera o último bit sair.
 *
 * @param quadro Bytes a enviar.
 * @param tamanho Número de bytes, de 1 a 256.
 * @return RS485_OK, RS485_ERRO_PARAMETRO ou RS485_ERRO_TEMPO.
 */
int32_t rs485_envia(const uint8_t *quadro, size_t tamanho);

/**
 * @brief Atende a interrupção do USART2.
 *
 * Chamada só por src/interrupcoes.c.
 */
void rs485_irq(void);

#endif /* RS485_H */
