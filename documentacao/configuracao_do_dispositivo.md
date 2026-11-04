# Configuração do dispositivo — oLoggerIOT

> **Criado em:** 29/10/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.52

## Origem e estado

Proposta funcional apresentada por Rodrigo Maluf durante o B2, em 29/10/2026, e detalhada no B4 em 30/10/2026. B2 e B3 foram confirmados; B4 continua em levantamento. Este documento também alimenta B5 (perfis), B7 (aplicativo), B8 (WhatsApp) e os blocos de segurança.

## Objetivo

Permitir que integradores configurem o dispositivo por linguagem natural no WhatsApp. O sistema em desenvolvimento recebe as solicitações e gera comandos estruturados para o firmware. O STM32 continua responsável pelo dispositivo, pela execução dos comandos e pelas respostas. A interpretação de linguagem natural fica no sistema; não foi solicitado processamento de linguagem natural no STM32.

## Instalação com esforço mínimo em campo — requisitos do B4

A Optmile pretende operar com vários integradores e instaladores. O objetivo é reduzir o trabalho de parametrização na instalação: o técnico deixa o equipamento online e a configuração mais complexa é conduzida pelo CCO através do sistema.

| Perfil | Responsabilidade e alcance informados |
|---|---|
| Técnico de campo / instalador | Acesso mínimo necessário para colocar o dispositivo online e permitir que ele receba os comandos de parametrização |
| Integrador | Pode configurar o dispositivo inteiro, incluindo calibração dos sensores e parâmetros Modbus |
| CCO | Controla a parte complexa da configuração através do sistema |

As configurações explicitamente citadas incluem **calibração dos sensores, parâmetros Modbus, rede Wi-Fi e APN do chip celular**. Wi-Fi e APN são exemplos do mínimo de conectividade em campo; a lista exata de ações permitidas ao instalador ainda será definida. Não atribuir ao técnico de campo acesso completo à calibração/Modbus somente porque essas funções estão disponíveis ao integrador.

As demais configurações serão enviadas pelo sistema. Não foi estabelecida exclusividade absoluta do CCO sobre todos esses parâmetros: o usuário também definiu que o integrador pode configurar o dispositivo inteiro. A matriz detalhada de permissões será levantada no B5.

Fluxo funcional esperado: instalação física → configuração mínima de conectividade → equipamento online → recebimento dos comandos do sistema para parametrização. A interação preferencial por WhatsApp e o aplicativo gateway USB continuam válidos. Quando não houver acesso ao servidor, o modo local permanece restrito ao necessário para estabelecer conectividade.

Ainda a levantar no B4: detalhes dos parâmetros de calibração, catálogo de parâmetros Modbus e prioridades de funcionalidades para bancada/piloto. Não foi escolhida estrutura de implementação.

## Calibração e configuração dos sensores — respostas do B4

Rodrigo Maluf definiu o seguinte comportamento:

- Permitir cadastrar os parâmetros do sensor.
- Disponibilizar/enviar a tensão medida na entrada da porta analógica para apoiar a calibração.
- Para sensores 4–20 mA, incluir **span do sensor e offset** na parametrização de calibração.
- Sensores digitais **não terão configuração local**. Isso não define, por si só, o catálogo de eventuais parâmetros remotos.

A tensão da entrada é uma informação de apoio à calibração, além do valor de medição convertido para a grandeza do sensor. O fluxo de acesso permanece sujeito ao perfil do usuário e ao canal disponível; não ampliar automaticamente a permissão do técnico de campo.

O usuário confirmou: **span é a faixa de medição do sensor**, com exemplo de 10 bar no caso 4–20 mA correspondente a 0–10 bar; **offset permite ajustar o zero do sensor**. O valor de 10 bar é o exemplo informado, não uma constante para todos os sensores. Não foi solicitado um fator de ganho adicional independente do span. Convenção de sinal, limites e precisão do offset ainda serão detalhados, sem fixar fórmula nesta etapa. Não foram definidos número de pontos, cálculo automático ou processo de calibração dos sensores diferenciais. O destino e o formato da leitura de tensão serão detalhados junto aos comandos.

## Acionamento das saídas e partida após falta de energia — B4

Em 30/10/2026, Rodrigo Maluf definiu que as saídas deverão permitir:

