# Análise dos hardwares OloggerMulti e y7080

> **Criado em:** 29/10/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.3.7
> **Estado:** parte elétrica resolvida conforme confirmação de Rodrigo Maluf; análise dos arquivos preservada como histórico documental.

## Resultado

Os arquivos descrevem dois modelos diferentes da família oLoggerIOT, coerentes com a premissa de STM32 como controlador central e módulos de comunicação específicos por modelo. Já permitem levantar os recursos e um mapa preliminar de pinos para o firmware em C.

Rodrigo Maluf confirmou que **a parte elétrica já está toda resolvida**. Os achados HW-001 a HW-004 abaixo ficam encerrados como bloqueios elétricos por essa confirmação. As evidências descrevem os arquivos recebidos, não uma nova verificação da solução implementada. Detalhes finais de MCU, pinos e interface USB serão incorporados quando necessários à implementação do firmware; nenhuma alteração física específica foi presumida.

## Fontes e método

Foram lidos os dois ZIPs em `nogit/gerbes/` e todas as 19 folhas `.kicad_sch`: 10 do OloggerMulti e 9 do y7080. A folha `y7080/acessorios.kicad_sch` está vazia. As folhas fornecidas não contêm uma hierarquia que as reúna; as conexões entre elas foram inferidas pela igualdade dos rótulos globais. Referências de componentes foram cruzadas dentro de cada modelo.

O KiCad CLI 10.0.5 exportou 19 netlists XML. Foram comparados os nomes de rede associados aos pads nos atributos X2 dos Gerbers de cobre com as netlists. Os ZIPs passaram na verificação de integridade e contêm as quatro camadas de cobre, máscaras, serigrafias, pasta, contorno e arquivos de furação. Inventário, hashes SHA-256, conexões e divergências estão em [evidencias.json](../nogit/analise_hardware/evidencias.json).

Não foram fornecidos `.kicad_pro` nem `.kicad_pcb`. A comparação de atributos não verifica continuidade física das trilhas, curtos geométricos, planos, impedância RF ou DRC do PCB. Também não confirma quais componentes foram efetivamente montados. Os arquivos originais foram preservados.

## Comparação dos modelos

| Recurso | OloggerMulti | y7080 |
|---|---|---|
| MCU indicado | U1 STM32L412C8Ux | U2 STM32L071CBUx |
| Núcleo e frequência máxima do componente | Cortex-M4F, 80 MHz [F1] | Cortex-M0+, 32 MHz [F2] |
| Flash interna do componente indicado | 64 KB [F1] | 128 KB [F2] |
| SRAM | 40 KB [F3] | 20 KB [F4] |
| Memória externa | IC1 GD25Q128ESIG, SPI | IC5 GD25Q128ESIG, SPI |
| Capacidade externa nominal | 128 Mbit = 16 MiB [F5] | 128 Mbit = 16 MiB [F5] |
| Comunicação embarcada | U9 ESP32-C6-MINI-1 | U1 com valor `SIMY7080` e footprint `XCVR_SIM7080G` |
| Entradas 4–20 mA | 4 canais | 1 canal |
| Entradas digitais dedicadas | 4 canais com TLP291-4GB | Uma linha `INTERRUPT` externa identificada; sem o bloco de quatro entradas do Multi |
| Saídas dedicadas | 4 estágios com BC847 | Controles do circuito diferencial; sem o bloco de quatro saídas do Multi |
| Sensor diferencial | Não identificado | INA350, REF3212, TLV9002, TPS61040 e seleções PC11/K8 |
| RS-485 | SP3485CN | SP3485CN |
| USB físico | Micro-USB e proteção TPD2EUSB30 | Micro-USB e proteção TPD2EUSB30 |
| USB nativo do MCU | Disponível [F3] | Não disponível no STM32L071 indicado [F4] |
| Armazenamento adicional | Conector de cartão ligado ao ESP32 por SPI | Não identificado |
| Componentes nas netlists, sem símbolos de alimentação | 130 | 164 |
| Camadas / espessura declaradas | 4 / 1,6 mm | 4 / 1,6 mm |
| Dimensão no arquivo `.gbrjob` | 68,55 × 64,05 mm | 65,963 × 40,05 mm |
| Contorno pelas coordenadas de Edge Cuts | 68,50 × 64,00 mm | 65,913 × 40,00 mm |
| Data declarada de geração dos Gerbers | 20/09/2026 | 14/10/2026 |

