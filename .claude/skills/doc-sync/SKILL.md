---
name: doc-sync
description: Use ao concluir qualquer tarefa do backlog do firmware oLoggerIOT. Atualiza documentação, dicionário de funções, backlog, sprint, changelog, estado de implementação e STATUS_PROJETO.md, com versão e histórico.
---

# Sincronização da documentação

Toda tarefa só está pronta quando a documentação afetada está atualizada. Use este checklist ao fechar cada `FW-###`.

## Checklist

1. **Código:** compila os builds afetados (skill `device-profile-build`).
2. **Dicionário:** cada função nova ou alterada entra em `documentacao/dicionario_funcoes.md`. Quando `tools/gera_dicionario.py` existir (FW-004 e FW-008), regenere a partir do Doxygen em vez de editar à mão.
3. **Regras:** se o comportamento mudou, atualize o documento de regra afetado em `documentacao/`. Exemplos: `configuracao_do_dispositivo.md`, `telemetria_e_armazenamento.md`, `comunicacao_iot.md`, `mapa_modbus.md`, `composicao_builds.md`.
4. **Integração:** se o contrato com o sistema mudou (tópico, JSON, comando), atualize `documentacao/integracao_sistema.md`.
5. **Operação:** se o comportamento de campo mudou, atualize `documentacao/manual_operacao_device.md`.
6. **Estado:** atualize `documentacao/ESTADO_IMPLEMENTACAO.md`, marcando o módulo como implementado, em teste ou pendente.
7. **Changelog:** registre a mudança em `CHANGELOG.md`, na seção "Não lançado", com o `FW-###`.
8. **Backlog:** marque o `FW-###` como concluído em `planejamento/BACKLOG.md`, com a data.
9. **Sprint:** marque o item como concluído no arquivo da sprint em `planejamento/sprints/`.
10. **Status:** atualize `sistema-init/STATUS_PROJETO.md` com a versão incrementada e uma linha no histórico.
11. **Índice:** se criou documento novo, adicione-o em `documentacao/README.md` e em `documentacao/INDICE_AGENTES.md`.

## Versão e histórico

- Todo documento alterado incrementa a versão e ganha uma linha no histórico com data e descrição.
- Atualize a data de última modificação no cabeçalho.
- Nunca apague linha de histórico.

## Se algo não foi concluído

Registre o motivo na sprint e mova o item para a sprint seguinte. Não marque como concluído. Não registre no changelog o que não foi entregue.
