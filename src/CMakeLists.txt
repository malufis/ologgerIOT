# -----------------------------------------------------------------------------
# Imagem STM32 do oLoggerIOT. Vale para os dois MCUs: o HAL, o startup e o
# linker script vêm das variáveis definidas em cmake/hal-stm32*.cmake.
# -----------------------------------------------------------------------------

add_executable(ologgeriot_stm32
  main.c
  ${OLOG_STARTUP}
)

target_link_libraries(ologgeriot_stm32 PRIVATE ${OLOG_HAL_TARGET})
set_target_properties(ologgeriot_stm32 PROPERTIES SUFFIX ".elf")

target_link_options(ologgeriot_stm32 PRIVATE
  ${OLOG_MCU_CPU_FLAGS}
  -T${OLOG_LINKER_SCRIPT}
  -specs=nano.specs
  -Wl,--gc-sections
  -Wl,-Map=$<TARGET_FILE_DIR:ologgeriot_stm32>/ologgeriot_stm32.map
)

add_custom_command(TARGET ologgeriot_stm32 POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:ologgeriot_stm32> ologgeriot_stm32.bin
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:ologgeriot_stm32>
  COMMENT "Gerando .bin e medindo a imagem STM32"
)
