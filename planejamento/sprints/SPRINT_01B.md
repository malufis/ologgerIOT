# SPRINT_01B — Entrada digital

> **Criado em:** 03/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.1.13
> **Período:** 03/11/2026 a 06/11/2026 (paralela à SPRINT_01, bloqueada pela ferrite L2 não soldada)

## Meta

Entrada digital de sinal ativo em nível alto, com filtro de ruído, modo discreto (0 ou 1) ou de frequência em Hz, e despertar do STM32 do sleep. Fica fora desta sprint a integração com Modbus e telemetria, que entra nas sprints 03 e 05.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-052 | Biblioteca `lib/io`: leitura do nível das entradas D1 a D4 | Código pronto em 03/11/2026 (`io_entrada`), compilado nos dois builds sem avisos; ativo = pino em nível baixo. A tarefa `io_tarefa` (leitura a cada 10 ms, filtro de 50 ms) entra na imagem do OloggerMulti (21 556 bytes de código); `estado_bruto` e `estado_filtrado` são lidos por SWD. Falta a leitura em bancada com tensão aplicada. D1 será retirada da placa (HW-008); na bancada usar até 12 V, porque o 4K7 em 0402 não suporta 24 V |
| FW-053 | `io_filtro`: filtro de ruído por número de amostras, com conversão de tempo (padrão de 50 ms) | Concluído no PC em 03/11/2026: 13 casos e 131 verificações em `tests/io`, sem falhas. Build STM32 dos dois devices compila sem avisos (ver histórico) |
| FW-054 | Modo de frequência em Hz por contagem de bordas | Pendente |
| FW-055 | Contagem de pulsos dormindo pela interrupção externa (PB5 a PB8) | Código pronto em 03/11/2026 (`io_entrada_acordar_habilita`, `io_entrada_irq`, `io_entrada_coleta_pulsos`; tratador `EXTI9_5_IRQHandler` em `interrupcoes.c`, que também pede para o STM32 acordar); compila nos dois builds. **Incompleto:** teste em bancada (roteiro, testes 9 e 10) |
| FW-056 | Roteiro e execução de bancada da entrada digital | Roteiro escrito em 03/11/2026 (`documentacao/roteiro_bancada_entrada_digital.md`, 12 testes). Execução pendente: precisa do device |
| FW-057 | `io_pulsos`: contador de pulsos e inatividade de 60 s | Concluído no PC em 03/11/2026: 15 verificações. Integrado à `io_tarefa` (`io_tarefa_pulsos`, `io_tarefa_pode_dormir`, `io_tarefa_trata_despertar`); os dois devices compilam sem avisos |
| FW-058 | Entrada em Stop 2 pelo contador de pulsos | Código pronto em 03/11/2026 (`lib/energia`): dorme quando `io_tarefa_pode_dormir`, para a aquisição analógica ao dormir e acorda pelo pulso; compila no `ologgermulti`. **Desabilitado por padrão** (`energia_define_habilitada`) e **não testado em hardware**. O despertar para transmitir (1, 2 ou 5 vezes ao dia, ou por entrada analógica) é de outra sprint e entra por `energia_solicita_acordar` |
| FW-059 | Contagem de pulsos em hardware (LPTIM1, entrada D4) durante o sleep, sem acordar | A confirmar: com quatro leitores, só um conta em hardware; depende do RM0394. Menos prioritário depois da decisão de contar no tratador |
| FW-060 | Totalizador (hodômetro) por entrada, guardado nos registradores de backup | Concluído no PC em 03/11/2026: 36 verificações, incluindo queda de energia em cada gravação; integrado à `io_tarefa` e ao tratador da interrupção; compila no `ologgermulti`. Acesso ao TAMP **não testado em hardware** |

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para a sprint seguinte.
- Testes no PC passando (`tests/`) e build STM32 da composição `ologgermulti` compilado.
- `documentacao/README.md`, `documentacao/configuracao_do_dispositivo.md`, `documentacao/dicionario_funcoes.md` e `BACKLOG.md` atualizados.

## Pendências para o responsável técnico

- Período de leitura da entrada digital: define quantas amostras cabem nos 50 ms do filtro padrão (afeta FW-052).
- Faixa de frequência máxima da entrada em Hz e janela de medição (afeta FW-054).

## Uso como contador de pulsos (03/11/2026)

O responsável técnico vai usar o device como contador de pulsos, para economizar energia: acorda, conta o pulso e, se ficar 1 minuto sem pulso, dorme. Os outros meios de acordar o módulo ficam para outra sprint. Como isso foi implementado e o que depende de decisão:

