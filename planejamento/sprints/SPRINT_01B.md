# SPRINT_01B — Entrada digital

> **Criado em:** 03/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.2.1
> **Período:** 03/11/2026 a 06/11/2026 (paralela à SPRINT_01, bloqueada pela ferrite L2 não soldada)

## Meta

Entrada digital ativa com tensão no campo, em modo discreto (0 ou 1), de frequência em Hz ou de contador de pulsos, com filtro de ruído, totalizador estilo hodômetro e despertar do STM32 do sleep. Fica fora desta sprint a integração com Modbus e telemetria, que entra nas sprints 03 e 05, e o despertar do módulo para transmitir, que é de outra sprint.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-052 | Biblioteca `lib/io`: leitura do estado das entradas D1 a D4 | Código pronto em 03/11/2026 (`io_entrada` e `io_tarefa`); ativo é pino em nível baixo. Compila no `ologgermulti`; o `y7080` não leva a entrada digital. Falta a leitura em bancada com tensão aplicada |
| FW-053 | `io_filtro`: filtro de ruído por número de amostras, com conversão de tempo | Concluído em 03/11/2026: 135 verificações no PC. Padrão de 50 ms no estado e 20 ms nos pulsos |
| FW-054 | Modo de frequência em Hz por contagem de bordas | Pendente. Faixa de projeto de 0 a 20 kHz; o ajuste fica para depois, por decisão do responsável técnico |
| FW-055 | Contagem de pulsos dormindo pela interrupção externa (PB5 a PB8) | Código pronto em 03/11/2026: o tratador `EXTI9_5_IRQHandler` (em `interrupcoes.c`) soma o pulso e pede para o STM32 acordar. Bancada pendente (roteiro, testes 9 e 10) |
| FW-056 | Roteiro e execução de bancada da entrada digital | Roteiro escrito em 03/11/2026 (`documentacao/roteiro_bancada_entrada_digital.md`, 12 testes). Execução pendente: precisa do device |
| FW-057 | `io_pulsos`: contador de pulsos e inatividade | Concluído em 03/11/2026: 25 verificações no PC. Integrado à `io_tarefa` |
| FW-058 | Entrada em Stop 2 pelo contador de pulsos | Código pronto em 03/11/2026 (`lib/energia`), **desabilitado por padrão** (`energia_define_habilitada`); só no STM32L4. Bancada pendente |
| FW-059 | Contagem de pulsos em hardware (LPTIM1, entrada D4) durante o sleep | A confirmar: só a D4 conta em hardware; depende do RM0394 e da taxa de pulsos |
| FW-060 | Totalizador (hodômetro) por entrada, nos registradores de backup | Lógica concluída em 03/11/2026: 36 verificações no PC, com queda de energia em cada gravação. Registradores de backup (TAMP) sem teste em hardware |

## Estado da bancada (03/11/2026)

- **D1 retirada** das entradas digitais, por decisão do responsável técnico (achado HW-008).
- **Limite de 12 V** na entrada. O resistor de 4K7 em 0402 não suporta 24 V; passa a 0603 no próximo layout, e 24 V ainda pede potência maior.
- Sem a D1, a entrada fica sem supressor de surto e ESD e sem diodo contra inversão de polaridade. Fonte de bancada limitada em corrente.
- O device está disponível só no dia seguinte (04/11/2026); nenhum item da fase 1B passou por hardware.

## Como funciona (consolidado em 03/11/2026)

