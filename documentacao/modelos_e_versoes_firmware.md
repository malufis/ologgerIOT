# Modelos e versões do firmware — oLoggerIOT

> **Criado em:** 29/10/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.4.1

## Entendimento atual do projeto

Desenvolver o firmware para **os dois modelos: OloggerMulti e y7080**, dentro da família oLoggerIOT. Há **um único projeto de firmware**, com todas as funcionalidades. Cada modelo gera o **seu build**, que leva só as funcionalidades daquele device.

## Decisão de implementação (02/11/2026)

- **Um projeto, todas as funcionalidades.** Código em `src/` e `lib/`.
- **Um build por tipo de device:** `ologgermulti` e `y7080`.
- **Composição por build em um arquivo:** `builds/composicao.cmake` diz o que entra em cada build. Detalhes em `composicao_builds.md`.
- **Pinagem por placa:** `devices/ologgermulti/` e `devices/y7080/`.
- **Imagens por build:**
  - OloggerMulti: imagem STM32L412C8 e imagem ESP32-C6.
  - y7080: imagem STM32L071CB.

A composição de cada build está confirmada em `builds/composicao.cmake` (02/11/2026). Ver `composicao_builds.md`.

## Ideia original do usuário (29/10/2026)

Ter **uma compilação única e um arquivo de setup que indique o que o dispositivo contém**. Origem: esclarecimento explícito de Rodrigo Maluf em 29/10/2026.

Em 02/11/2026, o responsável técnico esclareceu: é **um build para cada tipo de device**, com um arquivo que indica o que entra em cada build. Essa é a forma vigente.

## Decisões técnicas ainda abertas

- Uso do conector ligado à UART0 do ESP32 no OloggerMulti, que o firmware não usa até confirmação.
- Escrita em equipamento Modbus externo e tratamento de valor inválido: ver `modbus_mapeamento_dados.md`.
- Versionamento e publicação das imagens de cada build.
- Medição de flash e RAM de cada build no FW-006.

## Correção dos registros anteriores

- **0.1.0:** seleção de modelo na geração ou gravação. Substituída pelo build por device.
- **0.2.0:** compilação única com arquivo de setup, sem decisão de formato.
- **0.3.0:** imagem única por MCU com setup em flash, lido em runtime. Substituída em 02/11/2026 pelo build por device com composição em arquivo, como o responsável técnico esclareceu.

## Estado da implementação

Há documentação, padrões e análise de hardware no repositório. Há o arquivo de composição proposto, validado no CMake. Ainda não há código de aplicação, nem `CMakeLists.txt` raiz, nem binários produzidos. Ver `ESTADO_IMPLEMENTACAO.md`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 29/10/2026 | 0.1.0 | Registrados os dois modelos como alvos do firmware e a seleção/identificação da imagem na geração e gravação |
| 29/10/2026 | 0.2.0 | Corrigida interpretação anterior: usuário propõe compilação única com arquivo de setup; separação e organização técnicas serão definidas depois; foco atual no entendimento do projeto |
| 02/11/2026 | 0.3.0 | Decisão de imagem única por MCU com composição em setup de flash (registro depois substituído) |
| 02/11/2026 | 0.4.0 | Esclarecido pelo responsável técnico: um build por tipo de device, um projeto com todas as funcionalidades e um arquivo que indica a composição de cada build (`builds/composicao.cmake`) |
| 02/11/2026 | 0.4.1 | Composição confirmada: OloggerMulti com saída em frequência, único rádio ESP32-C6 e UART de rádio externo; y7080 com entrada diferencial, celular e MQTT, sem digitais |