- **Modo sleep: o pulso acorda o STM32 e ele fica acordado (corrigido em 03/11/2026).** Dormindo, só a entrada digital fica ativa e o STM32 está em Stop 2. O pulso gera uma interrupção na borda que ativa a entrada (descida do pino, por causa do optoacoplador). O tratador soma o pulso no contador e no totalizador (já gravado, sem depender da RAM) e pede para o STM32 acordar. **Acordado, ele fica acordado** contando pelos filtros e só dorme de novo depois do tempo de inatividade sem pulso (60 s padrão, configurável). Com pulso constante ele não acorda e dorme a cada pulso. Não há filtro na interrupção, para o pulso que acorda não se perder. Contra ruído elétrico numa entrada solta: cada borda espúria acorda o STM32 e gasta energia.
- **Ao acordar:** `io_tarefa_retoma` desliga a interrupção externa (para não contar duas vezes), soma os pulsos contados dormindo e ressincroniza os filtros com o estado lido naquele momento, para um pulso ainda ativo não ser contado de novo pela leitura de 2 ms. O pulso que acordou já conta como atividade e reinicia o tempo de inatividade.
- **Acordado:** a leitura a cada 2 ms conta os pulsos com o filtro de 20 ms. A interrupção externa fica desligada enquanto acordado (`io_entrada_acordar_desabilita`), para não contar duas vezes.
- **Taxa de 5 a 10 pulsos por segundo (decidido em 03/11/2026).** Pulsos de leitores, sem repique. Com pulso constante o STM32 permanece acordado, então o consumo é o do modo ativo, não o de acordar e dormir sem parar. O consumo em Stop 2 e em modo ativo não foi medido nem estimado: **medir na bancada** (FW-056, teste 8).
- **Filtro dos pulsos: 20 ms (decidido em 03/11/2026).** A contagem usa um filtro próprio de 20 ms, separado do filtro de 50 ms do estado discreto. A leitura passou a ser a cada 2 ms (10 amostras por janela). Um pulso só é aceito se durar mais que 20 ms mais uma amostra; pulso ou intervalo menor é descartado. Confirmar que o medidor gera pulsos mais largos que isso, com folga.
- **Tempo de inatividade configurável, padrão de 60 s (decidido em 03/11/2026).** `io_tarefa_define_inatividade` altera o valor para todas as entradas. A gravação do valor entre reinícios e o comando para alterá-lo ficam para o módulo de configuração.
- **Despertar para transmitir (03/11/2026).** O módulo acorda para transmitir 1, 2 ou 5 vezes ao dia, ou por uma entrada analógica. É outra sprint e depende do RTC e do módulo de sleep.
- **Contar dormindo, sem perder dado (03/11/2026).** Contar sem acordar é preferível, mas nenhum pulso pode ser perdido antes da transmissão. Verificado no banco de dados do STM32CubeMX (`STM32L412C8Ux.xml`): D4 (PB5) tem `LPTIM1_IN1`, D1 (PB7) tem `LPTIM1_IN2`, D3 (PB6) tem `LPTIM1_ETR` e D2 (PB8) não tem sinal do LPTIM. Pelo que conheço do RM0394, a contagem de pulsos externos no LPTIM1 usa a entrada IN1, ou seja, só a D4 conta em hardware dormindo (a confirmar no RM0394). O LPTIM2 usa PB1, que já é a entrada 4–20 mA 4. As outras entradas contam acordando a cada pulso, como já está implementado. Como os pulsos são de leitores, sem repique, o filtro curto do LPTIM (poucos ciclos do clock de 32 768 Hz) não é problema, e a contagem em hardware na D4 é viável: zero despertares para contar.
- **Não perder a contagem num reinício.** A contagem está na RAM: o Stop 2 a preserva, mas um reset ou queda de energia a perde. Para não perder dado, a contagem precisa ser guardada em registrador de backup ou na flash e só descartada após a confirmação do servidor, como já vale para a telemetria. A definir.

## Decisões de 03/11/2026: quatro leitores, Stop 2 e totalizador

- **Até quatro leitores ao mesmo tempo.** As quatro entradas contam pulsos. A taxa é de 5 a 10 pulsos por segundo por leitor, então até 40 despertares por segundo. As quatro linhas (PB5 a PB8) dividem o vetor `EXTI9_5`, e o tratador atende as quatro. Só a D4 poderia contar em hardware (FW-059).
- **Stop 2 liberado.** `lib/energia` cria a tarefa de gerência de energia. Ela só dorme se estiver habilitada e se `io_tarefa_pode_dormir()` (60 s padrão sem pulso em todas as entradas). Ao dormir, ela para a aquisição analógica (`aquisicao_para`) e liga a contagem pela interrupção externa. Sai do sleep por `energia_solicita_acordar()`: o pulso (tratador `EXTI9_5_IRQHandler`) ou, na sprint do despertar para transmitir, o alarme do RTC. O clock fica no MSI, que o Stop 2 preserva. **O ESP32 só é acordado para transmitir**; nada disto o toca.
- **Totalizador estilo hodômetro.** Cada entrada tem um total de 32 bits que só soma e volta a zero ao estourar. Quem o lê calcula a diferença entre duas leituras, então uma transmissão perdida não perde contagem. `io_tarefa_total(canal)` lê e `io_tarefa_define_total(canal, valor)` iguala ao medidor na instalação. Fica nos registradores de backup do TAMP, em duas cópias com complemento por entrada, e o tratador da interrupção grava a cada pulso. Uma queda de energia no meio da gravação deixa o total antigo ou o novo, nunca perdido (testado no PC).

