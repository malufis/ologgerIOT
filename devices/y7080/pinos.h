/**
 * @file pinos.h
 * @brief Pinagem da placa y7080 (STM32L071CBU6).
 *
 * Cada função tem um par de macros: PINO_<FUNCAO>_PORTA e PINO_<FUNCAO>_NUM.
 * O código de módulo usa só esses nomes, nunca o número do pino. Origem: netlist
 * do esquema (U2), conferida em 02/11/2026. Pinos a validar na bancada estão
 * marcados com "validar". Esta placa não tem entradas nem saídas digitais.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef PINOS_H
#define PINOS_H

#include "hal_stm32.h"

/* Entrada analógica 4-20 mA. */
#define PINO_ENTRADA_4_20MA_1_PORTA GPIOA
#define PINO_ENTRADA_4_20MA_1_NUM   GPIO_PIN_0

/* Medição diferencial: saída do INA350, monitor da alimentação e controles. */
#define PINO_DIFERENCIAL_SENSE_PORTA       GPIOB
#define PINO_DIFERENCIAL_SENSE_NUM         GPIO_PIN_1
#define PINO_DIFERENCIAL_MONITOR_VCC_PORTA GPIOB
#define PINO_DIFERENCIAL_MONITOR_VCC_NUM   GPIO_PIN_0
#define PINO_DIFERENCIAL_PWR_EN_PORTA      GPIOB
#define PINO_DIFERENCIAL_PWR_EN_NUM        GPIO_PIN_13
#define PINO_DIFERENCIAL_EN_K8_PORTA       GPIOB
#define PINO_DIFERENCIAL_EN_K8_NUM         GPIO_PIN_15
/* Excitação do sensor PC11. O nome do sensor não indica o pino PC11 do MCU. */
#define PINO_DIFERENCIAL_EN_PC11_PORTA     GPIOA
#define PINO_DIFERENCIAL_EN_PC11_NUM       GPIO_PIN_15

/* Linha externa de interrupção (rede INTERRUPT, com R23 não montado). Validar. */
#define PINO_INTERRUPCAO_PORTA GPIOB
#define PINO_INTERRUPCAO_NUM   GPIO_PIN_12

/* Modem celular: UART de comandos AT (LPUART1) e liga/desliga. */
#define PINO_MODEM_TX_PORTA     GPIOB
#define PINO_MODEM_TX_NUM       GPIO_PIN_10
#define PINO_MODEM_RX_PORTA     GPIOB
#define PINO_MODEM_RX_NUM       GPIO_PIN_11
/* PWRKEY pelo R22 de 1 kOhm. Nível, modo de saída e temporização a validar. */
#define PINO_MODEM_PWRKEY_PORTA GPIOC
#define PINO_MODEM_PWRKEY_NUM   GPIO_PIN_13

/* RS-485 (USART2). DE e /RE ligados à mesma rede. */
#define PINO_RS485_TX_PORTA    GPIOA
#define PINO_RS485_TX_NUM      GPIO_PIN_2
#define PINO_RS485_RX_PORTA    GPIOA
#define PINO_RS485_RX_NUM      GPIO_PIN_3
#define PINO_RS485_DE_RE_PORTA GPIOB
#define PINO_RS485_DE_RE_NUM   GPIO_PIN_9

/* Serial externa (USART1), nos conectores. */
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
#define PINO_FLASH_JUMPER_PORTA   GPIOB
#define PINO_FLASH_JUMPER_NUM     GPIO_PIN_14

/* USB. O STM32L071CB não tem periférico USB (achado HW-001): só os pinos. */
#define PINO_USB_DM_PORTA GPIOA
#define PINO_USB_DM_NUM   GPIO_PIN_11
#define PINO_USB_DP_PORTA GPIOA
#define PINO_USB_DP_NUM   GPIO_PIN_12

/* Depuração por SWD. Não reconfigurar em produção sem decisão explícita. */
#define PINO_SWDIO_PORTA GPIOA
#define PINO_SWDIO_NUM   GPIO_PIN_13
#define PINO_SWCLK_PORTA GPIOA
#define PINO_SWCLK_NUM   GPIO_PIN_14

#endif /* PINOS_H */
