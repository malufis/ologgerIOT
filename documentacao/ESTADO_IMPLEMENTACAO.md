# Estado da implementação — oLoggerIOT

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.1.0

Documento vivo. Diz o que existe no código, o que está em teste e o que falta. Agentes e desenvolvedores leem aqui antes de começar um item. Atualizado pela skill `doc-sync` a cada item fechado e revisado pela skill `sprint-close`.

## Situação atual

- **Sprint:** SPRINT_00 (base do projeto).
- **Código de aplicação:** nenhum módulo implementado ainda.
- **Pronto:** estrutura de pastas com README (FW-001); CMake raiz com seleção de build por device (FW-002); imagens vazias dos dois builds STM32 compilando (FW-006).
- **Ferramentas prontas:** Doxygen com dicionário gerado (FW-004), modelo de arquivo (FW-005) e verificador de padrão (FW-008).
- **Próximo:** composição e pinagem (FW-003), build do ESP32 (FW-007) e base FreeRTOS (FW-046).

## Módulos

| Módulo | Local | Estado | Item | Testes |
|---|---|---|---|---|
| Estrutura de pastas | raiz | Implementado | FW-001 | Verificação de existência |
| Composição dos builds | `builds/composicao.cmake` | Confirmada em 02/11/2026; validada no CMake | FW-003 | Sintaxe e erros do CMake nos dois builds |
| Configuração CMake (raiz, toolchain, CPU por MCU) | `CMakeLists.txt`, `cmake/` | Configuração concluída; alvos pendentes | FW-002 | Configura os dois builds e recusa device inválido |
| Imagem STM32 do OloggerMulti (L412) | `src/`, `cmake/hal-stm32l4.cmake`, `include/stm32l4xx_hal_conf.h`, `devices/ologgermulti/` | Compila e linka sem avisos (1432 bytes de código, 12 de dados, 1564 de BSS). Executa no STM32L412 real | FW-006 | Build vazio com `-Werror`; em hardware, PC em `0x0800....` e SysTick ativo e contando (02/11/2026) |
| Imagem STM32 do y7080 (L071) | `cmake/hal-stm32l0.cmake`, `include/stm32l0xx_hal_conf.h`, `devices/y7080/` | Compila e linka sem avisos (1348 bytes de código, 20 de dados, 5660 de BSS, incluindo pilha e heap reservados) | FW-006 | Build vazio com `-Werror` |
| Build ESP32 | `esp32/` | Não iniciado | FW-007 | — |
| Doxygen e dicionário de funções | `Doxyfile`, `tools/gera_dicionario.py` | Implementado | FW-004 | 8 testes do gerador; Doxygen barra função e parâmetro sem documentação |
| Modelo de arquivo C | `tools/modelos/` | Implementado | FW-005 | Compila sem avisos para Cortex-M4 e M0+ |
| Verificação de padrão | `tools/verifica_padrao.py` | Implementado | FW-008 | 12 testes: tamanho de função e arquivo, cabeçalho e alocação dinâmica |
| ADC e aquisição | `lib/adc/` | Não iniciado | FW-009 | — |
| Sensor (média por segundo) | `lib/sensor/` | Não iniciado | FW-010 | — |
| Conversão 4–20 mA | `lib/sensor/` | Não iniciado | FW-011 | — |
| Fila de telemetria | `lib/fila/` | Não iniciado | FW-019 | — |
| Modbus RTU | `lib/modbus/` | Não iniciado | FW-026 a FW-029 | — |
| Agenda offline | `lib/agenda/` | Não iniciado | FW-032 | — |
| Sessão de configuração | `lib/sessao/` | Não iniciado | FW-037 | — |
| MQTT no ESP32 | `esp32/components/` | Não iniciado | FW-034 | — |

Legenda de estado: **Não iniciado**, **Em desenvolvimento**, **Em teste**, **Implementado** (código e testes passam).

## Decisões em aberto que afetam a implementação

- **Execução em hardware do OloggerMulti não verificada (02/11/2026):** a imagem vazia foi gravada e verificada no STM32L412 por SWD, mas o chip inicia pela memória de sistema (PC em `0x1FFF....`). Causa confirmada (achado HW-006 em `analise_hardware_ologgeriot.md`): o pino 44 (PH3/BOOT0) do U1 está sem ligação no esquema do OloggerMulti e flutua em nível alto. O y7080 tem o BOOT0 ligado ao R16 de 100 kΩ. **Contornado sem intervenção física em 02/11/2026:** gravados `nSWBOOT0=0` e `nBOOT0=1` nos bits de opção da unidade de bancada, e o firmware passou a executar. Confirmado também depois de desligar e religar a placa: bits mantidos, PC em `0x0800....` e SysTick contando, lidos sem resetar o chip. O ajuste precisa entrar no procedimento de gravação de toda unidade (FW-051). A correção definitiva é um pull-down no pino 44 na próxima revisão da placa.
- Ferramenta de verificação estática MISRA e o ADR de escolha, previstos no padrão de código (FW-008 entregou só o verificador do projeto).
- Pinagem de cada placa em `devices/` (FW-003), a partir do mapa preliminar de `documentacao/composicao_builds.md`. Confirmar em bancada.
- Protocolo do rádio externo na UART do OloggerMulti (`RADIO_UART_EXTERNO`).
- Armazenamento de credencial no ESP32: NVS criptografada ou criptografia de flash (P-004 e P-006).
- Secure boot: pendente, a registrar em ADR antes da fase 7.
- Forma de referenciar `lib/` a partir do projeto ESP-IDF (FW-007).
- Validação de TLS 1.3 na versão do mbedTLS do ESP-IDF (fase 7).
- Validação NB-IoT em bancada (P-026, FW-044).

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 02/11/2026 | 0.1.0 | Criado com o estado após FW-001 e os módulos previstos no backlog |