Limitações e riscos conhecidos, a verificar na bancada:

- **Nada disto foi testado em hardware.** Entrar em Stop 2, o acesso ao TAMP (clock `RTCAPBEN` e `DBP`) e a contagem no tratador só compilam.
- **Bateria no VBAT.** Os registradores de backup só sobrevivem à falta de energia se o pino VBAT tiver bateria. Confirmar na placa.
- **Primeira partida.** Com os registradores zerados, `io_totalizador_corrompido` indica verdadeiro e o total recomeça em zero. Na instalação, usar `io_tarefa_define_total` para igualar ao medidor.
- **Depurador.** Em Stop 2 a conexão SWD cai, a menos que o depurador mantenha o clock de depuração. Habilitar o sleep na bancada exige essa configuração, ou o uso de `energia_solicita_acordar` por um pulso.
- **Aquisição analógica.** Ao dormir, `aquisicao_para` para o timer, o DMA, o ADC e a referência interna; ao acordar, `aquisicao_retoma` descarta o segundo parcial e as leituras antigas, e a leitura volta a ficar disponível ao fechar o primeiro segundo. Dormindo não há leitura analógica; acordar por uma entrada analógica fica para outra sprint.
- **Tempo.** O tick do FreeRTOS para no sleep, e os tempos medidos por ele (como o de inatividade) não contam o tempo dormido. A hora certa virá do RTC.
- **Pulsos sem filtro dormindo.** Ruído elétrico em entrada solta gera interrupções falsas e acorda o STM32 sem necessidade. Entradas não usadas devem ficar com o sinal definido.
- **Potência do 4K7 e a D1.** Valem os alertas do HW-008.

## Pendência técnica: limite de frequência do pino

Objetivo: saber a maior frequência que a entrada digital do STM32L412 consegue receber em modo de frequência. Não foi possível confirmar no datasheet nesta data. O PDF oficial (`stm32l412c8.pdf`, DS12469 Rev 9) não abriu no ambiente, por tempo esgotado, e a busca não trouxe as tabelas de timers e de características elétricas. Referências a conferir: DS12469 (seção de timers e de características elétricas dos GPIOs) e RM0394 (timers TI e modo de clock externo). Regra de referência, sem a tabela do datasheet: AN4776 indica `fTIMxCLK ≥ 3 × f(entrada)` no modo TI. O núcleo é de até 80 MHz. Resultado: limite provisório da ordem de 26 MHz no modo TI, a confirmar.

Informação recebida do responsável técnico em 03/11/2026, de fonte externa e **não verificada em datasheet**: amostragem do registrador de entrada por timer e DMA chega a cerca de 20 a 26 MHz no STM32L4 (80 MHz) e a cerca de 8 a 10 MHz no STM32L0 (32 MHz); leitura por polling chega a cerca de 16 a 20 MHz no L4 e 6 a 8 MHz no L0. Esses números descrevem amostragem de nível, não contagem de bordas, e coincidem em ordem de grandeza com o limite do timer acima.

**O limite do pino não é o limite da placa.** No OloggerMulti a entrada passa pelo optoacoplador TLP291-4 (`entradas_digitais.kicad_sch`), com resistores de 4K7 e 10k. Os tempos de subida e descida do optoacoplador com essa carga tendem a limitar a frequência útil a poucas dezenas de kHz, muito abaixo do pino. Isto é inferência a confirmar no datasheet do TLP291 e por medição em bancada (FW-056). O y7080 não tem entrada digital na composição, então o valor do STM32L0 não se aplica. Não usar nenhum desses valores no código antes da confirmação.

**Escopo da frequência (03/11/2026):** o responsável técnico informou que não haverá frequência alta e que o ajuste fica para depois. Referência recebida, de fonte externa e não verificada: a saída de pulsos dos inversores WEG gera no máximo 10 kHz ou 20 kHz, ajustável por parâmetro para corresponder a 100% da velocidade. Consequências para o FW-054, a confirmar:

- A faixa de projeto é de 0 a 20 kHz, muito abaixo do limite do pino. A medição por contagem de bordas em timer atende com folga; interrupção por borda em cada pulso a 20 kHz custaria dezenas de milhares de interrupções por segundo e deve ser evitada.
- A frequência em 100% da velocidade (10 ou 20 kHz) é configuração por entrada, para converter Hz em percentual de velocidade.
- O filtro de 50 ms vale só para o modo discreto. No modo de frequência ele eliminaria o sinal (o período a 20 kHz é de 50 µs). Esse modo precisa de filtro próprio, de largura mínima de pulso, a definir.
- O limite real a 20 kHz é o optoacoplador TLP291 com a carga de 10k, a validar em bancada. A tensão de campo e a faixa do sinal do inversor continuam em aberto.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.1.0 | Criada a sprint com FW-052 a FW-056 |
| 03/11/2026 | 0.1.1 | FW-053 concluído no PC: filtro de ruído em `lib/io`, com testes |
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
