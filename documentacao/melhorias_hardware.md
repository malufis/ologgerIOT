# Melhorias de hardware — próxima revisão das placas

> **Criado em:** 03/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.1.0

Lista das mudanças de hardware que apareceram durante o desenvolvimento do firmware e que o responsável técnico quer tratar na **próxima revisão das placas**. Cada item aponta o achado de origem em `analise_hardware_ologgeriot.md`. Este documento só registra; a decisão de entrar no layout e a priorização são do responsável técnico. As placas de bancada atuais seguem com os contornos descritos em cada item.

## Placa OloggerMulti

| # | Mudança proposta | Origem | Motivo | Estado |
|---|---|---|---|---|
| 1 | Pull-down de ~10 kΩ entre a base e o emissor de cada transistor de saída (Q3 a Q6), ou do pino ao terra | HW-009 | A base não tem resistor para o terra: durante o reset e o bootloader o pino flutua e o estado da saída não é garantido. O requisito é manter as saídas desligadas durante atualização e reinício | Anotado em 03/11/2026 pelo responsável técnico para a próxima revisão. Hoje o firmware só leva o pino a desligado depois do `HAL_Init` |
| 2 | Pull-down no pino 44 (PH3/BOOT0) do U1 | HW-006 | O pino está sem ligação no esquema e flutua em nível alto; a placa inicia pelo bootloader da ST | Contornado por bits de opção na unidade de bancada (FW-051) |
| 3 | Resistor de entrada digital (R3 a R6, 4K7) em 0603, com potência maior que 100 mW se a entrada for de 24 V | HW-008 | O 4K7 em 0402 dissipa ~110 mW em 24 V. Em 12 V (~25 mW) cabe | O responsável técnico decidiu 0603 no próximo layout. Para 24 V ainda pede 0805 ou 1206 de 0,25 W, ou subir o valor para ~10k. Bancada limitada a 12 V |
| 4 | Supressor de surto e ESD nas entradas digitais com tensão de operação acima de 24 V (família SMAJ ou SM5S, de ~30 V), no nó `IN-Dx` | HW-008 | A D1 original (ESDA6V1SC5, ruptura de 6,1 V) conflitava com a fonte de 12 a 24 V e foi retirada na bancada. Sem ela, a entrada fica sem proteção de surto | A decidir, se a placa for para campo industrial |
| 5 | Proteção contra inversão de polaridade nas entradas digitais (por exemplo, diodo em série com o LED) | HW-008 | O LED do optoacoplador suporta cerca de 5 V reversos e não tem diodo em série | A decidir |
| 6 | Confirmar a bateria no pino VBAT | Pendência P-030 | Os registradores de backup do totalizador de pulsos só sobrevivem à falta de energia com bateria no VBAT | A confirmar na placa; pode virar mudança de layout |
| 7 | Avaliar remapear a saída 1 (PB12) para um pino com canal de timer, se a saída em frequência precisar de PWM de hardware nas quatro saídas | Pendência do FW-023 | Só PB13, PB14 e PB15 têm canal de timer; PB12 só tem `TIM1_BKIN`. Pela decisão atual a frequência será gerada por temporizador com tratador, o que dispensa a mudança | Opcional, a decidir |

## Montagem e processo (não é mudança de layout)

| # | Ponto | Origem | Estado |
|---|---|---|---|
| 8 | Conferir a montagem da ferrite L2 (BLM18HE152SN1D) em toda placa nova: sem ela o VDDA cai para ~2,37 V e o ADC lê errado | HW-007 | Na placa de bancada, soldar a L2 |
| 9 | Gravar os bits de opção `nSWBOOT0=0` e `nBOOT0=1` em toda unidade do OloggerMulti enquanto o item 2 não for corrigido | HW-006, FW-051 | Procedimento e script pendentes |

## Regras da instalação, fora do hardware da placa

- **Carga indutiva nas saídas:** relé ou contator ligado a uma saída precisa de diodo de roda livre. O responsável técnico instalará o diodo no relé externo (decidido em 03/11/2026).
- **Corrente máxima da saída:** a saída é um BC847 em coletor aberto; a corrente máxima deve ser conferida no datasheet do transistor antes de ligar carga.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.1.0 | Criado a pedido do responsável técnico, com os pontos de HW-006 a HW-009, a bateria do VBAT e o remapeamento opcional da saída 1 |
