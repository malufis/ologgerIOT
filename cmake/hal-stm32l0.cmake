# -----------------------------------------------------------------------------
# HAL, CMSIS e startup do STM32L0 a partir do pacote STM32CubeL0.
#
# Entrada: OLOG_STM32CUBE_L0, pasta do pacote, por exemplo
#          C:/Users/<usuario>/STM32Cube/Repository/STM32Cube_FW_L0_V1.12.4
# Saídas:  alvo hal_stm32l0, e as variáveis OLOG_HAL_TARGET,
#          OLOG_STARTUP, OLOG_LINKER_SCRIPT e OLOG_HAL_DEFINE usadas em src/.
# -----------------------------------------------------------------------------

if(NOT OLOG_STM32CUBE_L0)
  message(FATAL_ERROR "Informe -DOLOG_STM32CUBE_L0=<pasta do STM32Cube_FW_L0>.")
endif()

set(L0 ${OLOG_STM32CUBE_L0})
if(NOT EXISTS ${L0}/Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal.h)
  message(FATAL_ERROR "Pacote STM32CubeL0 não encontrado em ${L0}.")
endif()

set(OLOG_HAL_TARGET hal_stm32l0)
set(OLOG_HAL_DEFINE STM32L071xx)
set(OLOG_STARTUP ${L0}/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/gcc/startup_stm32l071xx.s)
set(OLOG_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/devices/y7080/STM32L071CBU6_FLASH.ld)

add_library(${OLOG_HAL_TARGET} STATIC
  ${L0}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c
  ${L0}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c
  ${L0}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c
  ${L0}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ex.c
  ${L0}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c
  ${L0}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.c
  ${L0}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c
  ${L0}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c
  ${L0}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c
  ${L0}/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/system_stm32l0xx.c
)

target_include_directories(${OLOG_HAL_TARGET} PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${L0}/Drivers/STM32L0xx_HAL_Driver/Inc
  ${L0}/Drivers/CMSIS/Device/ST/STM32L0xx/Include
  ${L0}/Drivers/CMSIS/Include
)

target_compile_definitions(${OLOG_HAL_TARGET} PUBLIC ${OLOG_HAL_DEFINE} USE_HAL_DRIVER)
