# SPRINT_00 — Base do projeto

> **Criado em:** 01/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.1.9
> **Período:** 02/11/2026 a 06/11/2026

## Meta

Base do projeto pronta para as sprints seguintes: estrutura de pastas, CMake com seleção de build por device, composição dos builds, Doxygen, modelo de cabeçalho e build vazio dos dois builds.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-001 | Estrutura de pastas com README | Concluído em 02/11/2026 |
| FW-002 | CMake raiz com seleção de build por device | Concluído em 02/11/2026: configura os dois builds, recusa device inválido (branch `feat/fw-002-cmake-raiz`) |
| FW-003 | Revisão da composição e pinagem por placa | Concluído em 02/11/2026: `pinos.h` dos dois builds conferidos contra as netlists (31 e 26 funções, sem duplicado) e compilados com o HAL real; `tools/verifica_pinos.py` com 7 testes |
| FW-046 | Base FreeRTOS nos dois alvos, com alocação estática | Concluído no STM32 em 02/11/2026: OloggerMulti executa no chip real (9,9 ciclos/s medidos para 10 esperados, 101 de 128 palavras de pilha livres); y7080 compila sem avisos, sem teste em hardware. Parte ESP32 fica para depois do STM32 |
| FW-004 | Doxygen em português | Concluído em 02/11/2026: `Doxyfile` falha em item sem documentação; `tools/gera_dicionario.py` gera o dicionário a partir do XML (8 testes) |
| FW-005 | Modelo de cabeçalho | Concluído em 02/11/2026: `tools/modelos/modelo.h` e `modelo.c`, compilados sem avisos para M4 e M0+ |
| FW-006 | Build vazio do STM32 | Concluído em 02/11/2026: imagens do OloggerMulti (L412, 1432 bytes de código) e do y7080 (L071, 1348 bytes de código) compilam e linkam sem avisos. A do OloggerMulti executa no STM32L412 real (BOOT0 contornado por bits de opção) |
| FW-007 | Build vazio do ESP32 | Movido para a SPRINT_07 em 03/11/2026: só começa depois de finalizados os testes do STM32 e de o STM32 estar validado. Não conta mais como pendente desta sprint |
| FW-008 | Script de verificação do padrão C | Concluído em 02/11/2026: `tools/verifica_padrao.py` (função de 40 linhas, arquivo de 400, cabeçalho e alocação dinâmica; 12 testes). Ferramenta MISRA e ADR seguem pendentes |

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para outra sprint. O FW-007 foi movido para a SPRINT_07 em 03/11/2026.
- Build vazio dos dois builds STM32 sem erro: `ologgermulti` e `y7080`. A imagem ESP32 do `ologgermulti` sai deste critério junto com o FW-007 e vai para a SPRINT_07.
- `documentacao/README.md`, `documentacao/padrao_codigo_c.md` e `BACKLOG.md` atualizados.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.1.9 | FW-007 movido para a SPRINT_07 (decisão de 03/11/2026). Todos os itens restantes estão concluídos; sprint pronta para o `sprint-close`, que ainda não foi executado |
| 03/11/2026 | 0.1.8 | Conferido o estado: 8 dos 9 itens concluídos; FW-007 adiado e sem sprint definida. Cabeçalho duplicado corrigido. Os dois builds STM32 seguem compilando sem avisos com o código das sprints 01 e 01B (24 952 e 4 944 bytes de código) |
| 02/11/2026 | 0.1.7 | FW-046 concluído no STM32 (FreeRTOS com alocação estática, tarefa de vida medida no hardware); FW-007 e a parte ESP32 do FW-046 ficam para depois |
| 02/11/2026 | 0.1.6 | FW-003 concluído; FW-007 e FW-046 pendentes. Corrigido aviso do linker no y7080 (segmento RWX) |
| 02/11/2026 | 0.1.5 | FW-004, FW-005 e FW-008 concluídos; FW-003, FW-007 e FW-046 pendentes |
| 02/11/2026 | 0.1.4 | FW-046 adicionado (base FreeRTOS nos dois alvos), por decisão de usar FreeRTOS em todo o sistema |
| 02/11/2026 | 0.1.3 | Composição por build em `builds/composicao.cmake`: FW-002 seleciona o build por device; FW-003 revisa a composição e a pinagem; critério de build com os dois builds |
| 02/11/2026 | 0.1.2 | FW-002 e FW-003 redefinidos para imagem única e setup de composição; critério de build ajustado para imagem STM32 e ESP32 |
| 02/11/2026 | 0.1.1 | FW-001 concluído (estrutura de pastas com README); FW-002 a FW-008 pendentes |
| 01/11/2026 | 0.1.0 | Criada a sprint com oito itens da base do projeto |