A diferença de 0,05 mm entre as dimensões do job e do contorno coincide com a largura do traço de contorno. As dimensões acima distinguem a caixa declarada no job das coordenadas do perfil.

Os 16 MB discutidos na entrevista correspondem, nos dois esquemáticos, a uma **flash NOR externa ligada ao STM32 por SPI**. Essa capacidade é armazenamento persistente; não é a RAM disponível para executar firmware ou manter buffers. A memória é de 16.777.216 bytes pela conversão de 128 Mbit. [F5]

## Mapa preliminar do OloggerMulti

Fonte: `OloggerMulti.kicad_sch` cruzado com as demais folhas do modelo. Configuração de funções alternativas deve ser validada na implementação. A pinagem de sinais abaixo é a observada nos arquivos.

| Função / rótulo | Pino STM32 | Observação |
|---|---|---|
| `MCU-4A20-01` | PA1 | Primeiro canal analógico |
| `MCU-4A20-02` | PA0 | Segundo canal analógico |
| `MCU-4A20-03` / `04` | PB0 / PB1 | Demais canais analógicos |
| `MCU-D1` / `D2` | PB7 / PB8 | Entradas digitais |
| `MCU-D3` / `D4` | PB6 / PB5 | Entradas digitais |
| `MCU-DOUT1` a `4` | PB12 a PB15 | Saídas via BC847 |
| `UART2_TX` / `RX` | PA2 / PA3 | SP3485, DI / RO |
| `485_EN` | PB9 | DE e /RE ligados à mesma rede |
| `UART3_TX` / `RX` | PB10 / PB11 | Ligados ao ESP32, respectivamente GPIO13 / GPIO12 |
| `UART1_TX` / `RX` | PA9 / PA10 | Serial externa |
| `USART1_RTS` / `CTS` | PB3 / PB4 | Conector externo |
| `SPI1_CS` / `SCK` / `MISO` / `MOSI` | PA4 / PA5 / PA6 / PA7 | Flash externa |
| `FLASH-IO` | PC13 | Jumper da flash; finalidade detalhada a validar |
| `USB-` / `USB+` | PA11 / PA12 | D− / D+ via resistores e proteção |
| `SWDIO` / `SWCLK` | PA13 / PA14 | Programação e depuração |

Na folha `esp32.kicad_sch`, UART3 do STM32 chega aos GPIO12/13 do C6, enquanto UART0 do ESP32 (`ESP-RX`/`ESP-TX`) vai ao conector externo. Portanto, esses dois caminhos devem ser distinguidos no firmware e na programação inicial. GPIO12/13 são usados pelo USB Serial/JTAG do C6 por padrão; o uso como UART exige configurar o roteamento e não depender simultaneamente dessa função USB nesses pinos. [F6]

O ESP32 também tem GPIO5 ligado a `RESET_STM32`, conector de cartão e sinais de reset/boot próprios. Isso não altera a decisão de STM32 como orquestrador: o uso dessas conexões deve ser explicitado e controlado.

As entradas 4–20 mA têm resistores de 150 Ω e buffers GS8552. Idealmente, 4 mA produzem 0,6 V e 20 mA produzem 3,0 V no shunt, antes das tolerâncias e dos efeitos do circuito. A conversão final exige calibração e definição de unidades. As saídas BC847 são estágios de transistor, não relés. Correntes e cargas admissíveis dependem da revisão elétrica. A presença de optoacopladores nas entradas não comprova isolamento galvânico do conjunto; há ligações de terras por resistores de 0 Ω que precisam ser consideradas.

## Mapa preliminar do y7080

Fonte: `stm32.kicad_sch` cruzado com as demais folhas do modelo.

