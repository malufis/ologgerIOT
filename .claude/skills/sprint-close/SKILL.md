---
name: sprint-close
description: Use ao terminar uma sprint do firmware oLoggerIOT, antes de declarar a sprint fechada. Faz a sincronização completa de toda a documentação do sistema, do changelog e do índice dos agentes, e confirma cada item com o responsável.
---

# Fechamento de sprint

Sprint só é fechada com o sistema inteiro documentado. Este checklist roda uma vez por sprint, depois que todos os itens foram concluídos ou movidos.

## 1. Itens

- Todo item da sprint está concluído com data, ou movido para a próxima sprint com motivo.
- Cada item concluído tem testes passando, com saída registrada, e imagens compiladas registradas.

## 2. Documentos de estado

- `documentacao/ESTADO_IMPLEMENTACAO.md`: todo módulo tocado na sprint está com estado atualizado, e a seção "Próxima sprint" aponta para o que vem depois.
- `CHANGELOG.md`: a seção "Não lançado" tem os itens da sprint agrupados por tipo (Adicionado, Alterado, Corrigido, Removido). Ao fechar, a seção recebe o número da versão e a data da sprint.
- `planejamento/BACKLOG.md` e `planejamento/sprints/SPRINT_NN.md`: itens marcados, histórico atualizado, sprint com situação "Concluída".

## 3. Código e dicionário

- `documentacao/dicionario_funcoes.md`: toda função criada ou alterada na sprint está listada, com módulo, arquivo e status.
- A tabela foi regenerada no fim da sprint e `python tools/gera_dicionario.py --check` sai com 0.
- Padrão de código e de ESP-IDF respeitados: `python tools/verifica_padrao.py` sai com 0 e `doxygen Doxyfile` roda sem avisos.

## 4. Regras e contratos

- Cada documento de regra alterado (`documentacao/*.md`) tem versão incrementada e linha no histórico.
- Se o contrato com o sistema mudou, `documentacao/integracao_sistema.md` está atualizado.
- Se a composição de algum build mudou, `builds/composicao.cmake` e `documentacao/composicao_builds.md` estão atualizados.

## 5. Índices para agentes e desenvolvedores

- `INICIAR.md`: a sprint atual e o próximo item estão corretos.
- `documentacao/INDICE_AGENTES.md`: todo documento novo da sprint está listado com o assunto que responde.
- `documentacao/README.md`: índice humano atualizado.
- `README.md` do repositório: versão atual, dependências e comandos de build conferidos.

## 6. Estado do projeto

- `sistema-init/STATUS_PROJETO.md`: versão incrementada, fase atual, pendências e próximo passo.
- `CLAUDE.md`: sem mudança, a não ser que o caminho de início tenha mudado.

## 7. Verificação final

Rode o agente `doc-keeper` sobre a sprint. Ele lista divergências por documento. A sprint só é declarada fechada quando a lista estiver vazia ou com os conflitos encaminhados ao responsável técnico.

## 8. Registro

Adicione uma linha em `planejamento/sprints/SPRINT_NN.md` com a data de fechamento, a versão do changelog e o resultado da verificação final.
