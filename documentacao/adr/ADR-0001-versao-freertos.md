# ADR-0001 — Versão do FreeRTOS nos dois builds STM32

> **Criado em:** 04/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.0

**Estado:** Proposta, a decidir pelo responsável técnico.

## Contexto

- O sistema usa FreeRTOS com alocação estática em todo o firmware (`documentacao/padrao_codigo_c.md`, seção FreeRTOS).
- O build do OloggerMulti (STM32L412, Cortex-M4F) usa o FreeRTOS que vem no pacote STM32CubeL4 1.18.2: **V10.3.1**. O build do y7080 (STM32L071, Cortex-M0+) usa o do pacote STM32CubeL0 1.12.4: **V10.2.0**. As duas versões são diferentes e vêm do pacote da ST, fora do repositório.
- O ESP32 usa o FreeRTOS do ESP-IDF, com versão própria e adaptações do ambiente; esta decisão não o inclui.
- O código de aplicação (`lib/`) é um só para os dois builds. Hoje ele usa só a API comum (`xTaskCreateStatic`, `vTaskDelay`, `vTaskDelayUntil`, seções críticas, notificações), que existe nas duas versões. O y7080 ainda não executa nenhuma tarefa de aplicação em hardware.

## Opções

1. **Manter os pacotes da ST (situação atual).** Sem trabalho. O comportamento do kernel pode divergir entre os dois builds, e o repositório depende de um caminho externo (`OLOG_STM32CUBE_L4` e `OLOG_STM32CUBE_L0`) para compilar.
2. **Copiar uma versão única para o repositório** (por exemplo, em `third_party/FreeRTOS`), com as portas Cortex-M4F e Cortex-M0+. Os dois builds usam o mesmo kernel e o código fica reproduzível sem o pacote da ST para o kernel (o HAL continua vindo do pacote). Custo: copiar e manter o kernel e a licença.
3. **Como a opção 2, mas com uma versão LTS mais nova do FreeRTOS.** Ganha correções de segurança e de bugs. Custo adicional: revalidar o que já foi medido no hardware (FW-046) e conferir a compatibilidade com o `FreeRTOSConfig.h`.

## Consequências

- Escolher a opção 2 ou 3 exige um item de backlog: copiar o kernel, ajustar `cmake/freertos.cmake` e refazer a medição do FW-046 (tick, pilha livre e ausência de alocação dinâmica) nos dois builds.
- A opção 1 deixa o y7080 sem teste em hardware do kernel V10.2.0; o risco aparece quando o y7080 passar a rodar tarefas.
- Não foi levantada nenhuma diferença de comportamento entre V10.2.0 e V10.3.1 que afete o código atual; **isso não foi verificado nas notas de versão**.

## Decisão

Pendente. Recomendação do agente, a confirmar: opção 2 com a versão que já foi medida no OloggerMulti, e avaliar a LTS (opção 3) como item separado.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.0 | Criado como proposta |
