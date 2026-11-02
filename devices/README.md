# devices/ — pinagem por placa

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.2.0

Pinagem e mapeamento de hardware de cada placa, em `devices/ologgermulti/` e `devices/y7080/`. O CMake inclui a pasta do build escolhido. O código do módulo usa só o nome do pino (por exemplo `PIN_ENTRADA_4_20MA_1`), e não sabe em qual placa está.

A composição de funcionalidades de cada build não fica aqui. Ela fica em `builds/composicao.cmake`.

Criação dos arquivos de pinagem: FW-003.

Regras: `documentacao/composicao_builds.md` e `documentacao/padrao_codigo_c.md`.
