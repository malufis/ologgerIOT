# -----------------------------------------------------------------------------
# HAL, CMSIS e startup do STM32L4 a partir do pacote STM32CubeL4.
#
# Entrada: OLOG_STM32CUBE_L4, pasta do pacote, por exemplo
#          C:/Users/<usuario>/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.2
# Saídas:  alvo hal_stm32l4, e as variáveis OLOG_HAL_TARGET,
#          OLOG_STARTUP, OLOG_LINKER_SCRIPT e OLOG_HAL_DEFINE usadas em src/.
# -----------------------------------------------------------------------------

if(NOT OLOG_STM32CUBE_L4)
  message(FATAL_ERROR "Informe -DOLOG_STM32CUBE_L4=<pasta do STM32Cube_FW_L4>.")
endif()

set(L4 ${OLOG_STM32CUBE_L4})
if(NOT EXISTS ${L4}/Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h)
  message(FATAL_ERROR "Pacote STM32CubeL4 não encontrado em ${L4}.")
endif()

set(OLOG_HAL_TARGET hal_stm32l4)
set(OLOG_HAL_DEFINE STM32L412xx)
set(OLOG_STARTUP ${L4}/Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/gcc/startup_stm32l412xx.s)
set(OLOG_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/devices/ologgermulti/STM32L412C8_FLASH.ld)
set(OLOG_FREERTOS_ROOT ${L4}/Middlewares/Third_Party/FreeRTOS/Source)
set(OLOG_FREERTOS_PORT ARM_CM4F)

add_library(${OLOG_HAL_TARGET} STATIC
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
  ${L4}/Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/system_stm32l4xx.c
)

target_include_directories(${OLOG_HAL_TARGET} PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${L4}/Drivers/STM32L4xx_HAL_Driver/Inc
  ${L4}/Drivers/CMSIS/Device/ST/STM32L4xx/Include
  ${L4}/Drivers/CMSIS/Include
)

target_compile_definitions(${OLOG_HAL_TARGET} PUBLIC ${OLOG_HAL_DEFINE} USE_HAL_DRIVER)
