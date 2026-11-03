/**
 * @file stm32l4xx_hal_conf.h
 * @brief Configuração do HAL do STM32L4 para o firmware oLoggerIOT.
 *
 * Habilita só os módulos usados pelo firmware e fixa as frequências de
 * referência. Módulos novos entram aqui quando forem usados.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef STM32L4XX_HAL_CONF_H
#define STM32L4XX_HAL_CONF_H

/* Módulos do HAL habilitados. */
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED

/* Frequências de referência, em Hz. */
#define HSE_VALUE            8000000U
#define HSE_STARTUP_TIMEOUT  100U
#define HSI_VALUE            16000000U
#define HSI48_VALUE          48000000U
#define LSI_VALUE            32000U
#define LSE_VALUE            32768U
#define LSE_STARTUP_TIMEOUT  5000U
#define MSI_VALUE            4000000U
#define EXTERNAL_CLOCK_VALUE 8000000U
#define VDD_VALUE            3300U

/* Prioridade da interrupção do tick do HAL. */
#define TICK_INT_PRIORITY    0x0FU

/* Sem RTOS no build vazio. O FreeRTOS entra na FW-046. */
#define USE_RTOS             0U

/* Desempenho da flash e da memória. */
#define PREFETCH_ENABLE          1U
#define INSTRUCTION_CACHE_ENABLE 1U
#define DATA_CACHE_ENABLE        1U

/* Verificação de parâmetros desligada na imagem final. */
/* #define USE_FULL_ASSERT 1U */
#ifdef  USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/* Definições básicas do HAL e cabeçalhos dos módulos habilitados. */
#include "stm32l4xx_hal_def.h"

#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32l4xx_hal_rcc.h"
#endif
#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32l4xx_hal_dma.h"
#endif
#ifdef HAL_ADC_MODULE_ENABLED
  #include "stm32l4xx_hal_adc.h"
#endif
#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32l4xx_hal_tim.h"
#endif
#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32l4xx_hal_gpio.h"
#endif
#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32l4xx_hal_cortex.h"
#endif
#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32l4xx_hal_flash.h"
#endif
#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32l4xx_hal_pwr.h"
#endif

#endif /* STM32L4XX_HAL_CONF_H */
