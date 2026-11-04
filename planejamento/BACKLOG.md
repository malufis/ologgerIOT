# Backlog do firmware — oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.2.23

## Regras

- Tarefas pequenas, de até um dia de trabalho. Itens maiores são divididos.
- Cada item tem identificador `FW-###`, fase do plano, sprint e critério de aceite.
- **Regra de pronto:** a tarefa só fecha quando o código compila para o build afetado, os testes passam e os documentos afetados estão atualizados (regras, mapa Modbus, dicionário de funções, `STATUS_PROJETO.md`).
- Ao fechar uma tarefa, registrar no histórico do documento alterado e na sprint.
- Sprints de uma semana. Índice em `planejamento/sprints/`.
- **Situação:** *Concluído* (código e testes passam, documentação atualizada); *Código pronto* (compila e a lógica passa nos testes do PC, mas falta validar em hardware); *Em andamento*; *Pendente*; *A confirmar* (depende de decisão do responsável técnico); *Adiado*. Só o item *Concluído* cumpre a regra de pronto.

## Prazo e risco

Prazo de referência do protótipo de bancada: 29/11/2026. São quatro semanas a partir de 01/11/2026, o que cobre as sprints 00 a 03. A data é um domingo, na véspera do início da SPRINT_04 (30/11/2026). O escopo do protótipo inclui Modbus, MQTT e NB-IoT, que estão nas sprints 05, 07 e 09.

**Risco:** o cronograma de uma semana por sprint não cabe no protótipo completo. Antes da SPRINT_03 será preciso reavaliar o escopo do protótipo com o responsável técnico.

**Reavaliação de 03/11/2026:**

- **Entrou escopo novo:** a fase 1B (entrada digital, contador de pulsos, totalizador e Stop 2, FW-052 a FW-060) não estava no plano original. A maior parte está em código pronto, mas falta validar em bancada.
- **A bancada é o gargalo:** a SPRINT_01 está bloqueada pela ferrite L2 (HW-007), e a validação da fase 1B e do FW-012 depende do device. Nada do que foi implementado desde 02/11/2026 passou por hardware, exceto a aquisição analógica parcial e o FreeRTOS.
- **ESP32 para depois da validação do STM32:** o FW-007 e a parte ESP32 do FW-046 foram para a SPRINT_07 e só começam depois de finalizados os testes do STM32 e de o STM32 estar validado (decisão de 03/11/2026). A fase 7 (MQTT) depende disso, e o atraso na validação do STM32 empurra o ESP32 e o protótipo completo.
- **Sem decisão de escopo ainda:** o que sai ou entra no protótipo continua com o responsável técnico.

## Itens

### Fase 0 — Base do projeto (SPRINT_00)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-001 | Criar estrutura de pastas `src/`, `include/`, `lib/`, `devices/`, `tests/`, `tools/` com README em cada uma | Pastas existem, README explica o propósito | Concluído em 02/11/2026 |
| FW-002 | Criar CMake raiz que recebe `-DDEVICE=ologgermulti` ou `y7080`, lê `builds/composicao.cmake` e liga só as funcionalidades do build (ver `documentacao/composicao_builds.md`) | Configura sem erro os dois builds e recusa device e funcionalidade inválidos | Concluído em 02/11/2026 |
| FW-003 | Revisar a composição proposta em `builds/composicao.cmake` com o responsável técnico e criar a pinagem em `devices/ologgermulti/` e `devices/y7080/` | Composição aprovada; pinagem de cada build incluída pelo CMake | Concluído em 02/11/2026 |
| FW-004 | Configurar Doxygen em português e gerar XML para o dicionário | `doxygen` gera saída sem erro | Concluído em 02/11/2026 |
| FW-005 | Criar modelo de cabeçalho de arquivo (data, versão, descrição) | Modelo usado em todo arquivo novo | Concluído em 02/11/2026 |
| FW-006 | Build vazio do STM32 (CMake com toolchain ARM e HAL) | Compila para o alvo STM32 | Concluído em 02/11/2026 |
| FW-007 | Build vazio do ESP32 (ESP-IDF) | Compila para o alvo ESP32 | Movido para a SPRINT_07 (decisão de 03/11/2026): só começa depois de finalizados os testes do STM32 e de o STM32 estar validado |
| FW-008 | Criar script de verificação do padrão C (tamanho de função e cabeçalho) | Script falha com função acima do limite | Concluído em 02/11/2026 |
| FW-046 | Base FreeRTOS no STM32 e no ESP-IDF: configuração com alocação estática, tabela de prioridades e uma task de teste em cada alvo (`padrao_codigo_c.md`, seção FreeRTOS) | Task de teste roda nos dois alvos, mede pilha e não usa alocação dinâmica | Concluído no STM32 em 02/11/2026; a parte ESP32 fica com o FW-007, na SPRINT_07, depois da validação do STM32 |

