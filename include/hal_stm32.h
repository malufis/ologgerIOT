/**
 * @file hal_stm32.h
 * @brief Cabeçalho único do HAL do STM32 para o código de aplicação.
 *
 * Escolhe o HAL do MCU do build pela macro do alvo, definida em
 * cmake/hal-stm32*.cmake. O código de aplicação inclui só este arquivo.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef HAL_STM32_H
#define HAL_STM32_H

#if defined(STM32L412xx)
  #include "stm32l4xx_hal.h"
#elif defined(STM32L071xx)
  #include "stm32l0xx_hal.h"
#else
  #error "MCU sem HAL mapeado em include/hal_stm32.h."
#endif

#endif /* HAL_STM32_H */
