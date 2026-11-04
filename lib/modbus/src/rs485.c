/**
 * @file rs485.c
 * @brief Implementação do driver da RS-485 (USART2) do STM32L412.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "rs485.h"

#include "FreeRTOS.h"
#include "task.h"

#include "hal_stm32.h"
#include "modbus_rtu.h"
#include "pinos.h"

/** Prioridade da interrupção do USART2: numericamente maior que o limite do FreeRTOS (5). */
#define PRIORIDADE_USART2 6U
/** Silêncio de fim de quadro, em tempos de bit (3,5 caracteres de 11 bits, arredondado). */
#define SILENCIO_BITS     40U
/** Velocidades, em bit/s, na ordem dos códigos do registrador 40400. */
static const uint32_t velocidades[RS485_VELOCIDADES] = {2400U, 9600U, 57600U, 115200U};

/** Buffer de recepção e estado, mexidos pela interrupção. */
static uint8_t rx[MODBUS_QUADRO_MAX];
static volatile size_t rx_n;
static volatile bool rx_pronto;
static volatile bool rx_erro;
/** Estado do envio. */
static const uint8_t *tx;
static volatile size_t tx_n;
static volatile size_t tx_i;
static volatile bool tx_ativo;
/** Tarefa avisada pela interrupção. */
static volatile TaskHandle_t tarefa_aviso;
static uint32_t velocidade_atual = 9600U;

/** Coloca o DE/RE em nível alto (transmite) ou baixo (recebe). */
static void define_direcao(bool transmite)
{
  HAL_GPIO_WritePin(PINO_RS485_DE_RE_PORTA, PINO_RS485_DE_RE_NUM, transmite ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/** Configura os pinos: TX e RX na função alternativa 7 (USART2) e DE/RE como saída em nível baixo. */
static void configura_pinos(void)
{
  GPIO_InitTypeDef gpio = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(PINO_RS485_DE_RE_PORTA, PINO_RS485_DE_RE_NUM, GPIO_PIN_RESET);
  gpio.Pin = PINO_RS485_DE_RE_NUM;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PINO_RS485_DE_RE_PORTA, &gpio);

  gpio.Pin = PINO_RS485_TX_NUM | PINO_RS485_RX_NUM;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(PINO_RS485_TX_PORTA, &gpio);
}

/**
 * @brief Calcula o divisor da velocidade.
 *
 * @param codigo Código da velocidade, de 0 a 3.
 * @return Valor de BRR.
 */
static uint32_t divisor(uint16_t codigo)
{
  uint32_t bps = velocidades[codigo];

  velocidade_atual = bps;
  return (HAL_RCC_GetPCLK1Freq() + (bps / 2U)) / bps;
}

int32_t rs485_define_velocidade(uint16_t codigo)
{
  if (codigo >= RS485_VELOCIDADES)
  {
    return RS485_ERRO_PARAMETRO;
  }
  USART2->CR1 &= ~USART_CR1_UE;
  USART2->BRR = divisor(codigo);
  USART2->CR1 |= USART_CR1_UE;
  return RS485_OK;
}

int32_t rs485_inicia(uint16_t codigo)
{
  if (codigo >= RS485_VELOCIDADES)
  {
    return RS485_ERRO_PARAMETRO;
  }
  configura_pinos();
  __HAL_RCC_USART2_CLK_ENABLE();

  USART2->CR1 = 0U;
  USART2->CR2 = USART_CR2_RTOEN;
  USART2->CR3 = 0U;
  USART2->RTOR = SILENCIO_BITS;
  USART2->BRR = divisor(codigo);
  USART2->ICR = 0xFFFFFFFFU;
  USART2->CR1 = USART_CR1_UE | USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE | USART_CR1_RTOIE;

  HAL_NVIC_SetPriority(USART2_IRQn, PRIORIDADE_USART2, 0U);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  return RS485_OK;
}

size_t rs485_recebe(uint8_t *quadro, size_t capacidade, uint32_t espera_ms)
{
  size_t n = 0U;
  size_t i;

  if ((quadro == NULL) || (capacidade == 0U))
  {
    return 0U;
  }
  tarefa_aviso = xTaskGetCurrentTaskHandle();
  if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(espera_ms)) == 0U)
  {
    return 0U;
  }

  taskENTER_CRITICAL();
  if (rx_pronto && !rx_erro && (rx_n <= capacidade))
  {
    n = rx_n;
    for (i = 0U; i < n; i++)
    {
      quadro[i] = rx[i];
    }
  }
  rx_n = 0U;
  rx_pronto = false;
  rx_erro = false;
  taskEXIT_CRITICAL();
  return n;
}

