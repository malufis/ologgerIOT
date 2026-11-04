# -----------------------------------------------------------------------------
# Testes de unidade no PC. Projeto CMake separado do firmware: usa o compilador
# do PC, sem HAL e sem hardware.
#
#   cmake -S tests -B build/tests -G Ninja -DCMAKE_C_COMPILER=gcc
#   cmake --build build/tests
#   ctest --test-dir build/tests --output-on-failure
#
# Cada módulo testável (lógica pura) entra aqui com seu teste.
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.22)
project(oLoggerIOT_testes C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Werror -pedantic)

enable_testing()

# Módulo sensor: média por segundo e conversão 4-20 mA.
add_executable(test_sensor
  sensor/test_sensor.c
  ../lib/sensor/src/sensor.c
)
target_include_directories(test_sensor PRIVATE ../lib/sensor/include .)
target_link_libraries(test_sensor PRIVATE m)
add_test(NAME sensor COMMAND test_sensor)

# Módulo io: filtro de ruído da entrada digital.
add_executable(test_io_filtro
  io/test_io_filtro.c
  ../lib/io/src/io_filtro.c
)
target_include_directories(test_io_filtro PRIVATE ../lib/io/include .)
add_test(NAME io_filtro COMMAND test_io_filtro)

# Módulo io: contador de pulsos e inatividade.
add_executable(test_io_pulsos
  io/test_io_pulsos.c
  ../lib/io/src/io_pulsos.c
)
target_include_directories(test_io_pulsos PRIVATE ../lib/io/include .)
add_test(NAME io_pulsos COMMAND test_io_pulsos)

# Módulo io: estado da saída digital discreta (ligada, desligada, acionada por tempo).
add_executable(test_saida
  io/test_saida.c
  ../lib/io/src/saida.c
)
target_include_directories(test_saida PRIVATE ../lib/io/include .)
add_test(NAME saida COMMAND test_saida)

# Módulo agenda: formato da configuração persistente (fuso e agendas).
add_executable(test_agenda_config
  agenda/test_agenda_config.c
  ../lib/agenda/src/agenda_config.c
  ../lib/agenda/src/agenda_aplicador.c
  ../lib/agenda/src/agenda.c
)
target_include_directories(test_agenda_config PRIVATE ../lib/agenda/include .)
add_test(NAME agenda_config COMMAND test_agenda_config)

# Módulo config: tabela de parâmetros de configuração com endereços do mapa Modbus.
add_executable(test_config
  config/test_config.c
  ../lib/config/src/config.c
  ../lib/config/src/config_carga.c
)
target_include_directories(test_config PRIVATE ../lib/config/include .)
target_link_libraries(test_config PRIVATE m)
add_test(NAME config COMMAND test_config)

# Módulo armazem: configuração persistente em dois espaços de flash, com flash simulada.
add_executable(test_armazem
  armazem/test_armazem.c
  ../lib/armazem/src/armazem.c
)
target_include_directories(test_armazem PRIVATE ../lib/armazem/include .)
add_test(NAME armazem COMMAND test_armazem)

# Módulo tempo: conversão do tempo UTC em data e hora locais com o fuso da instalação.
add_executable(test_tempo
  tempo/test_tempo.c
  ../lib/tempo/src/tempo.c
)
target_include_directories(test_tempo PRIVATE ../lib/tempo/include .)
add_test(NAME tempo COMMAND test_tempo)

# Módulo agenda: agenda semanal de 336 intervalos de 30 minutos de uma saída.
add_executable(test_agenda
  agenda/test_agenda.c
  ../lib/agenda/src/agenda.c
)
target_include_directories(test_agenda PRIVATE ../lib/agenda/include .)
add_test(NAME agenda COMMAND test_agenda)

# Módulo agenda: aplicação das agendas pelo horário atual.
add_executable(test_agenda_aplicador
  agenda/test_agenda_aplicador.c
  ../lib/agenda/src/agenda_aplicador.c
  ../lib/agenda/src/agenda.c
)
target_include_directories(test_agenda_aplicador PRIVATE ../lib/agenda/include .)
add_test(NAME agenda_aplicador COMMAND test_agenda_aplicador)

# Módulo io: modo manual e automático de uma saída e a prioridade entre as fontes.
add_executable(test_controle_saida
  io/test_controle_saida.c
  ../lib/io/src/controle_saida.c
  ../lib/io/src/saida.c
)
target_include_directories(test_controle_saida PRIVATE ../lib/io/include .)
add_test(NAME controle_saida COMMAND test_controle_saida)

# Módulo io: regra do sensor (limiares, sentido e histerese de uma entrada analógica).
add_executable(test_regra_sensor
  io/test_regra_sensor.c
  ../lib/io/src/regra_sensor.c
  ../lib/io/src/controle_saida.c
  ../lib/io/src/saida.c
)
target_include_directories(test_regra_sensor PRIVATE ../lib/io/include .)
target_link_libraries(test_regra_sensor PRIVATE m)
add_test(NAME regra_sensor COMMAND test_regra_sensor)

# Módulo io: frequência da saída em frequência a partir da corrente 4-20 mA.
add_executable(test_freq_saida
  io/test_freq_saida.c
  ../lib/io/src/freq_saida.c
)
target_include_directories(test_freq_saida PRIVATE ../lib/io/include .)
target_link_libraries(test_freq_saida PRIVATE m)
add_test(NAME freq_saida COMMAND test_freq_saida)

# Módulo io: gerador de onda quadrada por acumulador de fase (saída em frequência).
add_executable(test_gerador_freq
  io/test_gerador_freq.c
  ../lib/io/src/gerador_freq.c
)
target_include_directories(test_gerador_freq PRIVATE ../lib/io/include .)
target_link_libraries(test_gerador_freq PRIVATE m)
add_test(NAME gerador_freq COMMAND test_gerador_freq)

# Módulo io: tipo das saídas e exclusão entre frequência e sleep.
add_executable(test_tipo_saida
  io/test_tipo_saida.c
  ../lib/io/src/tipo_saida.c
)
target_include_directories(test_tipo_saida PRIVATE ../lib/io/include .)
add_test(NAME tipo_saida COMMAND test_tipo_saida)

# Módulo io: totalizador de pulsos com duas cópias e complemento.
add_executable(test_io_totalizador
  io/test_io_totalizador.c
  ../lib/io/src/io_totalizador.c
)
target_include_directories(test_io_totalizador PRIVATE ../lib/io/include .)
add_test(NAME io_totalizador COMMAND test_io_totalizador)
