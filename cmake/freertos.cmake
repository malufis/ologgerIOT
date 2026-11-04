# -----------------------------------------------------------------------------
# FreeRTOS a partir do pacote STM32Cube do chip do build.
#
# Entradas: OLOG_FREERTOS_ROOT e OLOG_FREERTOS_PORT, definidas em
#           cmake/hal-stm32*.cmake, e OLOG_HAL_TARGET.
# Saída:    alvo freertos. Só alocação estática: não há arquivo de heap.
# A configuração é include/FreeRTOSConfig.h.
# -----------------------------------------------------------------------------

set(FR ${OLOG_FREERTOS_ROOT})
if(NOT EXISTS ${FR}/tasks.c)
  message(FATAL_ERROR "FreeRTOS não encontrado em ${FR}.")
endif()

add_library(freertos STATIC
  ${FR}/tasks.c
  ${FR}/queue.c
  ${FR}/list.c
  ${FR}/portable/GCC/${OLOG_FREERTOS_PORT}/port.c
)

target_include_directories(freertos PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${FR}/include
  ${FR}/portable/GCC/${OLOG_FREERTOS_PORT}
)

target_link_libraries(freertos PUBLIC ${OLOG_HAL_TARGET})
