# Iniciar — oLoggerIOT

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.1.0

Leia este arquivo no começo de todo prompt novo. Ele aponta onde está cada informação. Depois dele, use `documentacao/INDICE_AGENTES.md` para achar o documento de um assunto específico.

## Onde estamos

- **Fase:** 0 — base do projeto (`planejamento/PLANO_IMPLANTACAO_FIRMWARE.md`).
- **Sprint atual:** SPRINT_00, de 02/11/2026 a 06/11/2026 (`planejamento/sprints/SPRINT_00.md`).
- **Itens concluídos:** FW-001, FW-002, FW-004, FW-005, FW-006 e FW-008, no branch `feat/fw-002-cmake-raiz`.
- **Pendentes da SPRINT_00:** FW-003, FW-007 e FW-046.
- **Antes de fechar um item:** `python tools/verifica_padrao.py`, `doxygen Doxyfile` sem avisos e `python tools/gera_dicionario.py --check`.
- **Próximo item:** FW-003 (revisão da composição e pinagem por placa).
- **Estado do código:** ainda não há código de aplicação. Ver `documentacao/ESTADO_IMPLEMENTACAO.md`.
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
