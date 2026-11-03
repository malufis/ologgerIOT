/**
 * @file adc.c
 * @brief Implementação do driver do ADC com TIM6, ADC1 e DMA1 canal 1 (STM32L412).
 *
 * Canais do ADC1 confirmados no banco de dados do STM32CubeMX:
 * PA1 = IN6, PA0 = IN5, PB0 = IN15, PB1 = IN16.
 *
 * @date criacao 02/11/2026
 * @date modificacao 03/11/2026
 * @version 0.2.0
 */

#include "adc.h"

#include <stddef.h>

#include "hal_stm32.h"
#include "pinos.h"
#include "sensor.h"

/** Tick do timer que dispara o ADC, em hertz. */
#define ADC_TIMER_TICK_HZ       100000U
/** Quantidade de interrupções de metade por segundo. */
#define ADC_METADES_POR_SEG     10U
/** Prioridade da interrupção do DMA: não pode ser mais urgente que a do FreeRTOS. */
#define ADC_PRIORIDADE_DMA      6U

/* Estado do módulo. */
static ADC_HandleTypeDef hadc;
static DMA_HandleTypeDef hdma;
static TIM_HandleTypeDef htim;
static uint16_t buffer[2U * ADC_QUADROS_MEIA_MAX * ADC_SEQ_CANAIS] __attribute__((aligned(4)));
static adc_callback_t callback_usuario;
static uint16_t quadros_por_meia;
static uint16_t taxa_atual_sps;

/**
 * @brief Configura os quatro pinos das entradas 4-20 mA como analógicos.
 */
static void configura_pinos(void)
{
  GPIO_InitTypeDef gpio = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  gpio.Mode = GPIO_MODE_ANALOG;
  gpio.Pull = GPIO_NOPULL;

  gpio.Pin = PINO_ENTRADA_4_20MA_1_NUM | PINO_ENTRADA_4_20MA_2_NUM;
  HAL_GPIO_Init(PINO_ENTRADA_4_20MA_1_PORTA, &gpio);
  gpio.Pin = PINO_ENTRADA_4_20MA_3_NUM | PINO_ENTRADA_4_20MA_4_NUM;
  HAL_GPIO_Init(PINO_ENTRADA_4_20MA_3_PORTA, &gpio);
}

/**
 * @brief Configura o DMA circular do ADC, em palavras de 16 bits.
 *
 * @return ADC_OK ou ADC_ERRO_HAL.
 */
static int32_t configura_dma(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma.Instance = DMA1_Channel1;
  hdma.Init.Request = DMA_REQUEST_0;
  hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma.Init.MemInc = DMA_MINC_ENABLE;
  hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma.Init.Mode = DMA_CIRCULAR;
  hdma.Init.Priority = DMA_PRIORITY_HIGH;

  if (HAL_DMA_Init(&hdma) != HAL_OK)
  {
    return ADC_ERRO_HAL;
  }
  __HAL_LINKDMA(&hadc, DMA_Handle, hdma);

  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, ADC_PRIORIDADE_DMA, 0U);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

  return ADC_OK;
}

/**
 * @brief Configura o ADC1 para varrer as entradas a cada disparo do TIM6.
 *
 * @return ADC_OK ou ADC_ERRO_HAL.
 */
static int32_t configura_adc(void)
{
  __HAL_RCC_ADC_CLK_ENABLE();

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = ADC_SEQ_CANAIS;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T6_TRGO;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc.Init.OversamplingMode = DISABLE;

  return (HAL_ADC_Init(&hadc) == HAL_OK) ? ADC_OK : ADC_ERRO_HAL;
}

/**
 * @brief Configura a sequência de canais: entradas 1, 2, 3 e 4, nesta ordem.
 *
 * @return ADC_OK ou ADC_ERRO_HAL.
 */
static int32_t configura_sequencia(void)
{
  static const uint32_t canais[ADC_SEQ_CANAIS] = {ADC_CHANNEL_6, ADC_CHANNEL_5, ADC_CHANNEL_15,
                                                  ADC_CHANNEL_16, ADC_CHANNEL_VREFINT};
  static const uint32_t ranks[ADC_SEQ_CANAIS] = {ADC_REGULAR_RANK_1, ADC_REGULAR_RANK_2,
                                                 ADC_REGULAR_RANK_3, ADC_REGULAR_RANK_4,
                                                 ADC_REGULAR_RANK_5};
  ADC_ChannelConfTypeDef cfg = {0};
  uint32_t i;

  cfg.SingleDiff = ADC_SINGLE_ENDED;
  cfg.OffsetNumber = ADC_OFFSET_NONE;
  cfg.Offset = 0U;

  for (i = 0U; i < ADC_SEQ_CANAIS; i++)
  {
    /* VREFINT precisa de tempo de amostragem longo (manual: mínimo de 4 us). */
    cfg.SamplingTime = (canais[i] == ADC_CHANNEL_VREFINT) ? ADC_SAMPLETIME_640CYCLES_5
                                                          : ADC_SAMPLETIME_24CYCLES_5;
    cfg.Channel = canais[i];
    cfg.Rank = ranks[i];
    if (HAL_ADC_ConfigChannel(&hadc, &cfg) != HAL_OK)
    {
      return ADC_ERRO_HAL;
    }
  }
  return ADC_OK;
}

