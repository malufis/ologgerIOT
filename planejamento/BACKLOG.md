# Backlog do firmware — oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.1.11

## Regras

- Tarefas pequenas, de até um dia de trabalho. Itens maiores são divididos.
- Cada item tem identificador `FW-###`, fase do plano, sprint e critério de aceite.
- **Regra de pronto:** a tarefa só fecha quando o código compila para o build afetado, os testes passam e os documentos afetados estão atualizados (regras, mapa Modbus, dicionário de funções, `STATUS_PROJETO.md`).
- Ao fechar uma tarefa, registrar no histórico do documento alterado e na sprint.
- Sprints de uma semana. Índice em `planejamento/sprints/`.

## Prazo e risco

Prazo de referência do protótipo de bancada: 29/11/2026. São quatro semanas a partir de 01/11/2026, o que cobre as sprints 00 a 03. O escopo do protótipo inclui Modbus, MQTT e NB-IoT, que estão nas sprints 05, 07 e 09. **Risco:** o cronograma de uma semana por sprint não cabe no protótipo completo. Antes da sprint 03 será preciso reavaliar o escopo do protótipo com o responsável técnico.

## Itens

### Fase 0 — Base do projeto (SPRINT_00)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-001 | Criar estrutura de pastas `src/`, `include/`, `lib/`, `devices/`, `tests/`, `tools/` com README em cada uma | Pastas existem, README explica o propósito |
| FW-002 | Criar CMake raiz que recebe `-DDEVICE=ologgermulti` ou `y7080`, lê `builds/composicao.cmake` e liga só as funcionalidades do build (ver `documentacao/composicao_builds.md`) | Configura sem erro os dois builds e recusa device e funcionalidade inválidos |
| FW-003 | Revisar a composição proposta em `builds/composicao.cmake` com o responsável técnico e criar a pinagem em `devices/ologgermulti/` e `devices/y7080/` | Composição aprovada; pinagem de cada build incluída pelo CMake |
| FW-004 | Configurar Doxygen em português e gerar XML para o dicionário | `doxygen` gera saída sem erro |
| FW-005 | Criar modelo de cabeçalho de arquivo (data, versão, descrição) | Modelo usado em todo arquivo novo |
| FW-006 | Build vazio do STM32 (CMake com toolchain ARM e HAL) | Compila para o alvo STM32 |
| FW-007 | Build vazio do ESP32 (ESP-IDF) | Compila para o alvo ESP32 |
| FW-008 | Criar script de verificação do padrão C (tamanho de função e cabeçalho) | Script falha com função acima do limite |
| FW-046 | Base FreeRTOS no STM32 e no ESP-IDF: configuração com alocação estática, tabela de prioridades e uma task de teste em cada alvo (`padrao_codigo_c.md`, seção FreeRTOS) | Task de teste roda nos dois alvos, mede pilha e não usa alocação dinâmica |

### Fase 1 — Aquisição analógica (SPRINT_01)

| ID | Tarefa | Aceite |
|---|---|---|
| FW-009 | Biblioteca `lib/adc`: configurar ADC com timer e DMA | Taxa de 10 a 1000 amostras por segundo |
| FW-010 | Biblioteca `lib/sensor`: média simples por segundo | Valor de 1 segundo calculado no PC com sequência sintética |
| FW-011 | Conversão de corrente para valor de engenharia com span, zero e offset | Conversão testada no PC |
| FW-012 | Teste em bancada com gerador de corrente 4–20 mA | Leitura dentro da tolerância |

### Fase 1B — Entrada digital (SPRINT_01B)

Entrada digital de sinal ativo em nível alto, discreta (0 ou 1) ou de frequência em Hz, com filtro de ruído e despertar do sleep. Decisões de 03/11/2026 em `documentacao/configuracao_do_dispositivo.md`.