- **Estado discreto:** `io_tarefa` lê D1 a D4 a cada 2 ms. O estado filtrado usa 50 ms. Variáveis `estado_bruto` e `estado_filtrado` lidas por SWD.
- **Contador de pulsos:** um filtro próprio de 20 ms alimenta o `io_pulsos`, que conta cada transição para ativo. Pulso ou intervalo menor que ~20 ms é descartado. Os pulsos vêm de leitores, sem repique, a 5 a 10 por segundo, com até quatro leitores ao mesmo tempo.
- **Totalizador (hodômetro):** cada entrada tem um total de 32 bits que só soma e volta a zero ao estourar. Quem o lê calcula a diferença entre duas leituras, então uma transmissão perdida não perde contagem. `io_tarefa_total(canal)` lê e `io_tarefa_define_total(canal, valor)` iguala ao medidor na instalação. Fica nos registradores de backup do TAMP, em duas cópias com complemento por entrada, gravado a cada pulso. Uma queda de energia no meio da gravação deixa o total antigo ou o novo, nunca perdido (testado no PC).
- **Sleep:** `lib/energia` cria a tarefa de gerência de energia. Se estiver habilitada e `io_tarefa_pode_dormir()` (60 s sem pulso em todas as entradas, configurável por `io_tarefa_define_inatividade`), ela para a aquisição analógica (`aquisicao_para`), liga a contagem pela interrupção externa e entra em Stop 2. O clock fica no MSI, que o Stop 2 preserva.
- **O pulso acorda o STM32, que fica acordado.** Em Stop 2 só a entrada digital fica ativa. O pulso gera uma interrupção na borda que ativa a entrada (descida do pino, por causa do optoacoplador); o tratador soma o pulso no contador e no totalizador (já gravado, sem depender da RAM) e pede para o STM32 acordar. Acordado, `io_tarefa_retoma` desliga a interrupção externa, soma os pulsos contados dormindo e ressincroniza os filtros com o estado lido naquele momento, para um pulso ainda ativo não ser contado de novo. Ele só dorme de novo depois do tempo de inatividade sem pulso, então um pulso constante não faz acordar e dormir a cada pulso. Não há filtro na interrupção, para o pulso que acorda não se perder.
- **Aquisição analógica:** ao dormir, `aquisicao_para` para o timer, o DMA, o ADC e a referência interna. Ao acordar, `aquisicao_retoma` descarta o segundo parcial e as leituras antigas, e a leitura volta a ficar disponível ao fechar o primeiro segundo. Dormindo não há leitura analógica; acordar por uma entrada analógica fica para outra sprint.
- **O ESP32 só é acordado para transmitir**, em outra sprint. O despertar para transmitir (1, 2 ou 5 vezes ao dia, ou por uma entrada analógica) entra por `energia_solicita_acordar` e depende do RTC (fase 6).

## Limitações e riscos, a verificar na bancada

- **Nada da fase 1B foi testado em hardware.** Entrar em Stop 2, o acesso ao TAMP (clock `RTCAPBEN` e `DBP`) e a contagem no tratador só compilam.
- **Bateria no VBAT.** Os registradores de backup só sobrevivem à falta de energia se o pino VBAT tiver bateria. Confirmar na placa.
- **Primeira partida.** Com os registradores zerados, `io_totalizador_corrompido` indica verdadeiro e o total recomeça em zero. Na instalação, usar `io_tarefa_define_total` para igualar ao medidor.
- **Depurador.** Em Stop 2 a conexão SWD cai, a menos que o depurador mantenha o clock de depuração. Habilitar o sleep na bancada exige essa configuração, ou o uso de um pulso para acordar.
- **Tempo.** O tick do FreeRTOS para no sleep, e os tempos medidos por ele (como o de inatividade) não contam o tempo dormido. A hora certa virá do RTC.
- **Pulsos sem filtro dormindo.** Ruído elétrico em entrada solta gera interrupções falsas e acorda o STM32 sem necessidade. Entradas não usadas devem ficar com o sinal definido.
- **Consumo não medido.** O consumo em Stop 2 e em modo ativo não foi medido nem estimado; medir na bancada (roteiro, teste 8).
- **Largura do pulso.** Um pulso só é aceito se durar mais que 20 ms mais uma amostra de 2 ms. Confirmar que os leitores geram pulsos mais largos, com folga.
- **Proteção da entrada (HW-008).** Sem a D1 e com o 4K7 em 0402, valem o limite de 12 V e os alertas do HW-008.
- **Contagem em hardware (FW-059).** Pela base do STM32CubeMX (`STM32L412C8Ux.xml`): D4 (PB5) tem `LPTIM1_IN1`, D1 (PB7) tem `LPTIM1_IN2`, D3 (PB6) tem `LPTIM1_ETR`, D2 (PB8) não tem sinal do LPTIM; o LPTIM2 usa PB1, que já é a entrada 4–20 mA 4. Pelo que conheço do RM0394, a contagem de pulsos externos usa a entrada IN1, ou seja, só a D4 conta em hardware dormindo (a confirmar no RM0394). Como os pulsos são sem repique, o filtro curto do LPTIM não é problema.

