# Changelog — oLoggerIOT firmware

> **Criado em:** 02/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.2.20

Formato baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/). Versões seguem a numeração do projeto (ver `sistema-init/STATUS_PROJETO.md`). Cada entrada cita o item do backlog (`FW-###`) quando houver.

## Não lançado

Trabalho das sprints 01 (aquisição analógica), 01B (entrada digital) e 04 (saídas), ainda em andamento, e documentação posterior à SPRINT_00.

### Adicionado
- FW-032 (parcial): `lib/agenda`, agenda semanal de 336 intervalos de 30 minutos por saída. `agenda_carrega_hex` valida por inteiro a cadeia de 84 caracteres hexadecimais recebida do CCO por MQTT (sem alterar a agenda anterior em caso de erro) e `agenda_estado` devolve ligada, desligada ou sem agenda para um instante. 57 verificações no PC. Falta o RTC, o armazenamento persistente, a aplicação por intervalo e o recebimento por MQTT. Formato em `documentacao/agenda_operacional_offline.md` (P-042).
- FW-030 (parcial): `lib/tempo`, conversão do tempo UTC em segundos para data, hora e dia da semana locais (0 = segunda) com o fuso da instalação em minutos (−720 a +840). 12 testes no PC, com anos bissextos (2000, 2024 e 2100), virada de dia e de ano, fuso de meia hora e limite de 32 bits. Falta o driver do RTC e a conferência NTP.
- FW-023 (parcial): `io_frequencia_cadastra` e `io_frequencia_atualiza`, que replicam na saída em frequência a leitura do sensor (frequência de 4 mA a 20 mA, 0 Hz na falha). Decidido: o sensor manda na saída em frequência e os comandos manual e automático não a influenciam.
- FW-023 (parcial): `io_frequencia` liga o gerador ao TIM6 (interrupção a 100 kHz, que só roda com alguma saída acima de 0 Hz) e aos pinos PB13 a PB15; `io_frequencia_define_tipo` cede o pino ao gerador (`io_saidas_cede_pino`) e `io_frequencia_define_hz` define a frequência; o tratador `TIM6_IRQHandler` fica em `src/interrupcoes.c`. Compila nos dois builds; **sem teste em hardware**.
- FW-023 (parcial): `lib/io/gerador_freq`, onda quadrada de 50% por acumulador de fase, uma frequência independente por saída (de 0 a 20 kHz, resolução de 0,001 Hz), 0 Hz em nível baixo, pensada para a interrupção de um timer a 100 kHz. 10 testes no PC. A 20 kHz o duty fica em 40% ou 60% (cinco ticks por período). Falta ligar ao timer e aos pinos.
- FW-061 (lado das saídas): `lib/io/tipo_saida` guarda o tipo de cada saída (0 discreto, 1 frequência) e aplica a exclusão com o sleep: a frequência é recusada com o sleep ligado e na saída 1; `io_frequencia_sleep_permitido` é o gancho `sleep_permitido` de `energia`, ligado em `main.c` no build com `SAIDA_FREQUENCIA`. 8 testes no PC.
- FW-023 (parcial): `lib/io/freq_saida`, frequência da saída "pulsada" a partir da corrente 4–20 mA. `freq_saida_cadastra` valida as frequências de 4 mA e de 20 mA (0 a 20 kHz); `freq_saida_calcula` interpola linearmente, prende a corrente nos extremos da faixa e leva a saída a 0 Hz na falha do sensor. 9 testes no PC. Decidido: em 0 Hz a saída fica em nível baixo, duty cycle de 50% e só as saídas 2 a 4 (PB13 a PB15) têm modo frequência. Falta gerar o sinal (timer).
- FW-063 (parcial): `lib/io/regra_sensor`, regra do sensor de uma entrada analógica para uma saída digital. `regra_sensor_cadastra` valida por inteiro o sentido (0 liga no mínimo, 1 liga no máximo) e os limiares; `regra_sensor_avalia` devolve o pedido da fonte do sensor com histerese (entre os limiares mantém o pedido anterior). Com a regra cadastrada, a saída ignora a agenda e o CCO. 9 testes no PC. Falta o cadastro pelos endereços Modbus e por MQTT e a ligação à entrada analógica.
- FW-025: modo manual e automático por saída em `lib/io` (`controle_saida`, 82 verificações no PC) e nas quatro saídas (`io_saidas_comando_manual`, `io_saidas_modo_automatico`, `io_saidas_pedido`, `io_saidas_sensor_falha`, `io_saidas_define_regra_sensor`, `io_saidas_define_conectividade`, `io_saidas_define_estabilidade`, `io_saidas_modo`). O comando manual prevalece; em automático vale a regra do sensor, se cadastrada, ou, sem ela, a agenda (comando local, só offline) e o CCO (só online); a falha do sensor desliga em automático e mantém em manual, e a saída só volta a operar pelo sensor depois de 5 minutos estáveis; a volta ao automático avalia na hora e, sem pedido, mantém o último estado válido. Compila no `ologgermulti` (26 280 bytes). Sem alerta ao CCO e sem teste em hardware.
- FW-062 (backlog): layout da flash interna do STM32 com bootloader. ADR-0003 decidido em 04/11/2026: a atualização usa a imagem na flash externa com um bootloader.
- `documentacao/desvios_misra.md`: registro do desvio DEV-001 (`MISRA-DEV` em `lib/adc/src/adc.c`), que estava marcado no código sem registro.
- `documentacao/adr/`: três decisões de arquitetura em estado de proposta (versão do FreeRTOS, ferramenta MISRA, atualização e proteção). Registrado o risco de espaço de flash do STM32L412C8 (64 KB, imagem atual de 25,8 KB).
- FW-024: retardo de acionamento (relé de tempo, o "acionamento por tempo") por saída discreta: `saida_define_atraso`, `io_saidas_define_atraso` e `io_saidas_aguardando`. Todo comando que liga a saída espera o retardo; desligar durante a espera cancela a partida; ligar com a saída ligada ou com o retardo em andamento é descartado. 36 verificações no PC.
- FW-061: o sleep passa a ligar e desligar por `energia_define_habilitada`, que devolve `ENERGIA_ERRO_BLOQUEADO` se um módulo impede (gancho `sleep_permitido`). Serve à exclusão mútua com a saída em frequência (FW-023). `energia_ganchos_t` ganhou o campo `sleep_permitido`.
- `documentacao/melhorias_hardware.md`: lista das mudanças de hardware para a próxima revisão das placas (pull-down das saídas, resistor e proteção das entradas, VBAT).
- FW-022: saídas digitais discretas no OloggerMulti. `saida` (estado: desligada, aguardando o retardo ou ligada, lógica pura), `io_saida` (pinos PB12 a PB15, com o nível desligado escrito antes de virar saída) e `io_saidas` (quatro saídas e tarefa de 100 ms que liga as saídas ao fim do retardo). As saídas partem desligadas. Compila no `ologgermulti` (25 792 bytes); o `y7080` não muda. Sem teste em hardware.
- HW-009: as saídas digitais não têm pull-down na base do transistor, e só PB13 a PB15 têm canal de timer.
- Decisão de 03/11/2026: o FW-007 (build do ESP32) e a parte ESP32 do FW-046 vão para a SPRINT_07 e só começam depois de finalizados os testes do STM32 e de o STM32 estar validado.
- Documentação sincronizada em 03/11/2026 (skill `doc-sync`): coluna de situação em todos os itens do backlog, arquivos das sprints 01 e 02 a 09, reescrita de `ESTADO_IMPLEMENTACAO.md` e `SPRINT_01B.md`, novas pendências P-028 a P-033 em `STATUS_PROJETO.md`, seções novas no manual de operação, na telemetria e no padrão de código, e atualização do README, do índice de agentes, de `lib/README.md` e de `tests/README.md`.
- HW-008 (03/11/2026): D1 retirada das entradas digitais da placa de bancada; limite de 12 V com o resistor de 4K7 em 0402.
- FW-058/FW-056: `adc_para`, `adc_retoma`, `aquisicao_para` e `aquisicao_retoma` param e retomam a aquisição analógica ao dormir. Roteiro de bancada da entrada digital em `documentacao/roteiro_bancada_entrada_digital.md` (12 testes, ainda não executados).
- FW-058: `lib/energia` com tarefa de gerência de energia em Stop 2 (`energia_inicia`, `energia_define_habilitada`, `energia_solicita_acordar`). Começa desabilitada. Ganchos ligados às entradas digitais em `main.c`. Sem teste em hardware.
- FW-060: totalizador (hodômetro) de 32 bits por entrada em `io_totalizador`, guardado nos registradores de backup (`io_backup`) com duas cópias e complemento e gravado a cada pulso. 36 verificações no PC, incluindo queda de energia em cada gravação. `io_tarefa_total` e `io_tarefa_define_total`.
- FW-055: a interrupção externa das entradas conta o pulso no contador e no totalizador e, pelo tratador `EXTI9_5_IRQHandler` em `src/interrupcoes.c`, pede para o STM32 acordar. O tratador fica em `src/` porque o vetor do startup é fraco e o linker não extrai objeto de biblioteca estática por referência fraca. Acordado, o STM32 só dorme de novo depois do tempo de inatividade, então um pulso constante não faz acordar e dormir a cada pulso. `io_entrada_coleta_pulsos` lê e zera o contador; `io_tarefa_retoma` soma os pulsos contados dormindo e ressincroniza os filtros.
- FW-057: `io_pulsos` em `lib/io`, contador de pulsos por entrada com tempo de inatividade de 60 s por padrão e configurável (`io_pulsos_define_timeout`, `io_tarefa_define_inatividade`) e soma dos pulsos contados dormindo (`io_pulsos_soma`). Filtro dos pulsos de 20 ms, separado do de 50 ms do estado discreto, com leitura das entradas a cada 2 ms. 25 verificações no PC. Integrado à `io_tarefa` (`io_tarefa_pulsos`, `io_tarefa_pode_dormir`, `io_tarefa_trata_despertar`).
- FW-052/FW-053: tarefa `io_tarefa` em `lib/io`, criada em `main.c` no build `ologgermulti`. Lê D1 a D4 a cada 2 ms, aplica o filtro de 50 ms no estado e o de 20 ms nos pulsos e guarda o estado bruto e o filtrado para leitura por SWD. O `y7080` não leva a entrada digital.
- FW-052: leitura das entradas digitais D1 a D4 em `lib/io` (`io_entrada`). A polaridade vem de `pinos.h`: o optoacoplador inverte, então ativo é pino em nível baixo. Compila no `ologgermulti`; leitura em bancada pendente. Achado HW-008 na análise de hardware (proteção D1 de ~6 V com campo de 12 a 24 V; D1 retirada na bancada).
- FW-053: filtro de ruído da entrada digital em `lib/io` (`io_filtro`): a mudança de estado só é aceita após N amostras seguidas do nível oposto. Conversão de tempo em amostras (`io_filtro_amostras_do_tempo`), com padrão de 50 ms arredondado para cima. Testes no PC em `tests/io`. Builds STM32 de `ologgermulti` (L412) e `y7080` (L071) compilam sem avisos com `-Werror`.
- Planejamento da entrada digital: fase 1B no backlog (FW-052 a FW-060) e SPRINT_01B. Decisões registradas em `documentacao/configuracao_do_dispositivo.md`.
- Aquisição analógica (fase 1, em bancada): driver do ADC com TIM6, ADC1 e DMA, tarefa de aquisição e módulo `sensor` (média por segundo e conversão 4-20 mA). Entrada 4 medida com 8 mA: 8,007 mA. Calibração de bancada por dois pontos, referência efetiva de 2,367 V; causa do fator ~1,37 em aberto.