int32_t rs485_envia(const uint8_t *quadro, size_t tamanho)
{
  /* Tempo de envio: 11 bits por byte, mais uma folga de 50 ms. */
  uint32_t limite_ms = (uint32_t)(((tamanho * 11U * 1000U) / velocidade_atual) + 50U);

  if ((quadro == NULL) || (tamanho == 0U) || (tamanho > MODBUS_QUADRO_MAX))
  {
    return RS485_ERRO_PARAMETRO;
  }
  tarefa_aviso = xTaskGetCurrentTaskHandle();
  (void)ulTaskNotifyTake(pdTRUE, 0U); /* Limpa um aviso antigo. */

  taskENTER_CRITICAL();
  tx = quadro;
  tx_n = tamanho;
  tx_i = 0U;
  tx_ativo = true;
  rx_n = 0U;
  taskEXIT_CRITICAL();

  define_direcao(true);
  USART2->CR1 |= USART_CR1_TXEIE;
  if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(limite_ms)) == 0U)
  {
    USART2->CR1 &= ~(USART_CR1_TXEIE | USART_CR1_TCIE);
    tx_ativo = false;
    define_direcao(false);
    return RS485_ERRO_TEMPO;
  }
  return RS485_OK;
}

/** Trata o fim do silêncio: o quadro recebido está completo. */
static void quadro_completo(BaseType_t *acordou)
{
  USART2->ICR = USART_ICR_RTOCF;
  if ((rx_n > 0U) && !tx_ativo)
  {
    rx_pronto = true;
    if (tarefa_aviso != NULL)
    {
      vTaskNotifyGiveFromISR(tarefa_aviso, acordou);
    }
  }
}

/** Trata o byte recebido (ou o descarta durante o envio ou em estouro do buffer). */
static void byte_recebido(void)
{
  uint8_t b = (uint8_t)USART2->RDR;

  if (tx_ativo)
  {
    return;
  }
  if (rx_n < MODBUS_QUADRO_MAX)
  {
    rx[rx_n] = b;
    rx_n++;
  }
  else
  {
    rx_erro = true;
  }
}

/** Trata o envio: próximo byte ou fim da transmissão. */
static void envio(uint32_t isr, BaseType_t *acordou)
{
  if (((isr & USART_ISR_TXE) != 0U) && ((USART2->CR1 & USART_CR1_TXEIE) != 0U))
  {
    if (tx_i < tx_n)
    {
      USART2->TDR = tx[tx_i];
      tx_i++;
    }
    else
    {
      USART2->CR1 = (USART2->CR1 & ~USART_CR1_TXEIE) | USART_CR1_TCIE;
    }
  }
  if (((isr & USART_ISR_TC) != 0U) && ((USART2->CR1 & USART_CR1_TCIE) != 0U))
  {
    USART2->ICR = USART_ICR_TCCF;
    USART2->CR1 &= ~USART_CR1_TCIE;
    define_direcao(false);
    tx_ativo = false;
    if (tarefa_aviso != NULL)
    {
      vTaskNotifyGiveFromISR(tarefa_aviso, acordou);
    }
  }
}

void rs485_irq(void)
{
  uint32_t isr = USART2->ISR;
  BaseType_t acordou = pdFALSE;

  if ((isr & (USART_ISR_ORE | USART_ISR_FE | USART_ISR_NE | USART_ISR_PE)) != 0U)
  {
    USART2->ICR = USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_NECF | USART_ICR_PECF;
    rx_erro = true;
  }
  if ((isr & USART_ISR_RXNE) != 0U)
  {
    byte_recebido();
  }
  if ((isr & USART_ISR_RTOF) != 0U)
  {
    quadro_completo(&acordou);
  }
  envio(isr, &acordou);
  portYIELD_FROM_ISR(acordou);
}