| Função / rótulo | Pino STM32 | Observação |
|---|---|---|
| `MCU-4A20-01` | PA0 | Um canal 4–20 mA, shunt de 150 Ω e GS8552 |
| `SENSE` | PB1 | Saída analógica do INA350 |
| `MONITOR VCC` | PB0 | Divisor de 100 kΩ / 10 kΩ vindo de +12V |
| `SENSOR-PWR-EN` | PB13 | Controle do front-end analógico |
| `SENSOR-EN-K8` | PB15 | Controle de excitação K8 |
| `SENSOR-EN-PC11` | PA15 | Controle de excitação PC11; o nome do sensor não indica o pino PC11 do MCU |
| `STM32-NBIOT-TX` / `RX` | PB10 / PB11 | Caminho AT do modem; LPUART1 nesses pinos [F4] |
| `NBIOT-PWRKEY` | PC13 via R22 de 1 kΩ | Validar nível, modo de saída e temporização com o manual do módulo real |
| `UART2_TX` / `RX` | PA2 / PA3 | SP3485, DI / RO |
| `485_EN` | PB9 | DE e /RE na mesma rede |
| `UART1_TX` / `RX` | PA9 / PA10 | Serial externa nos conectores |
| `USART1_RTS` / `CTS` | PB3 / PB4 | Conector externo |
| `SPI1_CS` / `SCK` / `MISO` / `MOSI` | PA4 / PA5 / PA6 / PA7 | Flash externa |
| `FLASH-IO` | PB14 | Jumper da flash; difere de PC13 no Multi |
| `INTERRUPT` | PB12 | Linha externa, com R23 marcado NM |
| `USB-` / `USB+` | PA11 / PA12 | Rótulos presentes, mas MCU indicado sem periférico USB |
| `SWDIO` / `SWCLK` | PA13 / PA14 | Programação e depuração |

O circuito diferencial tem referência rotulada `REF_1V25`, polarização `INA_VREF`, alimentação comutada, boost e seleções por jumpers COMA/COMB. O firmware precisará de sequenciamento e tempo de estabilização antes de ler o ADC. Ganhos, escalas, tipo de sensor e lógica dos jumpers ainda devem ser definidos a partir da montagem e dos sensores reais.

O nome `SIMY7080` e o footprint SIM7080G não determinam sozinhos o módulo efetivamente montado. A SIMCom publica documentação de compatibilidade entre SIM7080G e Y7080E; isso reforça a necessidade de confirmar o código comercial antes de escolher o manual AT. [F7]

## Achados prioritários

**Atualização de estado:** os itens elétricos HW-001 a HW-004 foram considerados resolvidos pelo responsável técnico. As descrições seguintes preservam a análise anterior dos arquivos e não são solicitações atuais de revisão elétrica.

### HW-001 — USB do y7080

**Evidência:** o esquemático liga o Micro-USB a PA11/PA12 de U2, identificado como STM32L071CBUx. O datasheet desse componente não oferece periférico USB. [F4]

**Impacto:** a presença do conector não permite implementar o gateway celular por USB CDC nativo nesse MCU. Resolver a escolha do componente ou do caminho USB antes de incluir essa funcionalidade no firmware desse modelo. Alternativas para avaliação: revisão com MCU adequado, ponte USB–UART ou adaptador externo ligado à UART disponível. Nenhuma alternativa foi escolhida nesta análise.

### HW-002 — Alimentação e VBUS no OloggerMulti

**Evidência:** nos Gerbers, o pad 1 do conector USB J1, VDD/VDDA/VDDUSB do STM32, alimentação do ESP32, flash e saída do regulador aparecem sob a mesma rede `VBUS`. Nas folhas separadas, essa alimentação recebe nomes diferentes: `5V`, `+3.3V`, `3.3V_A` e `VBUS`.

**Impacto:** há indicação de compartilhamento indevido entre VBUS USB e a alimentação lógica, além de divergência entre documentos. Validar no PCB e na placa se os 5 V do USB chegam diretamente à alimentação lógica ou à saída do regulador. O STM32L412 especifica alimentação até 3,6 V e o ESP32-C6-MINI-1 opera em 3,0–3,6 V. [F3, F8] A classificação é de risco a confirmar por geometria/continuidade, não uma medição de tensão já realizada.

