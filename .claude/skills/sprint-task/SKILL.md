---
name: sprint-task
description: Use para executar uma tarefa FW-### do backlog do firmware oLoggerIOT, do início ao fechamento: escolha, implementação em pequenos passos, teste e documentação. Tarefas são pequenas, de até um dia.
---

# Execução de tarefa de sprint

## Fluxo

1. **Início:** leia `INICIAR.md` e `documentacao/INDICE_AGENTES.md` se for o começo de sessão. Pegue o próximo `FW-###` da sprint atual em `planejamento/sprints/`. Leia o item no `planejamento/BACKLOG.md` e o documento de regra citado.
2. **Confirme o escopo:** se o item depender de decisão não registrada, pare e registre a pendência. Não invente regra.
3. **Implemente em pequenos passos:** uma função ou um módulo por vez, cada um com seu teste. Para C, siga `c-firmware-standards`. Para ESP32, siga `esp-idf-standards`.
4. **Teste:** escreva o teste do critério de aceite. Rode no PC quando possível. Para hardware, registre o roteiro de bancada.
5. **Compile:** siga `device-profile-build`.
6. **Documente:** siga `doc-sync`, incluindo dicionário, estado e changelog.
7. **Feche:** registre o resultado no item e na sprint.

## Regras

- Uma tarefa por vez. Não misture itens na mesma alteração.
- Não feche a tarefa sem os testes passando e a documentação atualizada.
- Se o teste falhar, relate a falha com a saída. Não altere o teste para passar.
- Se a tarefa ficar grande (diff acima de cerca de 300 linhas), divida em novos itens no backlog antes de continuar.
- Biblioteca nova começa pela interface pública e pelo teste dela.

## Ao fim da sprint

Quando o último item da sprint fechar, use a skill `sprint-close`.
