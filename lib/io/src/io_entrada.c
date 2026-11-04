/**
 * @file io_entrada.c
 * @brief Implementação da leitura das entradas digitais D1 a D4.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#include "io_entrada.h"

#include <stddef.h>

#include "hal_stm32.h"
#include "io_totalizador.h"
#include "pinos.h"

/** Porta e número do pino de uma entrada. */
typedef struct
{
  GPIO_TypeDef *porta; /**< Porta do GPIO. */
  uint16_t pino;       /**< Máscara do pino na porta. */
} entrada_pino_t;

/** Prioridade da interrupção externa: o tratador não usa o FreeRTOS, e 6 segue o driver do ADC. */
#define IO_ENTRADA_PRIORIDADE_EXTI 6U

/* Pulsos contados pela interrupção externa desde a última coleta, por entrada. */
static volatile uint32_t pulsos_dormindo[IO_ENTRADAS_QTD];

/* Índice 0 é a entrada D1. */
static const entrada_pino_t pinos[IO_ENTRADAS_QTD] = {
  {PINO_ENTRADA_DIGITAL_1_PORTA, PINO_ENTRADA_DIGITAL_1_NUM},
  {PINO_ENTRADA_DIGITAL_2_PORTA, PINO_ENTRADA_DIGITAL_2_NUM},
  {PINO_ENTRADA_DIGITAL_3_PORTA, PINO_ENTRADA_DIGITAL_3_NUM},
  {PINO_ENTRADA_DIGITAL_4_PORTA, PINO_ENTRADA_DIGITAL_4_NUM},
};

int32_t io_entrada_inicia(void)
{
  GPIO_InitTypeDef gpio = {0};
  uint8_t i;

  /* As quatro entradas estão na porta B (pinos.h). */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Pull = GPIO_NOPULL;
  for (i = 0U; i < IO_ENTRADAS_QTD; i++)
  {
    gpio.Pin = pinos[i].pino;
    HAL_GPIO_Init(pinos[i].porta, &gpio);
  }
  return IO_OK;
}

int32_t io_entrada_acordar_habilita(void)
{
  GPIO_InitTypeDef gpio = {0};
  uint8_t i;

  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Só a borda que ativa a entrada acorda: é o início de um pulso, nunca o fim. */
  gpio.Mode = (PINO_ENTRADA_DIGITAL_NIVEL_ATIVO == GPIO_PIN_RESET) ? GPIO_MODE_IT_FALLING
                                                                    : GPIO_MODE_IT_RISING;
  gpio.Pull = GPIO_NOPULL;
  for (i = 0U; i < IO_ENTRADAS_QTD; i++)
  {
    gpio.Pin = pinos[i].pino;
    HAL_GPIO_Init(pinos[i].porta, &gpio);
    __HAL_GPIO_EXTI_CLEAR_IT(pinos[i].pino);
  }

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, IO_ENTRADA_PRIORIDADE_EXTI, 0U);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  return IO_OK;
}

void io_entrada_acordar_desabilita(void)
{
  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
  (void)io_entrada_inicia(); /* O modo de entrada simples também desliga a linha no EXTI. */
}

uint32_t io_entrada_coleta_pulsos(uint8_t canal)
{
  uint32_t primask;
  uint32_t total;

  if ((canal < 1U) || (canal > IO_ENTRADAS_QTD))
  {
    return 0U;
  }

  /* Seção crítica curta: a interrupção também altera o contador. */
  primask = __get_PRIMASK();
  __disable_irq();
  total = pulsos_dormindo[canal - 1U];
  pulsos_dormindo[canal - 1U] = 0U;
  __set_PRIMASK(primask);
  return total;
}

void io_entrada_irq(void)
{
  uint8_t i;

  for (i = 0U; i < IO_ENTRADAS_QTD; i++)
  {
    if (__HAL_GPIO_EXTI_GET_IT(pinos[i].pino) != 0U)
    {
      __HAL_GPIO_EXTI_CLEAR_IT(pinos[i].pino);
      pulsos_dormindo[i]++;
      (void)io_totalizador_soma((uint8_t)(i + 1U), 1U); /* Já gravado: não depende da RAM. */
    }
  }
}

int32_t io_entrada_le(uint8_t canal, bool *ativa)
{
  GPIO_PinState nivel;

  if ((ativa == NULL) || (canal < 1U) || (canal > IO_ENTRADAS_QTD))
  {
    return IO_ERRO_PARAMETRO;
  }

  nivel = HAL_GPIO_ReadPin(pinos[canal - 1U].porta, pinos[canal - 1U].pino);
  *ativa = (nivel == PINO_ENTRADA_DIGITAL_NIVEL_ATIVO);
  return IO_OK;
}
