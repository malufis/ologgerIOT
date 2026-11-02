# -----------------------------------------------------------------------------
# Toolchain ARM GCC para os MCUs STM32 do oLoggerIOT.
#
# Uso: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake ...
# As opções de CPU e FPU de cada MCU ficam em cmake/mcu.cmake.
# -----------------------------------------------------------------------------

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Sem executável de teste na fase de detecção: o link de teste exige script de linker.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

find_program(CMAKE_C_COMPILER arm-none-eabi-gcc REQUIRED)
find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc REQUIRED)
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_SIZE arm-none-eabi-size REQUIRED)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")