### Fase 1 — Aquisição analógica (SPRINT_01)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-009 | Biblioteca `lib/adc`: configurar ADC com timer e DMA | Taxa de 10 a 1000 amostras por segundo | Código pronto; medido em bancada com referência provisória; refazer depois da L2 |
| FW-010 | Biblioteca `lib/sensor`: média simples por segundo | Valor de 1 segundo calculado no PC com sequência sintética | Concluído em 02/11/2026; 57 verificações passam no PC (reexecutado em 03/11/2026) |
| FW-011 | Conversão de corrente para valor de engenharia com span, zero e offset | Conversão testada no PC | Concluído no código; calibração de dois pontos, refazer em cinco depois da L2 |
| FW-012 | Teste em bancada com gerador de corrente 4–20 mA | Leitura dentro da tolerância | Em andamento: 8 mA na entrada 4 lidos como 8,007 mA; bloqueado pela L2 (HW-007) |

### Fase 1B — Entrada digital (SPRINT_01B)

Entrada digital ativa com tensão no campo (de 12 a 24 V; na bancada, até 12 V), discreta (0 ou 1), de frequência em Hz ou contador de pulsos, com filtro de ruído, totalizador estilo hodômetro e despertar do sleep. Decisões de 03/11/2026 em `documentacao/configuracao_do_dispositivo.md`.

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-052 | Biblioteca `lib/io`: leitura do estado das entradas digitais D1 a D4 pelas macros de `pinos.h`. No OloggerMulti o optoacoplador inverte o sinal: com tensão no campo o pino fica em nível baixo, e a leitura entrega "ativa" | Leitura confere com a tensão aplicada na bancada | Código pronto em 03/11/2026; leitura em bancada pendente |
| FW-053 | `io_filtro`: filtro de ruído que aceita a mudança após N amostras seguidas do nível oposto | Testes no PC: mudança após N amostras, pulso curto descartado, ruído alternado sem mudança | Concluído em 03/11/2026 (135 verificações no PC) |
| FW-054 | Modo de frequência em Hz: contagem de bordas em janela, escolhido por entrada na configuração | Teste no PC com sinal sintético de frequência conhecida | Pendente; faixa de 0 a 20 kHz, ajuste fica para depois |
| FW-055 | Contagem de pulsos dormindo: interrupção externa na borda que ativa a entrada (PB5 a PB8) soma o pulso no contador e no totalizador e pede para o STM32 acordar | Teste em bancada: o pulso acorda o STM32 em Stop 2, o pulso que acordou é contado uma vez e nenhum se perde | Código pronto em 03/11/2026; bancada pendente |
| FW-056 | Roteiro e execução de bancada da entrada digital: nível, filtro, contador de pulsos, totalizador e sleep (`documentacao/roteiro_bancada_entrada_digital.md`) | Relatório registrado | Roteiro escrito em 03/11/2026; execução pendente (precisa do device) |
| FW-057 | `io_pulsos`: contador de pulsos por entrada, com 60 s padrão (configurável) sem pulso para poder dormir, e soma dos pulsos contados dormindo | Testes no PC: contagem, pulso mantido, despertar sem contar duas vezes, inatividade e volta do contador de tempo | Concluído em 03/11/2026 (25 verificações no PC) |
| FW-058 | Entrada em Stop 2 pelo contador de pulsos: dorme após o tempo sem pulso, para a aquisição analógica ao dormir, acorda pelo pulso e fica acordado enquanto houver pulso; o despertar para transmitir fica em outra sprint | Teste em bancada: ciclo de dormir e acordar, pulso constante sem acordar e dormir a cada pulso, com a corrente medida | Código pronto em 03/11/2026, desabilitado por padrão; bancada pendente |
| FW-059 | Contagem de pulsos em hardware (LPTIM1 na entrada D4, PB5) durante o sleep, sem acordar. **A confirmar**: com quatro leitores só um conta assim; depende do RM0394 | Teste em bancada: pulsos contados em Stop 2 sem acordar, conferidos com um gerador | A confirmar |
| FW-060 | Totalizador (hodômetro) de 32 bits por entrada, nos registradores de backup, com duas cópias e complemento e gravação a cada pulso | Teste no PC: soma, volta a zero e queda de energia em cada gravação; em bancada: reinício e queda de energia sem perder o total | Lógica concluída em 03/11/2026 (36 verificações no PC); registradores de backup em bancada pendentes |

