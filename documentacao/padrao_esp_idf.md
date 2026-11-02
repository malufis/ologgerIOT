# Padrão de desenvolvimento ESP-IDF — firmware oLoggerIOT

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.1.2

## Escopo

Módulo de comunicação do ESP32-C6 (OloggerMulti), desenvolvido com ESP-IDF. O ESP32 é sempre subordinado ao STM32, que é o orquestrador (ver `documentacao/comunicacao_iot.md`). Este padrão vale para todo código ESP-IDF. O código C puro segue `documentacao/padrao_codigo_c.md`.

## Versão e configuração

- Versão de referência: **ESP-IDF v5.4.4**, instalada em `C:\Espressif\frameworks\esp-idf-v5.4.4`. O alvo `esp32c6` é suportado nessa versão. Fixar a versão no README do firmware na FW-007. Atualizar a versão só em tarefa própria, com nota no histórico.
- `sdkconfig.defaults` é versionado. `sdkconfig` e `sdkconfig.old` são gerados e **não** são versionados.
- Toda mudança em Kconfig (`idf.py menuconfig`) vai para `sdkconfig.defaults`, com comentário do motivo.
- Dependência externa (componente) só com versão fixada em `idf_component.yml`.
- Segredo (senha, chave, certificado) nunca entra em `sdkconfig.defaults` nem no código.

## Estrutura

- Projeto ESP-IDF em `esp32/`, com `CMakeLists.txt` raiz, `main/` e `components/`.
- Cada componente tem seu `CMakeLists.txt` com `idf_component_register`, declarando `REQUIRES` e `PRIV_REQUIRES` explícitos. Não depender de inclusão transitiva.
- Código C puro compartilhado com o STM32 fica em `lib/` e é referenciado pelo componente ESP-IDF. A forma exata é decidida na FW-007 e registrada aqui.
- Um componente, uma responsabilidade. Tamanho segue o limite de arquivo e função do padrão C.

## Erros

- Toda função que retorna `esp_err_t` tem o retorno tratado. Nenhum erro é ignorado.
- `ESP_ERROR_CHECK` só na inicialização do boot, onde a falha justifica parar. Nunca no caminho de execução normal, porque aborta o sistema.
- Erro em runtime: registrar com `ESP_LOGE`, retornar o código ao chamador e, se afetar o sistema, enviar alerta pelo módulo de alertas.
- Usar `esp_err_to_name` na mensagem de log.

## Log

- Cada módulo declara `static const char *TAG = "<modulo>";`.
- `ESP_LOGE` para falha, `ESP_LOGW` para anomalia recuperada, `ESP_LOGI` para marco de estado, `ESP_LOGD` e `ESP_LOGV` só em desenvolvimento.
- Nunca logar credencial, chave privada, certificado ou senha, nem o conteúdo completo de quadro com dado sensível.
- Log não é usado em ISR nem no caminho de comunicação de tempo crítico.

## Tarefas FreeRTOS

- O sistema inteiro roda em FreeRTOS (decisão de 02/11/2026). Regras gerais em `documentacao/padrao_codigo_c.md`, seção FreeRTOS.
- Usar as variantes estáticas do FreeRTOS do ESP-IDF (`xTaskCreateStaticPinnedToCore`, `xQueueCreateStatic`, `xSemaphoreCreateMutexStatic`), com buffers declarados pelo componente. Evita alocação dinâmica da aplicação.
- Toda task tem nome, prioridade e tamanho de pilha definidos em constante nomeada. A tabela de tasks ativas fica em `documentacao/padrao_codigo_c.md`, seção FreeRTOS.
- Tamanho de pilha medido no teste com `uxTaskGetStackHighWaterMark`, com folga registrada.
- Nada de laço de espera ocupada. Esperar com `vTaskDelay`, fila, notificação de task ou event group.
- Comunicação entre tasks com `xQueue`, notificação ou event group. Variável compartilhada só com proteção explícita.
- Task não bloqueia indefinidamente. Toda espera tem timeout nomeado.
- Watchdog: as tasks alimentam o watchdog. Não desligar o watchdog para contornar atraso.

## Eventos e handlers

