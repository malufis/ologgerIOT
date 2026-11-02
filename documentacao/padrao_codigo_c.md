# Padrão de código C — firmware oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.4.0

## Base

Base: MISRA C:2012, para software embarcado. As regras do MISRA valem por padrão. Desvios são permitidos somente com justificativa escrita no próprio código, no formato `/* MISRA-DEV: regra X.Y, motivo */`, e listados em `documentacao/desvios_misra.md` quando forem criados.

Boas práticas do ESP-IDF (ESP32) estão em `documentacao/padrao_esp_idf.md`. Este documento vale para todo código C, nos dois MCUs.

## Regras do projeto

1. **Funções pequenas.** Cada função faz uma coisa. Limite de 40 linhas por função, verificado por `tools/verifica_padrao.py` (FW-008).
2. **Bibliotecas por módulo.** Cada módulo tem `lib/<modulo>/include/<modulo>.h` e `lib/<modulo>/src/<modulo>.c`. O módulo não acessa estado de outro módulo diretamente; usa a interface pública.
3. **Estado encapsulado.** Variáveis de módulo são `static`. Não há variável global compartilhada entre módulos sem interface.
4. **Sem alocação dinâmica.** Nada de `malloc`, `calloc` ou `free` no firmware. Buffers têm tamanho fixo, definido em tempo de compilação.
5. **Retorno de erro explícito.** Funções que podem falhar retornam código de erro. Nenhuma falha é silenciada.
6. **Sem números mágicos.** Constantes com nome, em `#define` ou `enum`, com unidade no nome (`TIMEOUT_MODBUS_MS`).
7. **Ponteiros verificados.** Todo ponteiro de entrada é verificado contra `NULL` antes do uso.
8. **Tipos de largura fixa.** Uso de `uint8_t`, `uint16_t`, `int32_t` e assim por diante. Nada de `int` para dado de protocolo.
9. **Limites de tamanho.** Arquivo com até 400 linhas. Acima disso, dividir em módulos.
10. **Funcionalidade por build.** Todo o código fica no projeto. O que cada build leva é definido em `builds/composicao.cmake`, que liga os módulos e define `OLOG_FEAT_<NOME>`. Código que depende de funcionalidade usa `#if OLOG_FEAT_<NOME>`. Nunca `#if` pelo nome do device ou da placa (ver `documentacao/composicao_builds.md`).
11. **Pinagem por placa, fora do código.** Os pinos de cada placa ficam em `devices/<build>/`, incluídos pelo CMake. O código usa o nome do pino, nunca número de pino de placa.
12. **Código condicional de plataforma.** `#if` para diferença de MCU ou de plataforma (STM32L4, STM32L0 e ESP32) é permitido, por macro do próprio alvo.

## FreeRTOS

O sistema inteiro roda em FreeRTOS: STM32 e ESP32. Não há laço principal com tarefas soltas.

- **Alocação estática.** `configSUPPORT_STATIC_ALLOCATION` ligado e `configSUPPORT_DYNAMIC_ALLOCATION` desligado no STM32. Tasks, filas, mutex, semáforos e timers são criados com as versões `*Static`, com buffers declarados pelo módulo. Isso mantém a regra 4 (sem alocação dinâmica).
- **Uma responsabilidade por task.** Cada task tem nome, prioridade e tamanho de pilha em constante nomeada. A tabela de tasks fica neste documento, na seção de tasks ativas.
- **Pilha medida.** `uxTaskGetStackHighWaterMark` no teste de cada task nova, com folga registrada.
- **Comunicação entre tasks.** Fila (`xQueue*`) ou notificação de task. Variável compartilhada só dentro de mutex, com seção curta e timeout nomeado.
- **Sem espera ocupada.** Esperar com `vTaskDelay`, `xQueueReceive`, `xSemaphoreTake` ou `ulTaskNotifyTake`, sempre com timeout nomeado. Nunca `while` esperando flag.
- **Interrupção.** Só as variantes `FromISR` dentro de ISR, e nenhum log nem chamada bloqueante.
- **Watchdog.** As tasks alimentam o watchdog independente. Não desligar para contornar atraso.
- **MISRA.** O código do FreeRTOS fica fora da regra de desvio. Desvio no código do projeto, quando houver, segue o formato `MISRA-DEV`.

