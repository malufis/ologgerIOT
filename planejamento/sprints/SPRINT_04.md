# SPRINT_04 — Saídas e controle

> **Criado em:** 03/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.12
> **Período:** 30/11/2026 a 04/12/2026

Em andamento desde 03/11/2026: o FW-022 e o FW-024 estão em código pronto; os demais itens estão pendentes. As datas são a sequência de semanas a partir de 02/11/2026 e mudam se a SPRINT_00, a SPRINT_01 ou a SPRINT_01B atrasarem.

## Meta

Saídas discretas (ligar e desligar, com retardo de acionamento), saída em frequência (a "pulsada"), modo manual e automático e a função de ligar e desligar o sleep. O prazo de referência do protótipo (29/11/2026, um domingo) é a véspera do início desta sprint.

## Itens

| ID | Tarefa | Situação |
|---|---|---|
| FW-022 | Saídas discretas: ligar e desligar, com as saídas desligadas na partida | Código pronto em 03/11/2026: ligar e desligar (`saida`, `io_saida`, `io_saidas`), com as saídas desligadas na partida. Bancada pendente |
| FW-023 | Saída em frequência (a "pulsada") proporcional a 4–20 mA, com 4 mA e 20 mA configuráveis; só pode ser habilitada com o sleep desligado | Parcial em 04/11/2026: `lib/io/freq_saida` pronto (cálculo proporcional da frequência a partir de 4–20 mA, 0 Hz na falha, 9 testes no PC). Decidido em 04/11/2026: em 0 Hz a saída fica em nível baixo (transistor desligado), o duty cycle é de 50%, e só as saídas 2, 3 e 4 (PB13, PB14 e PB15, com canal de timer) têm modo frequência; a saída 1 (PB12) fica só discreta. Falta gerar o sinal pelo timer |
| FW-024 | Retardo de acionamento (relé de tempo, o "acionamento por tempo") por saída discreta, com cancelamento por comando de desligar | Código pronto em 03/11/2026 (`saida_define_atraso`, `io_saidas_define_atraso`): 36 verificações no PC com o retardo, o comando descartado, o cancelamento por desligar e a volta do contador. Regras confirmadas em 03/11/2026. Bancada pendente |
| FW-025 | Modo manual e automático por saída, com prioridade do sensor | Código pronto em 04/11/2026 (`controle_saida`, `io_saidas`): 82 verificações no PC com manual, automático, regra do sensor, agenda local offline, comando do CCO online, falha do sensor com espera de 5 minutos e retardo. Falta o alerta ao CCO na chegada do comando manual. Bancada pendente |
| FW-061 | Ligar e desligar o modo sleep (modo de operação, registro 40412), com exclusão mútua: o sleep só liga se nenhuma saída estiver em frequência, e a saída em frequência só habilita com o sleep desligado | Lado do sleep pronto em 03/11/2026 (`energia_define_habilitada` devolve erro se um módulo impede). Lado das saídas pronto em 04/11/2026: `lib/io/tipo_saida` guarda o tipo de cada saída, recusa frequência com o sleep ligado e na saída 1, e `io_frequencia_sleep_permitido` é o gancho que impede ligar o sleep com saída em frequência (8 testes no PC). Falta o comando do Modbus (40166 e 40412) |
| FW-063 | Regra do sensor entrada analógica para saída digital (comando automático local): cadastro por MQTT ou Modbus (entrada associada, limiares máximo e mínimo e sentido, mais a frequência em 4 mA e 20 mA, todos com endereço Modbus) e o avaliador com histerese que gera os pedidos da fonte do sensor. Sentido 0: liga no mínimo e desliga no máximo; sentido 1: liga no máximo e desliga no mínimo. Sem regra cadastrada valem a agenda (offline) e o CCO (online) | Parcial em 04/11/2026: `lib/io/regra_sensor` pronto (cadastro validado, sentidos 0 e 1 e histerese, 9 testes no PC); falta ler e gravar o cadastro pelos endereços Modbus e por MQTT e ligar a regra à leitura da entrada analógica |

## Estado das saídas digitais (03/11/2026)

- **Hardware (netlist `extras.xml`):** cada saída é um NPN BC847 em coletor aberto. O pino do STM32 passa por 1k (R45 a R48) até a base, o emissor vai ao terra `GNDD` e o coletor vai ao conector J12 (`OUT-1` a `OUT-4`). **Pino em nível alto liga a saída**, que puxa `OUT-n` ao terra. Não há pull-down na base, nem diodo de roda livre. Pinos: PB12 a PB15.
- **Código:** `lib/io/saida` (estado: ligada, aguardando o retardo ou desligada, em lógica pura), `lib/io/io_saida` (pinos, com o nível desligado escrito antes de virar saída) e `lib/io/io_saidas` (quatro saídas e a tarefa que liga as saídas ao fim do retardo, a cada 100 ms). `io_saidas_prepara()` roda logo depois do `HAL_Init`, e as saídas partem desligadas.
- **Ainda não existe:** a saída em frequência, o alerta ao CCO na chegada do comando manual, e a ligação com os comandos do sistema e do Modbus. Nada foi testado em hardware.
- **Stop 2:** o pino mantém o estado, mas o retardo não corre, porque o tick do FreeRTOS para.

### Decisões de 03/11/2026