### Alterado
- Agenda das saídas (P-043, 04/11/2026): o fuso horário é configurado pelo CCO; o device guarda só a última agenda enviada, sem ordem de versão; depois de reinício a agenda assume o intervalo do horário atual.
- Sentido da regra do sensor (mapa Modbus 40367–40381) confirmado: 0 = liga no mínimo e desliga no máximo; 1 = liga no máximo e desliga no mínimo.
- `controle_saida`: as fontes de pedido passaram a se chamar `CONTROLE_FONTE_SENSOR` (regra cadastrada), `CONTROLE_FONTE_AGENDA` (local, só offline) e `CONTROLE_FONTE_CCO` (remota, só online). A agenda deixou de ser tratada como comando remoto. `io_saidas_define_regra_local` virou `io_saidas_define_regra_sensor`.
- `saida_liga` passou a receber o instante atual, para aplicar o retardo de acionamento.
- `energia_define_habilitada` passou de `void` para `int32_t` e pode recusar com `ENERGIA_ERRO_BLOQUEADO`.
- Padrão de código C (`documentacao/padrao_codigo_c.md`): as cinco regras de interrupções, sleep e armazenamento foram confirmadas pelo responsável técnico em 04/11/2026.

### Removido
- `io_saidas_liga` e `io_saidas_desliga`, substituídas pelo comando manual e pelos pedidos automáticos (FW-025).
- `saida_aciona_por` e `io_saidas_aciona` (acionamento que desligava sozinho depois de um tempo), por decisão de 03/11/2026: o "acionamento por tempo" é o retardo de acionamento.

