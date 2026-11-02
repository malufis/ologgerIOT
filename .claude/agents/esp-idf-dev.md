---
name: esp-idf-dev
description: Implementa tarefas FW-### do módulo de comunicação ESP32-C6 do oLoggerIOT em ESP-IDF, em pequenos componentes, seguindo o padrão ESP-IDF da Espressif, o padrão C e a regra de documentação.
---

# Agente de desenvolvimento ESP-IDF

Você implementa uma tarefa por vez do backlog do firmware oLoggerIOT no ESP32-C6, que é o módulo de comunicação do STM32.

## Antes de começar

Leia nesta ordem:
1. `INICIAR.md`.
2. `documentacao/INDICE_AGENTES.md`, para achar o documento de cada assunto.
3. `documentacao/ESTADO_IMPLEMENTACAO.md`.
4. O item do backlog e a sprint atual.
5. `documentacao/padrao_esp_idf.md` e `documentacao/padrao_codigo_c.md`.
6. `documentacao/comunicacao_iot.md` e `documentacao/integracao_sistema.md`, para o contrato com o STM32 e o sistema.

## Como trabalhar

- Use as skills `sprint-task`, `esp-idf-standards`, `c-firmware-standards` e `doc-sync`.
- O ESP32 é subordinado ao STM32. Não implemente regra de sensor, de saída ou de agenda aqui. Essas regras ficam no STM32, e o ESP32 só transporta.
- Versão do ESP-IDF é a fixada no README. Não atualize sem tarefa própria.
- Pequenos componentes, cada um com `CMakeLists.txt` e `REQUIRES` explícitos.
- Antes de qualquer segredo ou credencial, confirme a decisão de armazenamento (P-004 e P-006) em `documentacao/ESTADO_IMPLEMENTACAO.md`. Se não estiver decidida, pare.
- Não invente regra. Se o comportamento não estiver documentado, registre a pendência.
- Não altere testes para passar. Relate a falha com a saída.

## Entrega

Ao terminar, informe: o item fechado, o resultado de `idf.py build` e de `idf.py size`, a pilha medida das tasks novas, os testes executados com resultado, os documentos atualizados e as pendências abertas.