- **"Pulsada" é a saída em frequência.** Não é um acionamento por tempo. O mapa Modbus foi ajustado: tipo da saída 0 = discreto, 1 = frequência (pulsada), 2 reservado (P-037 resolvida).
- **O "acionamento por tempo" é o retardo de acionamento (relé de tempo).** Não existe um acionamento que liga e desliga sozinho depois de um tempo. A saída discreta só tem ligar, desligar e o retardo. O registrador de tempo de acionamento (40271 a 40285) fica reservado, porque o parâmetro é o retardo (40256 a 40270).
- **Retardo em todo comando que liga.** A saída discreta pode ter um retardo, por saída, em segundos. Todo comando de ligar espera o retardo; retardo zero liga na hora. Um comando de desligar durante a espera cancela a partida.
- **Contagem em curso.** Com um retardo em andamento, outro comando de ligar é descartado e a contagem em curso continua.
- **Saída já ligada.** Se a saída está ligada e chega outro comando de ligar, o último comando é descartado: nada muda e o retardo não é reaplicado.
- **Sleep e saída em frequência não coexistem.** A saída em frequência só pode ser habilitada se o modo sleep não estiver ativo, e o sleep só liga se nenhuma saída estiver em frequência. Ligar e desligar o sleep é uma função (FW-061), que corresponde ao modo de operação 40412 (0 = contínuo, 1 = sleep).
- **Diodo no relé externo.** O responsável técnico instalará o diodo de roda livre no relé externo.
- **Melhoria de hardware.** O pull-down da base dos transistores de saída (HW-009) e os demais pontos estão em `documentacao/melhorias_hardware.md`, para a próxima revisão das placas.

### Modo manual e automático (FW-025, 04/11/2026)

Implementado conforme o B4 e as definições de 04/11/2026 (`documentacao/configuracao_do_dispositivo.md`):

- Cada saída tem modo **manual** ou **automático**, e um comando muda só o modo da saída a que se destina.
- Um **comando manual direto** prevalece, põe a saída em manual e ela fica assim até o comando de modo automático. Respeita o retardo de acionamento.
- **Comandos automáticos locais:** a **regra do sensor** (cadastrada: entrada analógica para a saída digital, com máximo, mínimo e frequência) e a **agenda**, que funciona sem conectividade e só vale **offline**. **Comando automático remoto:** o do **CCO**, que vale **online**. Com regra do sensor, a agenda e o CCO são ignorados.
- **Conectividade:** ao ficar offline, a agenda é aplicada na hora. Ao reconectar, a saída mantém o último estado e espera um novo comando do CCO.
- **Falha do sensor:** em automático a saída desliga; em manual mantém o comando manual. Quando a falha termina, a saída só volta a operar pelo sensor depois de **5 minutos estáveis**; até lá fica desligada.
- **Volta ao automático:** avalia e executa a regra válida na hora; sem pedido válido, **mantém o último estado válido do comando**.
- **Reinício:** a saída volta desligada, em modo automático e offline (o modo manual não é guardado).
- **Cadastro da regra:** os campos vêm por MQTT ou Modbus e têm endereço Modbus (proposta no mapa, 40307 a 40381). O cadastro e o avaliador são o FW-063.
- **Alerta ao CCO** na chegada do comando manual: depende do espaço de alertas (FW-021) e ainda não existe. `io_saidas_comando_manual` informa se houve mudança de modo.

**Regra do sensor (FW-063, parcial):** `lib/io/regra_sensor` guarda sentido, mínimo e máximo, valida o cadastro por inteiro e devolve o pedido da fonte do sensor com histerese; com a regra cadastrada, a saída ignora a agenda e o CCO (o sensor tem prioridade sobre a agenda). **Saída em frequência (FW-023, parcial):** `lib/io/freq_saida` calcula a frequência proporcional entre os valores de 4 mA e de 20 mA (a de 4 mA pode ser maior), prende a corrente nos extremos e leva a saída a 0 Hz na falha. **Falta:** o cadastro pelos endereços Modbus e por MQTT e a ligação à leitura da entrada analógica (FW-063), a geração do sinal (FW-023), o alerta ao CCO e a ligação com os comandos do sistema e do Modbus. A interação do modo manual com a saída em frequência segue em aberto.

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
| 04/11/2026 | 0.1.12 | FW-061 com o lado das saídas pronto; FW-023 com as decisões de 0 Hz, duty cycle e canais |
| 04/11/2026 | 0.1.11 | FW-023 parcial: módulo `freq_saida` pronto e testado |
| 04/11/2026 | 0.1.10 | FW-063 parcial: módulo `regra_sensor` pronto e testado |
| 04/11/2026 | 0.1.9 | FW-063 com o sentido da comparação confirmado |
| 04/11/2026 | 0.1.8 | Agenda como comando local (só offline) e CCO online; cadastro da regra do sensor por MQTT ou Modbus; 82 verificações |
| 04/11/2026 | 0.1.7 | FW-025 com as definições de 04/11/2026 (local e remoto, 5 minutos, último estado, reinício); novo FW-063 |
| 04/11/2026 | 0.1.6 | FW-025 em código pronto: modo manual e automático por saída, com prioridade do sensor local e falha do sensor |
| 04/11/2026 | 0.1.5 | Correções da verificação do doc-keeper antes do fechamento da SPRINT_00 (números, nomes dos registradores, concordância e referências) |
| 03/11/2026 | 0.1.4 | O "acionamento por tempo" é o retardo de acionamento; ligar com a saída ligada ou aguardando é descartado; mapa Modbus das saídas ajustado (P-037) |
| 03/11/2026 | 0.1.3 | Meta ajustada: "pulsada" é a saída em frequência |
| 03/11/2026 | 0.1.2 | Decisões de 03/11/2026 registradas (pulsada é frequência, retardo de acionamento, sleep e frequência não coexistem); FW-024 em código pronto; novo FW-061 |
| 03/11/2026 | 0.1.1 | Sprint em andamento: FW-022 em código pronto; registrados o hardware das saídas, o que falta e as decisões a confirmar |
| 03/11/2026 | 0.1.0 | Criada a sprint a partir do backlog |