### HW-003 — Boot do ESP32 diferente entre esquemático e fabricação

**Evidência:** na netlist de `esp32.kicad_sch`, `ESP-IO0` está no pad 23, GPIO9, e o pad 12, GPIO0, está sem conexão. Nos Gerbers ocorre o inverso: `ESP-IO0` no pad 12 e pad 23 sem conexão.

**Impacto:** o botão/jumper de boot do arquivo fabricável não corresponde ao pino usado para entrada em modo de download do C6. GPIO8/GPIO9 participam da seleção de boot. [F8] Confirmar a revisão montada e corrigir a documentação/PCB aplicável antes de depender do botão na gravação inicial.

### HW-004 — EN do regulador do OloggerMulti

**Evidência:** na folha `alimentação.kicad_sch`, U7.5/EN aparece sem conexão. Nos Gerbers, U7.5 e U7.3/VIN compartilham a rede `Net-(U7-EN)`.

**Impacto:** as fontes não representam a mesma conexão de habilitação do regulador. Reconciliar a revisão antes da inicialização da placa; essa diferença não é uma correção de firmware.

### HW-005 — Integração exata do modem e medição diferencial

Confirmar o módulo celular montado, os sensores PC11/K8, a seleção dos jumpers e os níveis elétricos do caminho AT/PWRKEY. Há estágios de adaptação na UART. O firmware deve seguir o módulo real e a polaridade do circuito, sem presumir que todos os rádios tenham comandos ou tempos iguais.

### HW-006 — BOOT0 do OloggerMulti sem ligação

**Evidência:** na netlist do OloggerMulti, o pino 44 do U1 (STM32L412C8, função PH3/BOOT0) está em `unconnected-(U1-PH3-Pad44)`, e o ERC já o marcava. Não há pull-down, jumper nem resistor. No y7080, o pino 44 do U2 (STM32L071CB) está na rede `BOOT0`, ligada ao R16 de 100 kΩ.

**Confirmação em hardware (02/11/2026):** o L412 gravado por SWD iniciou na memória de sistema (PC em `0x1FFF....`), e os bits de opção estão no padrão (`nSWBOOT0 = 1`, BOOT0 lido do pino). O pino flutuando foi lido em nível alto. O responsável técnico confirmou que a placa do L4 não tem BOOT0 ligado.

**Impacto:** o nível de um pino flutuando é indefinido e pode variar com ruído e temperatura. Cada unidade de fábrica pode iniciar pelo bootloader da ST, e não pelo firmware.

**Aplicado em 02/11/2026:** na unidade de bancada, `nSWBOOT0 = 0` e `nBOOT0 = 1` foram gravados por SWD (`STM32_Programmer_CLI -c port=SWD mode=UR -ob nSWBOOT0=0 nBOOT0=1`) e conferidos na leitura de volta, com RDP no nível 0. Depois do reset, o PC ficou em `0x0800....`, e o SysTick ficou ativo e contando: o firmware executa.

**Caminhos:**
1. Corrigir a revisão da placa, com pull-down no pino 44. Resolve a causa, mas exige retrabalho.
2. Gravar `nSWBOOT0 = 0` e `nBOOT0 = 1` nos bits de opção. O chip passa a ignorar o pino e a iniciar sempre pela flash, e o PH3 fica livre. Funciona nas placas atuais, mas **precisa entrar no procedimento de gravação de toda unidade**, porque o padrão de fábrica do chip depende do pino.

### HW-007 — Escala do ADC do OloggerMulti ~1,37 acima do esperado

