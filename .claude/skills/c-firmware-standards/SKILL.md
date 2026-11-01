---
name: c-firmware-standards
description: Use ao escrever ou revisar código C do firmware oLoggerIOT (STM32 e ESP32). Garante funções pequenas, bibliotecas por módulo, documentação Doxygen em português e a base MISRA C:2012.
---

# Padrão de código C do firmware

Leia `documentacao/padrao_codigo_c.md` antes de escrever código. Este resumo não substitui o documento.

## Regras que você aplica sempre

1. Função com uma responsabilidade e no máximo 40 linhas.
2. Código de módulo em `lib/<modulo>/include/<modulo>.h` e `lib/<modulo>/src/<modulo>.c`.
3. Variável de módulo é `static`. Nada global compartilhado sem interface pública.
4. Nenhuma alocação dinâmica (`malloc`, `calloc`, `free`).
5. Função que pode falhar retorna código de erro. Nenhuma falha é silenciada.
6. Sem número mágico: constante com nome e unidade no nome.
7. Ponteiro de entrada verificado contra `NULL`.
8. Tipos de largura fixa (`uint8_t`, `uint16_t`, `int32_t`).
9. Arquivo com até 400 linhas.
10. Código específico de hardware fica atrás de `#if` de capacidade do perfil em `devices/`, nunca pelo nome da placa.

## Documentação

- Toda função tem Doxygen em português: `@brief`, `@param` para cada parâmetro, `@return`, e `@note` ou `@warning` quando couber.
- Todo arquivo novo tem cabeçalho com `@file`, `@brief`, data de criação, data de modificação e versão.

## Desvio do MISRA

Só com justificativa no código: `/* MISRA-DEV: regra X.Y, motivo */`. Registre o desvio em `documentacao/desvios_misra.md`.

## Antes de terminar

Rode `tools/verifica_padrao.py` quando existir (FW-008). Se a função passou de 40 linhas, divida antes de considerar a tarefa pronta.