/**
 * @brief Configura o TIM6 para gerar o disparo do ADC (TRGO) na taxa pedida.
 *
 * @param[in] taxa_sps Amostras por segundo por entrada.
 * @return    ADC_OK ou ADC_ERRO_HAL.
 */
static int32_t configura_timer(uint16_t taxa_sps)
{
  TIM_MasterConfigTypeDef mestre = {0};
  uint32_t clk = HAL_RCC_GetPCLK1Freq();

  if ((RCC->CFGR & RCC_CFGR_PPRE1_2) != 0U)
  {
    clk *= 2U; /* Com prescaler do APB1 maior que 1, o timer roda ao dobro. */
  }

  __HAL_RCC_TIM6_CLK_ENABLE();
  htim.Instance = TIM6;
  htim.Init.Prescaler = (clk / ADC_TIMER_TICK_HZ) - 1U;
  htim.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim.Init.Period = ((ADC_TIMER_TICK_HZ + (taxa_sps / 2U)) / taxa_sps) - 1U;
  htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  mestre.MasterOutputTrigger = TIM_TRGO_UPDATE;
  mestre.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if ((HAL_TIM_Base_Init(&htim) != HAL_OK) ||
      (HAL_TIMEx_MasterConfigSynchronization(&htim, &mestre) != HAL_OK))
  {
    return ADC_ERRO_HAL;
  }
  return ADC_OK;
}

/* Diagnóstico de bancada: conversões simples antes da aquisição contínua.
   [0] VREFINT, [1] entrada 4 com amostragem longa, [2] entrada 4 com amostragem
   curta, [3] código de retorno (0 = sucesso). Lidas por SWD. */
static volatile uint32_t diag_adc[8];

/**
 * @brief Converte um canal uma vez, disparado por software, e devolve a contagem.
 *
 * @param[in]  canal       Canal do ADC1.
 * @param[in]  amostragem  Tempo de amostragem do HAL.
 * @param[out] valor       Contagem obtida.
 * @return     ADC_OK ou ADC_ERRO_HAL.
 */
static int32_t converte_uma_vez(uint32_t canal, uint32_t amostragem, uint32_t *valor)
{
  ADC_ChannelConfTypeDef cfg = {0};

  cfg.Channel = canal;
  cfg.Rank = ADC_REGULAR_RANK_1;
  cfg.SamplingTime = amostragem;
  cfg.SingleDiff = ADC_SINGLE_ENDED;
  cfg.OffsetNumber = ADC_OFFSET_NONE;

  if ((HAL_ADC_ConfigChannel(&hadc, &cfg) != HAL_OK) || (HAL_ADC_Start(&hadc) != HAL_OK) ||
      (HAL_ADC_PollForConversion(&hadc, 10U) != HAL_OK))
  {
    return ADC_ERRO_HAL;
  }
  *valor = HAL_ADC_GetValue(&hadc);
  /* Estado bruto logo após a conversão, para a bancada: DR, ISR, CR e estado do HAL. */
  diag_adc[4] = ADC1->DR;
  diag_adc[5] = ADC1->ISR;
  diag_adc[6] = ADC1->CR;
  diag_adc[7] = HAL_ADC_GetState(&hadc);
  return (HAL_ADC_Stop(&hadc) == HAL_OK) ? ADC_OK : ADC_ERRO_HAL;
}

/**
 * @brief Prepara o ADC1 para conversões simples, disparadas por software.
 *
 * @return ADC_OK ou ADC_ERRO_HAL.
 */
static int32_t prepara_diagnostico(void)
{
  __HAL_RCC_ADC_CLK_ENABLE();

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 1U;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc.Init.OversamplingMode = DISABLE;

  if ((HAL_ADC_Init(&hadc) != HAL_OK) ||
      (HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED) != HAL_OK))
  {
    return ADC_ERRO_HAL;
  }
  ADC12_COMMON->CCR |= ADC_CCR_VREFEN;
  HAL_Delay(2U); /* Partida da referência interna. */
  return ADC_OK;
}

