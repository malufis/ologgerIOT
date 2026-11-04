# src/ — código de aplicação

> **Criado em:** 02/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.1

Ponto de entrada do firmware: `main` do STM32 (coletor e orquestrador) e do ESP32 (módulo de comunicação, ESP-IDF). Aqui fica somente a composição dos módulos da pasta `lib/`; a lógica de cada módulo fica em `lib/<modulo>/`.

Regras: `documentacao/padrao_codigo_c.md`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.1 | Adicionada a tabela de histórico, que faltava. As alterações anteriores não foram registradas aqui; ver o histórico do git |
| 02/11/2026 | 0.1.0 | Criado |
