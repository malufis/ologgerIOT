# Manual de operação do device — oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.2.0

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
| 03/11/2026 | 0.2.0 | Adicionada a seção de entradas digitais e contador de pulsos (em código, sem validação em bancada) |
| 01/11/2026 | 0.1.0 | Criado esqueleto do manual de operação |