- Handler de `esp_event` é curto: copia o dado para fila e retorna. Trabalho longo fica na task.
- Não chamar função bloqueante dentro de handler de evento de Wi-Fi ou de MQTT.
- Loop de eventos padrão criado uma vez, na inicialização.

## Memória

- Buffers estáticos, de tamanho fixo. Nada de `malloc` na aplicação, conforme a regra do projeto.
- O heap interno do ESP-IDF é usado pelo SDK e fica fora da regra da aplicação. Medir o uso com `heap_caps_get_free_size` no teste de longa duração.
- Strings com tamanho máximo nomeado. Usar `snprintf` com tamanho, nunca `sprintf`.

## NVS e armazenamento

- NVS inicializado com tratamento de `ESP_ERR_NVS_NO_FREE_PAGES` e `ESP_ERR_NVS_NEW_VERSION_FOUND`: apagar e reinicializar, com registro no log.
- Namespace NVS por módulo.
- Dado gravado com versão e CRC, como no padrão C.
- Credencial em NVS com criptografia de NVS ou criptografia de flash. A escolha depende das pendências P-004 e P-006 e é registrada em ADR.

## Segurança

- Credencial individual por device (certificado e chave), conforme `documentacao/comunicacao_iot.md`.
- Secure boot e criptografia de flash: decisão pendente, a registrar em ADR antes da fase 7.
- Nenhuma chave privada no repositório. Pasta `nogit/` ou gerenciador de segredo fora do repositório.

## Rede, Wi-Fi e MQTT

- Eventos de `esp_netif` e Wi-Fi tratados por handler curto.
- Reconexão segue a regra do projeto: cinco tentativas por rádio, um minuto entre elas, depois modo offline. Essa lógica fica no módulo de conexão do firmware, não no retry automático do SDK.
- MQTT pelo `esp_mqtt_client`, com MQTT 5.0 (`CONFIG_MQTT_PROTOCOL_5`) e TLS.
- TLS 1.3 deve ser validado na versão do mbedTLS do ESP-IDF escolhida. Registrar o resultado na fase 7 (ver pendência do plano).
- Tópicos montados a partir do caminho recebido do sistema, nunca fixos no código.

## Serial com o STM32

- UART com driver do ESP-IDF, buffer de recepção dimensionado e nomeado.
- Quadro com início, tamanho, CRC e fim. Protocolo nunca usa `printf` como transporte.
- Timeout de recepção nomeado. Quadro inválido é descartado e contado, não ignorado em silêncio.

## Atualização (OTA)

- Tabela de partições com dois slots OTA.
- Atualização com `esp_ota_ops`. Após o boot da nova versão, marcar como válida só depois de passar nas verificações. Sem isso, o rollback não funciona.
- Pacote validado (tamanho, versão, compatibilidade) antes da gravação, conforme `documentacao/atualizacao_firmware.md`.

## Energia

- Modos de sleep só com estado salvo antes de dormir. Ao acordar, o estado é reconstruído a partir do que foi salvo.
- Regras de sleep e intervalos seguem `documentacao/telemetria_e_armazenamento.md`.

## Build e tamanho

- Build pelo `idf.py`, com alvo explícito: `idf.py set-target esp32c6`.
- Avisos como erro.
- `idf.py size` após cada alteração relevante. Registrar flash e RAM usadas no item da tarefa.

## Testes

- Lógica sem periférico: teste de unidade com Unity, no PC pelo alvo `linux` do ESP-IDF quando for possível, ou no alvo real.
- Teste que depende de rádio, serial ou rede: roteiro de bancada, registrado na tarefa.
- Teste de longa duração: heap e pilha medidos no fim do teste.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 02/11/2026 | 0.1.0 | Criado padrão ESP-IDF: versão, estrutura, erros, log, tasks, eventos, memória, NVS, segurança, rede e MQTT, serial, OTA, energia, build e testes |
| 02/11/2026 | 0.1.1 | Versão de referência ESP-IDF v5.4.4, instalada em `C:\Espressif` |
| 02/11/2026 | 0.1.2 | Tasks com variantes estáticas do FreeRTOS, alinhado ao padrão FreeRTOS do projeto |