### Fase 2 — Regras de sensor (SPRINT_02)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-013 | Detecção de falha abaixo de 4 mA e acima de 20 mA | Testes de limite no PC | Pendente |
| FW-014 | Detecção de dente de serra: inversão acima do percentual em 5 s, com padrão 2% e 10% para inversor | Testes com linear, logarítmica e dente de serra | Pendente |
| FW-015 | Tratamento de zero (sensor sem carga) e primeira leitura após zero | Sem alerta de falha no zero | Pendente |
| FW-016 | Falha de sensor: saída em 0 Hz e alerta com sensor, porta e tipo | Teste de sequência de falha e recuperação | Pendente |

### Fase 3 — Média móvel, fila e alertas (SPRINT_03)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-017 | Média móvel de um envio sobre os valores de 1 segundo | Teste no PC | Pendente |
| FW-018 | Driver da flash SPI externa de 16 MiB (GD25Q128) | Leitura e escrita verificadas | Pendente; pré-requisito da atualização (ADR-0003) |
| FW-019 | Biblioteca `lib/fila`: fila de telemetria com substituição do mais antigo | Teste de fila cheia | Pendente |
| FW-020 | Confirmação de envio: remoção só após confirmação do servidor | Teste de envio perdido e confirmado | Pendente |
| FW-021 | Espaço de alertas com 300 registros | Teste de substituição | Pendente |

