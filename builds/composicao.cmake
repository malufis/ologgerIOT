# -----------------------------------------------------------------------------
# Composição dos builds do firmware oLoggerIOT
#
# Um único código-fonte (src/, lib/) contém todas as funcionalidades.
# Este arquivo diz o que entra em cada build (cada device).
#
# Uso:  cmake -DDEVICE=ologgermulti ...   ou   cmake -DDEVICE=y7080 ...
#
# Cada funcionalidade listada liga os módulos de lib/ dela e define
# OLOG_FEAT_<NOME> para o código C (#if OLOG_FEAT_<NOME>).
#
# Status: confirmada pelo responsável técnico em 02/11/2026.
# Itens marcados PENDENTE ficam fora até confirmação.
#
# Documento: documentacao/composicao_builds.md
# -----------------------------------------------------------------------------

# Catálogo: todas as funcionalidades que existem no projeto.
# Nome novo só entra aqui depois de existir o módulo correspondente em lib/.
set(OLOG_FEATURES_CATALOGO
  ENTRADA_4_20MA       # Entrada analógica 4–20 mA (lib/adc, lib/sensor)
  ENTRADA_DIFERENCIAL  # Entrada analógica diferencial (lib/adc)
  ENTRADA_DIGITAL      # Entrada digital (lib/io)
  SAIDA_DISCRETA       # Saída digital liga/desliga, por tempo e com retardo (lib/io)
  SAIDA_FREQUENCIA     # Saída digital em Hz proporcional a 4–20 mA (lib/io)
  RS485_MODBUS         # Modbus RTU mestre e escravo na RS-485 (lib/modbus)
  FLASH_EXTERNA        # Flash SPI externa de 16 MiB (lib/flash, lib/fila)
  RADIO_WIFI           # Wi-Fi pelo módulo ESP32 (esp32/)
  RADIO_CELULAR        # Modem celular (lib/modem)
  RADIO_UART_EXTERNO   # Rádio externo em UART, modo transparente (lib/radio_externo)
  MQTT                 # Comunicação MQTT com o broker (lib/mqtt, esp32/)
)

# -----------------------------------------------------------------------------
# Build: OloggerMulti
# MCU: STM32L412C8 + módulo ESP32-C6-MINI-1 (Wi-Fi)
# -----------------------------------------------------------------------------
if(DEVICE STREQUAL "ologgermulti")
  set(OLOG_MCU "STM32L412C8")
  set(OLOG_TEM_ESP32 ON)
  set(OLOG_FEATURES
    ENTRADA_4_20MA       # 4 entradas 4–20 mA
    ENTRADA_DIGITAL      # 4 entradas digitais
    SAIDA_DISCRETA       # 4 saídas digitais por transistor
    SAIDA_FREQUENCIA     # As saídas digitais também operam em frequência
    RS485_MODBUS         # RS-485
    FLASH_EXTERNA        # 16 MiB
    RADIO_WIFI           # ESP32-C6-MINI-1, único rádio da placa
    RADIO_UART_EXTERNO   # UART de comunicação externa, para outro rádio em modo transparente
    MQTT
  )

# -----------------------------------------------------------------------------
# Build: y7080
# MCU: STM32L071CB, sem ESP32
# -----------------------------------------------------------------------------
elseif(DEVICE STREQUAL "y7080")
  set(OLOG_MCU "STM32L071CB")
  set(OLOG_TEM_ESP32 OFF)
  set(OLOG_FEATURES
    ENTRADA_4_20MA       # 1 entrada 4–20 mA
    ENTRADA_DIFERENCIAL  # Circuito diferencial PC11/K8
    RS485_MODBUS         # RS-485
    FLASH_EXTERNA        # 16 MiB
    RADIO_CELULAR        # Módulo SIMY7080 com footprint SIM7080G
    MQTT                 # Cliente MQTT no próprio modem, por comandos AT
    # Sem entradas digitais, sem saídas digitais e sem Wi-Fi, confirmado em 02/11/2026.
  )

else()
  message(FATAL_ERROR
    "DEVICE inválido: '${DEVICE}'. Use -DDEVICE=ologgermulti ou -DDEVICE=y7080.")
endif()

# -----------------------------------------------------------------------------
# Validação: toda funcionalidade do build precisa existir no catálogo.
# -----------------------------------------------------------------------------
foreach(FEAT IN LISTS OLOG_FEATURES)
  if(NOT FEAT IN_LIST OLOG_FEATURES_CATALOGO)
    message(FATAL_ERROR
      "Funcionalidade '${FEAT}' do build '${DEVICE}' não existe no catálogo.")
  endif()
endforeach()

message(STATUS "Build ${DEVICE}: MCU ${OLOG_MCU}, funcionalidades: ${OLOG_FEATURES}")
