# Changelog — oLoggerIOT firmware

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.1.0

Formato baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/). Versões seguem a numeração do projeto (ver `sistema-init/STATUS_PROJETO.md`). Cada entrada cita o item do backlog (`FW-###`) quando houver.

## Não lançado

### Adicionado
- FW-001: estrutura de pastas `src/`, `include/`, `lib/`, `devices/`, `tests/` e `tools/`, com README em cada uma.
- Padrão de desenvolvimento ESP-IDF (`documentacao/padrao_esp_idf.md`).
- Composição dos builds: `builds/composicao.cmake`, confirmada pelo responsável técnico em 02/11/2026, e `documentacao/composicao_builds.md`. Validado no CMake para os dois builds e para os casos de erro.
- Funcionalidade `RADIO_UART_EXTERNO` no catálogo: rádio externo em UART, modo transparente, no OloggerMulti.
- FW-006 (hardware): imagem vazia do OloggerMulti gravada e executando no STM32L412 real, com o BOOT0 sem ligação contornado por bits de opção (`nSWBOOT0=0`, `nBOOT0=1`). Achado HW-006 na análise de hardware e item FW-051 no backlog.
- FW-004: `Doxyfile` em português que falha em função ou parâmetro sem documentação, e `tools/gera_dicionario.py`, que gera a tabela do dicionário de funções a partir do XML do Doxygen.
- FW-005: modelos de arquivo C em `tools/modelos/` (`modelo.h` e `modelo.c`).
- FW-008: `tools/verifica_padrao.py`, que verifica função de até 40 linhas, arquivo de até 400, cabeçalho e ausência de alocação dinâmica. 20 testes das ferramentas.
- FW-006: imagens vazias dos dois builds STM32 compilam e linkam sem avisos. OloggerMulti com HAL L4 v1.18.2 (STM32L412C8) e y7080 com HAL L0 v1.12.4 (STM32L071CB). Inclui linker scripts, configurações do HAL (`include/stm32l4xx_hal_conf.h`, `include/stm32l0xx_hal_conf.h`), cabeçalho único `include/hal_stm32.h` e `src/main.c`.
- FW-002: CMake raiz (`CMakeLists.txt`) com seleção de build por `-DDEVICE`, toolchain ARM (`cmake/toolchain-arm-none-eabi.cmake`) e opções de CPU por MCU (`cmake/mcu.cmake`). Configura os dois builds e recusa device inválido.
- Proposta de dados Modbus de equipamentos externos em `documentacao/modbus_mapeamento_dados.md`, aprovada: tabela de variáveis na RAM, mapeamento configurável em flash, limites de 8 equipamentos e 64 variáveis.
- Seção FreeRTOS no padrão C (`documentacao/padrao_codigo_c.md`, 0.4.0): sistema em FreeRTOS, com alocação estática.
- Itens FW-046 (base FreeRTOS) e FW-047 a FW-050 (mapeamento Modbus) no backlog.
- Skills `esp-idf-standards` e `sprint-close`.
- Agentes `esp-idf-dev` e `firmware-reviewer`.
- Documentos de entrada: `CLAUDE.md`, `INICIAR.md`, `README.md`, `CHANGELOG.md`, `documentacao/INDICE_AGENTES.md` e `documentacao/ESTADO_IMPLEMENTACAO.md`.

### Alterado
- Padrão de código C (`documentacao/padrao_codigo_c.md`, 0.3.0): funcionalidade por `#if OLOG_FEAT_<NOME>`, pinagem por placa e boas práticas, com pequenos códigos e bibliotecas.
- Modelo de build: um projeto com todas as funcionalidades e um build por tipo de device (`ologgermulti`, `y7080`), com a composição de cada build em `builds/composicao.cmake`. Esclarecido pelo responsável técnico em 02/11/2026.
- Skills `c-firmware-standards`, `device-profile-build`, `doc-sync` e `sprint-task`; agentes `firmware-dev` e `doc-keeper`.

## 0.1.0 — 01/11/2026

### Adicionado
- Estrutura inicial do firmware: documentação, planejamento, skills e agentes.
- Plano de implantação em 11 fases e backlog com 45 itens (FW-001 a FW-045), em sprints de uma semana.
- Análise dos esquemáticos e Gerbers do OloggerMulti e do y7080.
- Mapa Modbus, regras de telemetria, configuração, agenda offline, comunicação e atualização.