## Pendências para o responsável técnico

- Confirmar que os leitores geram pulsos mais largos que 20 ms, com folga, e a taxa máxima de 5 a 10 pulsos por segundo por leitor.
- Decidir se o FW-059 (contagem em hardware na D4) vale a pena, depois da medição de consumo em bancada.
- Faixa de frequência máxima da entrada em Hz, janela de medição e filtro do modo de frequência, quando o FW-054 for retomado.
- Definir os modos da entrada digital no mapa Modbus (discreto, contador de pulsos e frequência) e o endereço do totalizador. O mapa hoje diz "discreto ou pulsado" (40151–40165).
- Confirmar se vale um supressor de ~30 V no nó `IN-Dx` no próximo layout, para uso em campo industrial.
- Gerência de energia do y7080 (STM32L0): o módulo `lib/energia` só existe para o L4.

## Pendência técnica: limite de frequência do pino

Objetivo: saber a maior frequência que a entrada digital do STM32L412 consegue receber em modo de frequência. Não foi possível confirmar no datasheet nesta data. O PDF oficial (`stm32l412c8.pdf`, DS12469 Rev 9) não abriu no ambiente, por tempo esgotado, e a busca não trouxe as tabelas de timers e de características elétricas. Referências a conferir: DS12469 (seção de timers e de características elétricas dos GPIOs) e RM0394 (timers TI e modo de clock externo). Regra de referência, sem a tabela do datasheet: AN4776 indica `fTIMxCLK ≥ 3 × f(entrada)` no modo TI. O núcleo é de até 80 MHz. Resultado: limite provisório da ordem de 26 MHz no modo TI, a confirmar.

Informação recebida do responsável técnico em 03/11/2026, de fonte externa e **não verificada em datasheet**: amostragem do registrador de entrada por timer e DMA chega a cerca de 20 a 26 MHz no STM32L4 (80 MHz) e a cerca de 8 a 10 MHz no STM32L0 (32 MHz); leitura por polling chega a cerca de 16 a 20 MHz no L4 e 6 a 8 MHz no L0. Esses números descrevem amostragem de nível, não contagem de bordas, e coincidem em ordem de grandeza com o limite do timer acima.

**O limite do pino não é o limite da placa.** No OloggerMulti a entrada passa pelo optoacoplador TLP291-4 (`entradas_digitais.kicad_sch`), com resistores de 4K7 e 10k. Os tempos de subida e descida do optoacoplador com essa carga tendem a limitar a frequência útil a poucas dezenas de kHz, muito abaixo do pino. Isto é inferência a confirmar no datasheet do TLP291 e por medição em bancada (FW-056). O y7080 não tem entrada digital na composição, então o valor do STM32L0 não se aplica. Não usar nenhum desses valores no código antes da confirmação.

**Escopo da frequência (03/11/2026):** o responsável técnico informou que não haverá frequência alta e que o ajuste fica para depois. Referência recebida, de fonte externa e não verificada: a saída de pulsos dos inversores WEG gera no máximo 10 kHz ou 20 kHz, ajustável por parâmetro para corresponder a 100% da velocidade. Consequências para o FW-054, a confirmar:

