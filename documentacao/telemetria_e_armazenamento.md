# Telemetria, modos de operação e armazenamento — oLoggerIOT

> **Criado em:** 30/10/2026 · **Última modificação:** 03/11/2026 · **Versão:** 0.1.29

## Origem e estado

Respostas de Rodrigo Maluf durante o B2, em 30/10/2026. Este documento registra comportamentos desejados. Não define arquitetura, sistema de arquivos, formato de lote ou implementação dos modos de baixo consumo. O usuário confirmou o resumo de coleta, envio, sleep, alarmes e armazenamento e depois esclareceu nova medição sob comando e exclusão após recebimento pelo servidor. O resumo consolidado do B2 foi confirmado pelo usuário em 30/10/2026; a condição de recepção NB-IoT permanece a validar na integração. Os detalhes técnicos listados não representam decisões de implementação já tomadas.

## Setup e modos de operação

Os intervalos serão configurados no setup do dispositivo. Haverá possibilidade de operação contínua ou em sleep mode.

Exemplos de utilização informados pelo usuário:

- Equipamentos ligados à rede elétrica com intervalo de um minuto. Ainda é necessário distinguir o intervalo de aquisição do intervalo de transmissão nesse exemplo.
- Equipamentos a bateria com quatro envios por dia, dormindo no restante do dia. Aquisição e envio seguem regras configuráveis, com despertares independentes. Não foi definido se os quatro horários serão fixos ou igualmente espaçados.

A configuração por sensor e os valores dos intervalos ainda serão detalhados. A independência entre coleta e transmissão foi esclarecida pelo usuário conforme os eventos abaixo.

## Despertares e controle do rádio

| Evento | Comportamento requerido |
|---|---|
| Coleta programada sem envio | Despertar o STM32 para coletar/armazenar; o rádio permanece em sleep |
| Envio programado | Ativar o rádio, transmitir os dados e retornar o rádio ao sleep |
| Informação/comando da rede NB-IoT | Receber a informação, processar e devolver a resposta; retornar ao sleep |
| Alarme | Poder despertar o equipamento e transmitir o alerta fora da programação normal |

O comportamento de recepção pela rede foi informado pelo usuário como intenção de operação com o módulo NB-IoT utilizado. Não foi definido o modo exato de baixo consumo, o sinal que desperta o STM32 nem a latência de atendimento. Sleep do rádio não significa automaticamente desligar sua alimentação.

