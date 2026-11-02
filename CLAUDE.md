# oLoggerIOT — instruções para agentes

Antes de qualquer tarefa, leia `INICIAR.md`. Ele diz onde está o projeto, o que ler primeiro e onde procurar cada informação.

- Documentação, comentários e respostas em português do Brasil.
- Código C e ESP-IDF seguem `documentacao/padrao_codigo_c.md` e `documentacao/padrao_esp_idf.md`.
- Um projeto com todas as funcionalidades, e um build por tipo de device (`ologgermulti`, `y7080`). O que cada build leva está em `builds/composicao.cmake` (`documentacao/composicao_builds.md`). Nunca `#if` pelo nome do device.
- Pequenos passos: uma função ou módulo por vez, cada um com teste.
- Ao fechar tarefa, siga a skill `doc-sync`. Ao fechar sprint, siga a skill `sprint-close`.
- Não invente regra. Conflito de regra vai para o responsável técnico.