**Evidência (02/11/2026, bancada):**
- Entrada 4 com 4 mA, 8 mA e 20 mA, confirmados com multímetro em série e tensão no shunt (0,599 V, 1,200 V e 2,999 V). O seguidor de tensão tem ganho 1 e o pino do MCU mede a mesma tensão do shunt.
- O ADC lê 1036, 2077 e 4095 contagens (saturado). As leituras são proporcionais, sem deslocamento, o que dá uma referência efetiva de **~2,37 V**, enquanto a alimentação medida no C9 é 3,25 V.
- A referência interna VREFINT dá 2085–2092 contagens (esperado ~1527), o que implica VDDA de ~2,37 V pela calibração de fábrica (1652 a 3,0 V).
- Descartados: configuração do ADC (12 bits, alinhamento à direita, sem sobreamostragem, sem modo diferencial, calibrado), VREFBUF (não responde no L412), shunt, seguidor e o próprio driver (as conversões simples por software, sem timer e sem DMA, dão o mesmo resultado).

**Impacto:** o ADC satura a ~2,37 V no pino, ou seja, em ~15,8 mA. A faixa de 4 a 20 mA não pode ser medida por inteiro. A calibração por dois pontos (referência efetiva de 2,367 V) corrige a escala até esse limite, mas não resolve a saturação.

**Causa identificada (02/11/2026):** a ferrite **L2 (BLM18HE152SN1D)**, entre a rede +3,3 V e a alimentação analógica 3,3V_A, **não está soldada** na placa de bancada. Sem ela, o VDDA do chip cai para ~2,37 V, e o ADC converte com essa escala. Medidas: C9 em 3,25 V e VDDA do chip em 2,37 V. Correção: soldar L2 e conferir a resistência CC (quase 0 Ω) e a tensão do VDDA (3,25 V).

**Consequência para o firmware:** a calibração de 2,367 V em `aquisicao.h` é um remendo de bancada. Depois da solda, a referência volta para 3,25 V e a calibração deve ser refeita e revalidada, incluindo 20 mA.

### HW-008 — Entradas digitais com 12 a 24 V: potência do resistor de entrada

**Definição (03/11/2026):** a tensão de campo das entradas digitais é a mesma da entrada do CLP, de uma fonte de 12 a 24 V. Fonte: responsável técnico.

**Evidência (`entradas_digitais.kicad_sch`):** quatro canais com o optoacoplador TLP291-4GB (U2), resistores de 4K7 (R3 a R6) e de 10k (R7 a R10), todos no encapsulamento 0402, e a matriz de proteção ESDA6V1SC5 (D1). A netlist (`nogit/analise_hardware/OloggerMulti/entradas_digitais.xml`) confirma a topologia: `IN-Dx` → 4K7 → ânodo do LED do TLP291, cátodo no terra de campo (`Earth-GND`); no lado do STM32, o coletor do fototransistor está em `MCU-Dx` com o 10k para +3,3 V e o emissor no `GNDPWR`. Com tensão no campo o LED acende, o fototransistor conduz e o **pino do STM32 vai a nível baixo**. O firmware trata ativo como nível baixo (`PINO_ENTRADA_DIGITAL_NIVEL_ATIVO` em `devices/ologgermulti/pinos.h`). A matriz D1 (ESDA6V1SC5) liga cada `IN-Dx` ao terra de campo, antes do 4K7.

**Cálculo com essa topologia** (queda do LED de ~1,2 V, corrente `(V − 1,2) / 4,7 kΩ`):

| Tensão de campo | Corrente no LED | Potência no 4K7 |
|---|---|---|
| 12 V | ~2,3 mA | ~25 mW |
| 24 V | ~4,9 mA | ~110 mW |
| 30 V (fonte alta) | ~6,1 mA | ~176 mW |