- **Ligar e desligar** por comando.
- **Acionar por um tempo:** é o **retardo de acionamento** da saída (esclarecido em 03/11/2026). Não há desligamento automático ao final de um intervalo; ver "Saídas digitais — implementação" abaixo.
- **Relé de partida com atraso configurável:** depois da falta de energia, o sistema volta e informa que está normal. Um novo comando de ligar inicia a contagem; ao terminar o tempo configurado, a porta digital é ligada.

O requisito descreve o comportamento funcional esperado; não decide se o relé temporizado será implementado em firmware ou por componente externo. O tempo de partida será **configurável por saída**, conforme definição do usuário; valores, limites e padrão ainda serão esclarecidos.

Após o retorno da energia, enviar mensagem de que o sistema está normal e aguardar **um novo comando para ligar**; não religar automaticamente. Ao chegar esse comando, iniciar o temporizador e ligar a porta quando ele terminar. A mensagem de sistema normal não aguarda esse temporizador, pois ele começa somente com o comando. A correção do usuário substitui a interpretação anterior de contar o atraso a partir do retorno de energia.

Comandos seguem as confirmações já definidas no B3: recebido e depois resultado da execução. No acionamento com atraso, recebimento ocorre ao chegar o comando e execução concluída deve refletir a ligação após o temporizador. Se chegar um **comando de desligar durante a espera**, cancelar a partida e manter a saída desligada, conforme definição do usuário. O formato do resultado do comando de partida cancelado e o tratamento de outros comandos adicionais ainda serão definidos.

## Controle local de saída por entrada analógica — B4

Em 31/10/2026, o usuário informou que uma regra configurada no dispositivo poderá controlar o estado de uma porta digital a partir de uma entrada analógica, inclusive sem depender de comunicação com o sistema. A configuração pode ser feita na instalação ou enviada pelo CCO.

- A entrada analógica e a saída digital podem ser associadas conforme a configuração do equipamento; qualquer entrada analógica disponível pode ser vinculada a qualquer saída digital disponível.
- Uma mesma entrada analógica pode controlar mais de uma saída digital. O compartilhamento de uma saída por múltiplas entradas ou regras não foi definido.
- Exemplo informado pelo usuário: **ao atingir 5 bar, ligar; ao atingir 4 bar, desligar**. Esses valores ilustram uma regra com dois limiares e não são valores fixos para todas as instalações.
- Limiares, sentido das comparações e associação de portas devem ser parametrizáveis de acordo com a instalação. A forma de implementar e os limites ainda serão detalhados.
- Em modo automático, se agenda e regra do sensor solicitarem ações diferentes para a mesma saída, **prevalece a regra do sensor**. Se o sensor usado para controlar a saída discreta falhar, **a saída deve ser desligada**. Em modo manual, o usuário esclareceu que se mantém o comando manual enviado mesmo diante da falha do sensor; a falha continua gerando alerta ao sistema.
- A interação do acionamento por sensor com o atraso de partida configurável por saída e com a regra de novo comando após falta de energia ainda deverá ser esclarecida.

Este controle por limiares é uma funcionalidade adicional à conversão proporcional 4–20 mA para frequência em Hz descrita a seguir. A matriz de permissões de quem pode configurá-lo será detalhada no B5.

## Modos manual e automático — B4

O usuário definiu que cada **porta de saída** pode operar em **modo manual ou automático**: o comando altera o modo da porta à qual se destina, sem mudar automaticamente o modo das demais. O comando recebido deve informar se é manual ou automático; a origem na plataforma não determina o modo por si só.

- Um **comando manual** direto prevalece sobre agenda e regras automáticas da porta de destino. Essa porta passa a operar em modo manual e o dispositivo envia ao CCO um alerta informando a chegada do comando e a operação manual.
- A porta permanece em modo manual **até receber um comando de modo automático**. Comandos automáticos provenientes de regras da plataforma, da agenda ou do sensor local são identificados como automáticos.
- Ao receber o comando de retorno ao automático, a porta deve **avaliar e executar imediatamente a regra automática válida naquele momento**, sem aguardar a próxima leitura programada. Permanecem aplicáveis a prioridade do sensor sobre a agenda e as regras de partida já definidas. Se for necessária uma leitura nova ou se o dado estiver indisponível, o comportamento ainda será especificado.
- Em modo automático, se houver conflito na mesma saída, **a regra do sensor local prevalece tanto sobre a agenda quanto sobre uma regra automática da plataforma**, conforme definições do usuário. A agenda local só atua quando o módulo estiver offline. Na reconexão, o dispositivo informa que a conexão voltou, mantém o último estado da porta e aguarda novo comando da plataforma; as regras locais e o modo manual continuam aplicáveis. A detecção de conectividade e o tratamento de ordens pendentes ainda serão detalhados.
- Um comando originado no sistema pode ser manual ou automático, conforme sua classificação; não presumir que todo comando da nuvem seja manual.

