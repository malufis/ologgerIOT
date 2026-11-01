# Backlog do firmware — oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 01/11/2026 · **Versão:** 0.1.0

## Regras

- Tarefas pequenas, de até um dia de trabalho. Itens maiores são divididos.
- Cada item tem identificador `FW-###`, fase do plano, sprint e critério de aceite.
- **Regra de pronto:** a tarefa só fecha quando o código compila para o perfil de device afetado, os testes passam e os documentos afetados estão atualizados (regras, mapa Modbus, dicionário de funções, `STATUS_PROJETO.md`).
- Ao fechar uma tarefa, registrar no histórico do documento alterado e na sprint.
- Sprints de uma semana. Índice em `planejamento/sprints/`.

## Prazo e risco

Prazo de referência do protótipo de bancada: 29/11/2026. São quatro semanas a partir de 01/11/2026, o que cobre as sprints 00 a 03. O escopo do protótipo inclui Modbus, MQTT e NB-IoT, que estão nas sprints 05, 07 e 09. **Risco:** o cronograma de uma semana por sprint não cabe no protótipo completo. Antes da sprint 03 será preciso reavaliar o escopo do protótipo com o responsável técnico.

## Itens

### Fase 0 — Base do projeto (SPRINT_00)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-001 | Criar estrutura de pastas `src/`, `include/`, `lib/`, `devices/`, `tests/`, `tools/` com README em cada uma | Pastas existem, README explica o propósito |
| FW-002 | Criar CMake raiz com seleção de perfil de device (`-DDEVICE=ologgermulti` ou `y7080`) | Configura sem erro para cada perfil |
| FW-003 | Criar perfis `devices/ologgermulti.h` e `devices/y7080.h` com entradas, saídas, rádio e flash | Perfil declara as capacidades de cada device |
| FW-004 | Configurar Doxygen em português e gerar XML para o dicionário | `doxygen` gera saída sem erro |
| FW-005 | Criar modelo de cabeçalho de arquivo (data, versão, descrição) | Modelo usado em todo arquivo novo |
| FW-006 | Build vazio do STM32 (CMake com toolchain ARM e HAL) | Compila para o alvo STM32 |
| FW-007 | Build vazio do ESP32 (ESP-IDF) | Compila para o alvo ESP32 |
| FW-008 | Criar script de verificação do padrão C (tamanho de função e cabeçalho) | Script falha com função acima do limite |

### Fase 1 — Aquisição analógica (SPRINT_01)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-009 | Biblioteca `lib/adc`: configurar ADC com timer e DMA | Taxa de 10 a 1000 amostras por segundo |
| FW-010 | Biblioteca `lib/sensor`: média simples por segundo | Valor de 1 segundo calculado no PC com sequência sintética |
| FW-011 | Conversão de corrente para valor de engenharia com span, zero e offset | Conversão testada no PC |
| FW-012 | Teste em bancada com gerador de corrente 4–20 mA | Leitura dentro da tolerância |

### Fase 2 — Regras de sensor (SPRINT_02)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-013 | Detecção de falha abaixo de 4 mA e acima de 20 mA | Testes de limite no PC |
| FW-014 | Detecção de dente de serra: inversão acima do percentual em 5 s, com padrão 2% e 10% para inversor | Testes com linear, logarítmica e dente de serra |
| FW-015 | Tratamento de zero (sensor sem carga) e primeira leitura após zero | Sem alerta de falha no zero |
| FW-016 | Falha de sensor: saída em 0 Hz e alerta com sensor, porta e tipo | Teste de sequência de falha e recuperação |

### Fase 3 — Média móvel, fila e alertas (SPRINT_03)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-017 | Média móvel de um envio sobre os valores de 1 segundo | Teste no PC |
| FW-018 | Driver da flash SPI externa de 16 MiB (GD25Q128) | Leitura e escrita verificadas |
| FW-019 | Biblioteca `lib/fila`: fila de telemetria com substituição do mais antigo | Teste de fila cheia |
| FW-020 | Confirmação de envio: remoção só após confirmação do servidor | Teste de envio perdido e confirmado |
| FW-021 | Espaço de alertas com 300 registros | Teste de substituição |

### Fase 4 — Saídas e controle (SPRINT_04)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-022 | Saídas discretas e pulsadas | Testes de estado |
| FW-023 | Saída em frequência proporcional a 4–20 mA, com 4 mA e 20 mA configuráveis | Teste de proporcionalidade |
| FW-024 | Partida com atraso configurável e cancelamento por comando de desligar | Testes de temporização |
| FW-025 | Modo manual e automático por saída, com prioridade do sensor | Tabela de estados testada |

### Fase 5 — Modbus RTU (SPRINT_05)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-026 | Biblioteca `lib/modbus`: quadro RTU e CRC | Testes com quadros de referência |
| FW-027 | Mestre com timeout de 30 s e dez tentativas a cada 30 s | Simulador com ausência de resposta |
| FW-028 | Escravo com mapa de `documentacao/mapa_modbus.md` | Leitura e escrita dos registradores |
| FW-029 | Recuperação com cinco leituras válidas e defeito após uma hora | Teste de recuperação |

### Fase 6 — Relógio e agenda offline (SPRINT_06)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-030 | Driver do RTC e conferência NTP quatro vezes ao dia | Teste de atualização |
| FW-031 | Hora inválida: medições sem validade, saídas mantêm estado, aviso guardado | Teste de perda e correção de hora |
| FW-032 | Agenda semanal de 336 intervalos, aplicada somente offline | Teste de transição de intervalo |

### Fase 7 — MQTT e ESP32 (SPRINT_07)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-033 | Protocolo serial entre STM32 e ESP32 | Quadros validados |
| FW-034 | Cliente MQTT 5.0 com TLS 1.3 no ESP32 | Conexão com broker de teste |
| FW-035 | Tópicos do device a partir do caminho recebido do sistema | Teste com simulador |
| FW-036 | Reconexão: cinco tentativas por rádio, a cada minuto, depois offline | Teste de queda de rede |

### Fase 8 — Sessão e serial (SPRINT_08)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-037 | Sessão de configuração única com timeout de 5 minutos | Teste de concorrência e timeout |
| FW-038 | Recusa de comandos MQTT durante a sessão | Teste com comando recebido |
| FW-039 | Gateway serial no notebook (aplicação Python) com comandos restritos | Teste com chaveiro serial |

### Fase 9 — Atualização de firmware (SPRINT_08 e SPRINT_09)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-040 | Verificação de validade e compatibilidade do pacote | Teste com pacote inválido |
| FW-041 | Recuperação da versão anterior em falha ou queda de energia | Teste de queda durante gravação |
| FW-042 | Saídas desligadas durante atualização e retomada com tempo de partida | Teste de retomada |

### Fase 10 — Bancada e protótipo (SPRINT_09)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-043 | Roteiro de bancada do protótipo | Roteiro aprovado |
| FW-044 | Validação de NB-IoT com módulo e operadora (P-026) | Relatório de recepção e latência |
| FW-045 | Execução do roteiro de bancada e relatório | Relatório registrado |

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 01/11/2026 | 0.1.0 | Criado backlog com 45 tarefas em 11 fases, vinculadas às sprints |
