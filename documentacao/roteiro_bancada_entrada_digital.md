# Roteiro de bancada — entrada digital e contador de pulsos

> **Criado em:** 03/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.1.0

Roteiro do item FW-056, para o build `ologgermulti`. Nenhum dos testes abaixo foi executado: todo o código descrito aqui só foi compilado e testado no PC. Cada passo tem o resultado esperado e um campo para registrar o resultado medido. O que contradisser o esperado vira achado, não é ajustado no roteiro.

## Limites de segurança

- **Tensão de entrada: no máximo 12 V**, até o resistor de 4K7 passar a 0603 ou maior (achado HW-008). Em 24 V o 4K7 em 0402 passa de ~110 mW.
- Fonte de bancada **limitada em corrente** (por exemplo, 20 mA), com o terra de campo ligado ao terra de campo da placa.
- A proteção D1 (ESDA6V1SC5) deve estar **retirada** das entradas, conforme a decisão de 03/11/2026. Com ela montada, não aplicar mais que 5 V.
- Não inverter a polaridade: o LED do optoacoplador não tem diodo de proteção em série.
- Antes de medir consumo, conferir se o pino VBAT da placa tem bateria (necessária para o totalizador sobreviver à falta de energia).

## Material

- Placa OloggerMulti com a imagem do build `ologgermulti` gravada, depurador SWD e fonte de 3,3 V/USB da placa.
- Fonte de bancada de 5 V e de 12 V, limitada em corrente.
- Gerador de pulsos (ou segundo microcontrolador) que entregue 0 V / 12 V com largura e taxa ajustáveis; osciloscópio ou analisador lógico para conferir a largura real.
- Multímetro ou analisador de corrente de baixo consumo (faixa de microampères) em série com a alimentação, para o teste 7.

## Preparação

1. Compilar e gravar:
   `cmake -S . -B build/ologgermulti -G Ninja -DDEVICE=ologgermulti -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake -DOLOG_STM32CUBE_L4=<pasta>` e `cmake --build build/ologgermulti`.
2. No depurador, observar as variáveis `estado_bruto` e `estado_filtrado` (arquivo `lib/io/src/io_tarefa.c`) e os registradores de backup `TAMP->BKP0R` a `BKP15R`.
3. Cada entrada Dn usa quatro registradores de backup: valor (`BKP[4(n-1)]`), complemento (`+1`), e as mesmas duas palavras da segunda cópia (`+2` e `+3`). Em uma entrada válida o complemento é o valor invertido bit a bit.
4. Habilitar o sleep só nos testes 7 a 9, com `energia_define_habilitada(true)` ou escrevendo `1` na variável `habilitada` (arquivo `lib/energia/src/energia.c`).
5. Atenção ao depurador no Stop 2: a conexão SWD cai. Nos testes de sleep, manter o depurador conectado só se o clock de depuração do Stop estiver ativo no depurador, ou observar o resultado ao acordar.

## Testes

| # | Teste | Passos | Resultado esperado | Resultado medido |
|---|---|---|---|---|
| 1 | Nível das entradas | Aplicar 0 V e depois 5 V e 12 V em D1, uma de cada vez, repetir em D2 a D4 | Com tensão, o bit da entrada em `estado_bruto` e `estado_filtrado` vai a 1; sem tensão, 0. D1 é o bit 0 e D4 é o bit 3 | |
| 2 | Filtro de 50 ms do estado | Pulso de 30 ms em uma entrada | `estado_bruto` muda; `estado_filtrado` não muda. Um pulso de 100 ms muda os dois | |
| 3 | Contagem acordado | Pulsos de 12 V, largura 40 ms, a 5 Hz por 60 s em D1 | `io_tarefa_total(1)` aumenta em 300 (±1) | |
| 4 | Taxa máxima | Mesmo teste a 10 Hz, largura 40 ms, por 60 s | Aumento de 600 (±1), sem perda | |
| 5 | Largura mínima | Pulsos de 10 ms e de 30 ms, 5 Hz, 30 s | 10 ms não conta (filtro de 20 ms); 30 ms conta tudo. Registrar a menor largura que conta de forma estável | |
| 6 | Quatro entradas | Quatro geradores ao mesmo tempo, 5 a 10 Hz, 60 s | O total de cada entrada bate com o gerador respectivo, sem mistura | |
| 7 | Persistência | Contar 100 pulsos; reiniciar pelo botão de reset; depois, se houver bateria no VBAT, tirar e devolver a alimentação | O total após o reset é o mesmo. Na primeira partida com registradores zerados, `io_totalizador_corrompido` indica verdadeiro e o total começa em zero; `io_tarefa_define_total` iguala ao medidor | |
| 8 | Sleep por inatividade | Habilitar o sleep; parar os pulsos e esperar 60 s. Medir a corrente | O STM32 entra em Stop 2 após ~60 s sem pulso; a corrente cai. Registrar o valor em microampères | |
| 9 | Acordar pelo pulso e ficar acordado | Em sleep, enviar 1 pulso; depois pulsos a 5 Hz por 30 s; depois parar | O pulso acorda o STM32. Com pulso constante ele **permanece acordado** e não acorda e dorme a cada pulso. Dorme de novo só depois de ~60 s sem pulso. O total conta todos os pulsos, inclusive o que acordou | |
| 10 | Contagem entre o sleep e o acordar | Em sleep, 10 pulsos separados de 1 s; depois ler o total | O total aumentou em 10, sem perda e sem contar o pulso do despertar duas vezes | |
| 11 | Aquisição analógica depois do sleep | Aplicar 12 mA em uma entrada 4–20 mA, dormir e acordar | Ao dormir a corrente cai (o ADC é parado). Depois de acordar, a leitura só fica disponível ao fechar o primeiro segundo e bate com os 12 mA | |
| 12 | Tempo de inatividade configurável | Chamar `io_tarefa_define_inatividade(10000)` e repetir o teste 8 | Dorme após ~10 s | |

## Critério de aceite

- Os testes 1 a 12 com o resultado esperado, ou cada desvio registrado como achado com a causa.
- Corrente em sleep registrada. O valor esperado não foi estimado: é um dos resultados a medir.
- Largura mínima de pulso confirmada com a de 20 ms do filtro.

## Pendências conhecidas antes de executar

- A taxa de leitura de 2 ms e o filtro de 20 ms exigem que o medidor gere pulsos mais largos que isso, com folga.
- O clock do sistema fica no MSI. O firmware ainda não usa o PLL.
- O despertar do módulo para transmitir (RTC, 1, 2 ou 5 vezes ao dia) e o ESP32 não fazem parte deste roteiro. O ESP32 só é acordado para transmitir.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.1.0 | Criado o roteiro com 12 testes para a entrada digital, o contador de pulsos, o totalizador e o sleep |
