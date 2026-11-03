/**
 * @file io_saida.c
 * @brief Implementação do driver dos pinos das saídas digitais.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#include "io_saida.h"

#include <stddef.h>

#include "hal_stm32.h"
#include "pinos.h"

/** Porta e número do pino de uma saída. */
typedef struct
{
  GPIO_TypeDef *porta; /**< Porta do GPIO. */
  uint16_t pino;       /**< Máscara do pino na porta. */
} saida_pino_t;

/* Índice 0 é a saída 1. */
static const saida_pino_t pinos[IO_SAIDAS_QTD] = {
  {PINO_SAIDA_DIGITAL_1_PORTA, PINO_SAIDA_DIGITAL_1_NUM},
  {PINO_SAIDA_DIGITAL_2_PORTA, PINO_SAIDA_DIGITAL_2_NUM},
  {PINO_SAIDA_DIGITAL_3_PORTA, PINO_SAIDA_DIGITAL_3_NUM},
  {PINO_SAIDA_DIGITAL_4_PORTA, PINO_SAIDA_DIGITAL_4_NUM},
};

/**
 * @brief Nível do pino para o estado pedido.
 *
 * @param ligada true para a saída ligada.
 * @return Nível a escrever no pino.
 */
static GPIO_PinState nivel_do_estado(bool ligada)
{
  if (ligada)
  {
    return PINO_SAIDA_DIGITAL_NIVEL_LIGADO;
  }
  return (PINO_SAIDA_DIGITAL_NIVEL_LIGADO == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET;
}

int32_t io_saida_inicia(void)
{
  GPIO_InitTypeDef gpio = {0};
  uint8_t i;

  /* As quatro saídas estão na porta B (pinos.h). */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  for (i = 0U; i < IO_SAIDAS_QTD; i++)
  {
    /* Nível desligado no registrador antes de ligar o driver do pino: sem pulso na partida. */
    HAL_GPIO_WritePin(pinos[i].porta, pinos[i].pino, nivel_do_estado(false));
    gpio.Pin = pinos[i].pino;
    HAL_GPIO_Init(pinos[i].porta, &gpio);
  }
  return IO_SAIDA_OK;
}

int32_t io_saida_define(uint8_t canal, bool ligada)
{
  if ((canal < 1U) || (canal > IO_SAIDAS_QTD))
  {
    return IO_SAIDA_ERRO_PARAMETRO;
  }

  HAL_GPIO_WritePin(pinos[canal - 1U].porta, pinos[canal - 1U].pino, nivel_do_estado(ligada));
  return IO_SAIDA_OK;
}

int32_t io_saida_le(uint8_t canal, bool *ligada)
{
  if ((ligada == NULL) || (canal < 1U) || (canal > IO_SAIDAS_QTD))
  {
    return IO_SAIDA_ERRO_PARAMETRO;
  }

  /* Com o pino como saída, o nível lido é o que ele comanda. */
  *ligada = (HAL_GPIO_ReadPin(pinos[canal - 1U].porta, pinos[canal - 1U].pino) ==
             PINO_SAIDA_DIGITAL_NIVEL_LIGADO);
  return IO_SAIDA_OK;
}
