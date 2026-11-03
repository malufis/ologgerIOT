# SPRINT_04 — Saídas e controle

> **Criado em:** 03/11/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.1.4
> **Período:** 30/11/2026 a 04/12/2026

Em andamento desde 03/11/2026: o FW-022 e o FW-024 estão em código pronto; os demais itens estão pendentes. As datas são a sequência de semanas a partir de 02/11/2026 e mudam se a SPRINT_00, a SPRINT_01 ou a SPRINT_01B atrasarem.

## Meta

Saídas discretas (ligar e desligar, com retardo de acionamento), saída em frequência (a "pulsada"), modo manual e automático e a função de ligar e desligar o sleep. O prazo de referência do protótipo (29/11/2026, um domingo) é a véspera do início desta sprint.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-022 | Saídas discretas: ligar e desligar, com as saídas desligadas na partida | Código pronto em 03/11/2026: ligar e desligar (`saida`, `io_saida`, `io_saidas`), com as saídas desligadas na partida. Bancada pendente |
| FW-023 | Saída em frequência (a "pulsada") proporcional a 4–20 mA, com 4 mA e 20 mA configuráveis; só pode ser habilitada com o sleep desligado | Pendente |
| FW-024 | Retardo de acionamento (relé de tempo, o "acionamento por tempo") por saída discreta, com cancelamento por comando de desligar | Código pronto em 03/11/2026 (`saida_define_atraso`, `io_saidas_define_atraso`): 36 verificações no PC com o retardo, o comando descartado, o cancelamento por desligar e a volta do contador. Regras confirmadas em 03/11/2026. Bancada pendente |
| FW-025 | Modo manual e automático por saída, com prioridade do sensor | Pendente |
| FW-061 | Ligar e desligar o modo sleep (modo de operação, registro 40412), com exclusão mútua: o sleep só liga se nenhuma saída estiver em frequência, e a saída em frequência só habilita com o sleep desligado | Lado do sleep pronto em 03/11/2026 (`energia_define_habilitada` devolve erro se um módulo impede). Falta o lado das saídas (FW-023) e o comando do Modbus |

## Estado das saídas digitais (03/11/2026)

- **Hardware (netlist `extras.xml`):** cada saída é um NPN BC847 em coletor aberto. O pino do STM32 passa por 1k (R45 a R48) até a base, o emissor vai ao terra `GNDD` e o coletor vai ao conector J12 (`OUT-1` a `OUT-4`). **Pino em nível alto liga a saída**, que puxa `OUT-n` ao terra. Não há pull-down na base, nem diodo de roda livre. Pinos: PB12 a PB15.
- **Código:** `lib/io/saida` (estado: ligada, aguardando o retardo ou desligada, em lógica pura), `lib/io/io_saida` (pinos, com o nível desligado escrito antes de virar saída) e `lib/io/io_saidas` (quatro saídas e a tarefa que liga as saídas ao fim do retardo, a cada 100 ms). `io_saidas_prepara()` roda logo depois do `HAL_Init`, e as saídas partem desligadas.
- **Ainda não existe:** quem manda na saída (manual, automático, sensor, agenda), a saída em frequência e a ligação com comandos do sistema e do Modbus. Nada foi testado em hardware.
- **Stop 2:** o pino mantém o estado, mas o retardo não corre, porque o tick do FreeRTOS para.

### Decisões de 03/11/2026

- **"Pulsada" é a saída em frequência.** Não é um acionamento por tempo. O mapa Modbus foi ajustado: modo da saída 0 = discreto, 1 = frequência (pulsada), 2 reservado (P-037 resolvida).
- **O "acionamento por tempo" é o retardo de acionamento (relé de tempo).** Não existe um acionamento que liga e desliga sozinho depois de um tempo. A saída discreta só tem ligar, desligar e o retardo. O registrador de tempo de acionamento (40271 a 40285) fica reservado, porque o parâmetro é o retardo (40256 a 40270).
- **Retardo em todo comando que liga.** A saída discreta pode ter um retardo, por saída, em segundos. Todo comando de ligar espera o retardo; retardo zero liga na hora. Um comando de desligar durante a espera cancela a partida.
- **Contagem em curso.** Com um retardo em andamento, outro comando de ligar é descartado e a contagem em curso continua.
- **Saída já ligada.** Se a saída está ligada e chega outro comando de ligar, o último comando é descartado: nada muda e o retardo não é reaplicado.
- **Sleep e saída em frequência não coexistem.** A saída em frequência só pode ser habilitada se o modo sleep não estiver ativo, e o sleep só liga se nenhuma saída estiver em frequência. Ligar e desligar o sleep é uma função (FW-061), que corresponde ao modo de operação 40412 (0 = contínuo, 1 = sleep).
- **Diodo no relé externo.** O responsável técnico instalará o diodo de roda livre no relé externo.
- **Melhoria de hardware.** O pull-down da base dos transistores de saída (HW-009) e os demais pontos estão em `documentacao/melhorias_hardware.md`, para a próxima revisão das placas.

### Ainda a confirmar

- **Frequência por software:** só PB13 a PB15 têm canal de timer e PB12 não tem. Quatro frequências independentes pedem um temporizador com tratador, que para em Stop 2, daí a exclusão mútua com o sleep. Fonte: base do STM32CubeMX.
- **Retardo e depois de falta de energia:** as saídas partem desligadas, então depois de uma falta de energia sempre esperam um novo comando, como o B4 descreve.

## Critério de encerramento

- Todos os itens concluídos, ou os não concluídos registrados com motivo e movidos para a sprint seguinte.
- Tabela de estados testada no PC e em bancada.
- Documentação atualizada pela skill `doc-sync` e sprint fechada pela skill `sprint-close`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.1.4 | O "acionamento por tempo" é o retardo de acionamento; ligar com a saída ligada ou aguardando é descartado; mapa Modbus das saídas ajustado (P-037) |
| 03/11/2026 | 0.1.3 | Meta ajustada: "pulsada" é a saída em frequência |
| 03/11/2026 | 0.1.2 | Decisões de 03/11/2026 registradas (pulsada é frequência, retardo de acionamento, sleep e frequência não coexistem); FW-024 em código pronto; novo FW-061 |
| 03/11/2026 | 0.1.1 | Sprint em andamento: FW-022 em código pronto; registrados o hardware das saídas, o que falta e as decisões a confirmar |
| 03/11/2026 | 0.1.0 | Criada a sprint a partir do backlog |
