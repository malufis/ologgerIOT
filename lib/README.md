# lib/ — bibliotecas por módulo

> **Criado em:** 02/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.2.2

Cada módulo fica em `lib/<modulo>/`, com `include/<modulo>.h` (interface pública) e `src/<modulo>.c` (implementação). Um módulo não acessa o estado de outro diretamente; usa a interface pública. Módulo novo entra em `lib/CMakeLists.txt`; módulo de funcionalidade opcional entra dentro de `if(<FUNCIONALIDADE> IN_LIST OLOG_FEATURES)`.

## Módulos existentes (03/11/2026)

| Módulo | Conteúdo | Funcionalidade | Item |
|---|---|---|---|
| `rtos` | Tabela de prioridades, memória da tarefa ociosa, gancho de pilha e tick | todos | FW-046 |
| `vida` | Tarefa de vida que mede a folga de pilha | todos | FW-046 |
| `adc` | ADC com TIM6 e DMA (STM32L412), com parada e retomada | `ENTRADA_4_20MA` | FW-009 |
| `sensor` | Média por segundo e conversão 4–20 mA (lógica pura, testada no PC) | `ENTRADA_4_20MA` | FW-010, FW-011 |
| `aquisicao` | Tarefa de aquisição das entradas 4–20 mA | `ENTRADA_4_20MA` | FW-009 a FW-012 |
| `io` | Entradas digitais: leitura (`io_entrada`), filtro (`io_filtro`), contador de pulsos (`io_pulsos`), totalizador (`io_totalizador`, `io_backup`) e tarefa (`io_tarefa`). Saídas digitais: estado e retardo (`saida`), modo manual e automático (`controle_saida`), pinos (`io_saida`) e as quatro saídas (`io_saidas`) | `ENTRADA_DIGITAL`, `SAIDA_DISCRETA` | FW-022, FW-052 a FW-060 |
| `energia` | Gerência de energia em Stop 2, só no STM32L4, desabilitada por padrão | MCU | FW-058 |

Módulos previstos pelo backlog e ainda não iniciados: `fila`, `modbus`, `agenda`, `sessao`, `mqtt`, entre outros das fases 3 a 9.

Os módulos de lógica pura (`sensor`, `io_filtro`, `io_pulsos`, `io_totalizador`, `saida`, `controle_saida`) têm testes no PC em `tests/`. Os que acessam hardware só são verificados por compilação e em bancada.

Regras: `documentacao/padrao_codigo_c.md`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.2.2 | Módulo `io` inclui o `controle_saida` (modo manual e automático) |
| 03/11/2026 | 0.2.1 | Módulo `io` passa a incluir as saídas digitais discretas |
| 03/11/2026 | 0.2.0 | Lista dos módulos existentes: rtos, vida, adc, sensor, aquisicao, io e energia |
| 02/11/2026 | 0.1.0 | Criado |
