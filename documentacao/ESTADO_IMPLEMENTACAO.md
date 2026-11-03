# Estado da implementação — oLoggerIOT

> **Criado em:** 02/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.2.2

Documento vivo. Diz o que existe no código, o que está em teste e o que falta. Agentes e desenvolvedores leem aqui antes de começar um item. Atualizado pela skill `doc-sync` a cada item fechado e revisado pela skill `sprint-close`.

## Situação atual

- **Sprints em andamento:** SPRINT_00 (todos os itens concluídos ou movidos; pronta para o `sprint-close`) e SPRINT_01B (entrada digital e contador de pulsos). A SPRINT_01 (aquisição analógica) está bloqueada pela ferrite L2.
- **Dois builds compilam sem avisos com `-Werror`** (03/11/2026): `ologgermulti` com 24 952 bytes de código e `y7080` com 4 944. O `y7080` não leva `ENTRADA_DIGITAL`, nem a gerência de energia.
- **Testes no PC (03/11/2026):** sensor (57 verificações), filtro de ruído (135), contador de pulsos (25) e totalizador (36); verificadores das ferramentas (27 testes). Todos passam. O executável do `sensor` foi bloqueado uma vez pela política de Controle de Aplicativo do Windows depois de um relink, mas recompilado rodou e passou.
- **Hardware verificado em 02/11/2026:** a imagem do OloggerMulti executa no STM32L412 real, com FreeRTOS e a tarefa de vida. A aquisição analógica mede a entrada 4 com 8 mA injetados e mostra 8,007 mA.
- **Nada da entrada digital, do contador de pulsos, do totalizador e da gerência de energia foi testado em hardware.** Só compilam e têm testes de lógica no PC. O roteiro de bancada está em `documentacao/roteiro_bancada_entrada_digital.md`.
- **Bancada de 03/11/2026:** D1 retirada das entradas digitais, limite de 12 V na entrada, resistor de 4K7 em 0402 mantido até o novo layout (achado HW-008).
- **Bloqueio da fase 1 (HW-007):** a ferrite L2 não foi soldada na placa de bancada. Sem ela, o VDDA cai para ~2,37 V e o ADC satura em ~15,8 mA. Soldar a L2, conferir a resistência (quase 0 Ω) e o VDDA (3,25 V), voltar a referência do firmware para 3,25 V e refazer a calibração em 4, 8, 12, 16 e 20 mA.
- **Risco no FreeRTOS:** entre a criação da primeira tarefa e `vTaskStartScheduler` o SysTick fica mascarado. Chamada do HAL com *timeout* nessa janela nunca expira. Inicializar hardware antes de criar tarefas, ou dentro delas.
- **Risco no sleep:** o tick do FreeRTOS para em Stop 2, e os tempos medidos por ele (como o de inatividade) não contam o tempo dormido. A hora certa virá do RTC (fase 6).
- **Próximo:** executar o roteiro de bancada da entrada digital quando o device estiver disponível; resolver o HW-007 e fechar o FW-012. O build do ESP32 (FW-007) e a parte ESP32 do FW-046 foram para a SPRINT_07 e só começa depois de finalizados os testes do STM32 e de o STM32 estar validado (decisão de 03/11/2026). O despertar do módulo para transmitir (RTC, 1, 2 ou 5 vezes ao dia, ou por entrada analógica) é de outra sprint.

## Módulos