| ID | Tarefa | Aceite |
|---|---|---|
| FW-052 | Biblioteca `lib/io`: leitura do nível das entradas digitais D1 a D4 pelas macros de `pinos.h`; nível alto = ativo | Leitura confere com a tensão aplicada na bancada |
| FW-053 | `io_filtro`: filtro de ruído que aceita a mudança após N amostras seguidas do nível oposto | Testes no PC: mudança após N amostras, pulso curto descartado, ruído alternado sem mudança |
| FW-054 | Modo de frequência em Hz: contagem de bordas em janela, escolhido por entrada na configuração | Teste no PC com sinal sintético de frequência conhecida |
| FW-055 | Contagem de pulsos dormindo: interrupção externa na borda que ativa a entrada (PB5 a PB8) soma o pulso no contador e no totalizador e pede para o STM32 acordar | Teste em bancada: o pulso acorda o STM32 em Stop 2, o pulso que acordou é contado uma vez e nenhum se perde |
| FW-056 | Roteiro e execução de bancada da entrada digital: nível, filtro, contador de pulsos, totalizador e sleep (`documentacao/roteiro_bancada_entrada_digital.md`) | Relatório registrado |
| FW-057 | `io_pulsos`: contador de pulsos por entrada, com 60 s padrão sem pulso para poder dormir, e registro do pulso que acordou | Testes no PC: contagem, pulso mantido, despertar sem contar duas vezes, inatividade e volta do contador de tempo |
| FW-058 | Entrada em Stop 2 pelo contador de pulsos: dorme após o tempo sem pulso, para a aquisição analógica ao dormir, acorda pelo pulso e fica acordado enquanto houver pulso; o despertar para transmitir fica em outra sprint | Teste em bancada: ciclo de dormir e acordar, pulso constante sem acordar e dormir a cada pulso, com a corrente medida |
| FW-059 | Contagem de pulsos em hardware (LPTIM1 na entrada D4, PB5) durante o sleep, sem acordar. **A confirmar**: com quatro leitores só um conta assim; depende do RM0394 | Teste em bancada: pulsos contados em Stop 2 sem acordar, conferidos com um gerador |
| FW-060 | Totalizador (hodômetro) de 32 bits por entrada, nos registradores de backup, com duas cópias e complemento e gravação a cada pulso | Teste no PC: soma, volta a zero e queda de energia em cada gravação; em bancada: reinício e queda de energia sem perder o total |

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
| FW-047 | `lib/tabela_variaveis`: 64 variáveis na RAM, com estado, marca de tempo e mutex estático | Teste de leitura e escrita concorrente no PC, estados e limite de 64 |
| FW-048 | `lib/mapeamento`: configuração de até 8 equipamentos, validação completa e gravação atômica em flash com CRC | Teste de configuração válida, inválida e de queda na gravação |
| FW-049 | `lib/modbus_mestre` em task FreeRTOS, não bloqueante: um equipamento sem resposta não atrasa os demais | Simulador com um equipamento mudo e os outros respondendo no prazo |
| FW-050 | `lib/modbus_escravo`: expõe a tabela no mapa fixo com registrador de status por variável | Leitura de valor e de estado pelo simulador Modbus |

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
| FW-051 | Procedimento de gravação do OloggerMulti com os bits de opção `nSWBOOT0=0` e `nBOOT0=1`, porque o BOOT0 do U1 está sem ligação (HW-006) | Script ou roteiro executado em uma placa nova, com leitura de volta dos bits e do PC em `0x0800....` |

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 01/11/2026 | 0.1.0 | Criado backlog com 45 tarefas em 11 fases, vinculadas às sprints |
| 02/11/2026 | 0.1.1 | FW-001 concluído: pastas `src/`, `include/`, `lib/`, `devices/`, `tests/`, `tools/` com README |
| 02/11/2026 | 0.1.2 | FW-002 e FW-003 reescritos para imagem única por MCU e setup de composição, no lugar de seleção de perfil de compilação; regra de pronto e documentação ampliadas (estado, changelog, dicionário) |
| 02/11/2026 | 0.1.5 | FW-051 adicionado: procedimento de gravação com bits de opção, por causa do BOOT0 sem ligação no OloggerMulti |
| 02/11/2026 | 0.1.4 | FreeRTOS em todo o sistema: FW-046 na base (SPRINT_00); FW-047 a FW-050 para o mapeamento Modbus (SPRINT_05), conforme `documentacao/modbus_mapeamento_dados.md` aprovado |
| 02/11/2026 | 0.1.3 | Composição por build: FW-002 seleciona o build por `-DDEVICE` e lê `builds/composicao.cmake`; FW-003 revisa a composição e cria a pinagem por placa; setup em runtime removido |
| 03/11/2026 | 0.1.11 | Correção: o pulso acorda o STM32 e ele fica acordado, dormindo só após o tempo de inatividade; FW-055, FW-056 e FW-058 reescritos |
| 03/11/2026 | 0.1.10 | FW-058 (Stop 2) e FW-060 (totalizador em hodômetro) detalhados e implementados; FW-059 rebaixado |
| 03/11/2026 | 0.1.9 | FW-055 redefinido: o tratador da interrupção externa conta o pulso e o STM32 volta a dormir |
| 03/11/2026 | 0.1.8 | Fase 1B: FW-059 (contagem em hardware no LPTIM1) e FW-060 (contagem persistente) propostos, a confirmar |
| 03/11/2026 | 0.1.7 | Fase 1B: uso da entrada digital como contador de pulsos que dorme após 60 s sem pulso; FW-055 e FW-056 reescritos e FW-057 e FW-058 adicionados |
| 03/11/2026 | 0.1.6 | Fase 1B adicionada (SPRINT_01B) com FW-052 a FW-056 para a entrada digital: nível alto ativo, filtro de ruído, modo de frequência, despertar do sleep e bancada |
