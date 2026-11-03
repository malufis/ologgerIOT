# SPRINT_05 — Modbus RTU

> **Criado em:** 03/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.1.0
> **Período:** 07/12/2026 a 11/12/2026

Sprint planejada. Nenhum item foi iniciado. As datas são a sequência de semanas a partir de 02/11/2026 e mudam se a SPRINT_00, a SPRINT_01 ou a SPRINT_01B atrasarem.

## Meta

Quadro RTU e CRC, mestre com timeout e tentativas, escravo, recuperação e a tabela de variáveis com o mapeamento dos equipamentos externos.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-026 | Biblioteca `lib/modbus`: quadro RTU e CRC | Pendente |
| FW-027 | Mestre com timeout de 30 s e dez tentativas a cada 30 s | Pendente |
| FW-028 | Escravo com mapa de `documentacao/mapa_modbus.md` | Pendente |
| FW-029 | Recuperação com cinco leituras válidas e defeito após uma hora | Pendente |
| FW-047 | `lib/tabela_variaveis`: 64 variáveis na RAM, com estado, marca de tempo e mutex estático | Pendente |
| FW-048 | `lib/mapeamento`: configuração de até 8 equipamentos, validação completa e gravação atômica em flash com CRC | Pendente |
| FW-049 | `lib/modbus_mestre` em task FreeRTOS, não bloqueante: um equipamento sem resposta não atrasa os demais | Pendente |
| FW-050 | `lib/modbus_escravo`: expõe a tabela no mapa fixo com registrador de status por variável | Pendente |

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para a sprint seguinte.
- Simulador Modbus com ausência de resposta, resposta parcial e recuperação.
- Documentação atualizada pela skill `doc-sync` e sprint fechada pela skill `sprint-close`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.1.0 | Criada a sprint a partir do backlog |