| Módulo | Local | Estado | Item | Testes |
|---|---|---|---|---|
| Estrutura de pastas | raiz | Implementado | FW-001 | Verificação de existência |
| Composição dos builds | `builds/composicao.cmake` | Confirmada em 02/11/2026; validada no CMake | FW-003 | Sintaxe e erros do CMake nos dois builds |
| FreeRTOS no STM32 (kernel, hooks e SysTick) | `include/FreeRTOSConfig.h`, `cmake/freertos.cmake`, `lib/rtos/`, `src/interrupcoes.c` | Implementado; executa no L412 real | FW-046 | Tick de 1 ms medido no hardware (9,9 ciclos/s para 10 esperados); sem símbolo de alocação dinâmica no binário |
| Tarefa de vida | `lib/vida/` | Implementada; executa no L412 real | FW-046 | Contador subindo no chip; 101 de 128 palavras de pilha livres |
| Pinagem por placa | `devices/ologgermulti/pinos.h`, `devices/y7080/pinos.h`, `tools/verifica_pinos.py` | Implementada | FW-003 | Conferida contra as netlists (31 e 26 funções); compila com o HAL real; 7 testes do verificador |
| Configuração CMake (raiz, toolchain, CPU por MCU) | `CMakeLists.txt`, `cmake/` | Implementada | FW-002 | Configura os dois builds e recusa device inválido |
| Imagem STM32 do OloggerMulti (L412) | `src/`, `cmake/hal-stm32l4.cmake`, `include/stm32l4xx_hal_conf.h`, `devices/ologgermulti/` | Compila e linka sem avisos (24 952 bytes de código em 03/11/2026). Executa no STM32L412 real com a versão de 02/11/2026 | FW-006 | Build com `-Werror`; em hardware, PC em `0x0800....` e SysTick contando |
| Imagem STM32 do y7080 (L071) | `cmake/hal-stm32l0.cmake`, `include/stm32l0xx_hal_conf.h`, `devices/y7080/` | Compila e linka sem avisos (4 944 bytes de código em 03/11/2026). Sem teste em hardware | FW-006 | Build com `-Werror` |
| Build ESP32 | `esp32/` | Não iniciado; depois da validação do STM32 (SPRINT_07) | FW-007 | — |
| Doxygen e dicionário de funções | `Doxyfile`, `tools/gera_dicionario.py` | Implementado; 56 funções no dicionário | FW-004 | 8 testes do gerador; Doxygen barra função e parâmetro sem documentação |
| Modelo de arquivo C | `tools/modelos/` | Implementado | FW-005 | Compila sem avisos para Cortex-M4 e M0+ |
| Verificação de padrão | `tools/verifica_padrao.py` | Implementado; 32 arquivos sem violação | FW-008 | 12 testes: tamanho de função e arquivo, cabeçalho e alocação dinâmica |
| ADC (TIM6, ADC1 e DMA) | `lib/adc/` | Implementado para o L412; medido em bancada com referência provisória de 2,367 V. Parada e retomada (`adc_para`, `adc_retoma`) só compilam | FW-009 | Entrada 4 com 8 mA: 8,007 mA. Refazer depois de soldar a L2 |
| Sensor (média por segundo) | `lib/sensor/` | Implementado | FW-010 | 57 verificações em `tests/sensor` passam (03/11/2026) |
| Conversão 4–20 mA | `lib/sensor/` | Implementado | FW-011 | Calibração de dois pontos em bancada; refazer em 5 pontos depois da L2 |
| Aquisição analógica (tarefa) | `lib/aquisicao/` | Implementada; medida em bancada. Parada e retomada ao dormir só compilam | FW-009 a FW-012 | Bancada parcial: 8 mA na entrada 4 |
| Leitura das entradas digitais | `lib/io/` (`io_entrada`) | Código pronto; compila no `ologgermulti`. Ativo é pino em nível baixo (optoacoplador) | FW-052 | Sem teste em hardware |
| Filtro de ruído | `lib/io/` (`io_filtro`) | Implementado; 50 ms no estado, 20 ms nos pulsos | FW-053 | 135 verificações no PC |
| Contador de pulsos e inatividade | `lib/io/` (`io_pulsos`) | Implementado; 60 s padrão, configurável | FW-057 | 25 verificações no PC |
| Totalizador (hodômetro) | `lib/io/` (`io_totalizador`, `io_backup`) | Lógica implementada; gravação nos registradores de backup (TAMP) só compila | FW-060 | 36 verificações no PC, com queda de energia em cada gravação; TAMP sem teste em hardware |
| Tarefa das entradas digitais | `lib/io/` (`io_tarefa`) | Código pronto; leitura a cada 2 ms, filtros, pulsos e totalizador | FW-052, FW-053, FW-057 | Sem teste em hardware |
| Contagem dormindo (interrupção externa) | `lib/io/`, `src/interrupcoes.c` | Código pronto; o pulso acorda o STM32 e ele fica acordado | FW-055 | Sem teste em hardware |
| Gerência de energia (Stop 2) | `lib/energia/` | Código pronto; **desabilitado por padrão**; só no STM32L4 | FW-058 | Sem teste em hardware |
| Modo de frequência em Hz | `lib/io/` | Não iniciado; faixa de 0 a 20 kHz, ajuste fica para depois | FW-054 | — |
| Contagem em hardware (LPTIM1, D4) | `lib/io/` | Não iniciado; a confirmar | FW-059 | — |
| Fila de telemetria | `lib/fila/` | Não iniciado | FW-019 | — |
| Modbus RTU | `lib/modbus/` | Não iniciado | FW-026 a FW-029 | — |
| Agenda offline | `lib/agenda/` | Não iniciado | FW-032 | — |
| Sessão de configuração | `lib/sessao/` | Não iniciado | FW-037 | — |
| MQTT no ESP32 | `esp32/components/` | Não iniciado | FW-034 | — |

