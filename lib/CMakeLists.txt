# -----------------------------------------------------------------------------
# Módulos do firmware. Cada módulo é uma biblioteca com include/ e src/.
# Módulo novo entra aqui; módulo de funcionalidade opcional entra dentro de
# if(<FUNCIONALIDADE> IN_LIST OLOG_FEATURES).
# -----------------------------------------------------------------------------

add_library(rtos STATIC rtos/src/rtos.c)
target_include_directories(rtos PUBLIC rtos/include)
target_link_libraries(rtos PUBLIC freertos)

add_library(vida STATIC vida/src/vida.c)
target_include_directories(vida PUBLIC vida/include)
target_link_libraries(vida PUBLIC rtos freertos)

# Entradas e saídas digitais (lib/io). Cada funcionalidade traz os seus arquivos. A lógica
# pura (io_filtro, io_pulsos, io_totalizador, saida) é testada no PC em tests/io.
set(IO_FONTES)
if(ENTRADA_DIGITAL IN_LIST OLOG_FEATURES)
  list(APPEND IO_FONTES io/src/io_filtro.c io/src/io_entrada.c io/src/io_pulsos.c
    io/src/io_backup.c io/src/io_totalizador.c io/src/io_tarefa.c)
endif()
if(SAIDA_DISCRETA IN_LIST OLOG_FEATURES)
  list(APPEND IO_FONTES io/src/saida.c io/src/controle_saida.c io/src/io_saida.c io/src/io_saidas.c)
endif()
if(IO_FONTES)
  add_library(io STATIC ${IO_FONTES})
  target_include_directories(io PUBLIC io/include)
  target_link_libraries(io PUBLIC ${OLOG_HAL_TARGET} rtos freertos)
endif()

# Gerência de energia com Stop 2. Existe só para o STM32L4; o do STM32L0 (y7080) é a fazer.
if(OLOG_MCU STREQUAL "STM32L412C8")
  add_library(energia STATIC energia/src/energia.c)
  target_include_directories(energia PUBLIC energia/include)
  target_link_libraries(energia PUBLIC ${OLOG_HAL_TARGET} rtos freertos)
endif()

# Média por segundo e conversão 4-20 mA. Lógica pura, testada no PC (tests/).
if(ENTRADA_4_20MA IN_LIST OLOG_FEATURES)
  add_library(sensor STATIC sensor/src/sensor.c)
  target_include_directories(sensor PUBLIC sensor/include)

  # O driver do ADC existe só para o STM32L412. O do STM32L071 (y7080) é a fazer.
  if(OLOG_MCU STREQUAL "STM32L412C8")
    add_library(adc STATIC adc/src/adc.c)
    target_include_directories(adc PUBLIC adc/include)
    target_link_libraries(adc PUBLIC sensor ${OLOG_HAL_TARGET})

    add_library(aquisicao STATIC aquisicao/src/aquisicao.c)
    target_include_directories(aquisicao PUBLIC aquisicao/include)
    target_link_libraries(aquisicao PUBLIC adc sensor rtos freertos)
  endif()
endif()
