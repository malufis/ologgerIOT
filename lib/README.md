# lib/ — bibliotecas por módulo

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.1.0

Cada módulo fica em `lib/<modulo>/`, com `include/<modulo>.h` (interface pública) e `src/<modulo>.c` (implementação). Um módulo não acessa o estado de outro diretamente; usa a interface pública.

Módulos previstos pelo backlog: `adc`, `sensor`, `fila`, `modbus`, entre outros das fases 1 a 9.

Regras: `documentacao/padrao_codigo_c.md`.
