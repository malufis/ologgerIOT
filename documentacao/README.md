# Documentação do oLoggerIOT — índice

> **Criado em:** 01/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.2.2

Ponto de entrada para agentes e programadores. Comece por `INICIAR.md` na raiz do repositório. Para achar um assunto, use `INDICE_AGENTES.md`. Este arquivo lista todos os documentos. Cada documento informa a versão e tem tabela de histórico.

## Entrada e estado

| Documento | Conteúdo |
|---|---|
| `../INICIAR.md` | Ponto de partida de cada sessão: onde estamos e onde procurar. |
| `INDICE_AGENTES.md` | Índice por assunto e por skill/agente, para agentes. |
| `ESTADO_IMPLEMENTACAO.md` | O que já está implementado, em teste ou pendente. |
| `../CHANGELOG.md` | Mudanças por versão e por item do backlog. |
| `../sistema-init/STATUS_PROJETO.md` | Estado do projeto, decisões e histórico. Referência principal. |
| `../sistema-init/TRANSFERENCIA_CONTINUIDADE.md` | Resumo para continuidade entre agentes. |

## Requisitos e regras do device

| Documento | Conteúdo |
|---|---|
| `configuracao_do_dispositivo.md` | Calibração, falhas de sensor, dente de serra, saídas, alarmes e modos. |
| `telemetria_e_armazenamento.md` | Coleta, fila, envio, sleep, alertas (300 registros) e validade de horário. |
| `agenda_operacional_offline.md` | Agenda semanal offline, relógio, NTP e comandos adiados. |
| `comunicacao_iot.md` | Modbus RTU, MQTT, credenciais, tópicos e confirmações. |
| `atualizacao_firmware.md` | Atualização, recuperação e saídas durante a atualização. |
| `modelos_e_versoes_firmware.md` | Modelos OloggerMulti e y7080 e a decisão de um build por device. |
| `composicao_builds.md` | Funcionalidades de cada build, catálogo e composição proposta. A lista fica em `builds/composicao.cmake`. |
| `analise_hardware_ologgeriot.md` | Análise dos esquemáticos e Gerbers. |
| `roteiro_bancada_entrada_digital.md` | Roteiro de bancada da entrada digital, do contador de pulsos, do totalizador e do sleep (FW-056). |
| `melhorias_hardware.md` | Mudanças de hardware anotadas para a próxima revisão das placas (pull-down das saídas, resistor e proteção das entradas, VBAT). |

## Interfaces

| Documento | Conteúdo |
|---|---|
| `mapa_modbus.md` | Mapa Modbus do device (norma, float em 2 registradores, 15 posições por tipo). |
| `modbus_mapeamento_dados.md` | Como o device guarda os dados dos equipamentos Modbus externos: tabela na RAM e mapeamento configurável. |
| `integracao_sistema.md` | Contrato de integração com o sistema: MQTT, JSON, comandos e serial. |

## Desenvolvimento

| Documento | Conteúdo |
|---|---|
| `../planejamento/PLANO_IMPLANTACAO_FIRMWARE.md` | Fases de implantação, aceite e testes. |
| `../planejamento/BACKLOG.md` | Backlog com itens FW-### e vínculo com as fases. |
| `../planejamento/sprints/` | Sprints de uma semana, com metas e tarefas. |
| `padrao_codigo_c.md` | Padrão de código C (base MISRA C:2012, pequenos códigos, boas práticas e funcionalidade por `#if OLOG_FEAT_<NOME>`). |
| `padrao_esp_idf.md` | Padrão de desenvolvimento ESP-IDF da Espressif. |
| `dicionario_funcoes.md` | Dicionário de funções criadas no firmware. |
| `manual_operacao_device.md` | Operação do device em campo: comissionamento, sessão, falhas e atualização. |

## Padrões

- Documentos em português do Brasil.
- Cabeçalho com data de criação, data de modificação e versão.
- Alterações geram incremento de versão e linha no histórico.
- Toda tarefa concluída atualiza os documentos afetados (regra de pronto em `planejamento/BACKLOG.md` e checklist da skill `doc-sync`).
- Toda sprint fechada passa pelo checklist da skill `sprint-close`.
- Comunicação do device é sempre por MQTT ou serial.
