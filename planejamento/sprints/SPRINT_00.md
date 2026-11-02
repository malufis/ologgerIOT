# SPRINT_00 — Base do projeto

> **Criado em:** 01/11/2026 · **Última modificação:** 01/11/2026 · **Versão:** 0.1.0
> **Período:** 02/11/2026 a 06/11/2026
> **Última modificação:** 02/11/2026 · **Versão:** 0.1.4

## Meta

Base do projeto pronta para as sprints seguintes: estrutura de pastas, CMake com seleção de build por device, composição dos builds, Doxygen, modelo de cabeçalho e build vazio dos dois builds.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-001 | Estrutura de pastas com README | Concluído em 02/11/2026 |
| FW-002 | CMake raiz com seleção de build por device | Pendente |
| FW-003 | Revisão da composição e pinagem por placa | Pendente |
| FW-046 | Base FreeRTOS nos dois alvos, com alocação estática | Pendente (adicionado em 02/11/2026) |
| FW-004 | Doxygen em português | Pendente |
| FW-005 | Modelo de cabeçalho | Pendente |
| FW-006 | Build vazio do STM32 | Pendente |
| FW-007 | Build vazio do ESP32 | Pendente |
| FW-008 | Script de verificação do padrão C | Pendente |

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para a SPRINT_01.
- Build vazio dos dois builds sem erro: `ologgermulti` (imagem STM32 e imagem ESP32) e `y7080` (imagem STM32).
- `documentacao/README.md`, `documentacao/padrao_codigo_c.md` e `BACKLOG.md` atualizados.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 01/11/2026 | 0.1.0 | Criada a sprint com oito itens da base do projeto |
| 02/11/2026 | 0.1.1 | FW-001 concluído (estrutura de pastas com README); FW-002 a FW-008 pendentes |
| 02/11/2026 | 0.1.2 | FW-002 e FW-003 redefinidos para imagem única e setup de composição; critério de build ajustado para imagem STM32 e ESP32 |
| 02/11/2026 | 0.1.4 | FW-046 adicionado (base FreeRTOS nos dois alvos), por decisão de usar FreeRTOS em todo o sistema |
| 02/11/2026 | 0.1.3 | Composição por build em `builds/composicao.cmake`: FW-002 seleciona o build por device; FW-003 revisa a composição e a pinagem; critério de build com os dois builds |