Em caso de falha do sensor vinculado, **em automático a saída desliga; em manual mantém o comando manual enviado**. A falha do sensor continua sendo comunicada ao sistema conforme a regra de alerta definida. Para saída em modo frequência, a regra específica de 0 Hz em falha permanece registrada; a interação desse modo com comando manual ainda não foi especificada.

Detalhar quando o alerta ao CCO é entregue durante desconexão, confirmação dos comandos e comportamento após reinício. Permanecem aplicáveis as confirmações de recebido e resultado e as regras de fila já registradas, com a semântica de cada evento a especificar.

## Modos da saída digital — B4

O usuário definiu que uma saída digital pode ser usada em dois modos:

- **Discreto:** ligada ou desligada, incluindo as ações e a partida temporizada já descritas.
- **Frequência:** saída com frequência determinada, em Hz; pode representar uma leitura de entrada analógica de 4–20 mA.

A entrada analógica interligada à saída digital em frequência poderá ser **escolhida na configuração**, conforme o usuário. O usuário confirmou que a frequência correspondente a **4 mA** e a frequência correspondente a **20 mA** serão configuráveis, com conversão proporcional dos valores intermediários. O exemplo 4 mA → 0 Hz e 20 mA → 100 Hz é ilustrativo; não foi definido como padrão obrigatório.

Quando o sensor/entrada falhar, a saída deve ir para **0 Hz**. Após o sensor voltar a funcionar, aguardar **um minuto em funcionamento normal** antes de retomar a replicação da leitura. Se a falha retornar durante a espera, a saída continua em 0 Hz e a contagem de recuperação recomeça.

**Critérios de falha informados pelo usuário:** corrente abaixo de 4 mA, corrente acima de 20 mA ou salto entre leituras consecutivas superior a um **percentual configurável da leitura anterior**. A janela de menos de 5 segundos foi informada anteriormente. Os limites de corrente são estritamente abaixo de 4 mA e acima de 20 mA; os próprios valores de 4 e 20 mA não foram alterados. Quando detectada uma dessas falhas, aplica-se o comportamento de 0 Hz e recuperação acima.

O usuário confirmou que uma **falha do sensor deve gerar alerta ao sistema**, identificando **o sensor, a porta e a falha detectada**. Uma leitura normal de zero, com 4 mA e sem condição de falha, indica sensor sem carga e **não gera alerta de falha**. Nesse caso, a saída em frequência segue o mapeamento válido configurado para 4 mA; 0 Hz por falha não deve ser aplicado apenas por a grandeza medida ser zero.

O usuário esclareceu que sua referência anterior a **escala logarítmica** dizia respeito à suavização de pequenas oscilações, como 5,25 → 5,29 → 5,20, preservando a identificação de falhas. Não foi solicitada uma transformação matemática logarítmica da grandeza. A detecção de falhas deve permanecer eficaz apesar da suavização. A técnica de detecção e filtragem não foi definida.