### Fase 4 — Saídas e controle (SPRINT_04)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-022 | Saídas discretas: ligar e desligar, com as saídas desligadas na partida | Testes de estado | Código pronto em 03/11/2026: ligar e desligar (`saida`, `io_saida`, `io_saidas`), com as saídas desligadas na partida. Bancada pendente |
| FW-023 | Saída em frequência (a "pulsada") proporcional a 4–20 mA, com 4 mA e 20 mA configuráveis; só pode ser habilitada com o sleep desligado | Teste de proporcionalidade | Parcial em 04/11/2026: `lib/io/freq_saida` pronto (cálculo proporcional da frequência a partir de 4–20 mA, 0 Hz na falha, 9 testes no PC). Decidido em 04/11/2026: em 0 Hz a saída fica em nível baixo (transistor desligado), o duty cycle é de 50%, e só as saídas 2, 3 e 4 (PB13, PB14 e PB15, com canal de timer) têm modo frequência; a saída 1 (PB12) fica só discreta. `lib/io/gerador_freq` pronto (onda quadrada de 50% por acumulador de fase a 100 kHz de tick, uma frequência independente por saída, 0 Hz em nível baixo, 10 testes no PC; a 20 kHz o duty fica em 40% ou 60% por ter só cinco ticks por período). `io_frequencia` liga o gerador ao TIM6 (interrupção a 100 kHz, só roda com alguma saída acima de 0 Hz, prioridade 6) e aos pinos PB13 a PB15, com `io_frequencia_define_tipo` e `io_frequencia_define_hz`; compila nos dois builds, imagem do `ologgermulti` com 26,8 KB. Sem teste em hardware (frequência, carga da CPU e forma de onda a medir na bancada). `io_frequencia_cadastra` e `io_frequencia_atualiza` replicam a leitura do sensor (0 Hz na falha); o sinal será testado na bancada. Falta ligar à leitura real da entrada analógica associada e ao cadastro por Modbus e MQTT |
| FW-024 | Retardo de acionamento (relé de tempo, o "acionamento por tempo") por saída discreta, com cancelamento por comando de desligar | Testes de temporização | Código pronto em 03/11/2026 (`saida_define_atraso`, `io_saidas_define_atraso`): 36 verificações no PC com o retardo, o comando descartado, o cancelamento por desligar e a volta do contador. Regras confirmadas em 03/11/2026. Bancada pendente |
| FW-025 | Modo manual e automático por saída, com prioridade do sensor | Tabela de estados testada | Código pronto em 04/11/2026 (`controle_saida`, `io_saidas`): 82 verificações no PC com manual, automático, regra do sensor, agenda local offline, comando do CCO online, falha do sensor com espera de 5 minutos e retardo. Falta o alerta ao CCO na chegada do comando manual. Bancada pendente |
| FW-061 | Ligar e desligar o modo sleep (modo de operação, registro 40412), com exclusão mútua: o sleep só liga se nenhuma saída estiver em frequência, e a saída em frequência só habilita com o sleep desligado | Testes: ligar o sleep com uma saída em frequência é recusado, e habilitar a frequência com o sleep ligado também | Lado do sleep pronto em 03/11/2026 (`energia_define_habilitada` devolve erro se um módulo impede). Lado das saídas pronto em 04/11/2026: `lib/io/tipo_saida` guarda o tipo de cada saída, recusa frequência com o sleep ligado e na saída 1, e `io_frequencia_sleep_permitido` é o gancho que impede ligar o sleep com saída em frequência (8 testes no PC). Falta o comando do Modbus (40166 e 40412) |
| FW-063 | Regra do sensor entrada analógica para saída digital (comando automático local): cadastro por MQTT ou Modbus (entrada associada, limiares máximo e mínimo e sentido, mais a frequência em 4 mA e 20 mA, todos com endereço Modbus) e o avaliador com histerese que gera os pedidos da fonte do sensor. Sentido 0: liga no mínimo e desliga no máximo; sentido 1: liga no máximo e desliga no mínimo. Sem regra cadastrada valem a agenda (offline) e o CCO (online) | Testes no PC: sentido 0 e sentido 1 com histerese (exemplo: sentido 1 liga em 5 bar e desliga em 4 bar), valor entre os limiares mantém o pedido anterior, e sem regra cadastrada valem a agenda offline e o CCO online; cadastro lido e gravado pelos endereços do mapa Modbus | Parcial em 04/11/2026: `lib/io/regra_sensor` pronto (cadastro validado, sentidos 0 e 1 e histerese, 9 testes no PC); falta ler e gravar o cadastro pelos endereços Modbus e por MQTT e ligar a regra à leitura da entrada analógica |

### Fase 5 — Modbus RTU (SPRINT_05)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-026 | Biblioteca `lib/modbus`: quadro RTU e CRC | Testes com quadros de referência | Pendente |
| FW-027 | Mestre com timeout de 30 s e dez tentativas a cada 30 s | Simulador com ausência de resposta | Pendente |
| FW-028 | Escravo com mapa de `documentacao/mapa_modbus.md` | Leitura e escrita dos registradores | Pendente |
| FW-029 | Recuperação com cinco leituras válidas e defeito após uma hora | Teste de recuperação | Pendente |
| FW-047 | `lib/tabela_variaveis`: 64 variáveis na RAM, com estado, marca de tempo e mutex estático | Teste de leitura e escrita concorrente no PC, estados e limite de 64 | Pendente |
| FW-048 | `lib/mapeamento`: configuração de até 8 equipamentos, validação completa e gravação atômica em flash com CRC | Teste de configuração válida, inválida e de queda na gravação | Pendente |
| FW-049 | `lib/modbus_mestre` em task FreeRTOS, não bloqueante: um equipamento sem resposta não atrasa os demais | Simulador com um equipamento mudo e os outros respondendo no prazo | Pendente |
| FW-050 | `lib/modbus_escravo`: expõe a tabela no mapa fixo com registrador de status por variável | Leitura de valor e de estado pelo simulador Modbus | Pendente |