## 0.2.0 — 06/11/2026 (SPRINT_00, base do projeto)

Fechada em 04/11/2026, antes do fim da semana da sprint. A numeração 0.1.0 já existia (estrutura inicial, 01/11/2026), então esta versão é a 0.2.0. O FW-007 (build do ESP32) foi movido para a SPRINT_07.

### Adicionado
- FW-001: estrutura de pastas `src/`, `include/`, `lib/`, `devices/`, `tests/` e `tools/`, com README em cada uma.
- Padrão de desenvolvimento ESP-IDF (`documentacao/padrao_esp_idf.md`).
- Composição dos builds: `builds/composicao.cmake`, confirmada pelo responsável técnico em 02/11/2026, e `documentacao/composicao_builds.md`. Validado no CMake para os dois builds e para os casos de erro.
- Funcionalidade `RADIO_UART_EXTERNO` no catálogo: rádio externo em UART, modo transparente, no OloggerMulti.
- FW-046 (STM32): FreeRTOS com alocação estática nos dois builds (V10.3.1 no L4 e V10.2.0 no L0), com `include/FreeRTOSConfig.h`, `lib/rtos` (tabela de prioridades, memória da tarefa ociosa, gancho de pilha e tick) e `lib/vida` (tarefa de vida). SysTick compartilhado com o HAL. No L412 real: 9,9 ciclos/s medidos para 10 esperados e 101 de 128 palavras de pilha livres. Dicionário de funções com as 4 primeiras funções públicas.
- FW-003: pinagem das duas placas em `devices/<build>/pinos.h` (31 funções no OloggerMulti e 26 no y7080), conferida contra as netlists, e `tools/verifica_pinos.py`. 27 testes das ferramentas.
- FW-006 (hardware): imagem vazia do OloggerMulti gravada e executando no STM32L412 real, com o BOOT0 sem ligação contornado por bits de opção (`nSWBOOT0=0`, `nBOOT0=1`). Achado HW-006 na análise de hardware e item FW-051 no backlog.
- FW-004: `Doxyfile` em português que falha em função ou parâmetro sem documentação, e `tools/gera_dicionario.py`, que gera a tabela do dicionário de funções a partir do XML do Doxygen.
- FW-005: modelos de arquivo C em `tools/modelos/` (`modelo.h` e `modelo.c`).
- FW-008: `tools/verifica_padrao.py`, que verifica função de até 40 linhas, arquivo de até 400, cabeçalho e ausência de alocação dinâmica. 20 testes das ferramentas.
- FW-006: imagens vazias dos dois builds STM32 compilam e linkam sem avisos. OloggerMulti com HAL L4 v1.18.2 (STM32L412C8) e y7080 com HAL L0 v1.12.4 (STM32L071CB). Inclui linker scripts, configurações do HAL (`include/stm32l4xx_hal_conf.h`, `include/stm32l0xx_hal_conf.h`), cabeçalho único `include/hal_stm32.h` e `src/main.c`.
- FW-002: CMake raiz (`CMakeLists.txt`) com seleção de build por `-DDEVICE`, toolchain ARM (`cmake/toolchain-arm-none-eabi.cmake`) e opções de CPU por MCU (`cmake/mcu.cmake`). Configura os dois builds e recusa device inválido.
- Proposta de dados Modbus de equipamentos externos em `documentacao/modbus_mapeamento_dados.md`, aprovada: tabela de variáveis na RAM, mapeamento configurável em flash, limites de 8 equipamentos e 64 variáveis.
- Seção FreeRTOS no padrão C (`documentacao/padrao_codigo_c.md`, 0.4.0): sistema em FreeRTOS, com alocação estática.
- Itens FW-046 (base FreeRTOS) e FW-047 a FW-050 (mapeamento Modbus) no backlog.
- Skills `esp-idf-standards` e `sprint-close`.
- Agentes `esp-idf-dev` e `firmware-reviewer`.
- Documentos de entrada: `CLAUDE.md`, `INICIAR.md`, `README.md`, `CHANGELOG.md`, `documentacao/INDICE_AGENTES.md` e `documentacao/ESTADO_IMPLEMENTACAO.md`.

### Alterado
- Padrão de código C (`documentacao/padrao_codigo_c.md`, 0.3.0): funcionalidade por `#if OLOG_FEAT_<NOME>`, pinagem por placa e boas práticas, com pequenos códigos e bibliotecas.
- Modelo de build: um projeto com todas as funcionalidades e um build por tipo de device (`ologgermulti`, `y7080`), com a composição de cada build em `builds/composicao.cmake`. Esclarecido pelo responsável técnico em 02/11/2026.
- Skills `c-firmware-standards`, `device-profile-build`, `doc-sync` e `sprint-task`; agentes `firmware-dev` e `doc-keeper`.

### Corrigido
- Corrigido o aviso do linker (segmento RWX) no y7080: os dois builds fecham com zero avisos.

## 0.1.0 — 01/11/2026

### Adicionado
- Estrutura inicial do firmware: documentação, planejamento, skills e agentes.
- Plano de implantação em 11 fases e backlog com 45 itens (FW-001 a FW-045), em sprints de uma semana.
- Análise dos esquemáticos e Gerbers do OloggerMulti e do y7080.
- Mapa Modbus, regras de telemetria, configuração, agenda offline, comunicação e atualização.
