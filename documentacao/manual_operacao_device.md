# Manual de operação do device — oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.2.6

## Para quem é

Técnico de campo, integrador e equipe de suporte. Este manual descreve como operar o device no campo. Regras do sistema (perfis, painéis, chat) estão fora deste manual.

## Estado atual

Documento em construção. As seções abaixo são preenchidas conforme cada fase é implementada. Nenhuma seção descreve comportamento ainda não implementado. Em 03/11/2026 a seção "Entradas digitais e contador de pulsos" descreve o que está em código mas **ainda não foi validado em bancada**.

## Entradas digitais e contador de pulsos (OloggerMulti)

Só o OloggerMulti tem entradas digitais (D1 a D4). Em código e ainda sem validação em bancada.

- **Ligação:** a entrada é ativa quando há tensão no campo. A fonte de campo é de 12 a 24 V, a mesma da entrada do CLP. **Na placa de bancada o limite é 12 V** (resistor de 4K7 em 0402), com a proteção D1 retirada e sem proteção contra inversão de polaridade. Ligar o terra de campo e usar fonte limitada em corrente.
- **Modos:** discreto (0 ou 1) e contador de pulsos. O modo de frequência em Hz ainda não existe. Cada entrada de contador tem um total de 32 bits, como um hodômetro: só soma, guardado em memória que sobrevive a reset e à falta de energia (precisa de bateria no pino VBAT). Na instalação, igualar o total à leitura do medidor.
- **Pulso:** o leitor deve gerar pulsos mais largos que 20 ms, a no máximo 5 a 10 por segundo. Pulso mais curto é descartado.
- **Economia de energia:** com o sleep habilitado, o STM32 dorme depois de 60 s sem pulso em todas as entradas. O pulso o acorda, e ele fica acordado contando até outros 60 s sem pulso. Dormindo, a leitura analógica 4–20 mA para. O ESP32 só é acordado para transmitir. **O sleep vem desabilitado de fábrica**; não há ainda comando para habilitá-lo em campo.
- **Entrada solta:** uma entrada ligada a nada pode captar ruído e acordar o STM32 sem necessidade. Entradas sem uso devem ficar com o sinal definido.
- Roteiro de teste de bancada: `documentacao/roteiro_bancada_entrada_digital.md`.

## Saídas digitais (OloggerMulti)

Quatro saídas, no conector J12. Em código e ainda sem validação em bancada.

- **Tipo:** cada saída é um transistor NPN em coletor aberto. Ligada, ela **puxa `OUT-n` ao terra** do equipamento. A carga deve ser ligada entre uma tensão positiva e a saída, e a fonte da carga deve ter o terra em comum.
- **Carga:** a corrente máxima da saída ainda não foi confirmada no datasheet do transistor; não ligar carga pesada antes disso. **Carga indutiva (relé, contator) precisa de diodo de roda livre externo**, porque a saída não tem proteção; o diodo fica no relé externo.
- **Estado:** as saídas partem desligadas e não religam sozinhas depois de falta de energia. Durante o reset e a gravação do firmware o estado da saída não é garantido (achado HW-009).
- **Retardo de acionamento (relé de tempo):** uma saída pode ter um retardo. Todo comando de ligar espera o retardo e então liga; um comando de desligar durante a espera cancela a partida. Com a saída já ligada, ou com o retardo em andamento, outro comando de ligar é descartado. No sleep do STM32 o retardo não corre.
- **Modo manual e automático:** cada saída tem o seu modo. Um comando manual põe a saída em manual e ela só volta ao automático com o comando de modo automático. Em automático, a saída segue a **regra do sensor**, cadastrada no equipamento (entrada analógica para a saída, com máximo, mínimo e frequência, por MQTT ou Modbus). **Sem regra do sensor**, ela segue a **agenda** quando o equipamento está **offline** e o **comando do CCO** quando está online. Se o sensor falhar, a saída desliga em automático e mantém o comando em manual; depois da falha, ela só volta a operar pelo sensor depois de 5 minutos estáveis. Depois de um reinício a saída volta desligada e em modo automático.

## Seções planejadas

1. **Modelos e perfis:** OloggerMulti e y7080, entradas, saídas e rádios de cada um.
2. **Instalação:** fixação, alimentação, sensores e cabos RS-485.
3. **Comissionamento:** conectividade mínima pelo notebook, ativação do device no sistema e verificação do primeiro envio.
4. **Sessão de configuração:** início pelo notebook ou pelo sistema, bloqueio de comandos e término após cinco minutos sem atividade.
5. **Gateway serial:** aplicação Python no notebook, chaveiro serial e comandos permitidos.
6. **Falhas e diagnóstico:** falha de sensor, dente de serra, Modbus sem resposta, hora inválida e alertas.
7. **Atualização de firmware:** verificação, retomada e recuperação.
8. **Manutenção:** substituição de bateria do relógio, limpeza e verificação periódica.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.2.6 | Agenda offline e comando do CCO online; cadastro da regra do sensor por MQTT ou Modbus |
| 04/11/2026 | 0.2.5 | Comandos automáticos locais e remotos e a espera de 5 minutos depois da falha |
| 04/11/2026 | 0.2.4 | Adicionado o modo manual e automático das saídas |
| 03/11/2026 | 0.2.3 | O "acionamento por tempo" é o retardo de acionamento; ligar com a saída ligada ou aguardando é descartado; mapa Modbus das saídas ajustado (P-037) |
| 03/11/2026 | 0.2.2 | Saídas digitais: retardo de acionamento e diodo no relé externo |
| 03/11/2026 | 0.2.1 | Adicionada a seção de saídas digitais (em código, sem validação em bancada) |
| 03/11/2026 | 0.2.0 | Adicionada a seção de entradas digitais e contador de pulsos (em código, sem validação em bancada) |
| 01/11/2026 | 0.1.0 | Criado esqueleto do manual de operação |
