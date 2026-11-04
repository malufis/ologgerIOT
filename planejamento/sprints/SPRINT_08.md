# SPRINT_08 — Sessão, serial e início da atualização

> **Criado em:** 03/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.1
> **Período:** 28/12/2026 a 01/01/2027

Sprint planejada. Nenhum item foi iniciado. As datas são a sequência de semanas a partir de 02/11/2026 e mudam se a SPRINT_00, a SPRINT_01 ou a SPRINT_01B atrasarem.

## Meta

Sessão de configuração única, recusa de comandos MQTT durante a sessão, gateway serial e verificação do pacote de atualização.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-037 | Sessão de configuração única com timeout de 5 minutos | Pendente |
| FW-038 | Recusa de comandos MQTT durante a sessão | Pendente |
| FW-039 | Gateway serial no notebook (aplicação Python) com comandos restritos | Pendente |
| FW-040 | Verificação de validade e compatibilidade do pacote | Pendente |
| FW-062 | Layout da flash interna do STM32 com bootloader (ADR-0003): reservar o espaço do bootloader, mover o início da aplicação no linker, e medir e registrar a folga de flash e de RAM por fase | Pendente; criado em 04/11/2026 pela decisão do ADR-0003. Se a imagem passar de ~40 KB antes disso, antecipar |

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para a sprint seguinte.
- Testes de sessão concorrente e de timeout, e de pacote inválido.
- Documentação atualizada pela skill `doc-sync` e sprint fechada pela skill `sprint-close`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.1 | Incluído o FW-062 (layout da flash interna com bootloader) |
| 03/11/2026 | 0.1.0 | Criada a sprint a partir do backlog |
