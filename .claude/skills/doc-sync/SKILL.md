---
name: doc-sync
description: Use ao concluir qualquer tarefa do backlog do firmware oLoggerIOT. Atualiza documentação, dicionário de funções, backlog, sprint e STATUS_PROJETO.md, com versão e histórico.
---

# Sincronização da documentação

Toda tarefa só está pronta quando a documentação afetada está atualizada. Use este checklist ao fechar cada `FW-###`.

## Checklist

1. **Código:** compila para o perfil afetado (use a skill `device-profile-build`).
2. **Dicionário:** regenere as funções a partir do Doxygen com `tools/gera_dicionario.py`. Não edite a tabela à mão.
3. **Regras:** se o comportamento mudou, atualize o documento de regras afetado em `documentacao/`. Exemplos: `configuracao_do_dispositivo.md`, `telemetria_e_armazenamento.md`, `comunicacao_iot.md`, `mapa_modbus.md`.
4. **Integração:** se o contrato com o sistema mudou (tópico, JSON, comando), atualize `documentacao/integracao_sistema.md`.
5. **Operação:** se o comportamento de campo mudou, atualize `documentacao/manual_operacao_device.md`.
6. **Backlog:** marque o `FW-###` como concluído em `planejamento/BACKLOG.md`, com a data.
7. **Sprint:** marque o item como concluído no arquivo da sprint em `planejamento/sprints/`.
8. **Status:** atualize `sistema-init/STATUS_PROJETO.md` com a versão incrementada e uma linha no histórico.
9. **Índice:** se criou documento novo, adicione-o em `documentacao/README.md`.

## Versão e histórico

- Todo documento alterado incrementa a versão e ganha uma linha no histórico com data e descrição.
- Atualize a data de última modificação no cabeçalho.
- Nunca apague linha de histórico.

## Se algo não foi concluído

Registre o motivo na sprint e mova o item para a sprint seguinte. Não marque como concluído.