Legenda de estado: **Não iniciado**, **Em desenvolvimento**, **Em teste**, **Implementado** (código e testes passam). "Código pronto" quer dizer que compila e os testes de lógica no PC passam, mas ainda não foi validado em hardware.

## Decisões em aberto que afetam a implementação

- **Execução em hardware do OloggerMulti (HW-006):** o pino 44 (PH3/BOOT0) do U1 está sem ligação no esquema do OloggerMulti e flutua em nível alto. Contornado em 02/11/2026 com os bits de opção `nSWBOOT0=0` e `nBOOT0=1` na unidade de bancada, sem intervenção física. O ajuste precisa entrar no procedimento de gravação de toda unidade (FW-051). A correção definitiva é um pull-down no pino 44 na próxima revisão da placa.
- **Entrada digital de 12 a 24 V (HW-008):** a proteção D1 (ESDA6V1SC5, ruptura de 6,1 V) foi retirada na bancada em 03/11/2026. O resistor de 4K7 em 0402 limita a entrada a 12 V; passará a 0603 no próximo layout, e para 24 V ainda precisa de potência maior. Sem a D1 a entrada não tem supressor de surto e ESD, nem diodo contra inversão de polaridade. Se for para campo industrial, avaliar um supressor de ~30 V no próximo layout.
- **Bateria no pino VBAT:** os registradores de backup do totalizador só sobrevivem à falta de energia se o VBAT tiver bateria. Confirmar na placa.
- **Largura mínima do pulso:** o filtro de 20 ms exige que o medidor gere pulsos mais largos que isso. Confirmar com os leitores.
- **Frequência em Hz (FW-054):** faixa de projeto de 0 a 20 kHz, mas o limite real é o optoacoplador TLP291 com o 10k; confirmar o limite do pino no datasheet e medir em bancada. Ajuste fica para depois.
- **Contagem em hardware na D4 (FW-059):** só a D4 (PB5) tem `LPTIM1_IN1`, pela base do STM32CubeMX; confirmar no RM0394.
- **Modo do despertar e debug:** em Stop 2 a conexão SWD cai. Habilitar o sleep na bancada exige o clock de depuração do Stop ou uso por pulso.
- **Mapa Modbus:** o modo da entrada digital (40151–40165) hoje é "discreto ou pulsado". Falta definir os modos (discreto, contador de pulsos e frequência) e o endereço do totalizador.
- Versão do FreeRTOS: os pacotes da ST trazem V10.3.1 (L4) e V10.2.0 (L0), então os dois builds usam kernels diferentes. Decidir se fixa uma só versão para os dois, copiada para o repositório.
- Ferramenta de verificação estática MISRA e o ADR de escolha, previstos no padrão de código (FW-008 entregou só o verificador do projeto).
- Protocolo do rádio externo na UART do OloggerMulti (`RADIO_UART_EXTERNO`).
- Armazenamento de credencial no ESP32: NVS criptografada ou criptografia de flash (P-004 e P-006).
- Secure boot: pendente, a registrar em ADR antes da fase 7.
- Forma de referenciar `lib/` a partir do projeto ESP-IDF (FW-007).
- Validação de TLS 1.3 na versão do mbedTLS do ESP-IDF (fase 7).
- Validação NB-IoT em bancada (P-026, FW-044).
- Gerência de energia do y7080 (STM32L0): o módulo `lib/energia` só existe para o L4.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.2.2 | FW-007 movido para a SPRINT_07, depois da validação do STM32; SPRINT_00 pronta para o `sprint-close` |
| 03/11/2026 | 0.2.1 | Corrigido o estado do teste do `sensor` (passa, 57 verificações) e o `y7080` sem entrada digital |
| 03/11/2026 | 0.2.0 | Reescrito com o estado real: aquisição analógica, entrada digital, contador de pulsos, totalizador e gerência de energia; separação entre teste no PC, só compila e bancada; decisões em aberto atualizadas |
| 02/11/2026 | 0.1.0 | Criado com o estado após FW-001 e os módulos previstos no backlog |
