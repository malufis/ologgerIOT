# Changelog — oLoggerIOT firmware

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.1.0

Formato baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/). Versões seguem a numeração do projeto (ver `sistema-init/STATUS_PROJETO.md`). Cada entrada cita o item do backlog (`FW-###`) quando houver.

## Não lançado

### Adicionado
- FW-001: estrutura de pastas `src/`, `include/`, `lib/`, `devices/`, `tests/` e `tools/`, com README em cada uma.
- Padrão de desenvolvimento ESP-IDF (`documentacao/padrao_esp_idf.md`).
- Composição dos builds: `builds/composicao.cmake`, confirmada pelo responsável técnico em 02/11/2026, e `documentacao/composicao_builds.md`. Validado no CMake para os dois builds e para os casos de erro.
- Funcionalidade `RADIO_UART_EXTERNO` no catálogo: rádio externo em UART, modo transparente, no OloggerMulti.
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
