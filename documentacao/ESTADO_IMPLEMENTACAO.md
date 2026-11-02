# Estado da implementação — oLoggerIOT

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.1.0

Documento vivo. Diz o que existe no código, o que está em teste e o que falta. Agentes e desenvolvedores leem aqui antes de começar um item. Atualizado pela skill `doc-sync` a cada item fechado e revisado pela skill `sprint-close`.

## Situação atual

- **Sprint:** SPRINT_00 (base do projeto).
- **Código de aplicação:** nenhum módulo implementado ainda.
- **Pronto:** estrutura de pastas com README (FW-001).
- **Próximo:** CMake raiz com seleção de build por device (FW-002).

## Módulos

| Módulo | Local | Estado | Item | Testes |
|---|---|---|---|---|
| Estrutura de pastas | raiz | Implementado | FW-001 | Verificação de existência |
| Composição dos builds | `builds/composicao.cmake` | Confirmada em 02/11/2026; validada no CMake | FW-003 | Sintaxe e erros do CMake nos dois builds |
| Build STM32 | `CMakeLists.txt` | Não iniciado | FW-002, FW-006 | — |
| Build ESP32 | `esp32/` | Não iniciado | FW-007 | — |
| Doxygen | `Doxyfile` | Não iniciado | FW-004 | — |
| Verificação de padrão | `tools/verifica_padrao.py` | Não iniciado | FW-008 | — |
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
