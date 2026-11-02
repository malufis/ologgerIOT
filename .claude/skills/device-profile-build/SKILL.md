---
name: device-profile-build
description: Use ao compilar o firmware oLoggerIOT para o build de um device (ologgermulti ou y7080). Garante que só as funcionalidades listadas em builds/composicao.cmake entrem no build e que os dois builds continuem compilando.
---

# Build por tipo de device

## Modelo

Há **um único projeto de firmware**, com todas as funcionalidades, em `src/` e `lib/`. Há **um build por tipo de device**. Cada build leva só o que o device tem.

| Build | Comando | MCU | Imagens |
|---|---|---|---|
| OloggerMulti | `cmake -DDEVICE=ologgermulti` | STM32L412C8 e ESP32-C6 | Imagem STM32 e imagem ESP32 |
| y7080 | `cmake -DDEVICE=y7080` | STM32L071CB | Imagem STM32 |

O que entra em cada build está em `builds/composicao.cmake`. Pinagem da placa fica em `devices/<build>/`. Documento: `documentacao/composicao_builds.md`.

## Como compilar

1. Leia `builds/composicao.cmake` e confirme as funcionalidades do build.
2. Configure o CMake raiz com `-DDEVICE=<build>`. Ele valida o nome do device e cada funcionalidade contra o catálogo.
3. Compile a imagem STM32 com a toolchain ARM. Se o build for `ologgermulti`, compile também a imagem ESP32 com `idf.py set-target esp32c6` e `idf.py build`.
4. Meça o tamanho com `arm-none-eabi-size` (STM32) e `idf.py size` (ESP32). Registre no item da sprint.

## Regras

- Funcionalidade nova entra primeiro no catálogo de `builds/composicao.cmake`, quando o módulo existir em `lib/`.
- Para incluir ou tirar funcionalidade de um build, edite só `builds/composicao.cmake`. Não mude código para isso.
- Nunca use `#if` pelo nome do device. Use `#if OLOG_FEAT_<NOME>`.
- Pino de placa fica em `devices/<build>/`. O código usa o nome do pino.
- Mudança comum precisa compilar os dois builds. Mudança específica compila só o build afetado, e isso fica registrado no item.
- Se o build do y7080 estourar flash ou RAM, pare e registre a pendência. Não remova funcionalidade sem decisão.
- Funcionalidade marcada CONFIRMAR ou PENDENTE no arquivo não entra no build até ser confirmada.

## Antes de fechar a tarefa

Registre no item da sprint quais builds foram compilados, o resultado de `size` e o resultado do CMake.
