---
name: esp-idf-standards
description: Use ao escrever ou revisar código ESP-IDF do módulo de comunicação ESP32-C6 do oLoggerIOT. Garante boas práticas da Espressif: esp_err_t, logs por TAG, tasks e filas do FreeRTOS, handlers curtos, NVS, MQTT com TLS, OTA com rollback e build com idf.py.
---

# Padrão ESP-IDF do firmware

Leia `documentacao/padrao_esp_idf.md` antes de escrever código. Este resumo não substitui o documento. Código C puro segue também `documentacao/padrao_codigo_c.md`.

## Regras que você aplica sempre

1. **Erro:** todo `esp_err_t` é tratado. `ESP_ERROR_CHECK` só no boot. Em runtime, registre com `ESP_LOGE`, retorne o código e use `esp_err_to_name`.
2. **Log:** `static const char *TAG` por módulo. Nunca logue credencial, chave, certificado ou senha. Nada de log em ISR nem no caminho de comunicação crítico.
3. **Task:** nome, prioridade e pilha em constante nomeada. Pilha medida com `uxTaskGetStackHighWaterMark` no teste.
4. **Espera:** sem laço ocupado. Use `vTaskDelay`, fila, notificação ou event group, sempre com timeout nomeado.
5. **Handler de evento:** curto. Copie o dado para fila e retorne. Nada de I/O longo ou bloqueio.
6. **Memória:** buffer estático de tamanho fixo. Sem `malloc` na aplicação. Use `snprintf`, nunca `sprintf`.
7. **NVS:** trate `ESP_ERR_NVS_NO_FREE_PAGES` e `ESP_ERR_NVS_NEW_VERSION_FOUND`. Um namespace por módulo. Dado com versão e CRC.
8. **Segredo:** nada de chave ou senha em `sdkconfig.defaults` nem no código.
9. **Configuração:** mudança de Kconfig vai para `sdkconfig.defaults` com comentário. `sdkconfig` não é versionado.
10. **Componente:** versão fixada em `idf_component.yml`. `REQUIRES` e `PRIV_REQUIRES` explícitos.
11. **Watchdog:** nunca desligue para esconder atraso.

## Rede e comunicação

- Reconexão segue a regra do projeto: cinco tentativas por rádio, um minuto entre elas, depois offline. A lógica fica no módulo de conexão, não no retry do SDK.
- MQTT com `esp_mqtt_client`, MQTT 5.0 e TLS. Tópicos montados a partir do caminho recebido do sistema.
- Serial com o STM32: quadro com início, tamanho, CRC e fim. Quadro inválido é contado e descartado, nunca ignorado em silêncio.

## OTA e energia

- Dois slots OTA. Marque a nova versão como válida só depois das verificações. Sem isso, o rollback não funciona.
- Sleep só com estado salvo antes de dormir.

## Build e teste

- Build com `idf.py set-target esp32c6` e `idf.py build`. Avisos como erro.
- Depois de alteração relevante, rode `idf.py size` e registre flash e RAM no item.
- Lógica sem periférico: teste Unity, no alvo `linux` quando possível.
- Teste com rádio, serial ou rede: roteiro de bancada registrado na tarefa.

## Antes de terminar

Confirme que nenhum erro foi ignorado, que nenhuma credencial aparece em log ou arquivo versionado, e que a pilha de cada task nova foi medida.
