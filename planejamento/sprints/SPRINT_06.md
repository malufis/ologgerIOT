# SPRINT_06 — Relógio e agenda offline

> **Criado em:** 03/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.4
> **Período:** 14/12/2026 a 18/12/2026

Sprint planejada. Nenhum item foi iniciado. As datas são a sequência de semanas a partir de 02/11/2026 e mudam se a SPRINT_00, a SPRINT_01 ou a SPRINT_01B atrasarem.

## Meta

RTC com NTP, hora inválida e agenda semanal aplicada somente offline. O RTC também será a origem do despertar para transmitir.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-030 | Driver do RTC e conferência NTP quatro vezes ao dia; fuso horário da instalação configurado pelo CCO (40403, proposta) | Parcial em 04/11/2026: `lib/tempo` pronto (conversão do tempo UTC em data, hora e dia da semana locais com o fuso da instalação, 12 testes no PC, anos bissextos e virada de dia e de ano). Falta o driver do RTC, a conferência NTP e o recebimento do fuso (40403, proposta) |
| FW-031 | Hora inválida: medições sem validade, saídas mantêm estado, aviso guardado | Pendente |
| FW-032 | Agenda semanal de 336 intervalos por saída, recebida do CCO por MQTT: formato, validação, armazenamento persistente com CRC só da última agenda, e aplicação pelo horário atual (offline, ao ficar offline e depois de reinício) | Parcial em 04/11/2026: formato e validação prontos (`lib/agenda`, 57 verificações no PC). A aplicação pelo horário atual está pronta em `lib/agenda/agenda_aplicador` (13 testes, 42 verificações no PC): pede o estado do intervalo atual na primeira avaliação depois da partida, a cada virada de intervalo de 30 minutos e logo depois de carregar ou apagar a agenda; com hora inválida não pede nada. Falta o driver do RTC (FW-030), o armazenamento persistente, entregar o pedido à saída como fonte agenda (`io_saidas_pedido`) e o recebimento por MQTT (FW-035) |

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para a sprint seguinte.
- Testes de perda e recuperação de hora e de transição de intervalo da agenda.
- Documentação atualizada pela skill `doc-sync` e sprint fechada pela skill `sprint-close`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.4 | FW-032: aplicador das agendas pelo horário atual pronto e testado |
| 04/11/2026 | 0.1.3 | FW-030 parcial: módulo `tempo` pronto e testado |
| 04/11/2026 | 0.1.2 | FW-030 e FW-032 com as decisões do P-043 |
| 04/11/2026 | 0.1.1 | FW-032 com o formato da agenda recebida por MQTT, parcial |
| 03/11/2026 | 0.1.0 | Criada a sprint a partir do backlog |
