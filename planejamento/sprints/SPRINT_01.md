# SPRINT_01 — Aquisição analógica

> **Criado em:** 03/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.1.1
> **Período:** 09/11/2026 a 13/11/2026 (o trabalho começou antes, em 02/11/2026, e está bloqueado pela ferrite L2)

## Meta

ADC disparado por timer com DMA, média simples por segundo, conversão de corrente para valor de engenharia e teste em bancada com gerador de corrente 4–20 mA.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-009 | Biblioteca `lib/adc`: configurar ADC com timer e DMA | Código pronto; medido em bancada com referência provisória; refazer depois da L2 |
| FW-010 | Biblioteca `lib/sensor`: média simples por segundo | Concluído em 02/11/2026; 57 verificações passam no PC (reexecutado em 03/11/2026) |
| FW-011 | Conversão de corrente para valor de engenharia com span, zero e offset | Concluído no código; calibração de dois pontos, refazer em cinco depois da L2 |
| FW-012 | Teste em bancada com gerador de corrente 4–20 mA | Em andamento: 8 mA na entrada 4 lidos como 8,007 mA; bloqueado pela L2 (HW-007) |

## Estado

- Código em `lib/adc`, `lib/sensor` e `lib/aquisicao`. Entrada 4 lida com 8 mA injetados: 8,007 mA (multímetro: 8 mA), com calibração de dois pontos (4 e 8 mA) e referência efetiva de 2,367 V no lugar de 3,25 V.
- **Bloqueio (HW-007):** a ferrite L2 (BLM18HE152SN1D), entre +3,3 V e 3,3V_A, não está soldada na placa de bancada. Sem ela, o VDDA cai para ~2,37 V e o ADC satura em ~15,8 mA. Soldar a L2, conferir ~0 Ω e VDDA em 3,25 V, voltar a referência do firmware para 3,25 V e refazer a calibração em 4, 8, 12, 16 e 20 mA nas quatro entradas.
- Novo em 03/11/2026: `adc_para` e `adc_retoma` (e `aquisicao_para` e `aquisicao_retoma`), para parar a aquisição ao dormir. Só compilam, sem teste em hardware.
- O sleep faz a aquisição analógica parar. Acordar por uma entrada analógica fica para outra sprint.

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para a sprint seguinte.
- Em bancada, a taxa configurada e a média por segundo batem com um gerador de corrente, nas quatro entradas e em cinco pontos (4, 8, 12, 16 e 20 mA).
- Os testes do módulo `sensor` passando (57 verificações em 03/11/2026).
- Documentação atualizada pela skill `doc-sync` e sprint fechada pela skill `sprint-close`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.1.1 | Corrigido o estado do teste do `sensor`: passa, 57 verificações |
| 03/11/2026 | 0.1.0 | Criada a sprint com o estado real: código pronto e medido parcialmente em bancada, bloqueada pela L2 |
