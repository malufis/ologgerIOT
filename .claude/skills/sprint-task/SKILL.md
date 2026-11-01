---
name: sprint-task
description: Use para executar uma tarefa FW-### do backlog do firmware oLoggerIOT, do início ao fechamento: escolha, implementação, teste e documentação. Tarefas são pequenas, de até um dia.
---

# Execução de tarefa de sprint

## Fluxo

1. **Escolha:** pegue o próximo `FW-###` da sprint atual em `planejamento/sprints/`. Leia o item no `planejamento/BACKLOG.md` e o documento de regra citado.
2. **Confirme o escopo:** se o item depender de decisão não registrada, pare e registre a pendência. Não invente regra.
3. **Implemente:** siga a skill `c-firmware-standards`. Uma tarefa, uma responsabilidade.
4. **Teste:** escreva o teste do critério de aceite. Rode no PC quando possível. Para hardware, registre o roteiro de bancada.
5. **Compile:** siga a skill `device-profile-build`.
6. **Documente:** siga a skill `doc-sync`.
7. **Feche:** registre o resultado no item e na sprint.

## Regras

- Uma tarefa por vez. Não misture itens na mesma alteração.
- Não feche a tarefa sem os testes passando e a documentação atualizada.
- Se o teste falhar, relate a falha com a saída. Não altere o teste para passar.
- Se a tarefa ficar grande, divida em novos itens no backlog antes de continuar.