- A saída em Hz continua **proporcional à corrente 4–20 mA**, como confirmado pelo usuário.
- O usuário esclareceu que a comparação deve ocorrer entre **leituras consecutivas**: depois de ler 5 bar, a próxima leitura de 8 bar ou de 1 bar representa um salto que não deve ser aceito como normal.
- O usuário escolheu percentual configurável **da leitura anterior**, substituindo a referência à faixa total. Com leitura anterior de 5 bar, saltos para 8 bar e 1 bar correspondem, respectivamente, a 60% e 80% da leitura anterior. Os 50% antes citados são uma referência de exemplo; não foram definidos como valor padrão obrigatório.
- O usuário determinou considerar leituras **acima de 1% da escala do sensor** e informou que, quando a grandeza é zero, o sensor está sem carga. Assim, uma leitura zero da grandeza não é, por si só, falha do sensor. Os limites elétricos abaixo de 4 mA e acima de 20 mA continuam válidos; no exemplo de sensor 0–10 bar, 4 mA representa 0 bar e é diferente de corrente abaixo de 4 mA.
- O usuário escolheu **manter o valor medido das leituras positivas até 1% da escala**, sem usá-las na comparação percentual de saltos. Elas não são automaticamente convertidas para zero nem classificadas como falha por essa regra. A comparação usa leituras acima de 1%; em 01/11/2026, o usuário definiu que a referência da comparação percentual é a **última leitura acima de 1% da escala**, mesmo que leituras menores tenham ocorrido depois. Em 01/11/2026, o usuário definiu a amostragem: a taxa de cada entrada analógica é configurável no device entre **10 e 1000 amostras por segundo**. Mais amostras melhoram a qualidade do dado e aumentam o consumo de energia. Há duas etapas. (1) Dentro de cada segundo, as N amostras (de 10 a 1000) são reduzidas a um valor pela média simples. (2) Para o envio, os valores de 1 segundo são combinados por **média móvel**; a janela ainda não foi definida. Em 01/11/2026, o usuário definiu que o envio é a média móvel por envio, e que a comparação percentual usa o **valor de 1 segundo**. Após uma leitura zero (sensor sem carga) e o retorno a valores positivos, a primeira leitura acima de 1% não passa pela comparação percentual de saltos.
- Em 01/11/2026, o usuário explicou que **1% da escala é a variação normal do sensor**: em um sensor de 6 bar, a variação é de 0,06 bar. Propôs elevar para **2%** para maior segurança. Esclarecido na mesma data: o critério de falha é o **dente de serra**, isto é, a inversão de sentido entre leituras consecutivas acima do percentual configurado. Variações lineares ou logarítmicas, para cima ou para baixo, são operação normal, mesmo acima de 2%.
- **Porta sem inversor de frequência:** o percentual padrão é 2%. Dente de serra acima desse percentual gera falha, seguindo o padrão de falha de sensor.
- **Porta configurada como inversor de frequência:** o percentual é definido pelo sistema e pode ser maior, com padrão de 10%, porque o inversor acelera e desacelera conforme o PID configurado nele. Variações dentro do percentual configurado são normais.
- **Na falha por dente de serra:** a saída em frequência associada vai para 0 Hz, como nas demais falhas de sensor, com retorno após um minuto de funcionamento normal. O alerta informa o problema, e o valor real é enviado sem suavização e sem zerar o dado, para que o sistema reconheça a falha.
- Janela de 5 segundos confirmada pelo usuário em 01/11/2026 para identificar a inversão de sentido como dente de serra.
- Em 01/11/2026, o usuário definiu que a escala logarítmica **não é requisito**: variação linear ou logarítmica é operação normal. A média móvel de envio usa janela de **um envio**. Não foram escolhidos algoritmos de implementação.

A frequência de atualização e a relação entre quantidade de entradas e saídas ainda não foram definidas. Não foram definidos largura de pulso, duty cycle, estado elétrico em 0 Hz ou forma de geração do sinal.

## Entrada digital — respostas de 03/11/2026

- **Sinal ativo:** a entrada é ativa quando há tensão, ou seja, em nível alto.
- **Filtro de ruído:** a mudança de estado só é aceita depois de um tempo estável. Tempo padrão de **50 ms**, definido pelo responsável técnico em 03/11/2026. O firmware converte o tempo em número de amostras, arredondando para cima; a entrada é lida a cada 2 ms.
- **Tensão de campo:** de 12 a 24 V, a mesma fonte da entrada do CLP (03/11/2026). Na placa de bancada, a D1 foi retirada e o limite é 12 V, com o resistor de 4K7 em 0402 mantido até o novo layout (0603). Análise do circuito em `analise_hardware_ologgeriot.md`, achado HW-008.
- **Polaridade no pino:** no OloggerMulti o optoacoplador inverte o sinal. Com tensão no campo, o pino do STM32 fica em nível baixo; o firmware entrega o estado já corrigido (ativa = há tensão).
- **Sleep:** a entrada pode acordar o STM32 do sleep.
- **Contador de pulsos (03/11/2026):** o device pode ser usado como contador de pulsos, para economizar energia. O pulso acorda o STM32 (Stop 2), que fica acordado contando e só dorme de novo depois do tempo sem pulso, de 60 s por padrão e configurável; com pulso constante ele não acorda e dorme a cada pulso. Dormindo, só a entrada digital fica ativa. Os pulsos vêm de leitores, sem repique, a 5 a 10 por segundo no máximo, com até quatro leitores ao mesmo tempo. O filtro dos pulsos acordado é de 20 ms. Cada entrada tem um totalizador de 32 bits, como um hodômetro: só soma, volta a zero ao estourar, é guardado fora da RAM e sobrevive a reset e à falta de energia; o servidor calcula as diferenças. O ESP32 só é acordado para transmitir (1, 2 ou 5 vezes ao dia, ou por uma entrada analógica), em outra sprint. Contar dormindo é preferível, mas nenhum pulso pode ser perdido antes da transmissão. Detalhes e pendências em `planejamento/sprints/SPRINT_01B.md`.
- **Tipos de uso:** a entrada pode ser discreta (0 ou 1), contador de pulsos ou de frequência em Hz (faixa de projeto de 0 a 20 kHz; faixa máxima, janela de medição e filtro do modo de frequência ainda a definir).
- **Implementação:** o módulo `lib/io` atende a funcionalidade `ENTRADA_DIGITAL` do OloggerMulti. A escolha entre os modos fica em cada entrada, na configuração.
- **Itens de backlog:** FW-052 a FW-060, na fase 1B (SPRINT_01B).