### Fase 6 — Relógio e agenda offline (SPRINT_06)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-030 | Driver do RTC e conferência NTP quatro vezes ao dia; fuso horário da instalação configurado pelo CCO (40403, proposta) | Teste de atualização | Parcial em 04/11/2026: `lib/tempo` pronto (conversão do tempo UTC em data, hora e dia da semana locais com o fuso da instalação, 12 testes no PC, anos bissextos e virada de dia e de ano). `lib/rtc` escrito (OloggerMulti, feature `RELOGIO`): liga o LSE e o RTC sem reiniciar o domínio de backup, mantém a hora em UTC e `tempo_de_utc` faz o caminho inverso (testado no PC); compila sem avisos e **não foi testado em hardware** (bancada: subir o cristal, acertar, ler e conferir a hora depois de Stop 2 e de reinício). Falta a conferência NTP (FW-035 e ESP32), o recebimento do fuso (40403, proposta) e a hora inválida do FW-031 |
| FW-031 | Hora inválida: medições sem validade, saídas mantêm estado, aviso guardado | Teste de perda e correção de hora | Pendente |
| FW-032 | Agenda semanal de 336 intervalos por saída, recebida do CCO por MQTT: formato (84 caracteres hexadecimais por saída), validação, armazenamento persistente com CRC só da última agenda, e aplicação pelo horário atual: a cada 30 minutos somente offline, na hora em que o device fica offline e depois de reinício | Teste de transição de intervalo; cadastro recusado por inteiro quando inválido, sem alterar a agenda anterior | Parcial em 04/11/2026: formato e validação prontos (`lib/agenda`, 57 verificações no PC). A aplicação pelo horário atual está pronta em `lib/agenda/agenda_aplicador` (13 testes, 42 verificações no PC): pede o estado do intervalo atual na primeira avaliação depois da partida, a cada virada de intervalo de 30 minutos e logo depois de carregar ou apagar a agenda; com hora inválida não pede nada. `lib/agenda/agenda_tarefa` liga o RTC, o fuso e o aplicador às saídas: uma tarefa lê o RTC a cada segundo, aplica o fuso e entrega o pedido da fonte agenda por `io_saidas_pedido` (compila; **sem teste em hardware**); sem hora válida ou sem fuso do CCO nada é pedido. A agenda só funciona com o modo sleep desabilitado (decidido em 04/11/2026), então não há despertar no limite do intervalo. O fuso começa em Brasília (−180) e a agenda e o fuso gravados persistem ao reinício: `lib/armazem` (dois espaços, CRC-32, seguro contra queda de energia, 14 testes com flash simulada e corte de energia) e `agenda_config` (formato de 176 bytes, 7 testes) gravam nas duas últimas páginas da flash interna (`flash_interna`, linker com 60 KB de programa), 5 s depois da última mudança, e carregam na partida (compila; **sem teste em hardware**). Falta o recebimento por MQTT (FW-035) |

### Fase 7 — MQTT e ESP32 (SPRINT_07)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-033 | Protocolo serial entre STM32 e ESP32 | Quadros validados | Pendente |
| FW-034 | Cliente MQTT 5.0 com TLS 1.3 no ESP32 | Conexão com broker de teste | Pendente |
| FW-035 | Tópicos do device a partir do caminho recebido do sistema | Teste com simulador | Pendente |
| FW-036 | Reconexão: cinco tentativas por rádio, a cada minuto, depois offline | Teste de queda de rede | Pendente |

