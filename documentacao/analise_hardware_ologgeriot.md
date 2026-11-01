# Análise dos hardwares OloggerMulti e y7080

> **Criado em:** 29/10/2026 · **Última modificação:** 29/10/2026 · **Versão:** 0.1.2
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
| 29/10/2026 | 0.1.0 | Análise dos dois conjuntos, mapas preliminares de pinos, cruzamento de atributos Gerber, verificação ERC parcial e pendências para firmware |
| 29/10/2026 | 0.1.1 | Registrada confirmação do responsável técnico de que a parte elétrica está resolvida; encerrados bloqueios elétricos, preservando evidências históricas |
| 29/10/2026 | 0.1.2 | Alinhado foco no entendimento dos modelos e na ideia de compilação única com setup, sem antecipar separação da implementação |
