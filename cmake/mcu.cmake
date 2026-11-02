# -----------------------------------------------------------------------------
# Opções de CPU e FPU de cada MCU do projeto.
#
# Entrada: OLOG_MCU, definida por builds/composicao.cmake.
# Saída:   OLOG_MCU_CPU_FLAGS, usada nos alvos do firmware.
#
# MCU novo entra aqui antes de aparecer em um build.
# -----------------------------------------------------------------------------

if(OLOG_MCU STREQUAL "STM32L412C8")
  set(OLOG_MCU_CPU_FLAGS -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
elseif(OLOG_MCU STREQUAL "STM32L071CB")
  set(OLOG_MCU_CPU_FLAGS -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft)
else()
  message(FATAL_ERROR "MCU '${OLOG_MCU}' sem mapeamento em cmake/mcu.cmake.")
endif()
