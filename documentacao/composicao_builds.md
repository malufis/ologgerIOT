# Composição dos builds — oLoggerIOT

> **Criado em:** 02/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.2.12
> **Status:** composição confirmada pelo responsável técnico em 02/11/2026. A lista vigente está em `builds/composicao.cmake`.

## Princípio

Há **um único projeto de firmware**, com todas as funcionalidades, em `src/` e `lib/`. Há **um build por tipo de device**: `ologgermulti` e `y7080`. Cada build compila só as funcionalidades que o device tem.

A escolha do que entra em cada build fica em um único arquivo: `builds/composicao.cmake`. Esse arquivo não tem lógica de funcionamento. Ele só lista funcionalidades por build.

## Builds

| Build | MCU principal | Módulo de comunicação | Observação |
|---|---|---|---|
| `ologgermulti` | STM32L412C8U6 (UFQFPN48) | ESP32-C6-MINI-1 (ESP-IDF) | Imagem STM32 e imagem ESP32 |
| `y7080` | STM32L071CBU6 (UFQFPN48) | Não há ESP32 | Imagem STM32 |

Códigos de compra confirmados em 02/11/2026 (distribuidor, referências `511-STM32L412C8U6` e `511-STM32L071CBU6`, com as designações `Ologger-U1-MCU` e `y7080-U2-MCU`). Quantidade comprada: 7 unidades de cada MCU. Confirmam o chip de cada build: L412 com 64 KB de flash e L071CB com 128 KB.

Configuração (FW-002, validada em 02/11/2026):

```
cmake -S . -B build/ologgermulti -G Ninja -DDEVICE=ologgermulti -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake
cmake -S . -B build/y7080        -G Ninja -DDEVICE=y7080        -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake
```

A raiz do CMake lê `builds/composicao.cmake`, aplica as opções de CPU de `cmake/mcu.cmake` e define `OLOG_FEAT_<NOME>` para cada funcionalidade do build. Sem `DEVICE`, ou com device fora da lista, a configuração para com erro.

## Como o código usa a composição

- O CMake lê `builds/composicao.cmake` e liga só os arquivos de `lib/` das funcionalidades do build.
- Para cada funcionalidade, o CMake define `OLOG_FEAT_<NOME>`.
- O código C usa `#if OLOG_FEAT_<NOME>` para o que depende de funcionalidade. Esse é o único `#if` de escolha de comportamento.
- Pinagem da placa fica em `devices/<build>/`, incluída pelo CMake. O código não sabe o nome da placa.
- Nenhum `#if` escolhe código pelo nome do device (ver regras 10 a 12 em `documentacao/padrao_codigo_c.md`).

## Catálogo de funcionalidades

| Nome | Significado | Módulos |
|---|---|---|
| `ENTRADA_4_20MA` | Entrada analógica 4–20 mA | `lib/adc`, `lib/sensor` |
| `ENTRADA_DIFERENCIAL` | Entrada analógica diferencial | `lib/adc` |
| `ENTRADA_DIGITAL` | Entrada digital | `lib/io` |
| `SAIDA_DISCRETA` | Saída liga/desliga, com retardo de acionamento (relé de tempo) | `lib/io` |
| `SAIDA_FREQUENCIA` | Saída em Hz proporcional a 4–20 mA (a "pulsada") | `lib/io` |
| `RS485_MODBUS` | Modbus RTU mestre e escravo | `lib/modbus` |
| `FLASH_EXTERNA` | Flash SPI de 16 MiB | `lib/flash`, `lib/fila` |
| `RELOGIO` | RTC com o cristal de 32,768 kHz | `lib/rtc`, `lib/tempo` |
| `RADIO_WIFI` | Wi-Fi pelo ESP32-C6 | `esp32/` |
| `RADIO_CELULAR` | Modem celular | `lib/modem` |
| `RADIO_UART_EXTERNO` | Rádio externo ligado a UART, em modo transparente | `lib/radio_externo` |
| `MQTT` | Cliente MQTT com o broker | `lib/mqtt`, `esp32/` |

Funcionalidade nova entra primeiro no catálogo, quando o módulo correspondente existir. O CMake recusa nome fora do catálogo.

## Composição confirmada

| Funcionalidade | OloggerMulti | y7080 | Origem |
|---|---|---|---|
| `ENTRADA_4_20MA` | Sim (4 entradas) | Sim (1 entrada) | Análise de hardware |
| `ENTRADA_DIFERENCIAL` | Não | Sim | Confirmado em 02/11/2026 |
| `ENTRADA_DIGITAL` | Sim (4) | Não | Confirmado em 02/11/2026 |
| `SAIDA_DISCRETA` | Sim (4) | Não | Confirmado em 02/11/2026 |
| `SAIDA_FREQUENCIA` | Sim (nas saídas digitais) | Não | Confirmado em 02/11/2026 |
| `RS485_MODBUS` | Sim | Sim | Análise de hardware |
| `FLASH_EXTERNA` | Sim (16 MiB) | Sim (16 MiB) | Análise de hardware |
| `RELOGIO` | Sim (cristal de 32,768 kHz) | Não (sem cristal) | Informado em 04/11/2026 |
| `RADIO_WIFI` | Sim (ESP32-C6, único rádio) | Não | Confirmado em 02/11/2026 |
| `RADIO_CELULAR` | Não | Sim (SIMY7080, footprint SIM7080G) | Confirmado em 02/11/2026 |
| `RADIO_UART_EXTERNO` | Sim (UART de comunicação externa) | Não | Confirmado em 02/11/2026 |
| `MQTT` | Sim | Sim | Confirmado em 02/11/2026 |

