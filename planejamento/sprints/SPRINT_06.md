# SPRINT_06 — Relógio e agenda offline

> **Criado em:** 03/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.2
> **Período:** 14/12/2026 a 18/12/2026

Sprint planejada. Nenhum item foi iniciado. As datas são a sequência de semanas a partir de 02/11/2026 e mudam se a SPRINT_00, a SPRINT_01 ou a SPRINT_01B atrasarem.

## Meta

RTC com NTP, hora inválida e agenda semanal aplicada somente offline. O RTC também será a origem do despertar para transmitir.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-030 | Driver do RTC e conferência NTP quatro vezes ao dia; fuso horário da instalação configurado pelo CCO (40403, proposta) | Pendente |
| FW-031 | Hora inválida: medições sem validade, saídas mantêm estado, aviso guardado | Pendente |
| FW-032 | Agenda semanal de 336 intervalos por saída, recebida do CCO por MQTT: formato, validação, armazenamento persistente com CRC só da última agenda, e aplicação pelo horário atual (offline, ao ficar offline e depois de reinício) | Parcial em 04/11/2026: formato e validação prontos (`lib/agenda`, 57 verificações no PC). Falta o RTC (FW-030), o armazenamento persistente, a aplicação por intervalo e o recebimento por MQTT (FW-035) |

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para a sprint seguinte.
- Testes de perda e recuperação de hora e de transição de intervalo da agenda.
- Documentação atualizada pela skill `doc-sync` e sprint fechada pela skill `sprint-close`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.2 | FW-030 e FW-032 com as decisões do P-043 |
| 04/11/2026 | 0.1.1 | FW-032 com o formato da agenda recebida por MQTT, parcial |
| 03/11/2026 | 0.1.0 | Criada a sprint a partir do backlog |
