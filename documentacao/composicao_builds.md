# Composição dos builds — oLoggerIOT

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.2.0
> **Status:** composição confirmada pelo responsável técnico em 02/11/2026. A lista vigente está em `builds/composicao.cmake`.

## Princípio

Há **um único projeto de firmware**, com todas as funcionalidades, em `src/` e `lib/`. Há **um build por tipo de device**: `ologgermulti` e `y7080`. Cada build compila só as funcionalidades que o device tem.

A escolha do que entra em cada build fica em um único arquivo: `builds/composicao.cmake`. Esse arquivo não tem lógica de funcionamento. Ele só lista funcionalidades por build.

## Builds

| Build | MCU principal | Módulo de comunicação | Observação |
|---|---|---|---|
| `ologgermulti` | STM32L412C8 | ESP32-C6-MINI-1 (ESP-IDF) | Imagem STM32 e imagem ESP32 |
| `y7080` | STM32L071CB | Não há ESP32 | Imagem STM32 |

Comando (quando o FW-002 existir): `cmake -DDEVICE=ologgermulti` ou `cmake -DDEVICE=y7080`.

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
| `SAIDA_DISCRETA` | Saída liga/desliga e pulsada | `lib/io` |
| `SAIDA_FREQUENCIA` | Saída em Hz proporcional a 4–20 mA | `lib/io` |
| `RS485_MODBUS` | Modbus RTU mestre e escravo | `lib/modbus` |
| `FLASH_EXTERNA` | Flash SPI de 16 MiB | `lib/flash`, `lib/fila` |
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
| `RADIO_WIFI` | Sim (ESP32-C6, único rádio) | Não | Confirmado em 02/11/2026 |
| `RADIO_CELULAR` | Não | Sim (SIMY7080, footprint SIM7080G) | Confirmado em 02/11/2026 |
| `RADIO_UART_EXTERNO` | Sim (UART de comunicação externa) | Não | Confirmado em 02/11/2026 |
| `MQTT` | Sim | Sim | Confirmado em 02/11/2026 |

## Mapeamento de pinos (fonte para `devices/`)

A análise das netlists já tem o mapa preliminar de pinos das duas placas. Ele é a base da pinagem de FW-003, e os itens marcados abaixo precisam de confirmação na bancada.

- **OloggerMulti:** entradas 4–20 mA em PA1, PA0, PB0 e PB1; entradas digitais em PB7, PB8, PB6 e PB5; saídas em PB12 a PB15; RS-485 em PA2/PA3 com DE/RE em PB9; UART para o ESP32 em PB10/PB11; **UART de comunicação externa em PA9/PA10** (rótulo `UART1`, "serial externa"); flash SPI em PA4 a PA7.
- **y7080:** entrada 4–20 mA em PA0; medição diferencial com `SENSE` em PB1 e controles PB13, PB15 e PA15; modem celular em PB10/PB11 (LPUART1) e `NBIOT-PWRKEY` em PC13; RS-485 em PA2/PA3 com DE/RE em PB9; flash SPI em PA4 a PA7.

## Pendências

- Confirmar em bancada o mapeamento de pinos das duas placas. Itens críticos: nível e temporização do `NBIOT-PWRKEY` no y7080, e a UART de comunicação externa do OloggerMulti (PA9/PA10).
- Validar no manual AT do SIM7080G os comandos do cliente MQTT, que roda no modem (decidido em 02/11/2026).
- Rádio externo do OloggerMulti na UART1 do STM32 (PA9/PA10), decidido em 02/11/2026: o STM32 é o cérebro e recebe todos os dados externos. O conector ligado à UART0 do ESP32 fica fora do uso do firmware até confirmação do seu uso.
- Integrador por tipo de rádio externo: fase futura. `lib/radio_externo` só repassa bytes, sem interpretar (decidido em 02/11/2026).
- Medir flash e RAM de cada build (FW-006).

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 02/11/2026 | 0.1.0 | Criado com o princípio de um projeto e um build por device, o arquivo `builds/composicao.cmake`, o catálogo de funcionalidades e a composição proposta |
| 02/11/2026 | 0.2.0 | Composição confirmada: saída em frequência no OloggerMulti, UART de rádio externo (`RADIO_UART_EXTERNO`), y7080 com entrada diferencial, celular e MQTT; sem digitais no y7080; mapa de pinos preliminar incluído |