- A faixa de projeto é de 0 a 20 kHz, muito abaixo do limite do pino. A medição por contagem de bordas em timer atende com folga; interrupção por borda em cada pulso a 20 kHz custaria dezenas de milhares de interrupções por segundo e deve ser evitada.
- A frequência em 100% da velocidade (10 ou 20 kHz) é configuração por entrada, para converter Hz em percentual de velocidade.
- O filtro de 50 ms vale só para o modo discreto. No modo de frequência ele eliminaria o sinal (o período a 20 kHz é de 50 µs). Esse modo precisa de filtro próprio, de largura mínima de pulso, a definir.
- O limite real a 20 kHz é o optoacoplador TLP291 com a carga de 10k, a validar em bancada. A tensão de campo é de 12 a 24 V (na bancada, até 12 V).

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para a sprint seguinte.
- Testes no PC passando (`tests/`) e build STM32 da composição `ologgermulti` compilado.
- Roteiro de bancada executado e registrado (FW-056), com os resultados dos 12 testes.
- `documentacao/README.md`, `documentacao/configuracao_do_dispositivo.md`, `documentacao/dicionario_funcoes.md` e `BACKLOG.md` atualizados.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.2.1 | Corrigido: a entrada digital compila só no `ologgermulti`; o `y7080` não a leva |
| 03/11/2026 | 0.2.0 | Arquivo reescrito de forma consolidada: itens com a situação atual, estado da bancada (D1 retirada, 12 V, 4K7 em 0402), funcionamento, limitações e pendências. Removidos os textos que ficaram obsoletos (leitura a cada 10 ms, 131 e 15 verificações, "voltar a dormir a cada pulso", contagem "a definir") |
| 03/11/2026 | 0.1.13 | Correção da lógica do sleep: o pulso acorda o STM32 e ele fica acordado, dormindo só após o tempo de inatividade (antes eu havia feito voltar a dormir a cada pulso). Criados o gancho que para e retoma a aquisição analógica e o roteiro de bancada (FW-056). O ESP32 só acorda para transmitir |
| 03/11/2026 | 0.1.12 | Decisões: quatro leitores, Stop 2 liberado e totalizador estilo hodômetro. FW-058 (energia) e FW-060 (totalizador) implementados; compilam, sem teste em hardware |
| 03/11/2026 | 0.1.11 | Modo sleep redefinido: o tratador da interrupção externa conta o pulso e o STM32 volta a dormir; ao acordar, `io_tarefa_trata_despertar` soma os pulsos e ressincroniza os filtros. Pulsos de leitores, sem repique, 5 a 10 por segundo |
| 03/11/2026 | 0.1.10 | Decisões do responsável técnico: filtro de pulso de 20 ms (leitura a cada 2 ms), inatividade configurável com padrão de 60 s e despertar para transmitir em outra sprint. LPTIM verificado no CubeMX: só D4 conta pulsos em hardware. FW-059 e FW-060 propostos |
| 03/11/2026 | 0.1.9 | Uso como contador de pulsos: FW-057 concluído no PC e integrado à tarefa; despertar só na borda de ativação; FW-058 (Stop 2) proposto e a confirmar |
| 03/11/2026 | 0.1.8 | FW-055: fonte de despertar por interrupção externa nas duas bordas, com linha mascarada após o disparo; o teste de aceite depende do módulo de sleep, ainda sem item no backlog |
| 03/11/2026 | 0.1.7 | Criada a tarefa `io_tarefa` e ligada em `main.c`: lê D1 a D4 a cada 10 ms e aplica o filtro de 50 ms; imagem do OloggerMulti compila e inclui o módulo |
| 03/11/2026 | 0.1.6 | Decisão de retirar a D1 das entradas digitais; bancada limitada a 12 V até o resistor passar para 0603 ou maior |
| 03/11/2026 | 0.1.5 | FW-052: código de leitura pronto e compilado; polaridade do pino (ativo em nível baixo) confirmada pela netlist; alerta HW-008 sobre a proteção D1 com 12 a 24 V |
| 03/11/2026 | 0.1.4 | Escopo da frequência reduzido a 10 a 20 kHz (saída de pulsos de inversor), com a análise das consequências para o FW-054 |
| 03/11/2026 | 0.1.3 | Registrados os limites de amostragem informados pelo responsável técnico (não verificados) e a restrição do optoacoplador TLP291 como limite real da entrada |
| 03/11/2026 | 0.1.2 | Tempo padrão do filtro em 50 ms, com conversão para amostras; builds STM32 dos dois devices verificados; limite de frequência do pino pendente no datasheet |
| 03/11/2026 | 0.1.1 | FW-053 concluído no PC: filtro de ruído em `lib/io`, com testes |
| 03/11/2026 | 0.1.0 | Criada a sprint com FW-052 a FW-056 |
