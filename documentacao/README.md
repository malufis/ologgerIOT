# Documentação do oLoggerIOT — índice

> **Criado em:** 01/11/2026 · **Última modificação:** 01/11/2026 · **Versão:** 0.1.0

Ponto de entrada para agentes e programadores. Leia este índice antes de qualquer tarefa. Cada documento informa a versão e tem tabela de histórico.

## Estado e decisões

| Documento | Conteúdo |
|---|---|
| `sistema-init/STATUS_PROJETO.md` | Estado do projeto, decisões e histórico. Referência principal. |
| `sistema-init/TRANSFERENCIA_CONTINUIDADE.md` | Resumo para continuidade entre agentes. |

## Requisitos e regras do device

| Documento | Conteúdo |
|---|---|
| `documentacao/configuracao_do_dispositivo.md` | Calibração, falhas de sensor, dente de serra, saídas, alarmes e modos. |
| `documentacao/telemetria_e_armazenamento.md` | Coleta, fila, envio, sleep, alertas (300 registros) e validade de horário. |
| `documentacao/agenda_operacional_offline.md` | Agenda semanal offline, relógio, NTP e comandos adiados. |
| `documentacao/comunicacao_iot.md` | Modbus RTU, MQTT, credenciais, tópicos e confirmações. |
| `documentacao/atualizacao_firmware.md` | Atualização, recuperação e saídas durante a atualização. |
| `documentacao/modelos_e_versoes_firmware.md` | Modelos OloggerMulti e y7080, perfis e compilação única. |
| `documentacao/analise_hardware_ologgeriot.md` | Análise dos esquemáticos e Gerbers. |

## Interfaces

| Documento | Conteúdo |
|---|---|
| `documentacao/mapa_modbus.md` | Mapa Modbus do device (norma, float em 2 registradores, 15 posições por tipo). |
| `documentacao/integracao_sistema.md` | Contrato de integração com o sistema: MQTT, JSON, comandos e serial. |

## Desenvolvimento

| Documento | Conteúdo |
|---|---|
| `planejamento/PLANO_IMPLANTACAO_FIRMWARE.md` | Fases de implantação, aceite e testes. |
| `planejamento/BACKLOG.md` | Backlog com itens FW-### e vínculo com as fases. |
| `planejamento/sprints/` | Sprints de uma semana, com metas e tarefas. |
| `documentacao/padrao_codigo_c.md` | Padrão de código C (base MISRA C:2012 e regras do projeto). |
| `documentacao/dicionario_funcoes.md` | Dicionário de funções, mantido a partir do Doxygen. |
| `documentacao/manual_operacao_device.md` | Operação do device em campo: comissionamento, sessão, falhas e atualização. |

## Padrões

- Documentos em português do Brasil.
- Cabeçalho com data de criação, data de modificação e versão.
- Alterações geram incremento de versão e linha no histórico.
- Toda tarefa concluída atualiza os documentos afetados (regra de pronto em `planejamento/BACKLOG.md`).
- Comunicação do device é sempre por MQTT ou serial.
