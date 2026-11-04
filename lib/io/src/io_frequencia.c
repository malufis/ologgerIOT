/**
 * @file io_frequencia.c
 * @brief Implementação das saídas em frequência no firmware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.2.0
 */

#include "io_frequencia.h"

#include "FreeRTOS.h"
#include "task.h"

#include "gerador_freq.h"
#include "hal_stm32.h"
#include "io_saida.h"
#include "io_saidas.h"

/**
 * Prioridade da interrupção do TIM6. O valor 6 é numericamente maior que o limite de chamadas ao
 * FreeRTOS (5): as seções críticas do sistema a mascaram, e ela não chama o FreeRTOS.
 */
#define PRIORIDADE_TIM6 6U

/** Tipos das saídas; começam todas discretas (memória zerada = TIPO_SAIDA_DISCRETO). */
static tipos_saida_t tipos;
/** Geradores das saídas; o índice 0 é a saída 1, que não tem modo frequência. */
static gerador_freq_t geradores[TIPO_SAIDA_QTD];
/** Último nível escrito no pino de cada saída, para só escrever nas mudanças. */
static bool niveis[TIPO_SAIDA_QTD];

/**
 * @brief Liga o timer se alguma saída tem frequência, ou o desliga se nenhuma tem.
 *
 * Chamar dentro de seção crítica.
 */
static void atualiza_timer(void)
{
  bool algum = false;
  uint8_t i;

  for (i = 0U; i < TIPO_SAIDA_QTD; i++)
  {
    if (geradores[i].passo != 0U)
    {
      algum = true;
    }
  }
  if (algum)
  {
    TIM6->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    TIM6->CR1 &= ~TIM_CR1_CEN;
  }
}

int32_t io_frequencia_inicia(void)
{
  uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
  uint32_t relogio_timer = ((RCC->CFGR & RCC_CFGR_PPRE1) != 0U) ? (pclk1 * 2U) : pclk1;
  uint32_t periodo = relogio_timer / GERADOR_FREQ_TICK_HZ;

  if ((periodo < 2U) || (periodo > 65536U))
  {
    return IO_FREQUENCIA_ERRO_TIMER;
  }

  __HAL_RCC_TIM6_CLK_ENABLE();
  TIM6->CR1 = 0U;
  TIM6->PSC = 0U;
  TIM6->ARR = periodo - 1U;
  TIM6->EGR = TIM_EGR_UG;
  TIM6->SR = 0U;
  TIM6->DIER = TIM_DIER_UIE;
  HAL_NVIC_SetPriority(TIM6_IRQn, PRIORIDADE_TIM6, 0U);
  HAL_NVIC_EnableIRQ(TIM6_IRQn);
  return IO_FREQUENCIA_OK;
}

int32_t io_frequencia_define_tipo(uint8_t canal, uint16_t tipo, bool sleep_ativo)
{
  int32_t resultado;

  taskENTER_CRITICAL();
  resultado = tipos_saida_define(&tipos, canal, tipo, sleep_ativo);
  if (resultado == TIPO_SAIDA_OK)
  {
    (void)gerador_freq_limpa(&geradores[canal - 1U]);
    niveis[canal - 1U] = false;
    atualiza_timer();
  }
  taskEXIT_CRITICAL();

  if (resultado == TIPO_SAIDA_OK)
  {
    if (tipo == TIPO_SAIDA_FREQUENCIA)
    {
      (void)io_saidas_cede_pino(canal, true);
      (void)io_saida_define(canal, false); /* 0 Hz: nível baixo, transistor desligado. */
    }
    else
    {
      (void)io_saidas_cede_pino(canal, false);
    }
  }
  return resultado;
}

int32_t io_frequencia_tipo(uint8_t canal, uint16_t *tipo)
{
  int32_t resultado;

  taskENTER_CRITICAL();
  resultado = tipos_saida_tipo(&tipos, canal, tipo);
  taskEXIT_CRITICAL();
  return resultado;
}

int32_t io_frequencia_define_hz(uint8_t canal, float hz)
{
  uint16_t tipo = TIPO_SAIDA_DISCRETO;
  int32_t resultado;

  taskENTER_CRITICAL();
  resultado = tipos_saida_tipo(&tipos, canal, &tipo);
  if (resultado == TIPO_SAIDA_OK)
  {
    if (tipo != TIPO_SAIDA_FREQUENCIA)
    {
      resultado = IO_FREQUENCIA_ERRO_NAO_FREQUENCIA;
    }
    else if (gerador_freq_define(&geradores[canal - 1U], hz) != GERADOR_FREQ_OK)
    {
      resultado = IO_FREQUENCIA_ERRO_FREQUENCIA;
    }
    else
    {
      atualiza_timer();
    }
  }
  taskEXIT_CRITICAL();

  if ((resultado == IO_FREQUENCIA_OK) && (geradores[canal - 1U].passo == 0U))
  {
    (void)io_saida_define(canal, false); /* Parou: garante o nível baixo. */
    niveis[canal - 1U] = false;
  }
  return resultado;
}

bool io_frequencia_sleep_permitido(void)
{
  bool permitido;

  taskENTER_CRITICAL();
  permitido = !tipos_saida_algum_em_frequencia(&tipos);
  taskEXIT_CRITICAL();
  return permitido;
}

void io_frequencia_irq(void)
{
  uint8_t i;

  TIM6->SR = 0U;
  for (i = 0U; i < TIPO_SAIDA_QTD; i++)
  {
    if (geradores[i].passo != 0U)
    {
      bool nivel = gerador_freq_tick(&geradores[i]);

      if (nivel != niveis[i])
      {
        niveis[i] = nivel;
        (void)io_saida_define((uint8_t)(i + 1U), nivel);
      }
    }
  }
}