## Saídas digitais — implementação (03/11/2026)

- **Circuito:** cada saída é um transistor NPN em coletor aberto: ligada é `OUT-n` puxado ao terra. Pino de controle em nível alto liga. A carga é externa.
- **Estado na partida:** todas as saídas partem desligadas. Não religam sozinhas depois de falta de energia: aguardam novo comando, conforme a regra do B4.
- **Comandos implementados:** ligar e desligar. O "acionamento por tempo" é o retardo de acionamento (próximo item).
- **Retardo de acionamento (relé de tempo, confirmado em 03/11/2026):** a saída discreta pode ter um retardo configurável por saída, em segundos. **Todo comando que liga a saída espera o retardo**; retardo zero liga na hora. Um comando de desligar durante a espera cancela a partida. Com o retardo em andamento, outro comando de ligar é descartado e a contagem continua. Com a saída já ligada, outro comando de ligar é descartado (o retardo não é reaplicado).
- **Modo manual e automático (FW-025, 04/11/2026):** cada saída tem o seu modo. O comando manual direto prevalece, põe a saída em manual e vale até o comando de modo automático. Em automático, o pedido do sensor local prevalece sobre o da fonte externa (agenda, só offline, ou regra da plataforma). Falha do sensor: em automático desliga, em manual mantém o comando manual. A volta ao automático avalia e executa a regra válida na hora. Não especificado e tratado de forma conservadora, a confirmar (P-040): fim da falha reaplica o pedido válido; volta ao automático sem pedido mantém o estado; depois de reinício a saída volta desligada e em automático; o alerta ao CCO na chegada do comando manual depende do espaço de alertas e ainda não existe.
- **Ainda não implementado:** saída em frequência (FW-023) e o alerta ao CCO na chegada do comando manual.
- **"Pulsada" é a saída em frequência (decidido em 03/11/2026):** não é acionamento por tempo. A saída discreta é só ligada ou desligada, com o retardo de acionamento. O tipo da saída no mapa Modbus ficou 0 = discreto e 1 = frequência.
- **Sleep e frequência (decidido em 03/11/2026):** a saída em frequência só pode ser habilitada se o modo sleep não estiver ativo, e o sleep só liga se nenhuma saída estiver em frequência. O sleep tem uma função de ligar e desligar (modo de operação 40412).
- **Carga indutiva:** o diodo de roda livre fica no relé externo, instalado pelo responsável técnico.
- **Stop 2:** o pino mantém o estado, mas o retardo não corre durante o sleep.

## Fluxos propostos pelo usuário

### 1. Dispositivo com conectividade própria

1. O integrador envia uma mensagem pelo WhatsApp.
2. O sistema recebe a solicitação e gera o comando para o dispositivo.
3. O comando chega pelo broker MQTT, atravessa a rede e o módulo de comunicação presente no modelo (rádio 4G, ESP32 com Wi-Fi ou outra tecnologia externa) e é encaminhado ao STM32.
4. O STM32 processa o comando e sua resposta percorre o caminho inverso pelo módulo de comunicação e pela rede até o broker MQTT.
5. O sistema apresenta o resultado ao integrador pelo WhatsApp.

Fluxo lógico bidirecional: **WhatsApp ↔ sistema ↔ broker MQTT ↔ rede ↔ módulo de comunicação externo ↔ STM32**.

O módulo é uma camada explícita do equipamento. O STM32 coordena essa comunicação; rádios são integrados pelos comandos AT do fabricante e o ESP32 tem firmware próprio em ESP-IDF. A localização do cliente MQTT (STM32 ou módulo) depende da integração e ainda não foi definida; o fluxo não presume conexão direta do STM32 à rede.

