# lib/ — bibliotecas por módulo

> **Criado em:** 02/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.2.12

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
| `regra_sensor` | Regra do sensor: limiares mínimo e máximo, sentido e histerese de uma entrada analógica que ligam e desligam uma saída (lógica pura, testada no PC) | `SAIDA_DISCRETA` | FW-063 |
| `freq_saida` | Frequência da saída "pulsada" proporcional à corrente 4–20 mA, com 0 Hz na falha (lógica pura, testada no PC) | `SAIDA_FREQUENCIA` | FW-023 |
| `gerador_freq` | Onda quadrada de 50% por acumulador de fase, uma frequência independente por saída (lógica pura, testada no PC) | `SAIDA_FREQUENCIA` | FW-023 |
| `tipo_saida` | Tipo de cada saída (discreto ou frequência) e a exclusão com o sleep; `io_frequencia` guarda os tipos, liga o gerador ao TIM6 e aos pinos e dá o gancho do sleep (`tipo_saida` é lógica pura, testada no PC) | `SAIDA_FREQUENCIA` | FW-061, FW-023 |
| `tempo` | Conversão do tempo UTC em data, hora e dia da semana locais com o fuso da instalação, e a conversão inversa em UTC (lógica pura, testada no PC) | `SAIDA_DISCRETA`, `RELOGIO` | FW-030 |
| `rtc` | RTC do STM32L412 com o cristal de 32,768 kHz: hora em UTC, sem reiniciar o domínio de backup (sem teste em hardware) | `RELOGIO` | FW-030 |
| `agenda` | Agenda semanal de 336 intervalos de 30 minutos por saída: validação da cadeia hexadecimal recebida do CCO e consulta do estado de um instante, mais a aplicação pelo horário atual em `agenda_aplicador` (lógica pura, testada no PC) e a tarefa `agenda_tarefa`, que a liga ao RTC e às saídas (build com `SAIDA_DISCRETA` e `RELOGIO`; sem teste em hardware) | `SAIDA_DISCRETA` | FW-032 |

Módulos previstos pelo backlog e ainda não iniciados: `fila`, `modbus`, `agenda`, `sessao`, `mqtt`, entre outros das fases 3 a 9.

Os módulos de lógica pura (`sensor`, `io_filtro`, `io_pulsos`, `io_totalizador`, `saida`, `controle_saida`, `regra_sensor`, `freq_saida`, `gerador_freq`, `tipo_saida`, `tempo`, `agenda`) têm testes no PC em `tests/`. Os que acessam hardware só são verificados por compilação e em bancada.

Regras: `documentacao/padrao_codigo_c.md`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.2.12 | `agenda_tarefa` no módulo `agenda` |
| 04/11/2026 | 0.2.11 | Adicionado o módulo `rtc`; `tempo` ganha a conversão inversa |
| 04/11/2026 | 0.2.10 | `agenda_aplicador` no módulo `agenda` |
| 04/11/2026 | 0.2.9 | Adicionado o módulo `tempo` |
| 04/11/2026 | 0.2.8 | `io_frequencia` com o TIM6 e os pinos |
| 04/11/2026 | 0.2.7 | Adicionado o módulo `gerador_freq` |
| 04/11/2026 | 0.2.6 | Adicionados `tipo_saida` e `io_frequencia`; `freq_saida` passa à funcionalidade SAIDA_FREQUENCIA |
| 04/11/2026 | 0.2.5 | Adicionado o módulo `freq_saida` |
| 04/11/2026 | 0.2.4 | Adicionado o módulo `regra_sensor` |
| 04/11/2026 | 0.2.3 | Adicionado o módulo `agenda` |
| 04/11/2026 | 0.2.2 | Módulo `io` inclui o `controle_saida` (modo manual e automático) |
| 03/11/2026 | 0.2.1 | Módulo `io` passa a incluir as saídas digitais discretas |
| 03/11/2026 | 0.2.0 | Lista dos módulos existentes: rtos, vida, adc, sensor, aquisicao, io e energia |
| 02/11/2026 | 0.1.0 | Criado |
