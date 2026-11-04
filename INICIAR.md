# Iniciar — oLoggerIOT

> **Criado em:** 02/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.2.5

Leia este arquivo no começo de todo prompt novo. Ele aponta onde está cada informação. Depois dele, use `documentacao/INDICE_AGENTES.md` para achar o documento de um assunto específico.

## Onde estamos

- **Fase:** 0 (base do projeto) concluída, exceto o FW-007; fases 1 (aquisição analógica) e 1B (entrada digital e contador de pulsos) em andamento (`planejamento/PLANO_IMPLANTACAO_FIRMWARE.md`).
- **Sprints atuais:** SPRINT_00 (02/11/2026 a 06/11/2026) e SPRINT_01B (entrada digital, paralela). A SPRINT_04 (saídas) começou pelo FW-022. A SPRINT_01 (aquisição analógica) está bloqueada pela ferrite L2 (`planejamento/sprints/`).
- **Itens concluídos:** FW-001 a FW-006, FW-008, FW-046 (parte STM32), FW-010, FW-011 (no código; calibração a refazer depois da L2), FW-053 e FW-057. **Código pronto, sem teste em hardware:** FW-022, FW-024, FW-052, FW-055, FW-058 e a lógica do FW-060. PR #1 aberto para `main` (branch `feat/fw-002-cmake-raiz`).
- **ESP32:** o FW-007 e a parte ESP32 do FW-046 foram para a SPRINT_07 e só começam depois de finalizados os testes do STM32 e de o STM32 estar validado (decisão de 03/11/2026). A SPRINT_00 está pronta para o `sprint-close`.
- **Antes de fechar um item:** `python tools/verifica_padrao.py`, `doxygen Doxyfile` sem avisos e `python tools/gera_dicionario.py --check`.
- **Próximo:** executar o roteiro de bancada da entrada digital (`documentacao/roteiro_bancada_entrada_digital.md`, FW-056) quando o device estiver disponível, e soldar a L2 para fechar o FW-012. Na bancada: D1 retirada e limite de 12 V (HW-008).
- **Estado do código:** aquisição analógica, entrada digital, contador de pulsos, totalizador e gerência de energia implementados; os dois builds compilam sem avisos. Ver `documentacao/ESTADO_IMPLEMENTACAO.md`.
- **Prazo de referência:** protótipo de bancada em 29/11/2026 (`planejamento/BACKLOG.md`, seção "Prazo e risco").

## Ordem de leitura

1. Este arquivo.
2. `sistema-init/STATUS_PROJETO.md`: decisões e histórico. É a referência principal.
3. `documentacao/ESTADO_IMPLEMENTACAO.md`: o que já está implementado e o que falta.
4. `planejamento/BACKLOG.md` e a sprint atual.
5. O documento de regra citado no item que você vai executar.

## Onde procurar

| Preciso de | Onde está |
|---|---|
| Regra de comportamento do device | `documentacao/configuracao_do_dispositivo.md`, `telemetria_e_armazenamento.md`, `agenda_operacional_offline.md`, `comunicacao_iot.md`, `atualizacao_firmware.md` |
| Modbus (endereços e tipos) | `documentacao/mapa_modbus.md` |
| O que cada build leva (funcionalidades) | `builds/composicao.cmake` e `documentacao/composicao_builds.md` |
| Pinagem de cada placa | `devices/` |
| Contrato com o sistema (MQTT, JSON, comandos) | `documentacao/integracao_sistema.md` |
| Padrão de código C | `documentacao/padrao_codigo_c.md` |
| Padrão ESP-IDF | `documentacao/padrao_esp_idf.md` |
| Funções já existentes | `documentacao/dicionario_funcoes.md` |
| Changelog | `CHANGELOG.md` |
| Build e comandos | `README.md` |
| Operação em campo | `documentacao/manual_operacao_device.md` |
| Roteiro de bancada da entrada digital | `documentacao/roteiro_bancada_entrada_digital.md` |
| Hardware (esquemas, pinos) | `documentacao/analise_hardware_ologgeriot.md` |
| Plano, backlog e sprints | `planejamento/` |
| Decisões pendentes | `sistema-init/STATUS_PROJETO.md`, seção "Pendências" |

## Regras que valem sempre

- Pequenos passos: uma função ou módulo por vez, cada um com teste.
- Um projeto, um build por tipo de device. Funcionalidade por `#if OLOG_FEAT_<NOME>`; nada de código por nome de placa.
- Toda função nova entra no dicionário de funções.
- Toda tarefa fechada atualiza estado, changelog, backlog, sprint e status (`doc-sync`).
- Toda sprint fechada passa pelo checklist completo (`sprint-close`).
- Não invente regra. Pendência vai para o responsável técnico.

## Skills e agentes

- Skills: `sprint-task`, `c-firmware-standards`, `esp-idf-standards`, `device-profile-build` (builds por device), `doc-sync`, `sprint-close`.
- Agentes: `firmware-dev` (C), `esp-idf-dev` (ESP32), `firmware-reviewer` (revisão sem edição), `doc-keeper` (conferência de documentação).

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.2.5 | Correções da verificação do doc-keeper antes do fechamento da SPRINT_00 (números, nomes dos registradores, concordância e referências) |
| 04/11/2026 | 0.2.4 | Adicionada a tabela de histórico, que faltava. As alterações anteriores não foram registradas aqui; ver o histórico do git |
| 02/11/2026 | 0.1.0 | Criado |
