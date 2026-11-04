# SPRINT_07 — MQTT e ESP32

> **Criado em:** 03/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.1.1
> **Período:** 21/12/2026 a 25/12/2026

Sprint planejada. Nenhum item foi iniciado. As datas são a sequência de semanas a partir de 02/11/2026 e mudam se a SPRINT_00, a SPRINT_01 ou a SPRINT_01B atrasarem.

## Meta

Protocolo serial STM32 e ESP32, cliente MQTT 5.0 com TLS 1.3, tópicos do device e reconexão. Começa pelo FW-007 (build vazio do ESP32, com a parte ESP32 do FW-046), que só começa depois de finalizados os testes do STM32 e de o STM32 estar validado (decisão de 03/11/2026).

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-007 | Build vazio do ESP32 (ESP-IDF), com a parte ESP32 do FW-046 | Pendente; movido da SPRINT_00 em 03/11/2026. Começa depois da validação do STM32 |
| FW-033 | Protocolo serial entre STM32 e ESP32 | Pendente |
| FW-034 | Cliente MQTT 5.0 com TLS 1.3 no ESP32 | Pendente |
| FW-035 | Tópicos do device a partir do caminho recebido do sistema | Pendente |
| FW-036 | Reconexão: cinco tentativas por rádio, a cada minuto, depois offline | Pendente |

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para a sprint seguinte.
- Simulador Python de broker, com reconexão, queda e envio confirmado.
- Documentação atualizada pela skill `doc-sync` e sprint fechada pela skill `sprint-close`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.1.1 | Incluído o FW-007, vindo da SPRINT_00, e a condição de começar só depois da validação do STM32 |
| 03/11/2026 | 0.1.0 | Criada a sprint a partir do backlog |