void adc_diagnostico(void)
{
  uint32_t valor = 0U;
  int32_t erro;

  diag_adc[3] = 0xFFFFFFFFU; /* Marca "em andamento": distingue de "não executou". */
  configura_pinos();
  erro = prepara_diagnostico();

  if ((erro == ADC_OK) &&
      ((erro = converte_uma_vez(ADC_CHANNEL_VREFINT, ADC_SAMPLETIME_640CYCLES_5, &valor)) == ADC_OK))
  {
    diag_adc[0] = valor;
  }
  if ((erro == ADC_OK) &&
      ((erro = converte_uma_vez(ADC_CHANNEL_16, ADC_SAMPLETIME_640CYCLES_5, &valor)) == ADC_OK))
  {
    diag_adc[1] = valor;
  }
  if ((erro == ADC_OK) &&
      ((erro = converte_uma_vez(ADC_CHANNEL_16, ADC_SAMPLETIME_24CYCLES_5, &valor)) == ADC_OK))
  {
    diag_adc[2] = valor;
  }
  diag_adc[3] = (uint32_t)(-erro);
  (void)HAL_ADC_DeInit(&hadc);
}

/**
 * @brief Configura todo o hardware e executa os passos em sequência.
 *
 * @param[in] taxa_sps Amostras por segundo por entrada.
 * @return    ADC_OK ou ADC_ERRO_HAL.
 */
static int32_t configura_tudo(uint16_t taxa_sps)
{
  configura_pinos();

  if ((configura_adc() != ADC_OK) || (configura_dma() != ADC_OK) ||
      (configura_sequencia() != ADC_OK) || (configura_timer(taxa_sps) != ADC_OK))
  {
    return ADC_ERRO_HAL;
  }
  return ADC_OK;
}

int32_t adc_inicia(uint16_t taxa_sps, adc_callback_t callback)
{
  uint32_t total;

  if ((callback == NULL) || (taxa_sps < SENSOR_TAXA_MIN_SPS) || (taxa_sps > SENSOR_TAXA_MAX_SPS))
  {
    return ADC_ERRO_PARAMETRO;
  }

  callback_usuario = callback;
  taxa_atual_sps = taxa_sps;
  quadros_por_meia = (uint16_t)(taxa_sps / ADC_METADES_POR_SEG);
  if (quadros_por_meia == 0U)
  {
    quadros_por_meia = 1U;
  }
  total = 2U * (uint32_t)quadros_por_meia * ADC_SEQ_CANAIS;

  if (configura_tudo(taxa_sps) != ADC_OK)
  {
    return ADC_ERRO_HAL;
  }
  /* Habilita a referência interna no registrador comum do ADC (necessário para VREFINT). */
  ADC12_COMMON->CCR |= ADC_CCR_VREFEN;
  if (HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED) != HAL_OK)
  {
    return ADC_ERRO_HAL;
  }
  /* MISRA-DEV: regra 11.5, a API do HAL recebe o buffer como uint32_t* mesmo em modo de 16 bits */
  if (HAL_ADC_Start_DMA(&hadc, (uint32_t *)(void *)buffer, total) != HAL_OK)
  {
    return ADC_ERRO_HAL;
  }

  return (HAL_TIM_Base_Start(&htim) == HAL_OK) ? ADC_OK : ADC_ERRO_HAL;
}

int32_t adc_para(void)
{
  int32_t erro = ADC_OK;

  if ((HAL_TIM_Base_Stop(&htim) != HAL_OK) || (HAL_ADC_Stop_DMA(&hadc) != HAL_OK))
  {
    erro = ADC_ERRO_HAL;
  }
  ADC12_COMMON->CCR &= ~ADC_CCR_VREFEN; /* Desliga a referência interna. */
  if (HAL_ADC_DeInit(&hadc) != HAL_OK)
  {
    erro = ADC_ERRO_HAL;
  }
  return erro;
}

int32_t adc_retoma(void)
{
  if (callback_usuario == NULL)
  {
    return ADC_ERRO_PARAMETRO;
  }
  return adc_inicia(taxa_atual_sps, callback_usuario);
}

/**
 * @brief Tratador da interrupção do DMA1 canal 1, que atende o ADC1.
 */
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma);
}

/**
 * @brief Chamada do HAL quando a primeira metade do buffer enche.
 *
 * @param[in] adc_hal Manipulador do ADC que gerou o evento.
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *adc_hal)
{
  (void)adc_hal;
  callback_usuario(buffer, quadros_por_meia);
}

/**
 * @brief Chamada do HAL quando a segunda metade do buffer enche.
 *
 * @param[in] adc_hal Manipulador do ADC que gerou o evento.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc_hal)
{
  (void)adc_hal;
  callback_usuario(&buffer[(uint32_t)quadros_por_meia * ADC_SEQ_CANAIS], quadros_por_meia);
}
