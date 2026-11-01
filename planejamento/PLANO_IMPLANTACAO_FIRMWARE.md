# Plano de implantação do firmware — oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 01/11/2026 · **Versão:** 0.1.0

## Objetivo e escopo

Implantar o firmware do device (STM32 em C, coletor e orquestrador; ESP32 em ESP-IDF, módulo de comunicação) para os modelos OloggerMulti e y7080. Escopo do device apenas: regras do sistema (perfis, painéis, chat, banco, LGPD, hospedagem) ficam fora deste plano. O sistema é integrado por MQTT ou serial.

Prazo de referência: protótipo de bancada em um mês a partir de 29/10/2026, data derivada 29/11/2026. O protótipo inclui coleta e fila offline, MQTT com credencial individual, entradas e saídas (4–20 mA, saídas digitais, controle por sensor e falha de sensor) e Modbus RTU, com sensores reais e NB-IoT (módulo e operadora).

## Base técnica (definida)

- STM32 (L412 no OloggerMulti, L071 no y7080): C, CMake com toolchain ARM e HAL da ST.
- ESP32-C6 (OloggerMulti): ESP-IDF, CMake nativo do ESP-IDF.
- Comunicação do device: somente MQTT ou serial.
- Documentação: Doxygen em português; cada arquivo com data de criação, data de modificação e versão.
- Protocolos: MQTT 5.0 e TLS 1.3 (validar no módulo e no broker); Modbus Application Protocol v1.1b3 e Serial Line v1.02.
- Versionamento: data e build.
- Mapa Modbus: `documentacao/mapa_modbus.md`. Float em 2 registradores, big-endian (ABCD), 15 posições por tipo.
- Regras de comportamento: `documentacao/configuracao_do_dispositivo.md`, `documentacao/telemetria_e_armazenamento.md`, `documentacao/agenda_operacional_offline.md`, `documentacao/comunicacao_iot.md`, `documentacao/atualizacao_firmware.md`.

## Fases

### Fase 0 — Base do projeto
- Estrutura de pastas com README em cada uma.
- CMake do STM32 com HAL, do ESP32 com ESP-IDF, e configuração do Doxygen.
- Cabeçalho padrão em todos os arquivos.
- **Aceite:** build vazio dos dois alvos sem erros.

### Fase 1 — Aquisição analógica
- ADC disparado por timer, com DMA, em 10 a 1000 amostras por segundo por entrada.
- Média simples por segundo, com o valor de 1 segundo como saída da etapa.
- Corrente em mA e valor de engenharia por entrada, com span, zero e offset.
- **Aceite:** em bancada, a taxa configurada e a média por segundo batem com um gerador de corrente.

### Fase 2 — Regras de sensor
- Falha abaixo de 4 mA ou acima de 20 mA.
- Dente de serra: inversão de sentido acima do percentual, em janela de 5 segundos. Padrão de 2% sem inversor e 10% com inversor de frequência.
- Zero com sensor sem carga, sem alerta de falha. Primeira leitura após zero sem comparação percentual.
- Variação linear ou logarítmica é operação normal.
- Falha: saída em 0 Hz, alerta com sensor, porta e tipo de falha, e valor real sem suavização e sem zerar o dado.
- **Aceite:** testes no PC com sequências sintéticas (linear, logarítmica, dente de serra com e sem inversor).

### Fase 3 — Média móvel de envio e fila de telemetria
- Média móvel de um envio sobre os valores de 1 segundo.
- Fila persistente na flash SPI externa de 16 MiB, com substituição do arquivo mais antigo quando cheia.
- Remoção somente após confirmação do servidor. Cinco tentativas e depois retenção.
- Alertas em espaço separado, com 300 registros. Substitui o mais antigo quando cheio.
- **Aceite:** testes de fila cheia, envio perdido e confirmação recebida.

### Fase 4 — Saídas e controle
- Saídas digitais discretas, pulsadas e em frequência (Hz proporcional a 4–20 mA, com 4 mA e 20 mA configuráveis).
- Partida com atraso configurável por saída. Comando de desligar cancela a partida.
- Modo manual e automático por saída. Manual prevalece e alerta o CCO. Retorno ao automático executa a regra válida imediatamente.
- Prioridade: sensor local, depois agenda, depois regra da plataforma.
- Após falha de sensor, saída em automático desliga e saída em manual mantém o comando.
- **Aceite:** tabela de estados testada no PC e em bancada.