### Fase 8 — Sessão e serial (SPRINT_08)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-037 | Sessão de configuração única com timeout de 5 minutos | Teste de concorrência e timeout | Pendente |
| FW-038 | Recusa de comandos MQTT durante a sessão | Teste com comando recebido | Pendente |
| FW-039 | Gateway serial no notebook (aplicação Python) com comandos restritos | Teste com chaveiro serial | Pendente |

### Fase 9 — Atualização de firmware (SPRINT_08 e SPRINT_09)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-040 | Verificação de validade e compatibilidade do pacote | Teste com pacote inválido | Pendente |
| FW-041 | Recuperação da versão anterior em falha ou queda de energia | Teste de queda durante gravação | Pendente |
| FW-042 | Saídas desligadas durante atualização e retomada com tempo de partida | Teste de retomada | Pendente |
| FW-062 | Layout da flash interna do STM32 com bootloader (ADR-0003): reservar o espaço do bootloader, mover o início da aplicação no linker, e medir e registrar a folga de flash e de RAM por fase | Imagem compila e executa no novo endereço; tabela de folga por fase registrada em `documentacao/ESTADO_IMPLEMENTACAO.md` | Pendente; criado em 04/11/2026 pela decisão do ADR-0003 |

### Fase 10 — Bancada e protótipo (SPRINT_09)