### 2. Celular como gateway USB

1. O técnico executa a aplicação Python no notebook e conecta um chaveiro serial entre o notebook e o STM32. Decisão de 01/11/2026; não há app de celular.
2. O integrador continua enviando suas solicitações pelo WhatsApp.
3. O servidor gera os comandos; o aplicativo os recebe e os encaminha ao STM32 pela interface serial via USB.
4. O STM32 responde pela serial; o aplicativo encaminha a resposta ao sistema, que retorna o resultado pelo WhatsApp.

Este caminho permite configurar o dispositivo pelo notebook do técnico. Depende de o notebook conseguir acessar o sistema. O WhatsApp é usado pelo celular do integrador. O protocolo entre aplicativo e servidor ainda não foi definido; não presumir que será MQTT.

**Estado do hardware:** Rodrigo Maluf confirmou que a parte elétrica está resolvida. O fluxo USB permanece como requisito; o MCU e o caminho de interface efetivamente adotados serão refletidos na implementação do firmware. O achado anterior sobre o STM32L071 nos arquivos recebidos permanece no [histórico da análise](analise_hardware_ologgeriot.md), sem constituir bloqueio elétrico atual. Essa confirmação não especifica, por si só, qual solução USB foi adotada.

### 3. Sem acesso ao servidor

A aplicação do notebook oferece comandos locais pela serial, limitados ao necessário para estabelecer a conectividade do dispositivo. O catálogo exato desses comandos será levantado. A operação local restrita se aplica ao modo sem acesso ao servidor; não limita os comandos enviados pelo servidor através do gateway USB.

Depois de estabelecida a conectividade do dispositivo, a configuração continua pelo WhatsApp. A proposta não exige um chat de linguagem natural offline no aplicativo.

## Responsabilidades

| Componente | Responsabilidade informada ou decorrente dos fluxos |
|---|---|
| WhatsApp | Canal preferencial de interação do integrador em linguagem natural |
| Sistema em desenvolvimento | Receber solicitações, gerar comandos e devolver os resultados |
| Aplicativo do integrador | Gateway entre servidor e serial USB; comandos locais restritos para estabelecer conectividade |
| STM32 | Executar os comandos, controlar o dispositivo e responder pelo canal utilizado |
| Rádios, ESP32 e outros módulos externos | Intermediar a comunicação entre o STM32 e a rede usada para alcançar o broker MQTT, conforme os recursos do modelo |

## Pontos a detalhar