### Tasks ativas

| Task | Prioridade | Pilha | Responsabilidade |
|---|---|---|---|
| Em definição | — | — | Preenchida conforme cada task é criada, na sprint correspondente |

## Pequenos códigos e bibliotecas

- Uma tarefa do backlog gera uma alteração pequena, de preferência com menos de 300 linhas no diff. Acima disso, dividir o item antes de codificar.
- Uma função nova exige o teste dela no mesmo passo. Não acumular função sem teste.
- Cada biblioteca tem interface pequena. O `.h` expõe só o que outro módulo usa; o resto fica `static` no `.c`.
- Quando um trecho serve a dois módulos, ele vira biblioteca própria em `lib/`, e não é copiado.
- Tipo opaco quando o chamador não precisa ver a estrutura.
- Dependência segue uma direção: módulo de aplicação depende de biblioteca; biblioteca não depende de aplicação. Acesso ao hardware passa por uma camada de driver, nunca direto no módulo de regra.

## Boas práticas de C

- Todo `if`, `else`, `for` e `while` tem chaves, mesmo para uma linha.
- `switch` tem `default` e não tem queda de caso (`fallthrough`) sem comentário `/* fallthrough */` e justificativa.
- `const` em todo parâmetro e dado que não muda.
- `bool` de `<stdbool.h>` para decisões. Nada de inteiro como booleano.
- Tamanho de buffer e de contagem em `size_t`. Índice de vetor verificado contra o tamanho antes do acesso.
- Conversão de tipo explícita, com cast, e só quando o valor cabe no tipo de destino.
- Sem `goto`, exceto para saída única de limpeza de recursos, com comentário.
- Sem recursão.
- `volatile` só em dado compartilhado com ISR ou com hardware. Acesso à seção crítica curto e documentado.
- Sem comparação de ponto flutuante por igualdade. Usar faixa ou tolerância nomeada.
- Enum com valor explícito em cada item quando o valor vai para protocolo ou para a flash.
- Operações com risco de estouro (multiplicação, soma de contadores) verificadas ou com faixa garantida pelo chamador.
- Estrutura de dado gravada na flash ou enviada por protocolo tem versão, tamanho e CRC.

## Documentação obrigatória

- Toda função tem comentário Doxygen em português, com `@brief`, `@param` para cada parâmetro, `@return` e, quando aplicável, `@note` e `@warning`.
- Todo arquivo tem cabeçalho com `@file`, `@brief`, data de criação, data de modificação e versão.
- Toda alteração de comportamento atualiza o documento de regras afetado, o dicionário de funções e `documentacao/ESTADO_IMPLEMENTACAO.md`.

## Modelo de função

```c
/**
 * @brief Converte corrente em miliampères para valor de engenharia.
 *
 * @param[in]  corrente_ma  Corrente medida, em mA, entre 4,0 e 20,0.
 * @param[in]  span        Valor de engenharia em 20 mA.
 * @param[in]  zero        Valor de engenharia em 4 mA.
 * @param[out] valor       Valor convertido na unidade do sensor.
 * @return     0 em sucesso, código negativo em erro.
 */
int32_t conv_corrente_para_valor(float corrente_ma, float span, float zero, float *valor);
```

## Ferramentas

- Compilador: GCC ARM (STM32) e toolchain do ESP-IDF (ESP32), com avisos como erro.
- Verificação estática: ferramenta MISRA a escolher no FW-008, com registro da escolha em ADR.
- Verificação do padrão do projeto: `tools/verifica_padrao.py`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 01/11/2026 | 0.1.0 | Criado padrão de código com base MISRA C:2012 e regras do projeto |
| 02/11/2026 | 0.2.0 | Regra 10 trocada: composição por setup, não por `#if` de perfil; incluídas regras de pequenos códigos, bibliotecas e boas práticas de C; referência ao padrão ESP-IDF |
| 02/11/2026 | 0.3.0 | Composição por build, no lugar de setup em runtime: funcionalidades em `builds/composicao.cmake` e `#if OLOG_FEAT_<NOME>`; pinagem por placa em `devices/`; regras 10 a 12 reescritas |
| 02/11/2026 | 0.4.0 | Seção FreeRTOS: sistema inteiro em FreeRTOS, com alocação estática, filas, mutex e tabela de tasks ativas |
