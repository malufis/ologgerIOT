---
name: c-firmware-standards
description: Use ao escrever ou revisar código C do firmware oLoggerIOT (STM32 e ESP32). Garante funções e alterações pequenas, bibliotecas por módulo, funcionalidade por build (#if OLOG_FEAT_*), documentação Doxygen em português, boas práticas de C e a base MISRA C:2012.
---

# Padrão de código C do firmware

Leia `documentacao/padrao_codigo_c.md` antes de escrever código. Este resumo não substitui o documento. Para código ESP-IDF, leia também `documentacao/padrao_esp_idf.md` e use a skill `esp-idf-standards`.

## Regras que você aplica sempre

1. Função com uma responsabilidade e no máximo 40 linhas.
2. Código de módulo em `lib/<modulo>/include/<modulo>.h` e `lib/<modulo>/src/<modulo>.c`.
3. Variável de módulo é `static`. Nada global compartilhado sem interface pública.
4. Nenhuma alocação dinâmica (`malloc`, `calloc`, `free`).
5. Função que pode falhar retorna código de erro. Nenhuma falha é silenciada.
6. Sem número mágico: constante com nome e unidade no nome.
7. Ponteiro de entrada verificado contra `NULL`.
8. Tipos de largura fixa (`uint8_t`, `uint16_t`, `int32_t`). Nada de `int` para protocolo.
9. Arquivo com até 400 linhas.
10. Funcionalidade depende de `#if OLOG_FEAT_<NOME>`, definida pelo build em `builds/composicao.cmake`. Nunca escolha código por nome de device ou de placa.
11. Pino de placa fica em `devices/<build>/`. O código usa o nome do pino.
12. `#if` de plataforma só para diferença de MCU ou de plataforma, por macro do alvo.

## Pequenos códigos e bibliotecas

- Uma alteração por vez, de preferência com menos de 300 linhas no diff. Se for maior, divida o item antes.
- Toda função nova entra com o teste dela no mesmo passo.
- Interface do `.h` mínima: só o que outro módulo usa. O resto é `static` no `.c`.
- Trecho usado por dois módulos vira biblioteca em `lib/`, e não é copiado.
- Dependência vai numa direção: aplicação depende de biblioteca, biblioteca não depende de aplicação. Acesso ao hardware passa por driver.

## Boas práticas de C

- Chaves em todo `if`, `else`, `for` e `while`.
- `switch` com `default`. Queda de caso só com `/* fallthrough */` e justificativa.
- `const` em parâmetro e dado que não muda.
- `bool` de `<stdbool.h>` para decisão.
- `size_t` para tamanho e contagem. Índice verificado antes do acesso ao vetor.
- Conversão de tipo com cast explícito, e só quando o valor cabe.
- Sem `goto`, exceto saída única de limpeza com comentário. Sem recursão.
- `volatile` só para dado de ISR ou de hardware.
- Sem comparação de ponto flutuante por igualdade.
- Enum com valor explícito quando vai para protocolo ou flash.
- Dado gravado na flash ou enviado por protocolo tem versão, tamanho e CRC.

## Documentação

- Toda função tem Doxygen em português: `@brief`, `@param` para cada parâmetro, `@return`, e `@note` ou `@warning` quando couber.
- Todo arquivo novo tem cabeçalho com `@file`, `@brief`, data de criação, data de modificação e versão.
- Função nova entra no dicionário (`documentacao/dicionario_funcoes.md`).

## Desvio do MISRA

Só com justificativa no código: `/* MISRA-DEV: regra X.Y, motivo */`. Registre o desvio em `documentacao/desvios_misra.md`.

## Antes de terminar

Rode `tools/verifica_padrao.py` quando existir (FW-008). Se a função passou de 40 linhas ou o arquivo de 400, divida antes de considerar a tarefa pronta.
