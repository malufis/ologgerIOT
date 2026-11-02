# tools/modelos/ — modelos de arquivo

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.1.0

Modelos para todo arquivo C novo do firmware (FW-005):

- `modelo.h`: cabeçalho de módulo, com a interface pública documentada.
- `modelo.c`: implementação, com o estado do módulo `static` e o parâmetro verificado contra `NULL`.

## Como usar

1. Copie os dois arquivos para `lib/<modulo>/include/<modulo>.h` e `lib/<modulo>/src/<modulo>.c`.
2. Troque `modelo` pelo nome do módulo, em todos os nomes e na proteção de inclusão.
3. Preencha as datas de criação e de modificação e a versão.

O cabeçalho de arquivo é verificado por `tools/verifica_padrao.py`: `@file`, `@brief`, `@date criacao`, `@date modificacao` e `@version`.

Regras: `documentacao/padrao_codigo_c.md`.