## Módulos que não são funcionalidade do catálogo

- `lib/energia` (gerência de energia em Stop 2) existe só para o STM32L4 e entra no build `ologgermulti` pelo CMake, conforme o MCU. O STM32L0 do y7080 usa outra API de baixo consumo, ainda não implementada. Não é uma funcionalidade nova do catálogo.
- `lib/io` tem a leitura das entradas digitais, o filtro, o contador de pulsos, o totalizador (nos registradores de backup do STM32L4) e a tarefa. Entra com a funcionalidade `ENTRADA_DIGITAL`.
- Os modos da entrada digital (discreto, contador de pulsos e frequência) são configuração de cada entrada, não funcionalidades separadas.

## Pinagem

A pinagem de cada placa está em `devices/<build>/pinos.h` (FW-003), com um par `PINO_<FUNCAO>_PORTA` e `PINO_<FUNCAO>_NUM` por função. O CMake inclui a pasta do build escolhido, e o código usa só esses nomes.

- Conferida contra as netlists dos esquemas em 02/11/2026: 31 funções no OloggerMulti e 26 no y7080, sem pino duplicado e sem pino sem ligação.
- `python tools/verifica_pinos.py` repete a checagem de pino duplicado e de par incompleto.
- Pontos a validar na bancada estão marcados com "validar" nos cabeçalhos: PWRKEY do modem, jumper da flash, linha de interrupção do y7080 e saída MCLK do OloggerMulti.
- No OloggerMulti, o canal 4–20 mA 1 está no PA1 e o canal 2 no PA0, o inverso da ordem dos pinos. A serial externa (USART1, PA9/PA10) recebe o rádio externo.

## Pendências

- Confirmar em bancada o mapeamento de pinos das duas placas. Itens críticos: nível e temporização do `NBIOT-PWRKEY` no y7080, e a UART de comunicação externa do OloggerMulti (PA9/PA10).
- Validar no manual AT do SIM7080G os comandos do cliente MQTT, que roda no modem (decidido em 02/11/2026).
- Rádio externo do OloggerMulti na UART1 do STM32 (PA9/PA10), decidido em 02/11/2026: o STM32 é o cérebro e recebe todos os dados externos. O conector ligado à UART0 do ESP32 fica fora do uso do firmware até confirmação do seu uso.
- Integrador por tipo de rádio externo: fase futura. `lib/radio_externo` só repassa bytes, sem interpretar (decidido em 02/11/2026).
- Medir flash e RAM de cada build (FW-006). Em 03/11/2026: `ologgermulti` com 30 252 bytes de código (04/11/2026, com o RTC e a agenda) e `y7080` com 4 944.
- Gerência de energia do y7080 (STM32L0): o módulo `lib/energia` só existe para o L4.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.2.12 | Tamanho do build do OloggerMulti atualizado para 30 252 bytes de código |
| 04/11/2026 | 0.2.11 | Nova funcionalidade RELOGIO (RTC), só no OloggerMulti; tamanho do build atualizado para 27 116 bytes |
| 04/11/2026 | 0.2.10 | Tamanho do build do OloggerMulti atualizado para 26 280 bytes de código |
| 04/11/2026 | 0.2.9 | Tamanho do build do OloggerMulti atualizado para 26 280 bytes de código |
| 04/11/2026 | 0.2.8 | Tamanho do build do OloggerMulti atualizado para 26 280 bytes de código |
| 03/11/2026 | 0.2.7 | O "acionamento por tempo" é o retardo de acionamento; ligar com a saída ligada ou aguardando é descartado; mapa Modbus das saídas ajustado (P-037) |
| 03/11/2026 | 0.2.6 | Catálogo: saída discreta sem "pulsada" (que é a saída em frequência), com acionamento por tempo e retardo |
| 03/11/2026 | 0.2.5 | Tamanho do build do OloggerMulti atualizado para 26 280 bytes de código |
| 03/11/2026 | 0.2.4 | Tamanho do build do OloggerMulti atualizado para 26 280 bytes de código, com as saídas digitais |
| 03/11/2026 | 0.2.3 | Registrados os módulos que não são funcionalidade do catálogo (`lib/energia`, `lib/io`), o tamanho dos builds em 03/11/2026 e a pendência de energia do y7080 |
| 02/11/2026 | 0.2.2 | Mapa preliminar de pinos substituído pela pinagem em `devices/<build>/pinos.h`, conferida contra as netlists |
| 02/11/2026 | 0.2.1 | Códigos de MCU confirmados pela compra: STM32L412C8U6 (OloggerMulti) e STM32L071CBU6 (y7080), ambos em UFQFPN48 |
| 02/11/2026 | 0.2.0 | Composição confirmada: saída em frequência no OloggerMulti, UART de rádio externo (`RADIO_UART_EXTERNO`), y7080 com entrada diferencial, celular e MQTT; sem digitais no y7080; mapa de pinos preliminar incluído |
| 02/11/2026 | 0.1.0 | Criado com o princípio de um projeto e um build por device, o arquivo `builds/composicao.cmake`, o catálogo de funcionalidades e a composição proposta |