### Fase 5 — Modbus RTU
- Mestre para consultar equipamentos externos e escravo para responder ao mestre do cliente (1 a 247).
- Timeout de 30 segundos por consulta, fixo no firmware.
- Dez tentativas a cada 30 segundos. Defeito após uma hora contada do fim das dez tentativas.
- Recuperação com cinco leituras válidas consecutivas, depois um minuto de funcionamento normal para saída em frequência.
- Mapa de registradores conforme `documentacao/mapa_modbus.md`.
- **Aceite:** simulador Modbus com ausência de resposta, resposta parcial e recuperação.

### Fase 6 — Relógio, hora válida e agenda offline
- RTC com bateria. Conferência NTP quatro vezes ao dia e atualização no boot.
- Hora inválida: medições sem validade, saídas mantêm estado, comandos manuais aguardam, aviso de manutenção guardado para envio.
- Após correção da hora, executa o último comando de cada saída que aguardava.
- Agenda semanal de 336 intervalos de 30 minutos, aplicada somente offline.
- **Aceite:** testes de perda e recuperação de hora, e de transição de intervalo da agenda.

### Fase 7 — Comunicação MQTT e ESP32
- ESP32 como módulo de comunicação, com MQTT 5.0 e TLS 1.3.
- Credencial individual por device, armazenada com proteção. Teste de ativação antes de confirmar.
- Tópicos do device a partir do caminho enviado pelo sistema: SETUP, INFO, STATUS, COMANDO e resultado, telemetria e alertas.
- Confirmação de comando: recebido e depois executado ou falhou com motivo.
- Reconexão: cinco tentativas por rádio, a cada minuto, depois offline.
- Pendências desta fase: formato de SETUP e INFO, campos do heartbeat, nomes dos grupos do JSON.
- **Aceite:** simulador Python de broker, com reconexão, queda e envio confirmado.

### Fase 8 — Sessão de configuração e gateway serial
- Sessão única por dispositivo, iniciada pelo técnico (aplicação Python no notebook, serial) ou pelo sistema (MQTT).
- Comandos MQTT recusados com aviso durante a sessão. Término após cinco minutos sem atividade.
- Configuração pelo Modbus e pelo MQTT, com a mesma regra de sessão.
- Gateway serial: comandos restritos às configurações definidas.
- **Aceite:** testes de sessão concorrente e de timeout.

### Fase 9 — Atualização de firmware
- Verificação de validade e compatibilidade. Recuperação da versão anterior em falha ou queda de energia.
- Saídas digitais desligadas durante atualização e reinício. Retomada com tempo de partida.
- Resultado guardado na fila até confirmação do servidor.
- Confirmação pelo chat somente para atualização, conforme B8.
- **Aceite:** testes de queda de energia durante a gravação e de retorno à versão anterior.

### Fase 10 — Bancada e protótipo
- Escopo do protótipo: coleta e fila offline, MQTT com credencial, entradas e saídas, Modbus RTU, sensores 4–20 mA reais e NB-IoT.
- Validação de recepção NB-IoT em baixo consumo com módulo real e operadora (P-026).
- **Aceite:** roteiro de bancada executado com a placa, registrado em relatório.

## Testes (transversais)

- **PC:** unitários da lógica de sensor, saída, fila, agenda e Modbus.
- **Simulador Python:** broker MQTT local, comportamento offline, reconexão e sessão.
- **Bancada:** placa real, sensores reais, Modbus, MQTT e serial.

## Pendências do device

- Formato de SETUP e INFO.
- Campos do heartbeat (STATUS), incluindo conectividade do módulo.
- Nomes dos grupos do JSON (`analogico`, `digital`, `falhas`, `sistema`, `rede`), a confirmar.
- Build do ESP32 pelo ESP-IDF.
- Validação de TLS 1.3 na memória do STM32L4 e L0.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 01/11/2026 | 0.1.0 | Criado plano de implantação do firmware, em fases com aceite e testes no PC, simulador e bancada |
