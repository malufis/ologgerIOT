# Sprints do firmware — índice

> **Criado em:** 01/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.2.7

Sprints de uma semana. Cada sprint tem um arquivo com meta, itens do backlog, estado e critério de encerramento. Itens concluídos são marcados no arquivo da sprint e no `BACKLOG.md`.

| Sprint | Semana | Meta | Itens | Situação em 03/11/2026 |
|---|---|---|---|---|
| `SPRINT_00` | 02/11/2026 | Base do projeto e build vazio | FW-001 a FW-006, FW-008 e FW-046 | **Concluída em 04/11/2026** (versão 0.2.0 do changelog); FW-007 movido para a SPRINT_07 |
| `SPRINT_01` | 09/11/2026 | Aquisição analógica | FW-009 a FW-012 | Código pronto e medido em bancada; bloqueada pela ferrite L2 (HW-007) |
| `SPRINT_01B` | 03/11/2026 (paralela) | Entrada digital e contador de pulsos | FW-052 a FW-060 | Código pronto, lógica testada no PC; bancada pendente |
| `SPRINT_02` | 16/11/2026 | Regras de sensor | FW-013 a FW-016 | Planejada |
| `SPRINT_03` | 23/11/2026 | Média móvel, fila e alertas | FW-017 a FW-021 | Planejada |
| `SPRINT_04` | 30/11/2026 | Saídas e controle | FW-022 a FW-025 e FW-061 | Em andamento: FW-022, FW-024 e FW-025 em código pronto; lado do sleep do FW-061 pronto |
| `SPRINT_05` | 07/12/2026 | Modbus RTU | FW-026 a FW-029 e FW-047 a FW-050 | Planejada |
| `SPRINT_06` | 14/12/2026 | Relógio e agenda offline | FW-030 a FW-032 | Planejada |
| `SPRINT_07` | 21/12/2026 | MQTT e ESP32 | FW-007 e FW-033 a FW-036 | Planejada; o ESP32 só começa depois da validação do STM32 |
| `SPRINT_08` | 28/12/2026 | Sessão, serial e início da atualização | FW-037 a FW-040 e FW-062 | Planejada |
| `SPRINT_09` | 04/01/2027 | Atualização e bancada | FW-041 a FW-045 e FW-051 | Planejada |

As datas são a sequência de semanas a partir de 02/11/2026. O protótipo de bancada tem referência em 29/11/2026, que é a véspera do início da SPRINT_04. **Risco de prazo:** ver `planejamento/BACKLOG.md`, seção "Prazo e risco", que traz a reavaliação de 03/11/2026. A reavaliação do escopo com o responsável técnico acontece antes da SPRINT_03.

**ESP32 (03/11/2026).** O FW-007 (build do ESP32) e a parte ESP32 do FW-046 estão na SPRINT_07 e só começam depois de finalizados os testes do STM32 e de o STM32 estar validado (decisão de 03/11/2026). A SPRINT_00 foi fechada em 04/11/2026 pela skill `sprint-close`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.2.7 | SPRINT_04: FW-025 em código pronto |
| 04/11/2026 | 0.2.6 | SPRINT_08 inclui o FW-062 |
| 04/11/2026 | 0.2.5 | SPRINT_00 concluída em 04/11/2026 |
| 04/11/2026 | 0.2.4 | Correções da verificação do doc-keeper antes do fechamento da SPRINT_00 (números, nomes dos registradores, concordância e referências) |
| 03/11/2026 | 0.2.3 | SPRINT_04 inclui o FW-061 (sleep liga e desliga); FW-022 e FW-024 em código pronto |
| 03/11/2026 | 0.2.2 | SPRINT_04 em andamento: FW-022 em código pronto |
| 03/11/2026 | 0.2.1 | FW-007 movido para a SPRINT_07; SPRINT_00 pronta para o `sprint-close` |
| 03/11/2026 | 0.2.0 | Criados os arquivos das sprints 01 e 02 a 09, que só existiam no índice. Adicionada a coluna de situação. Corrigidos os itens: FW-047 a FW-050 na SPRINT_05, FW-051 na SPRINT_09 e FW-046 na SPRINT_00 |
| 03/11/2026 | 0.1.1 | Adicionada SPRINT_01B (entrada digital), paralela à SPRINT_01 |
| 01/11/2026 | 0.1.0 | Criado índice das sprints de SPRINT_00 a SPRINT_09 |
