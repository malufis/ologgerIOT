# devices/ — pinagem e memória por placa

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.3.0

Um subdiretório por build, escolhido pelo CMake pelo `-DDEVICE`:

- `ologgermulti/`: `pinos.h` (STM32L412C8U6) e `STM32L412C8_FLASH.ld`.
- `y7080/`: `pinos.h` (STM32L071CBU6) e `STM32L071CBU6_FLASH.ld`.

O `pinos.h` tem um par de macros por função, `PINO_<FUNCAO>_PORTA` e `PINO_<FUNCAO>_NUM`. O código de módulo usa só esses nomes e não sabe em qual placa está. A pinagem foi conferida contra as netlists dos esquemas, e `python tools/verifica_pinos.py` verifica pino duplicado e par incompleto.

A composição de funcionalidades de cada build não fica aqui. Ela fica em `builds/composicao.cmake`.

Regras: `documentacao/composicao_builds.md` e `documentacao/padrao_codigo_c.md`.
