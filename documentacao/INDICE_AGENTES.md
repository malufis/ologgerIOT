# Índice para agentes — oLoggerIOT

> **Criado em:** 02/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.2.2

Use este índice para achar o documento de cada assunto. Comece sempre por `INICIAR.md`. Para o índice completo, use `documentacao/README.md`.

## Pergunta → documento

| Se a pergunta é sobre... | Leia |
|---|---|
| Em que pé está o projeto e o que fazer agora | `INICIAR.md`, `planejamento/sprints/` |
| Decisão já tomada e histórico | `sistema-init/STATUS_PROJETO.md` |
| Decisão pendente | `sistema-init/STATUS_PROJETO.md`, seção "Pendências" |
| O que já está implementado | `documentacao/ESTADO_IMPLEMENTACAO.md` |
| O que mudou em cada versão | `CHANGELOG.md` |
| Uma função existente | `documentacao/dicionario_funcoes.md` |
| Como escrever código C | `documentacao/padrao_codigo_c.md` |
| Tasks, filas, mutex e FreeRTOS | `documentacao/padrao_codigo_c.md` (seção FreeRTOS) e `documentacao/padrao_esp_idf.md` |
| Como escrever código ESP-IDF | `documentacao/padrao_esp_idf.md` |
| O que cada build leva (funcionalidades) | `builds/composicao.cmake` e `documentacao/composicao_builds.md` |
| Pinagem de uma placa | `devices/` |
| Modelos OloggerMulti e y7080 | `documentacao/modelos_e_versoes_firmware.md` |
| Como compilar e rodar | `README.md`, `planejamento/BACKLOG.md` (FW-002, FW-006, FW-007) |
| Entrada e saída de sensor, falha, dente de serra | `documentacao/configuracao_do_dispositivo.md` |
| Alarmes, modos manual e automático | `documentacao/configuracao_do_dispositivo.md` |
| Coleta, fila, envio, alertas, sleep | `documentacao/telemetria_e_armazenamento.md` |
| Agenda semanal, relógio e NTP | `documentacao/agenda_operacional_offline.md` |
| Modbus RTU, endereços e tipos de registrador | `documentacao/mapa_modbus.md`, `documentacao/comunicacao_iot.md` |
| Dados de equipamentos Modbus externos (tabela, mapeamento, limites) | `documentacao/modbus_mapeamento_dados.md` |
| MQTT, tópicos, credencial e confirmação | `documentacao/comunicacao_iot.md`, `documentacao/integracao_sistema.md` |
| Formato de mensagem para o sistema (JSON) | `documentacao/integracao_sistema.md` |
| Atualização de firmware e recuperação | `documentacao/atualizacao_firmware.md` |
| Operação em campo (comissionamento, sessão) | `documentacao/manual_operacao_device.md` |
| Esquemas, pinos e componentes de hardware | `documentacao/analise_hardware_ologgeriot.md` |
| Entrada digital, contador de pulsos e totalizador | `documentacao/configuracao_do_dispositivo.md`, `planejamento/sprints/SPRINT_01B.md`, `lib/io/` |
| Sleep, Stop 2 e despertar do STM32 | `planejamento/sprints/SPRINT_01B.md`, `documentacao/telemetria_e_armazenamento.md`, `lib/energia/` |
| Roteiro de teste de bancada da entrada digital | `documentacao/roteiro_bancada_entrada_digital.md` |
| Saídas digitais (circuito, estado, comandos, frequência) | `planejamento/sprints/SPRINT_04.md`, `documentacao/configuracao_do_dispositivo.md`, `lib/io/` (`saida`, `io_saida`, `io_saidas`) |
| Bloqueios de bancada (L2, D1, limite de 12 V) | `documentacao/analise_hardware_ologgeriot.md` (HW-007 e HW-008) |
| Melhorias de hardware para a próxima revisão das placas | `documentacao/melhorias_hardware.md` |
| Módulos existentes em `lib/` | `lib/README.md` |
| Testes e critérios de aceite | `planejamento/PLANO_IMPLANTACAO_FIRMWARE.md`, `planejamento/BACKLOG.md` |
| Desvio do MISRA | `documentacao/desvios_misra.md` (criado quando houver desvio) |

## Skill → quando usar

| Skill | Quando |
|---|---|
| `sprint-task` | Executar um item FW-### do início ao fim |
| `c-firmware-standards` | Escrever ou revisar código C |
| `esp-idf-standards` | Escrever ou revisar código ESP-IDF |
| `device-profile-build` | Compilar as imagens |
| `doc-sync` | Fechar um item do backlog |
| `sprint-close` | Fechar uma sprint |

## Agente → quando usar

| Agente | Quando |
|---|---|
| `firmware-dev` | Implementar item em C |
| `esp-idf-dev` | Implementar item no ESP32 |
| `firmware-reviewer` | Revisar alteração antes de fechar (não edita) |
| `doc-keeper` | Conferir documentação antes de fechar sprint (não edita código) |

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.2.2 | Adicionada a entrada de melhorias de hardware |
| 03/11/2026 | 0.2.1 | Adicionada a entrada de saídas digitais |
| 03/11/2026 | 0.2.0 | Adicionadas as entradas de entrada digital, sleep, roteiro de bancada, bloqueios de bancada e módulos de lib |
| 02/11/2026 | 0.1.0 | Criado |
