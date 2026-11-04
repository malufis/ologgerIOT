# -----------------------------------------------------------------------------
# Imagem STM32 do oLoggerIOT. Vale para os dois MCUs: o HAL, o startup e o
# linker script vêm das variáveis definidas em cmake/hal-stm32*.cmake.
# -----------------------------------------------------------------------------

add_executable(ologgeriot_stm32
  main.c
  interrupcoes.c
  ${OLOG_STARTUP}
)

target_link_libraries(ologgeriot_stm32 PRIVATE ${OLOG_HAL_TARGET} freertos rtos vida)

# Aquisição analógica: só nos builds com driver de ADC (hoje, o OloggerMulti).
if(TARGET aquisicao)
  target_link_libraries(ologgeriot_stm32 PRIVATE aquisicao)
  target_compile_definitions(ologgeriot_stm32 PRIVATE OLOG_TEM_AQUISICAO=1)
endif()
# Entradas e saídas digitais: só nos builds com ENTRADA_DIGITAL ou SAIDA_DISCRETA (hoje, o
# OloggerMulti).
if(TARGET io)
  target_link_libraries(ologgeriot_stm32 PRIVATE io)
  if(ENTRADA_DIGITAL IN_LIST OLOG_FEATURES)
    target_compile_definitions(ologgeriot_stm32 PRIVATE OLOG_TEM_ENTRADA_DIGITAL=1)
  endif()
  if(SAIDA_DISCRETA IN_LIST OLOG_FEATURES)
    target_compile_definitions(ologgeriot_stm32 PRIVATE OLOG_TEM_SAIDA_DISCRETA=1)
  endif()
  if(SAIDA_FREQUENCIA IN_LIST OLOG_FEATURES)
    target_compile_definitions(ologgeriot_stm32 PRIVATE OLOG_TEM_SAIDA_FREQUENCIA=1)
  endif()
endif()
# Relógio de tempo real: só nos builds com RELOGIO (hoje, o OloggerMulti).
if(TARGET rtc)
  target_link_libraries(ologgeriot_stm32 PRIVATE rtc)
  target_compile_definitions(ologgeriot_stm32 PRIVATE OLOG_TEM_RELOGIO=1)
endif()
# Aplicação das agendas pelo RTC: só nos builds com saídas digitais e relógio.
if(TARGET agenda_tarefa)
  target_link_libraries(ologgeriot_stm32 PRIVATE agenda_tarefa)
  target_compile_definitions(ologgeriot_stm32 PRIVATE OLOG_TEM_AGENDA=1)
endif()
# Configuração persistente e regras do sensor: só nos builds que têm tudo o que ela configura.
if(TARGET config_tarefa)
  target_link_libraries(ologgeriot_stm32 PRIVATE config_tarefa)
  target_compile_definitions(ologgeriot_stm32 PRIVATE OLOG_TEM_CONFIG=1)
endif()
# Gerência de energia: só nos builds com driver de Stop 2 (hoje, o OloggerMulti).
if(TARGET energia)
  target_link_libraries(ologgeriot_stm32 PRIVATE energia)
  target_compile_definitions(ologgeriot_stm32 PRIVATE OLOG_TEM_ENERGIA=1)
endif()
set_target_properties(ologgeriot_stm32 PROPERTIES SUFFIX ".elf")

target_link_options(ologgeriot_stm32 PRIVATE
  ${OLOG_MCU_CPU_FLAGS}
  -T${OLOG_LINKER_SCRIPT}
  -specs=nano.specs
  -Wl,--gc-sections
  # Firmware bare-metal: o segmento de carga reúne flash e RAM, o que o linker
  # novo reporta como RWX. É o layout do script ST e não afeta a execução.
  -Wl,--no-warn-rwx-segments
  -Wl,-Map=$<TARGET_FILE_DIR:ologgeriot_stm32>/ologgeriot_stm32.map
)

add_custom_command(TARGET ologgeriot_stm32 POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:ologgeriot_stm32> ologgeriot_stm32.bin
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:ologgeriot_stm32>
  COMMENT "Gerando .bin e medindo a imagem STM32"
)