**Condição técnica a validar na integração:** em PSM o dispositivo não é alcançável por paging durante o período de sono. Em eDRX, a recepção ocorre em janelas periódicas negociadas com a rede. Portanto, a recepção solicitada depende do módulo real, do modo negociado, da operadora e do caminho MQTT. Não foi selecionado PSM ou eDRX nesta etapa nem prometido despertar imediato a qualquer momento. Referências: [GSMA — guia NB-IoT](https://www.gsma.com/IC/wp-content/uploads/2018/04/NB-IoT_Deployment_Guide_v2_5Apr2018.pdf) e [Cisco — recepção eDRX/indisponibilidade PSM](https://www.cisco.com/c/en/us/td/docs/wireless/asr_5000/21-28/mme-admin/21-28-mme-admin/m_edrx.html).

## Contador de pulsos e sleep (03/11/2026)

- O STM32 pode ser usado como contador de pulsos em Stop 2. O pulso o acorda, ele fica acordado contando e só dorme de novo depois de um tempo sem pulso (60 s, configurável). Com pulso constante não acorda e dorme a cada pulso.
- O ESP32 só é acordado para transmitir: 1, 2 ou 5 vezes ao dia, ou por uma entrada analógica. O despertar para transmitir e o RTC são de outra sprint.
- Cada entrada tem um totalizador de 32 bits que só soma, guardado fora da RAM. Os dados enviados levam o total absoluto, e o servidor calcula as diferenças. Nenhum pulso pode ser perdido antes da transmissão.
- Dormindo, a aquisição analógica 4–20 mA para. O consumo em sleep ainda não foi medido.
- O modo sleep liga e desliga por uma função (modo de operação 40412). Ele só liga se nenhuma saída estiver em frequência, e a saída em frequência só pode ser habilitada com o sleep desligado, porque o temporizador que a gera não roda em Stop 2.
- Regras completas em `documentacao/configuracao_do_dispositivo.md`; estado em `planejamento/sprints/SPRINT_01B.md`.

## Formas de envio

| Gatilho | Comportamento informado |
|---|---|
| Envio programado | Transmissão de lotes ou de um único payload da fila |
| Comando recebido via MQTT | Fazer nova medição do dado/canal ou grupo solicitado e enviar o resultado |
| Alarme do módulo/dispositivo | Um aviso quando o alarme é ativado e uma mensagem quando volta ao normal |

O usuário esclareceu que a solicitação remota deve provocar **nova medição**, em vez de consultar a última leitura ou o histórico. Quais canais compõem cada grupo será definido no catálogo de comandos. Alarmes podem despertar o equipamento e enviar alertas, conforme confirmação do usuário. Comandos pela rede devem ser recebidos e respondidos nas condições de disponibilidade do modo NB-IoT que será validado.

O entendimento já alinhado de comunicação Modbus RTU continua vigente. A representação e eventual divisão de um lote em mensagens/quadros ainda não foram definidas; o requisito de lotes não altera automaticamente o formato RTU.

## Alarmes por sinais e regras combinadas — B4

Em 30/10/2026, Rodrigo Maluf informou que os alertas poderão ser definidos por porta/sinal ativo e que suas regras devem permitir combinar condições de acordo com a ligação do sistema.

- Cada regra poderá conter **até quatro condições**, ligadas por **até três operadores** `&&` (E) ou `||` (OU), conforme esclarecimento do usuário.
- As condições podem envolver sinais de diferentes origens, incluindo entrada digital, entrada analógica e dados via Modbus RTU/RS-485. A associação aos sinais dependerá do que estiver ativo/configurado no equipamento.
- Para valores numéricos, as condições devem oferecer os operadores `<`, `>`, `<=`, `>=` e `==`. Para portas digitais, devem permitir comparação com os estados **ligada** e **desligada**.
- O significado elétrico de ligada/desligada para cada instalação ainda será definido; não presumir que um nível físico específico represente sempre o mesmo estado lógico.
- Exemplo informado: estado de bomba ligada/desligada por entrada digital ou Modbus combinado com pressão de saída por sensor analógico. Os alertas dependem do estado da bomba e são diferentes para bomba ligada e desligada.
- Exemplos **ilustrativos, não limiares ou alarmes aprovados**: `(bomba ligada) && (pressão abaixo de X)` e `(bomba desligada) && (pressão acima de Y)`. Os valores e as condições reais serão cadastrados conforme a instalação.
- Assim que a condição configurada de alarme for detectada, gerar **um aviso de alarme**, sem exigir tempo adicional de permanência para o disparo inicial. Quando a condição voltar ao normal, gerar **uma mensagem de retorno ao normal** após o período configurado de estabilidade. Não gerar avisos periódicos adicionais apenas porque a condição continua ativa.
- Para o retorno ao normal, o usuário definiu uma **histerese de tempo configurável por alarme, com mínimo de um minuto**: a condição deve permanecer normal pelo período configurado antes do envio da mensagem de normalização. Se voltar à condição de alarme antes de completar o tempo, a contagem é reiniciada.
- Dois minutos foi o exemplo inicialmente informado; ainda não foi definido como valor padrão. Esse requisito descreve um tempo de estabilidade; um eventual limite numérico diferente para retorno ainda não foi solicitado.
- A regra de uma notificação por evento não elimina as tentativas de entrega e confirmação já definidas: se o envio falhar, manter e reenviar o mesmo aviso conforme a política de comunicação. O tratamento de duplicatas será detalhado.
- O comportamento já confirmado de um alarme poder despertar o equipamento e enviar alerta continua válido. Detecção durante sleep e tratamento de leituras indisponíveis continuam a detalhar.
- **Falha de sensor:** o usuário confirmou envio de alerta ao sistema quando ocorrer falha, identificando o sensor, a porta e a falha detectada. Leitura normal de zero com 4 mA, sem condição de falha, indica sensor sem carga e não deve gerar alerta de falha. A saída em frequência associada e sua recuperação seguem os requisitos em [configuração do dispositivo](configuracao_do_dispositivo.md).

As condições são avaliadas **da esquerda para a direita**, na ordem configurada, conforme escolha do usuário. Assim, `A || B && C` é avaliada como `(A || B) && C`, sem dar precedência automática ao operador E. O eventual suporte a agrupamentos definidos pelo usuário não foi especificado. Não foi escolhida forma de armazenar ou implementar essas regras.

## Sem conectividade

O usuário confirmou que o STM32 deverá continuar coletando, armazenar as medições na flash e enviá-las quando houver reconexão. No modo sleep, essas atividades respeitam as regras configuráveis de aquisição e despertar; uma coleta isolada não exige acordar o rádio.

## Fila de envio e confirmação do servidor

A intenção do usuário é capturar as medições, guardá-las em uma fila na memória e transmiti-las. A fila serve aos dados pendentes de entrega:

1. Capturar e guardar os dados.
2. Transmitir no momento programado ou solicitado, em lote ou payload individual.
3. Fazer até **cinco tentativas de entrega** no ciclo. Intervalos e timeouts ainda serão definidos.
4. Se não houver confirmação após as cinco tentativas, manter os dados na memória para envio posterior.
5. Somente após o servidor confirmar que recebeu e salvou o lote ou payload identificado, retirar os dados correspondentes da fila e liberar o espaço. O arquivo pode ser apagado quando os dados nele contidos estiverem confirmados; uma confirmação parcial não autoriza excluir dados ainda pendentes do mesmo arquivo.

Não haverá retenção local deliberada dos dados após a confirmação. O histórico de longo prazo no servidor não foi especificado aqui. O usuário aceitou confirmação do recebimento e salvamento pelo sistema de destino, identificando o lote ou payload recebido. Codificação da confirmação, identificação, tratamento de duplicatas e confirmação perdida ainda serão definidos.

## Memória cheia

Quando o espaço destinado aos dados estiver cheio, continuar coletando e substituir o **arquivo mais antigo**, conforme escolha confirmada pelo usuário. Isso pode descartar dados ainda não confirmados pelo servidor. O sistema de arquivos, o tamanho e o critério de fechamento dos arquivos ainda serão definidos; registro ou aviso da perda não foram confirmados.

A substituição do arquivo mais antigo vale quando o espaço da fila estiver cheio; arquivos recebidos e confirmados pelo servidor já devem ser apagados pela regra normal. Prevenção de duplicatas, confirmação perdida e organização dos arquivos continuam a detalhar.

## Armazenamento de alertas

Os alertas devem ter um espaço separado da telemetria, que não precisa ser grande. Após a sincronização confirmada pelo servidor, o alerta pode ser apagado. Se o espaço de alertas estiver cheio, substituir o alerta mais antigo pelo novo, mesmo que o antigo ainda não tenha sido confirmado. Em 01/11/2026, o usuário definiu que o espaço de alertas tem **capacidade por número fixo de registros**; a quantidade máxima é de **300 alertas**, definida em 01/11/2026. Os eventos que entram nesse espaço são: **alarmes de condição**, **falha de sensor**, **falha de comunicação Modbus** e **aviso de hora inválida**. Também entram o **resultado de atualização** e o **comando com falha** (com o motivo). Os demais eventos ficam fora do espaço de alertas até nova decisão.

## Pontos a esclarecer

- Intervalos e horários de aquisição, envio e despertar; configuração global ou por sensor.
- Validar disponibilidade de comandos remotos no modo de baixo consumo do módulo NB-IoT e na operadora; sinal de despertar do STM32 e latência aceitável. Em 01/11/2026, o usuário definiu que a validação será feita por **teste com o módulo real e chip da operadora**, medindo recepção, despertar do STM32 e latência. O teste entra no protótipo de bancada do primeiro mês.
- Origem e detecção dos alarmes durante sleep; tratamento de leituras indisponíveis e valor padrão da histerese. Um aviso na ativação e uma mensagem após tempo normal configurável por alarme, com mínimo de um minuto, já definidos; despertar e envio por alarme já requeridos.
- Associação dos canais aos comandos de medição individual e por grupo; nova medição sob comando já definida.
- Limites de armazenamento, organização dos arquivos e codificação da confirmação de lotes/payloads. Cinco tentativas por ciclo e exclusão somente após recebimento/salvamento confirmado já definidos; intervalos e critérios do próximo ciclo a detalhar.

## Validade do horário

Dados sem horário válido não têm validade para a telemetria. Ao detectar hora inválida, o equipamento deve informar o problema e solicitar manutenção. Sem conexão, guardar o aviso para envio após a reconexão, identificando que o horário da ocorrência é desconhecido. O tratamento de outros eventos sem hora válida permanece em definição.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 03/11/2026 | 0.1.29 | Registrada a exclusão mútua entre o modo sleep e a saída em frequência |
| 03/11/2026 | 0.1.28 | Adicionada a seção de contador de pulsos e sleep: despertar por pulso, ESP32 só para transmitir e totalizador absoluto |
| 01/11/2026 | 0.1.27 | Usuário definiu 300 alertas como capacidade do espaço de alertas |
| 01/11/2026 | 0.1.26 | Usuário definiu que o teste NB-IoT entra no protótipo de bancada do primeiro mês |
| 01/11/2026 | 0.1.25 | Usuário definiu que o envio usa média móvel dos valores de 1 segundo, com janela a definir |
| 01/11/2026 | 0.1.24 | Usuário definiu validação de recepção NB-IoT em baixo consumo por teste com módulo real e operadora (P-026) |
| 01/11/2026 | 0.1.23 | Usuário incluiu resultado de atualização e comando com falha no espaço de alertas |
| 01/11/2026 | 0.1.22 | Usuário incluiu aviso de hora inválida no espaço de alertas; quantidade máxima de registros será definida na implementação |
| 01/11/2026 | 0.1.21 | Usuário definiu capacidade do espaço de alertas por número fixo de registros e eventos alarme de condição, falha de sensor e falha Modbus |
| 01/11/2026 | 0.1.18 | Usuário confirmou continuar coletando e substituir os dados mais antigos com memória cheia |
| 01/11/2026 | 0.1.19 | Usuário definiu espaço separado e pequeno para alertas, apagados após sincronização |
| 01/11/2026 | 0.1.20 | Usuário confirmou substituir o alerta mais antigo quando o espaço reservado estiver cheio |
| 31/10/2026 | 0.1.16 | Usuário definiu que dados sem horário válido não têm validade e exigiu informação do problema e solicitação de manutenção |
| 31/10/2026 | 0.1.17 | Usuário confirmou guardar aviso de falha do relógio sem conexão e enviá-lo após reconexão com horário da ocorrência desconhecido |
| 30/10/2026 | 0.1.0 | Registrados setup de intervalos, modos contínuo/sleep, lotes, comandos MQTT, alarmes, armazenamento offline e substituição do arquivo mais antigo |
| 30/10/2026 | 0.1.1 | Esclarecidos despertares independentes de coleta/envio, rádio em sleep entre transmissões, recepção NB-IoT e despertar por alarme; registrada validação de disponibilidade PSM/eDRX sem escolher implementação |
| 30/10/2026 | 0.1.2 | Usuário confirmou o resumo de coleta, envio, sleep, despertar por alarme e armazenamento; mantidos detalhes de dados e validação NB-IoT em aberto |
| 30/10/2026 | 0.1.3 | Definidas nova medição sob comando MQTT e fila de envio com retenção em falha e exclusão do arquivo após recebimento confirmado pelo servidor |
| 30/10/2026 | 0.1.4 | Resumo consolidado do B2 confirmado pelo usuário; detalhes de implementação e validação NB-IoT mantidos para etapas posteriores |
| 30/10/2026 | 0.1.5 | B3 detalhou até cinco tentativas por ciclo, fila preservada em falha e confirmação de recebimento/salvamento para lote ou payload individual antes de excluir os dados correspondentes |
| 30/10/2026 | 0.1.6 | B4 registrou regras de alerta por sinal ativo com até quatro concatenações usando E/OU, combinando entradas digitais, analógicas e Modbus; contagem e agrupamento a esclarecer |
| 30/10/2026 | 0.1.7 | Usuário definiu comparações numéricas <, >, <=, >= e == e estados ligada/desligada para portas digitais |
| 30/10/2026 | 0.1.8 | Usuário esclareceu limite de até quatro condições por regra, ligadas por até três operadores E/OU |
| 30/10/2026 | 0.1.9 | Usuário escolheu avaliação das condições combinadas da esquerda para a direita |
| 30/10/2026 | 0.1.10 | Usuário definiu um aviso de alarme na ativação e mensagem quando voltar ao normal; mantidas tentativas de entrega da comunicação |
| 30/10/2026 | 0.1.11 | Usuário definiu histerese temporal de dois minutos em condição normal antes da mensagem de normalização |
| 30/10/2026 | 0.1.12 | Usuário definiu histerese de normalização configurável por alarme com mínimo de um minuto; dois minutos mantidos como exemplo |
| 30/10/2026 | 0.1.13 | Usuário definiu disparo inicial assim que a condição de alarme for detectada; histerese de tempo aplica-se ao retorno ao normal |
| 30/10/2026 | 0.1.14 | Usuário confirmou alerta por falha de sensor, sem alerta para leitura zero normal a 4 mA |
| 30/10/2026 | 0.1.15 | Usuário confirmou identificação de sensor, porta e falha no alerta de sensor |

