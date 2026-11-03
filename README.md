# oLoggerIOT — firmware

> **Criado em:** 02/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.2.0

Firmware da família oLoggerIOT, para os modelos **OloggerMulti** e **y7080**. Coleta de sensores 4–20 mA e digitais, controle de saídas, telemetria offline, comunicação por MQTT ou serial e Modbus RTU.

## Arquitetura

- **STM32** (C, CMake com toolchain ARM e HAL): coletor e orquestrador. Controla sensores, saídas, telemetria e os meios de comunicação. STM32L412 no OloggerMulti e STM32L071 no y7080.
- **ESP32-C6** (ESP-IDF): módulo de comunicação subordinado ao STM32, no OloggerMulti.
- Um projeto com todas as funcionalidades, e um build por tipo de device. O arquivo `builds/composicao.cmake` diz o que entra em cada build; a descrição está em `documentacao/composicao_builds.md`.

## Estrutura

| Pasta | Conteúdo |
|---|---|
| `src/` | Ponto de entrada e composição dos módulos |
| `lib/` | Bibliotecas por módulo, cada uma com `include/` e `src/` |
| `include/` | Cabeçalhos globais |
| `builds/` | Composição de cada build (`composicao.cmake`) |
| `devices/` | Pinagem de cada placa |
| `tests/` | Testes de unidade no PC |
| `tools/` | Verificação de padrão, geração do dicionário e simulador |
| `esp32/` | Projeto ESP-IDF (a criar na FW-007) |
| `documentacao/` | Regras, padrões, integração, dicionário e estado |
| `planejamento/` | Plano, backlog e sprints |
| `sistema-init/` | Status do projeto e decisões |
| `.claude/` | Skills e agentes para o desenvolvimento |

## Documentação

- Ponto de entrada: `INICIAR.md`.
- Índice por assunto: `documentacao/INDICE_AGENTES.md`.
- Índice completo: `documentacao/README.md`.
- Estado do que está implementado: `documentacao/ESTADO_IMPLEMENTACAO.md`.
- Histórico de mudanças: `CHANGELOG.md`.

## Como compilar

Os dois builds STM32 compilam sem avisos com `-Werror` (verificado em 03/11/2026). O build do ESP32 ainda não existe (FW-007).

- Configurar o build STM32 (ferramenta ARM GCC e Ninja no PATH):

  ```
  cmake -S . -B build/ologgermulti -G Ninja -DDEVICE=ologgermulti -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake
  ```

  Troque `ologgermulti` por `y7080` para o outro build. Precisa também da pasta do pacote STM32CubeL4, passada com `-DOLOG_STM32CUBE_L4=<pasta>`, por exemplo `C:/Users/<usuario>/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.2`.

- Configurar o y7080 pede o pacote STM32CubeL0 em `-DOLOG_STM32CUBE_L0=<pasta>`, por exemplo `C:/Users/<usuario>/STM32Cube/Repository/STM32Cube_FW_L0_V1.12.4`.
- Compilar: `cmake --build build/ologgermulti` ou `cmake --build build/y7080`. Saem `ologgeriot_stm32.elf`, `.bin` e `.map` em `build/<device>/src/`.
- Testes de unidade da lógica no PC: `cmake -S tests -B build/tests -G Ninja -DCMAKE_C_COMPILER=gcc`, `cmake --build build/tests` e `ctest --test-dir build/tests` (ver `tests/README.md`).
- Imagem ESP32, só no build `ologgermulti` (`idf.py set-target esp32c6`, `idf.py build`): FW-007, ainda não iniciado.
- Versão de referência do ESP-IDF: v5.4.4, instalada em `C:\Espressif`. Fixar no README na FW-007.

## Padrões

- Código C: `documentacao/padrao_codigo_c.md` (base MISRA C:2012).
- Código ESP-IDF: `documentacao/padrao_esp_idf.md`.
- Documentação em português do Brasil, com cabeçalho, versão e histórico.
- Verificar o padrão do projeto: `python tools/verifica_padrao.py` (função, arquivo, cabeçalho e alocação dinâmica).
- Documentação do código: `doxygen Doxyfile` (falha em item sem comentário) e `python tools/gera_dicionario.py` para atualizar o dicionário de funções.
- Testes das ferramentas: `python -m unittest discover -s tools -p "test_*.py"`.
- Verificar a pinagem: `python tools/verifica_pinos.py`.
- Modelos de arquivo C novo: `tools/modelos/`.

## Desenvolvimento

- Backlog: `planejamento/BACKLOG.md`. Sprints de uma semana em `planejamento/sprints/`.
- Tarefas pequenas, de até um dia. Uma tarefa por alteração.
- Responsável técnico: Rodrigo Maluf. Cliente: Optmile.
- Branch principal: `main`. Trabalho em branch próprio e PR para `main`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.2.0 | Comandos de compilação e de teste atualizados: os dois builds STM32 compilam e há testes no PC |
| 02/11/2026 | 0.1.0 | Criado |
