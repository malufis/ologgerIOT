# Padrão de código C — firmware oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 01/11/2026 · **Versão:** 0.1.0

## Base

Base: MISRA C:2012, para software embarcado. As regras do MISRA valem por padrão. Desvios são permitidos somente com justificativa escrita no próprio código, no formato `/* MISRA-DEV: regra X.Y, motivo */`, e listados em `documentacao/desvios_misra.md` quando forem criados.

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
10. **Perfil de device.** Código específico de hardware fica atrás de `#if` de capacidade declarada no perfil (`devices/*.h`). Nunca por nome de placa.

## Documentação obrigatória

- Toda função tem comentário Doxygen em português, com `@brief`, `@param` para cada parâmetro, `@return` e, quando aplicável, `@note` e `@warning`.
- Todo arquivo tem cabeçalho com `@file`, `@brief`, data de criação, data de modificação e versão.
- Toda alteração de comportamento atualiza o documento de regras afetado e o dicionário de funções.

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
