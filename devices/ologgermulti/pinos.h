/**
 * @file pinos.h
 * @brief Pinagem da placa OloggerMulti (STM32L412C8U6).
 *
 * Cada função tem um par de macros: PINO_<FUNCAO>_PORTA e PINO_<FUNCAO>_NUM.
 * O código de módulo usa só esses nomes, nunca o número do pino. Origem: netlist
 * do esquema (U1), conferida em 02/11/2026. Pinos a validar na bancada estão
 * marcados com "validar".
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef PINOS_H
#define PINOS_H

#include "hal_stm32.h"

/* Entradas analógicas 4-20 mA. Atenção: o canal 1 está no PA1 e o 2 no PA0. */
#define PINO_ENTRADA_4_20MA_1_PORTA GPIOA
#define PINO_ENTRADA_4_20MA_1_NUM   GPIO_PIN_1
#define PINO_ENTRADA_4_20MA_2_PORTA GPIOA
#define PINO_ENTRADA_4_20MA_2_NUM   GPIO_PIN_0
#define PINO_ENTRADA_4_20MA_3_PORTA GPIOB
#define PINO_ENTRADA_4_20MA_3_NUM   GPIO_PIN_0
#define PINO_ENTRADA_4_20MA_4_PORTA GPIOB
#define PINO_ENTRADA_4_20MA_4_NUM   GPIO_PIN_1

/* Entradas digitais. */
#define PINO_ENTRADA_DIGITAL_1_PORTA GPIOB
#define PINO_ENTRADA_DIGITAL_1_NUM   GPIO_PIN_7
#define PINO_ENTRADA_DIGITAL_2_PORTA GPIOB
#define PINO_ENTRADA_DIGITAL_2_NUM   GPIO_PIN_8
#define PINO_ENTRADA_DIGITAL_3_PORTA GPIOB
#define PINO_ENTRADA_DIGITAL_3_NUM   GPIO_PIN_6
#define PINO_ENTRADA_DIGITAL_4_PORTA GPIOB
#define PINO_ENTRADA_DIGITAL_4_NUM   GPIO_PIN_5
/**
 * Nível do pino quando a entrada de campo está ativa (com tensão). O optoacoplador
 * TLP291 conduz e puxa o pino para o terra, então ativo = nível baixo. Origem: netlist
 * entradas_digitais.xml, U2 coletor em MCU-Dx com R7 a R10 (10k) para +3,3 V.
 */
#define PINO_ENTRADA_DIGITAL_NIVEL_ATIVO GPIO_PIN_RESET

/* Saídas digitais, por transistor. */
#define PINO_SAIDA_DIGITAL_1_PORTA GPIOB
#define PINO_SAIDA_DIGITAL_1_NUM   GPIO_PIN_12
#define PINO_SAIDA_DIGITAL_2_PORTA GPIOB
#define PINO_SAIDA_DIGITAL_2_NUM   GPIO_PIN_13
#define PINO_SAIDA_DIGITAL_3_PORTA GPIOB
#define PINO_SAIDA_DIGITAL_3_NUM   GPIO_PIN_14
#define PINO_SAIDA_DIGITAL_4_PORTA GPIOB
#define PINO_SAIDA_DIGITAL_4_NUM   GPIO_PIN_15

/* RS-485 (USART2). DE e /RE ligados à mesma rede. */
#define PINO_RS485_TX_PORTA    GPIOA
#define PINO_RS485_TX_NUM      GPIO_PIN_2
#define PINO_RS485_RX_PORTA    GPIOA
#define PINO_RS485_RX_NUM      GPIO_PIN_3
#define PINO_RS485_DE_RE_PORTA GPIOB
#define PINO_RS485_DE_RE_NUM   GPIO_PIN_9

/* UART para o módulo ESP32-C6 (USART3). */
#define PINO_ESP32_TX_PORTA GPIOB
#define PINO_ESP32_TX_NUM   GPIO_PIN_10
#define PINO_ESP32_RX_PORTA GPIOB
#define PINO_ESP32_RX_NUM   GPIO_PIN_11

/* Serial externa (USART1), para rádio externo em modo transparente. */
#define PINO_SERIAL_EXTERNA_TX_PORTA  GPIOA
#define PINO_SERIAL_EXTERNA_TX_NUM    GPIO_PIN_9
#define PINO_SERIAL_EXTERNA_RX_PORTA  GPIOA
#define PINO_SERIAL_EXTERNA_RX_NUM    GPIO_PIN_10
#define PINO_SERIAL_EXTERNA_RTS_PORTA GPIOB
#define PINO_SERIAL_EXTERNA_RTS_NUM   GPIO_PIN_3
#define PINO_SERIAL_EXTERNA_CTS_PORTA GPIOB
#define PINO_SERIAL_EXTERNA_CTS_NUM   GPIO_PIN_4

/* Flash SPI externa (SPI1). */
#define PINO_FLASH_CS_PORTA       GPIOA
#define PINO_FLASH_CS_NUM         GPIO_PIN_4
#define PINO_FLASH_SCK_PORTA      GPIOA
#define PINO_FLASH_SCK_NUM        GPIO_PIN_5
#define PINO_FLASH_MISO_PORTA     GPIOA
#define PINO_FLASH_MISO_NUM       GPIO_PIN_6
#define PINO_FLASH_MOSI_PORTA     GPIOA
#define PINO_FLASH_MOSI_NUM       GPIO_PIN_7
/* Jumper da flash (rede FLASH-IO). Finalidade a validar na bancada. */
#define PINO_FLASH_JUMPER_PORTA   GPIOC
#define PINO_FLASH_JUMPER_NUM     GPIO_PIN_13

/* USB. A placa liga o conector, mas o uso no firmware não está previsto. */
#define PINO_USB_DM_PORTA GPIOA
#define PINO_USB_DM_NUM   GPIO_PIN_11
#define PINO_USB_DP_PORTA GPIOA
#define PINO_USB_DP_NUM   GPIO_PIN_12

/* Depuração por SWD. Não reconfigurar em produção sem decisão explícita. */
#define PINO_SWDIO_PORTA GPIOA
#define PINO_SWDIO_NUM   GPIO_PIN_13
#define PINO_SWCLK_PORTA GPIOA
#define PINO_SWCLK_NUM   GPIO_PIN_14

/* Saída de clock (rede MCLK). Finalidade a validar na bancada. */
#define PINO_MCLK_PORTA GPIOA
#define PINO_MCLK_NUM   GPIO_PIN_8

#endif /* PINOS_H */
