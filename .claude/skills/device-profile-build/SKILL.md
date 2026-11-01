---
name: device-profile-build
description: Use ao compilar ou configurar o firmware oLoggerIOT para um device específico (OloggerMulti ou y7080). Garante que só o hardware do perfil escolhido seja compilado.
---

# Build por perfil de device

O firmware é único para todos os devices. Cada build usa um perfil em `devices/` e compila só os módulos que aquele hardware tem.

## Perfis

| Perfil | Arquivo | Rádio | Observação |
|---|---|---|---|
| OloggerMulti | `devices/ologgermulti.h` | ESP32-C6 (Wi-Fi) e rádio externo conforme montagem | STM32L412 |
| y7080 | `devices/y7080.h` | Módulo SIMCom SIM7080G (celular) | STM32L071 |

Confirme a montagem no perfil antes de compilar. Não presuma o rádio.

## Como compilar

1. Escolha o perfil: `-DDEVICE=ologgermulti` ou `-DDEVICE=y7080`.
2. Configure o CMake raiz, que lê o perfil e liga só os módulos declarados.
3. Compile o alvo do microcontrolador do perfil (STM32) e, se o perfil tiver ESP32, o alvo ESP-IDF.

## Regras

- Nunca compile um módulo que o perfil não declara. Se precisar, adicione a capacidade ao perfil antes.
- Nunca use nome de placa dentro do código C. Use a capacidade do perfil.
- Ao mudar um perfil, verifique os dois perfis. Uma mudança comum não pode quebrar o outro.

## Antes de fechar a tarefa

Compile os dois perfis quando a mudança for comum. Compile só o afetado quando a mudança for específica. Registre no item do backlog quais perfis foram compilados.