| ID | Tarefa | Aceite | Situação |
|---|---|---|---|
| FW-043 | Roteiro de bancada do protótipo | Roteiro aprovado | Pendente |
| FW-044 | Validação de NB-IoT com módulo e operadora (P-026) | Relatório de recepção e latência | Pendente |
| FW-045 | Execução do roteiro de bancada e relatório | Relatório registrado | Pendente |
| FW-051 | Procedimento de gravação do OloggerMulti com os bits de opção `nSWBOOT0=0` e `nBOOT0=1`, porque o BOOT0 do U1 está sem ligação (HW-006) | Script ou roteiro executado em uma placa nova, com leitura de volta dos bits e do PC em `0x0800....` | Parcial: bits de opção gravados na unidade de bancada em 02/11/2026; procedimento e script pendentes |

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.2.23 | FW-032: agenda só com o sleep desabilitado; persistência da agenda e do fuso; fuso padrão de Brasília |
| 04/11/2026 | 0.2.22 | FW-032: tarefa que aplica a agenda pelo RTC escrita, sem teste em hardware |
| 04/11/2026 | 0.2.21 | FW-030: driver do RTC escrito, sem teste em hardware |
| 04/11/2026 | 0.2.20 | FW-030: cristais do OloggerMulti e bateria opcional |
| 04/11/2026 | 0.2.19 | FW-032: aplicador das agendas pelo horário atual pronto e testado |
| 04/11/2026 | 0.2.18 | FW-030 parcial: módulo `tempo` pronto e testado |
| 04/11/2026 | 0.2.17 | FW-023: o sensor manda na saída em frequência |
| 04/11/2026 | 0.2.16 | FW-023: gerador ligado ao TIM6 e aos pinos (compila, sem teste em hardware) |
| 04/11/2026 | 0.2.15 | FW-023: gerador de frequência por acumulador de fase pronto e testado |
| 04/11/2026 | 0.2.14 | FW-061 com o lado das saídas pronto; FW-023 com as decisões de 0 Hz, duty cycle e canais |
| 04/11/2026 | 0.2.13 | FW-023 parcial: módulo `freq_saida` pronto e testado |
| 04/11/2026 | 0.2.12 | FW-063 parcial: módulo `regra_sensor` pronto e testado |
| 04/11/2026 | 0.2.11 | FW-030 com o fuso configurado pelo CCO; FW-032 com a aplicação pelo horário atual depois de reinício |
| 04/11/2026 | 0.2.10 | FW-063 com o sentido confirmado; FW-032 com o formato da agenda por MQTT (parcial) |
| 04/11/2026 | 0.2.9 | FW-063 com o cadastro por MQTT ou Modbus; FW-025 com a agenda local e o CCO online |
| 04/11/2026 | 0.2.8 | FW-025 com a regra local, o comando remoto do CCO e a espera de 5 minutos; novo FW-063 (regra local) |
| 04/11/2026 | 0.2.7 | FW-025 em código pronto |
| 04/11/2026 | 0.2.6 | Novo FW-062 (layout da flash interna com bootloader), pela decisão do ADR-0003 |
| 04/11/2026 | 0.2.5 | Correções da verificação do doc-keeper antes do fechamento da SPRINT_00 (números, nomes dos registradores, concordância e referências) |
| 03/11/2026 | 0.2.4 | O "acionamento por tempo" é o retardo de acionamento; ligar com a saída ligada ou aguardando é descartado; mapa Modbus das saídas ajustado (P-037) |
| 03/11/2026 | 0.2.3 | Decisões de 03/11/2026: "pulsada" é a saída em frequência (FW-023), o retardo de acionamento vale para a saída discreta (FW-024) e o sleep liga e desliga com exclusão mútua com a frequência (novo FW-061) |
| 03/11/2026 | 0.2.2 | FW-022: código pronto (saídas discretas e acionamento por tempo); a SPRINT_04 começou |
| 03/11/2026 | 0.2.1 | FW-007 e a parte ESP32 do FW-046 movidos para a SPRINT_07, para depois de finalizados os testes do STM32 e de ele estar validado |
| 03/11/2026 | 0.2.0 | Coluna de situação em todos os itens; textos de FW-052 e FW-057 corrigidos; seção "Prazo e risco" com a reavaliação de 03/11/2026 |
| 03/11/2026 | 0.1.11 | Correção: o pulso acorda o STM32 e ele fica acordado, dormindo só após o tempo de inatividade; FW-055, FW-056 e FW-058 reescritos |
| 03/11/2026 | 0.1.10 | FW-058 (Stop 2) e FW-060 (totalizador em hodômetro) detalhados e implementados; FW-059 rebaixado |
| 03/11/2026 | 0.1.9 | FW-055 redefinido: o tratador da interrupção externa conta o pulso e o STM32 volta a dormir |
| 03/11/2026 | 0.1.8 | Fase 1B: FW-059 (contagem em hardware no LPTIM1) e FW-060 (contagem persistente) propostos, a confirmar |
| 03/11/2026 | 0.1.7 | Fase 1B: uso da entrada digital como contador de pulsos que dorme após 60 s sem pulso; FW-055 e FW-056 reescritos e FW-057 e FW-058 adicionados |
| 03/11/2026 | 0.1.6 | Fase 1B adicionada (SPRINT_01B) com FW-052 a FW-056 para a entrada digital: nível alto ativo, filtro de ruído, modo de frequência, despertar do sleep e bancada |
| 02/11/2026 | 0.1.5 | FW-051 adicionado: procedimento de gravação com bits de opção, por causa do BOOT0 sem ligação no OloggerMulti |
| 02/11/2026 | 0.1.4 | FreeRTOS em todo o sistema: FW-046 na base (SPRINT_00); FW-047 a FW-050 para o mapeamento Modbus (SPRINT_05), conforme `documentacao/modbus_mapeamento_dados.md` aprovado |
| 02/11/2026 | 0.1.3 | Composição por build: FW-002 seleciona o build por `-DDEVICE` e lê `builds/composicao.cmake`; FW-003 revisa a composição e cria a pinagem por placa; setup em runtime removido |
| 02/11/2026 | 0.1.2 | FW-002 e FW-003 reescritos para imagem única por MCU e setup de composição, no lugar de seleção de perfil de compilação; regra de pronto e documentação ampliadas (estado, changelog, dicionário) |
| 02/11/2026 | 0.1.1 | FW-001 concluído: pastas `src/`, `include/`, `lib/`, `devices/`, `tests/`, `tools/` com README |
| 01/11/2026 | 0.1.0 | Criado backlog com 45 tarefas em 11 fases, vinculadas às sprints |
