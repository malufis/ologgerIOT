# oLoggerIOT — firmware

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.1.0

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

Ainda não há build. Os comandos serão registrados aqui quando estiverem prontos:

- Seleção do build (CMake): `cmake -DDEVICE=ologgermulti` ou `cmake -DDEVICE=y7080`. Depende da FW-002.
- Imagem STM32: FW-002 e FW-006.
- Imagem ESP32, só no build `ologgermulti` (`idf.py set-target esp32c6`, `idf.py build`): FW-007.
- Versão de referência do ESP-IDF: v5.4.4, instalada em `C:\Espressif`. Fixar no README na FW-007.

## Padrões

- Código C: `documentacao/padrao_codigo_c.md` (base MISRA C:2012).
- Código ESP-IDF: `documentacao/padrao_esp_idf.md`.
- Documentação em português do Brasil, com cabeçalho, versão e histórico.

## Desenvolvimento

- Backlog: `planejamento/BACKLOG.md`. Sprints de uma semana em `planejamento/sprints/`.
- Tarefas pequenas, de até um dia. Uma tarefa por alteração.
- Responsável técnico: Rodrigo Maluf. Cliente: Optmile.