- **Nível ativo:** a corrente do LED em 12 V (~2,3 mA) satura o fototransistor com folga, pois a pull-up de 10k só pede ~0,33 mA. Estimativa, a medir.
- **Potência:** um resistor 0402 costuma ser de 62,5 mW ou 100 mW. O consumo estimado em 24 V (~110 mW) e em 30 V (~176 mW) fica acima disso. É o ponto crítico. Conferir a potência do componente montado.
- **Proteção D1 em conflito com 12 a 24 V:** a D1 está ligada diretamente entre `IN-Dx` e o terra, sem resistência antes dela. O ESDA6V1SC5 é um supressor de 6,1 V de ruptura mínima, conforme a [página do produto na ST](https://www.st.com/ja/protections-and-emi-filters/esda6v1sc5.html) (confirmado em 03/11/2026; a tensão de operação reversa de ~5,25 V vem de listas de distribuidores, a conferir no PDF da ST), então conduz a partir de cerca de 6 V. Com 12 a 24 V no campo ele tenta grampear a fonte em ~6 V, com a corrente limitada só pela fonte, e o 4K7 não protege a D1 porque está depois dela. Resultado esperado: a entrada não opera de forma confiável acima de ~5 V e a D1 pode aquecer ou falhar. É o ponto mais grave deste achado. Confirmar em bancada, com fonte limitada em corrente, antes de aplicar 24 V.
- **Inversão de polaridade:** não há diodo de proteção visível em série com o LED. A tensão reversa máxima do LED é da ordem de 5 V. A conferir.
- **Velocidade:** com corrente menor no LED (12 V) o desligamento do fototransistor é mais lento. O pior caso para a frequência é 12 V.

**Posição do responsável técnico (03/11/2026):** o circuito do optoacoplador está correto; o mesmo 4K7 já é usado em outro projeto, a 12 V, e o resistor passará a 0603 no próximo layout. Análise: o resistor em série com o LED resolve a corrente do LED, mas **não protege a D1**, que está no nó `IN-Dx` antes dele. Em 12 V a potência no 4K7 é ~25 mW e já cabe em 0402; a mudança para 0603 só importa perto de 24 V. Um 0603 comum é de 100 mW (existem de 125 mW), e o consumo estimado em 24 V é ~110 mW e em 30 V ~176 mW. Para 24 V: usar 0805 ou 1206 de 0,25 W, ou subir o resistor para ~10k (~52 mW em 24 V, ~1,1 mA no LED em 12 V; margem do ganho de corrente a conferir). Sobre a D1: no outro projeto, conferir se o componente é o mesmo; se for outro supressor (por exemplo, de 24 V ou bidirecional de tensão maior), a questão não se aplica aqui.

**Executado em 03/11/2026:** a D1 (ESD) foi retirada das entradas digitais da placa de bancada, por decisão do responsável técnico, e será reaproveitada em outro projeto. Com isso o conflito de ~6 V some. Consequências: a entrada fica sem supressor de surto e ESD no terminal de campo; só o 4K7 e o optoacoplador limitam a energia. **O limite de entrada na bancada é 12 V**, com o resistor de 4K7 em 0402. No próximo layout o resistor passa a 0603, o que ainda não cobre 24 V (~110 mW contra 100 mW de um 0603 comum); para 24 V, usar um resistor de 0805 ou 1206 de 0,25 W, ou subir o valor para ~10k. Se a proteção for necessária em campo industrial, usar um supressor com tensão de operação acima de 24 V (por exemplo, da família SMAJ ou SM5S de ~30 V) no nó `IN-Dx`.

**Pendência:** confirmar a proteção contra inversão de polaridade, a potência do resistor montado e a proteção contra inversão. A parte elétrica foi declarada resolvida pelo responsável técnico; este achado é um alerta de verificação, não uma solicitação de alteração.

### HW-009 — Saídas digitais sem pull-down na base do transistor

**Evidência (netlist `extras.xml`, 03/11/2026):** cada uma das quatro saídas é um NPN BC847 (Q3 a Q6) em coletor aberto. O pino do STM32 (PB12 a PB15) passa por um resistor de 1k (R45 a R48, 0805) até a base. O emissor está no terra `GNDD` e o coletor vai ao conector J12 (`OUT-1` a `OUT-4`). Não há resistor entre base e emissor, nem diodo de roda livre. Pino em nível alto liga o transistor, que puxa `OUT-n` ao terra.

**Impacto:** durante o reset, no bootloader da ST ou antes de o firmware configurar o pino, o pino flutua e o estado do transistor não é garantido. O requisito é manter as saídas desligadas durante atualização e reinício (`atualizacao_firmware.md`). O firmware leva o pino ao nível desligado logo depois do `HAL_Init`, antes de configurá-lo como saída, mas isso não cobre o intervalo de reset e boot.

**Recomendação (anotada em 03/11/2026 pelo responsável técnico para a próxima revisão, em `melhorias_hardware.md`):** pull-down de ~10 kΩ entre a base e o emissor, ou do pino ao terra, no próximo layout. Medir em bancada o estado de `OUT-n` durante o reset. A carga indutiva (relé, contator) precisa de diodo de roda livre externo. A corrente máxima de coletor do BC847 deve ser conferida no datasheet antes de ligar carga; não foi verificada.

**Limite de frequência:** só PB13 (`TIM1_CH1N`), PB14 (`TIM15_CH1`, `TIM1_CH2N`) e PB15 (`TIM15_CH2`, `TIM1_CH3N`) têm canal de timer; PB12 só tem `TIM1_BKIN`. Fonte: base do STM32CubeMX (`STM32L412C8Ux.xml`). Afeta a saída em frequência (FW-023).

## Resultado do cruzamento dos Gerbers

| Verificação | OloggerMulti | y7080 |
|---|---:|---:|
| Pinos distintos nas netlists | 451 | 525 |
| Pads distintos com atributos de rede nos Gerbers | 449 | 531 |
| Pinos presentes nas duas fontes e comparados | 445 | 519 |
| Divergências de nomes de rede | 49 | 0 |

As 49 divergências do Multi se distribuem em 43 associações de alimentação para `VBUS`, 4 do bloco VIN/EN de U7 e 2 da seleção GPIO0/GPIO9. Não são 49 defeitos elétricos independentes: algumas decorrem de renomeação de rede ou de revisão. Os pinos ausentes de uma das fontes não foram contados como coincidências nem como divergências; pads expostos e símbolos com múltiplas unidades podem alterar as quantidades totais.

No y7080, as 519 correspondências de nomes não comprovam a correção elétrica do projeto; a incompatibilidade USB existe mesmo com coincidência entre esquema e Gerber.

## Verificações adicionais e limites

Foi executado ERC nas folhas dos MCUs, separadamente: 87 ocorrências no OloggerMulti e 104 no y7080. Os relatórios estão em `nogit/analise_hardware/<modelo>/erc_mcu.json`. Há avisos de bibliotecas/footprints ausentes, rótulos isolados e alimentação não dirigida, esperáveis em parte pela ausência do projeto completo e pela análise de folhas isoladas. Os totais não devem ser usados como quantidade de defeitos reais. Nenhum ERC completo do conjunto nem DRC do PCB foi declarado aprovado.

Foi exportado um PDF da folha ESP32 em `nogit/analise_hardware/OloggerMulti/esp32.pdf`. A leitura de componentes e redes foi feita pelas netlists e pelos arquivos originais; o PDF é um derivado para consulta. Os avisos do KiCad sobre acesso ao registro do Windows não impediram a criação dos XMLs e relatórios.

## Consequências para o firmware e próximos passos

1. Prosseguir com requisitos de firmware, considerando a parte elétrica resolvida pelo responsável técnico. Incorporar a identificação final dos componentes e interfaces quando necessária para configurar o modelo.
2. Continuar o entendimento das diferenças de MCU, pinos, canais e comunicação entre OloggerMulti e y7080. A ideia do usuário é uma compilação única com arquivo de setup que indique os recursos presentes; separação e organização da implementação serão definidas por ele posteriormente. Ver [entendimento de modelos e versões](modelos_e_versoes_firmware.md).
3. Manter aquisição, telemetria, configuração e orquestração como responsabilidades do STM32, conforme decisão do usuário. As integrações concretas serão ESP32-C6 e modem AT da variante confirmada.
4. Definir formato/retenção da telemetria, espaço da flash para dados e eventual imagem de atualização. Uma proposta de partições exige dimensionar o firmware e o mecanismo de recuperação; a flash externa não garante atualização remota segura por si só.
5. Validar o firmware em bancada: SWD, flash SPI, aquisição analógica, direção RS-485, UART de comunicação e caminho de configuração por celular.

Para concluir o B2, já há evidência da origem dos dados e do armazenamento. Frequências de coleta/envio, retenção e comportamento sem conexão continuam abertos. As funcionalidades e a distribuição entre bancada e campo serão levantadas no B4.

## Referências oficiais

- **F1:** [ST — STM32L412C8](https://www.st.com/en/microcontrollers-microprocessors/stm32l412c8.html), variante com 64 KB de flash.
- **F2:** [ST — STM32L071CB](https://www.st.com/en/microcontrollers-microprocessors/stm32l071cb.html), variante com 128 KB de flash.
- **F3:** [ST — DS12469, STM32L412](https://www.st.com/resource/en/datasheet/stm32l412c8.pdf), recursos, alimentação e pinagem.
- **F4:** [ST — DS10690, STM32L071](https://www.st.com/resource/en/datasheet/stm32l071cb.pdf), recursos e funções alternativas, incluindo PB10/PB11.
- **F5:** [GigaDevice — GD25Q128E](https://www.gigadevice.com/product/flash/spi-nor-flash/gd25q128e), flash de 128 Mbit.
- **F6:** [Espressif — GPIO do ESP32-C6](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/gpio.html) e [conexão serial](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/get-started/establish-serial-connection.html).
- **F7:** [SIMCom — SIM7080G e documentos de compatibilidade](https://www.simcom.com/product/SIM7080G.html), confirmar variante real antes de adotar manual.
- **F8:** [Espressif — ESP32-C6-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-c6-mini-1_mini-1u_datasheet_en.pdf), pinagem, alimentação e boot.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.3.7 | HW-009: recomendação do pull-down anotada na lista de melhorias de hardware |
| 03/11/2026 | 0.3.6 | Achado HW-009: saídas digitais sem pull-down na base do transistor e limites de timer por pino |
| 03/11/2026 | 0.3.5 | HW-008: D1 retirada na bancada, limite de 12 V e resistor de 4K7 em 0402 mantido até o novo layout |
| 29/10/2026 | 0.1.0 | Análise dos dois conjuntos, mapas preliminares de pinos, cruzamento de atributos Gerber, verificação ERC parcial e pendências para firmware |
| 29/10/2026 | 0.1.1 | Registrada confirmação do responsável técnico de que a parte elétrica está resolvida; encerrados bloqueios elétricos, preservando evidências históricas |
| 29/10/2026 | 0.1.2 | Alinhado foco no entendimento dos modelos e na ideia de compilação única com setup, sem antecipar separação da implementação |
| 03/11/2026 | 0.3.4 | HW-008: decisão de retirar a D1 das entradas digitais; registradas a perda de proteção contra surto e ESD e o limite de 12 V na bancada com o 4K7 em 0402 |
| 03/11/2026 | 0.3.3 | HW-008: ruptura de 6,1 V da D1 confirmada na página da ST; registrada a decisão de usar resistor 0603 no próximo layout e a análise de potência |
| 03/11/2026 | 0.3.2 | HW-008 atualizado com a netlist: topologia confirmada, pino ativo em nível baixo e proteção D1 de ~6 V em conflito com 12 a 24 V |
| 03/11/2026 | 0.3.1 | Achado HW-008: entradas digitais com fonte de 12 a 24 V; potência do resistor de entrada de 4K7 em 0402 e proteção contra inversão a verificar |
| 02/11/2026 | 0.3.0 | Achado HW-007: escala do ADC do OloggerMulti ~1,37 acima do esperado (referência efetiva de ~2,37 V com alimentação de 3,25 V); driver, configuração do ADC e circuito de entrada descartados |
| 02/11/2026 | 0.2.1 | HW-006 contornado por bits de opção na unidade de bancada; firmware executa no STM32L412 real |
| 02/11/2026 | 0.2.0 | Achado HW-006: BOOT0 do OloggerMulti sem ligação no esquema, confirmado na gravação por SWD; y7080 tem BOOT0 ligado ao R16 de 100 kΩ |
