# -----------------------------------------------------------------------------
# oLoggerIOT — raiz do CMake do firmware STM32
#
# Uso:
#   cmake -S . -B build/<device> -G Ninja -DDEVICE=<device> \
#         -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake
#
# <device>: ologgermulti ou y7080.
# Composição de cada build: builds/composicao.cmake.
# O ESP32 (ESP-IDF) não passa por aqui: fica em esp32/.
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.22)

if(NOT DEVICE)
  message(FATAL_ERROR "Informe o device: -DDEVICE=ologgermulti ou -DDEVICE=y7080.")
endif()

# Lê a composição do build. Valida DEVICE e cada funcionalidade do catálogo.
include(builds/composicao.cmake)

project(oLoggerIOT C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/mcu.cmake)

# Otimização (decidido em 04/11/2026): padrão -Os, o menor código, que cabe com folga nos 60 KB do
# STM32L412. Para depurar passo a passo use -DOLOG_OTIMIZACAO=g (-Og) ou 0 (sem otimização). As
# informações de depuração (-g) não ocupam flash.
set(OLOG_OTIMIZACAO "s" CACHE STRING "Nível de otimização: s (padrão), g, 0, 1 ou 2")
set_property(CACHE OLOG_OTIMIZACAO PROPERTY STRINGS s g 0 1 2)

# Avisos como erro, e seções separadas para o linker remover o que não é usado.
add_compile_options(
  ${OLOG_MCU_CPU_FLAGS}
  -O${OLOG_OTIMIZACAO}
  -g
  -Wall
  -Wextra
  -Werror
  -ffunction-sections
  -fdata-sections
)

# Cada funcionalidade do build vira OLOG_FEAT_<NOME>, usada em #if no código C.
foreach(FEAT IN LISTS OLOG_FEATURES)
  add_compile_definitions(OLOG_FEAT_${FEAT}=1)
endforeach()

# Pinagem da placa, quando o device tiver pasta própria (FW-003).
set(OLOG_PINAGEM_DIR ${CMAKE_SOURCE_DIR}/devices/${DEVICE})
if(EXISTS ${OLOG_PINAGEM_DIR})
  include_directories(${OLOG_PINAGEM_DIR})
endif()

# Imagem STM32: o HAL é escolhido pelo MCU do build.
if(OLOG_MCU STREQUAL "STM32L412C8")
  include(cmake/hal-stm32l4.cmake)
elseif(OLOG_MCU STREQUAL "STM32L071CB")
  include(cmake/hal-stm32l0.cmake)
endif()
include(cmake/freertos.cmake)
add_subdirectory(lib)
add_subdirectory(src)

message(STATUS "oLoggerIOT ${DEVICE}: MCU ${OLOG_MCU}, ${OLOG_MCU_CPU_FLAGS}")
message(STATUS "Funcionalidades: ${OLOG_FEATURES}")