- Formato único de comandos e respostas do firmware e relação com Modbus RTU. A escolha de Modbus para sensores/atuadores não define automaticamente o protocolo de configuração USB/MQTT.
- Interface USB do STM32, recursos exigidos do notebook, sistema operacional suportado e protocolo aplicativo ↔ servidor.
- Identificação do equipamento conectado e vinculação à conversa do integrador.
- Catálogo de comandos locais necessários para estabelecer conectividade.
- Tratamento de comandos ambíguos, validação de parâmetros e retorno de erro.
- Identificação e autorização do integrador, confirmação de ações que a exigirem e auditoria.
- Correlação de solicitações e respostas, comandos repetidos, expiração, perda de conexão e passagem do gateway USB para conectividade própria.
- Escopo de comandos por WhatsApp: configuração foi solicitada; autorização para outros acionamentos ainda será levantada.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.52 | FW-025: modo manual e automático por saída, prioridade do sensor local e falha do sensor |
| 04/11/2026 | 0.1.51 | Correções da verificação do doc-keeper antes do fechamento da SPRINT_00 (números, nomes dos registradores, concordância e referências) |
| 03/11/2026 | 0.1.50 | O "acionamento por tempo" é o retardo de acionamento; ligar com a saída ligada ou aguardando é descartado; mapa Modbus das saídas ajustado (P-037) |
| 03/11/2026 | 0.1.49 | Saídas digitais: "pulsada" é a saída em frequência, retardo de acionamento (relé de tempo), exclusão mútua entre frequência e sleep e diodo no relé externo |
| 03/11/2026 | 0.1.48 | Registrada a implementação das saídas digitais: circuito, estado na partida, comandos e o que falta |
| 03/11/2026 | 0.1.47 | Corrigidos o período de leitura (2 ms), os modos da entrada (discreto, contador de pulsos e frequência) e a faixa de itens do backlog (FW-052 a FW-060) |
| 03/11/2026 | 0.1.46 | Tensão de campo da entrada digital: D1 retirada na bancada e limite de 12 V até o novo layout |
| 29/10/2026 | 0.1.0 | Registrada configuração em linguagem natural pelo WhatsApp, gateway USB no celular e modo local restrito para estabelecer conectividade |
| 29/10/2026 | 0.1.1 | Explicitada a camada de módulos de comunicação e rede entre STM32 e broker MQTT, conforme correção do usuário |
| 29/10/2026 | 0.1.2 | Registrada dependência do gateway USB em relação ao MCU, com incompatibilidade no y7080 indicado pelos esquemáticos |
| 29/10/2026 | 0.1.3 | Registrada confirmação de parte elétrica resolvida; preservado requisito USB e histórico, sem presumir solução física específica |
| 30/10/2026 | 0.1.4 | B4 distinguiu instalador com acesso mínimo para conectividade, integrador com configuração completa e CCO na configuração complexa; registrados calibração, Modbus, Wi-Fi e APN |
| 30/10/2026 | 0.1.5 | Registrados cadastro dos parâmetros do sensor, envio da tensão da porta analógica, span e offset para 4–20 mA e ausência de configuração local de sensores digitais |
| 30/10/2026 | 0.1.6 | Usuário confirmou span como faixa de medição (exemplo 10 bar) e offset como ajuste de zero do sensor |
| 30/10/2026 | 0.1.7 | Usuário definiu ligar/desligar, acionamento por tempo e relé de partida com atraso após falta de energia; duração e comportamento ao fim da espera pendentes |
| 30/10/2026 | 0.1.8 | Usuário definiu atraso configurável, novo comando obrigatório para religar e mensagem de sistema normal; escopo do tempo e momento exato da mensagem a confirmar |
| 30/10/2026 | 0.1.9 | Usuário corrigiu a sequência: mensagem de sistema normal após retorno; novo comando inicia temporizador e a saída liga após o tempo configurado |
| 30/10/2026 | 0.1.10 | Usuário definiu tempo de partida configurável individualmente por saída |
| 30/10/2026 | 0.1.11 | Usuário definiu cancelamento de partida por comando de desligar e saídas digitais discretas ou por frequência, incluindo representação de entrada 4–20 mA em Hz |
| 30/10/2026 | 0.1.12 | Usuário confirmou frequências configuráveis em 4 mA e 20 mA e conversão proporcional dos valores intermediários |
| 30/10/2026 | 0.1.13 | Usuário definiu escolha da interligação entrada analógica/saída digital, 0 Hz em falha e retomada após um minuto de funcionamento normal |
| 30/10/2026 | 0.1.14 | Usuário informou falhas abaixo de 4 mA, acima de 20 mA ou histerese acima de 50% em menos de 5 s; registradas exigências de escala logarítmica e leitura sem dente de serra, com detalhes a esclarecer |
| 30/10/2026 | 0.1.15 | Usuário confirmou 50% da faixa total, saída em Hz proporcional e eliminação de oscilações rápidas sem mascarar variações abruptas repetidas; critérios de oscilação e escala logarítmica ainda a detalhar |
| 30/10/2026 | 0.1.16 | Usuário esclareceu comparação entre leituras consecutivas e suavização de pequenas oscilações; registrado conflito dos exemplos 5→8 e 5→1 com limiar anterior de 50% da faixa |
| 30/10/2026 | 0.1.17 | Usuário escolheu limite percentual configurável baseado na leitura anterior; substituída referência à faixa total, com tratamento de zero e padrão ainda pendentes |
| 01/11/2026 | 0.1.36 | Gateway do técnico passa a ser aplicação Python no notebook com chaveiro serial; não há app de celular; celular usa somente WhatsApp |
| 01/11/2026 | 0.1.35 | Finalizadas pendências: janela de 5 s confirmada, padrão de 10% com inversor, média móvel com janela de um envio e escala logarítmica não é requisito |
| 01/11/2026 | 0.1.34 | Usuário confirmou que a saída em frequência vai para 0 Hz na falha por dente de serra, com retorno após um minuto normal |
| 01/11/2026 | 0.1.33 | Usuário definiu que a falha é o dente de serra (inversão de sentido acima do percentual), com 2% padrão para porta sem inversor e percentual configurado pelo sistema para porta com inversor de frequência; variação linear ou logarítmica é normal; na falha, alerta e valor real sem suavização nem zeragem |
| 01/11/2026 | 0.1.32 | Usuário explicou 1% da escala como variação normal do sensor e propôs 2% para margem de segurança; falha por salto segue com limite percentual configurável. Corrigido o rótulo "salto de 1%", que misturava piso de comparação e limite de falha; aplicação do percentual pendente de confirmação |
| 01/11/2026 | 0.1.31 | Usuário separou a integração por segundo (média simples das amostras) do envio por média móvel; definiu envio por média móvel e salto de 1% sobre o valor de 1 segundo |
| 01/11/2026 | 0.1.30 | Usuário definiu amostragem de 10 a 1000 amostras por segundo por entrada, integração por segundo com média de 10 leituras, envio acumulado e primeira leitura após zero sem comparação percentual (P-027) |
| 01/11/2026 | 0.1.29 | Usuário definiu a última leitura acima de 1% da escala como referência da comparação percentual (P-027); amostragem pendente |
| 30/10/2026 | 0.1.18 | Usuário definiu considerar leituras acima de 1% da escala e informou que grandeza zero significa sensor sem carga; abrangência de leituras até 1% a esclarecer |
| 30/10/2026 | 0.1.19 | Usuário escolheu preservar leituras positivas até 1% da escala, excluindo-as somente da comparação percentual de saltos |
| 30/10/2026 | 0.1.20 | Usuário confirmou alerta em falha do sensor e ausência de alerta para leitura zero normal a 4 mA |
| 30/10/2026 | 0.1.21 | Usuário confirmou que o alerta de falha deve identificar o sensor, a porta e a falha detectada |
| 31/10/2026 | 0.1.22 | Usuário definiu controle local de saídas digitais por limiares de entrada analógica, com associações configuráveis e uma entrada podendo controlar várias saídas |
| 31/10/2026 | 0.1.23 | Usuário definiu modos manual/automático, classificação de comandos, aviso ao CCO, prioridade do sensor sobre agenda e desligamento da saída em falha do sensor |
| 31/10/2026 | 0.1.24 | Usuário definiu modo por porta e manutenção do comando manual na falha do sensor; em automático a saída controlada desliga |
| 31/10/2026 | 0.1.25 | Usuário definiu execução imediata da regra válida ao retornar a saída do modo manual para o automático |
| 31/10/2026 | 0.1.26 | Usuário definiu prioridade do sensor local sobre regra automática da plataforma em conflito na mesma saída |
| 31/10/2026 | 0.1.27 | Usuário definiu agenda local apenas offline e regra automática da plataforma com conectividade, preservada a prioridade do sensor local |
| 31/10/2026 | 0.1.28 | Usuário definiu aviso de reconexão, manutenção do último estado e espera por novo comando da plataforma |
| 03/11/2026 | 0.1.37 | Entrada digital: sinal ativo em nível alto, filtro de ruído, despertar do sleep e modo discreto ou de frequência em Hz; itens FW-052 a FW-056 criados |
| 03/11/2026 | 0.1.45 | Correção da lógica do sleep do contador de pulsos: o pulso acorda o STM32, que fica acordado até o tempo de inatividade; o ESP32 só acorda para transmitir |
| 03/11/2026 | 0.1.44 | Contador de pulsos: até quatro leitores, uso de Stop 2 e totalizador estilo hodômetro por entrada |
| 03/11/2026 | 0.1.43 | Modo sleep do contador de pulsos: a interrupção conta e o STM32 volta a dormir; pulsos de leitores, sem repique, de 5 a 10 por segundo |
| 03/11/2026 | 0.1.42 | Contador de pulsos: filtro de 20 ms, inatividade de 60 s configurável, despertar para transmitir em 1, 2 ou 5 vezes ao dia e requisito de não perder pulsos |
| 03/11/2026 | 0.1.41 | Registrado o uso da entrada digital como contador de pulsos que dorme após 1 minuto sem pulso e acorda com o pulso |
| 03/11/2026 | 0.1.40 | Registrada a polaridade da entrada digital no pino: ativo em nível baixo, por causa do optoacoplador |
| 03/11/2026 | 0.1.39 | Tensão de campo das entradas digitais definida em 12 a 24 V, mesma fonte da entrada do CLP |
| 03/11/2026 | 0.1.38 | Tempo padrão do filtro de ruído definido em 50 ms, com conversão para amostras arredondada para cima; limite de frequência de entrada a confirmar no datasheet do STM32L412 |

