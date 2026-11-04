# tests/ — testes

> **Criado em:** 02/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.2.5

Testes de unidade da lógica, compilados e executados no PC, sem hardware. Cada módulo tem seu teste em `tests/<modulo>/`. Testes que dependem de placa ficam no roteiro de bancada, e não aqui.

Testes de comunicação (broker MQTT e comportamento offline) ficam no simulador Python, em `tools/`.

## Como rodar

```
cmake -S tests -B build/tests -G Ninja -DCMAKE_C_COMPILER=gcc
cmake --build build/tests
ctest --test-dir build/tests --output-on-failure
```

Módulos testados em 03/11/2026: `sensor` (57 verificações), `io_filtro` (135), `io_pulsos` (25), `io_totalizador` (36, com queda de energia em cada gravação) e `saida` (36, com o retardo de acionamento). As macros de teste estão em `tests/teste.h`. Em uma máquina com política de Controle de Aplicativo do Windows, o executável recém-linkado pode ser bloqueado; aconteceu uma vez com `test_sensor.exe` em 03/11/2026, e recompilar resolveu.

Referência de testes: `planejamento/PLANO_IMPLANTACAO_FIRMWARE.md`, seção "Testes (transversais)".

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.2.5 | O teste de `saida` passou de 54 para 36 verificações: saíram os casos do acionamento que desligava sozinho, removido por decisão de 03/11/2026 |
| 03/11/2026 | 0.2.4 | O "acionamento por tempo" é o retardo de acionamento; ligar com a saída ligada ou aguardando é descartado; mapa Modbus das saídas ajustado (P-037) |
| 03/11/2026 | 0.2.3 | Testes de `saida` com o retardo de acionamento: 54 verificações |
| 03/11/2026 | 0.2.2 | Adicionado o módulo `saida` aos testes |
| 03/11/2026 | 0.2.1 | Corrigido o estado do teste do `sensor`: passa, 57 verificações |
| 03/11/2026 | 0.2.0 | Adicionados o comando para rodar os testes e os módulos testados |
| 02/11/2026 | 0.1.0 | Criado |
