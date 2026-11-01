# Comunicação interna e externa — oLoggerIOT

> **Criado em:** 29/10/2026 · **Última modificação:** 01/11/2026 · **Versão:** 0.3.21

## Estado e origem

Esclarecimento de Rodrigo Maluf durante o B2: **Modbus RTU é o padrão para sensores e atuadores**. Os módulos de rádio são integrados pelos **comandos AT fornecidos pelo fabricante**, com suporte específico para cada módulo. Esse esclarecimento substitui a interpretação anterior de RTU obrigatório também no controle dos rádios. Os resumos dos blocos B2 e B3 foram posteriormente confirmados pelo usuário em 30/10/2026. Este documento registra os requisitos alinhados e mantém detalhes técnicos e validações para as etapas posteriores; não define a arquitetura completa.

O controlador principal da família oLoggerIOT é o **STM32 (ARM), com firmware em C**. Ele coleta informações, aciona portas/saídas, controla toda a telemetria e orquestra as comunicações por rádios, ESP32, Modbus e serial. O **ESP32 usa ESP-IDF da Espressif e será sempre um módulo de comunicação do STM32**, podendo também armazenar telemetria. Cada modelo define os componentes, interfaces e quantidades disponíveis. Os rádios podem variar, incluindo módulos 4G, NB-IoT e categorias celulares a especificar.

Os esquemáticos dos modelos **OloggerMulti e y7080** identificam a memória como **GD25Q128ESIG externa, ligada ao STM32 por SPI**, com capacidade nominal de 128 Mbit (16 MiB). A capacidade destinada à telemetria e a política de armazenamento entre STM32 e ESP32, incluindo retenção, sincronização e funcionamento sem conexão, ainda não foram definidas. Identificação e referências: [análise do hardware](analise_hardware_ologgeriot.md).

No OloggerMulti, o MCU indicado nos arquivos é STM32L412C8Ux e o módulo é ESP32-C6-MINI-1. No y7080, o MCU indicado é STM32L071CBUx e o modem tem valor `SIMY7080` com footprint SIM7080G. Rodrigo Maluf confirmou que a parte elétrica já está resolvida. A análise dos arquivos foi preservada como histórico; a identificação final dos componentes e interfaces será incorporada à configuração do firmware conforme necessário, sem exigir nova revisão elétrica.

Na conectividade própria do equipamento, o caminho lógico é **sistema ↔ broker MQTT ↔ rede ↔ módulo de comunicação externo ↔ STM32**. O módulo pode ser rádio 4G, ESP32 com Wi-Fi ou outra tecnologia definida para o modelo. Essa camada deve aparecer explicitamente nos fluxos de comandos, respostas e telemetria. A localização do cliente MQTT, no STM32 ou no módulo, ainda será definida por integração.

## Referências normativas consultadas

- [Modbus Application Protocol Specification V1.1b3](https://www.modbus.org/file/secure/modbusprotocolspecification.pdf): funções, dados e transações.
- [Modbus over Serial Line Specification and Implementation Guide V1.02](https://www.modbus.org/file/secure/modbusoverserial.pdf): RTU e enlace serial.
- [Catálogo oficial de especificações](https://www.modbus.org/modbus-specifications): identifica as versões de referência.

## Base normativa

O quadro RTU contém endereço de 1 byte, função de 1 byte, dados e CRC de 2 bytes; seu limite é 256 bytes. O CRC é transmitido com o byte menos significativo primeiro. Na serial, a separação entre quadros usa silêncio de pelo menos 3,5 caracteres; intervalo interno superior a 1,5 caractere invalida o quadro. Acima de 19.200 bit/s, o guia recomenda temporizadores de 750 µs e 1,750 ms. No barramento há um mestre por vez; endereços individuais vão de 1 a 247 e o endereço 0 é broadcast sem resposta. Fonte: guia serial, seções 2.1, 2.2 e 2.5.1.

O protocolo de aplicação define coils, entradas discretas e registradores de 16 bits, com funções e respostas de exceção. Endereços e dados multibyte usam big-endian. A organização de valores que ocupam vários registradores será definida pelo projeto. Fonte: protocolo de aplicação, seções 4 e 6.

## Aplicação aos canais do projeto

| Canal | Diretriz do projeto | Detalhes pendentes |
|---|---|---|
| Sensores e atuadores Modbus por serial/RS-485 | Aplicar Modbus RTU e regras do enlace serial | Papel do MCU, baud rate, paridade, endereços e controle do transceptor |
| Entradas e saídas analógicas/digitais locais | Aquisição e acionamento conforme o hardware do modelo | Conversão das leituras e sua exposição no mapa Modbus, quando aplicável |
| MQTT por Wi-Fi, 4G, NB-IoT e outras redes disponíveis | Transportar comandos e respostas RTU, mantendo mapa e funções | Definir payload, tópicos e controle das transações |
| Bluetooth e serial de configuração | Configuração do dispositivo | Confirmar o alcance de RTU nesses canais; definir perfil e enquadramento |
| Rádio ↔ MCU | Comandos AT do fabricante | Fabricante/modelo, comandos, respostas, notificações assíncronas e particularidades de cada módulo |
| ESP32 | Módulo de comunicação do STM32, desenvolvido com ESP-IDF; pode armazenar telemetria | Interfaces, protocolo STM32 ↔ ESP32 e política de armazenamento |

**Alinhamento concluído em 30/10/2026:** comandos e respostas Modbus RTU serão mantidos na RS-485, serial e nos caminhos de rede Wi-Fi, 4G, NB-IoT e outras tecnologias disponíveis. Por MQTT, os quadros serão transportados no payload; o detalhamento do encapsulamento será uma convenção documentada do oLoggerIOT, separada da temporização serial. O controle dos rádios continua por comandos AT específicos. O usuário declarou encerrado o entendimento conceitual de Modbus RTU; isso não significa implementação ou definição de todos os parâmetros.

Na **RS-485**, aplicam-se os papéis mestre/escravo do barramento RTU. No caminho **Wi-Fi/MQTT**, a comunicação é entre clientes MQTT e broker, por publicação/assinatura; não há a estrutura de mestre/escravos do barramento serial. O conteúdo Modbus mantém a relação lógica entre quem solicita uma operação e quem responde. Wi-Fi identifica o meio de rede; essa organização decorre do uso de MQTT. Não estender a ausência de mestre serial à ausência de requisições e respostas.

## Interoperabilidade industrial — requisito confirmado

Em 30/10/2026, Rodrigo Maluf confirmou que o objetivo é seguir a comunicação Modbus RTU praticada no mercado, permitindo consultar CLPs, inversores, sensores e outros equipamentos compatíveis, além de integrar o oLoggerIOT à rede Modbus RTU do cliente.

- **Como mestre/cliente:** o oLoggerIOT inicia consultas e, quando configurado e autorizado, comandos aos equipamentos externos. A integração segue o mapa e as funções documentados pelo fabricante de cada equipamento.
- **Como escravo/servidor:** o oLoggerIOT responde ao mestre do cliente, disponibilizando suas medições, estados, configurações e comandos suportados em um mapa documentado.
- **Dados locais:** o usuário confirmou a distinção entre coils, discrete inputs, input registers e holding registers. Leituras analógicas 4–20 mA e diferenciais serão representadas por input registers; saídas binárias por coils; configurações numéricas graváveis por holding registers. Endereços e codificação continuam a definir.
- **Setup:** deverá descrever os recursos do equipamento e as associações necessárias. Campos e formato serão definidos depois, conforme orientação de foco no entendimento funcional.

O enlace RTU admite um mestre por barramento; consultar a seção 2.1 do guia serial oficial. A necessidade de operar nos dois papéis simultaneamente e a quantidade de interfaces/barramentos ainda serão levantadas. Não assumir dois mestres concorrendo na mesma rede.

A interoperabilidade exige respeitar funções, endereçamento, CRC, temporização, parâmetros seriais e exceções do padrão. O significado, a escala e a codificação de cada registrador dependem do mapa do equipamento. Nenhuma extensão proprietária do quadro RTU foi solicitada para essas integrações de campo.

### Falha de leitura de equipamento Modbus

Quando um sensor, CLP, inversor ou outro equipamento Modbus consultado não responder, o usuário definiu **dez tentativas, uma a cada 30 segundos**, antes de informar falha de comunicação. As tentativas continuam e, se a condição persistir, deve ser informado defeito após uma hora contada **do fim das dez tentativas sem resposta**. Considerar a comunicação restabelecida após **cinco leituras consecutivas bem-sucedidas** e enviar a mensagem ao sistema. Em 01/11/2026, o usuário definiu que a primeira consulta sem resposta **conta como uma das dez tentativas**, e que o timeout de cada consulta é **fixo no firmware**, não configurável por equipamento. Em 01/11/2026, o usuário definiu o timeout de cada consulta em **30 segundos**, fixo no firmware. Esta regra é específica da leitura Modbus e não altera as tentativas de conexão MQTT.

Os sinais Modbus seguem as regras de controle e falha já definidas para sensores. Após as dez tentativas sem resposta, as saídas discretas em modo automático que dependem do sensor com falha devem ser desligadas; em modo manual, permanece o comando manual, conforme a regra já definida. Para saída em frequência que reproduz sinal Modbus, aplicar a mesma regra já definida: em falha, 0 Hz; após o sensor voltar a funcionar, aguardar um minuto de funcionamento normal antes de retomar. A recuperação da comunicação também requer cinco leituras consecutivas bem-sucedidas; em 01/11/2026, o usuário definiu que as **cinco leituras consecutivas ocorrem primeiro e só então começa a contagem de um minuto de funcionamento normal** para retomar a saída em frequência.

Após cinco leituras consecutivas bem-sucedidas e o restabelecimento da comunicação, avaliar a regra atual do sensor para as saídas automáticas afetadas. Se a regra determinar que a saída ligue, aguardar o tempo de partida configurado para ela antes de ligar, respeitando as demais regras aplicáveis.

## Broker, fallback e credenciais — respostas do B3

Em 30/10/2026, Rodrigo Maluf definiu:

- O broker poderá ser **AWS IoT Core, EMQX ou RabbitMQ**. Não foi escolhido um único provedor. A integração RabbitMQ deverá usar sua interface MQTT; não foi solicitado AMQP no firmware.
- Comunicação sempre criptografada, com autenticação por certificado/chave. Credenciais devem ser armazenadas com segurança no dispositivo, preferencialmente no STM32 quando viável.
- O setup define a prioridade entre os meios de comunicação. Deve haver fallback para outro meio disponível quando o prioritário falhar.
- Cada equipamento tem identidade própria baseada no **UID do STM32**. O UID é identificador, não segredo nem prova criptográfica de identidade.
- **Decisão posterior do usuário:** certificado e chave privada individuais por equipamento, vinculados ao cliente empresarial no sistema. Substitui a proposta inicial de certificado compartilhado por cliente. Renovação remota prevista; periodicidade e implementação ainda não escolhidas.

### Rotação remota e armazenamento — avaliação e proposta

A rotação remota é viável; a AWS documenta emissão a partir de CSR e entrega do novo certificado ao dispositivo. Como proposta funcional, instalar a nova credencial sem apagar a atual, testar a conexão, confirmar ao sistema e só então retirar a anterior. Validar a recuperação em caso de falha de energia, conexão ou certificado inválido. Isso é uma proposta a detalhar, não uma implementação aprovada.

Certificado público e chave privada são elementos distintos. A chave privada é o segredo a proteger. Para credenciais individuais, a recomendação é gerar a chave no dispositivo e enviar apenas a solicitação de assinatura (CSR), recebendo o certificado correspondente. Renovar o certificado não significa necessariamente trocar a chave; a política deverá distinguir ambos.

Um mesmo certificado de autenticação compartilhado exigiria compartilhar a chave privada correspondente: a extração em um equipamento afetaria a credencial do cliente inteiro e impediria revogação individual por certificado. O usuário aceitou **certificado e chave por dispositivo**, mantendo vínculo ao cliente no sistema. A AWS recomenda credenciais únicas por dispositivo. Uma autoridade certificadora por cliente não foi decidida.

### Processo de credencial individual aceito pelo usuário

O usuário considerou plausível o esforço operacional em função da segurança e autorizou seguir com o processo apresentado:

1. O integrador autenticado conecta o equipamento ao aplicativo e identifica seu UID STM32.
2. O integrador vincula o equipamento ao cliente empresarial, com autorização validada pelo sistema.
3. O sistema e o equipamento executam o provisionamento da credencial individual, sem exigir que o técnico copie certificados ou manipule chaves privadas manualmente.
4. O dispositivo armazena a credencial com proteção, testa a conexão e confirma a ativação.
5. Renovação remota e revogação são tratadas individualmente; prever acompanhamento de falhas e recuperação.

Esse é o fluxo funcional aceito. A autenticação inicial, geração da chave, CSR, emissão, locais de armazenamento/TLS e política de recuperação continuam para detalhamento. Não foram criadas credenciais nem alterados brokers ou dispositivos.

Armazenamento no STM32 é uma preferência, não garantia já demonstrada. Proteções de leitura de flash, como RDP, deverão ser avaliadas para o MCU e o processo de manutenção. A localização do TLS importa: se o handshake ocorrer no rádio/ESP32, verificar se esse módulo precisa receber a chave ou admite operações criptográficas externas; guardar a chave no STM32 não garante que ela permaneça exclusivamente ali. O mecanismo de proteção e a divisão do TLS continuam a definir.

Referências consultadas: [AWS — certificados X.509](https://docs.aws.amazon.com/iot/latest/developerguide/x509-client-certs.html), [AWS — rotação por CSR](https://aws.amazon.com/blogs/iot/how-to-manage-iot-device-certificate-rotation-using-aws-iot/), [EMQX — autenticação X.509](https://docs.emqx.com/en/emqx/latest/guides/access-control/authn/x509.html), [RabbitMQ — plugin MQTT e TLS](https://www.rabbitmq.com/docs/mqtt), [ST — proteção RDP](https://www.st.com/content/st_com/en/technical-documents/an5156-introduction-to-security-for-stm32-mcus-stmicroelectronics_asset/chunk-006-topic-an5156-sec-feat-read-out-protection-rdp-.html).

## Tópicos, confirmações e tentativas — respostas do B3

- Cada equipamento terá seus próprios tópicos. Não haverá comandos MQTT dirigidos a grupos de dispositivos. Nomes, separação de direção e regras concretas de autorização ainda serão definidos.
- Comandos e telemetria exigem confirmação. Para comandos, o usuário escolheu **ambos os retornos**: primeiro recebido e depois o resultado da execução, executado ou falhou com o motivo. Recebimento não significa execução concluída.
- Após até cinco tentativas sem confirmação, guardar/manter a mensagem ou dado na memória para envio posterior. Intervalos, timeouts e o próximo ciclo serão detalhados. Não interpretar como cinco retransmissões adicionais à primeira tentativa.
- A telemetria pode ser enviada em lote ou como um único payload da fila. O servidor confirma recebimento e salvamento, identificando o lote/payload; somente os dados confirmados podem ser excluídos.
- Grupos de canais de um único equipamento, já mencionados no comando de nova medição, são distintos de grupos de dispositivos, que foram excluídos.
- A estratégia de identificação e tratamento de duplicatas deverá ser detalhada para que uma tentativa repetida não resulte em registros duplicados ou execução indevida de comandos. Validade de comandos adiados também permanece a definir.

Essas respostas fixam o comportamento de confirmação no sistema. A configuração concreta de QoS MQTT não foi escolhida, nem os envelopes/tópicos de confirmação. Os dois retornos devem ser associados ao mesmo comando; codificação, correlação, timeouts de cada etapa e tratamento de repetição serão detalhados depois.

## Pontos a decidir

- **Papel por interface:** mestre/escravo para RS-485; clientes e broker no transporte MQTT. Detalhar seleção e eventual simultaneidade dos papéis nos barramentos seriais, além da correlação de requisições/respostas pela rede.
- **Mapa por modelo:** definir identificação, versão e capacidades; endereços, tipos, unidades, escalas e permissões dos dados. Quantidades serão específicas do modelo.
- **Módulos de rádio:** identificar os modelos e obter seus manuais AT. Cada módulo exige integração específica; comandos e comportamento não serão presumidos idênticos entre fabricantes ou modelos.
- **Telemetria:** definir como leituras periódicas serão publicadas e distinguidas de respostas às requisições.
- **Configuração e comandos:** levantar operações antes de atribuir funções e registradores. Comandos textuais de terminal ou WhatsApp precisarão ser convertidos para as operações definidas.
- **Atualização remota:** foram definidos verificação de validade e compatibilidade e retorno à versão anterior em caso de falha, inclusive interrupção de energia. Transferência, fragmentação e mecanismos de validação e recuperação ainda precisam ser detalhados; ver [atualização de firmware](atualizacao_firmware.md).
- **Acesso simultâneo:** definir arbitragem entre serial, Bluetooth e internet, além de timeouts e repetição de comandos.
- **Proteção de acesso:** detalhar autenticação, autorização e proteção dos canais nos blocos de segurança.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 29/10/2026 | 0.1.0 | Registro da escolha explícita de Modbus RTU, referências oficiais e pontos de adaptação por canal |
| 29/10/2026 | 0.2.0 | Esclarecido Modbus RTU para sensores/atuadores e AT específico por rádio; registrados STM32 em C e ESP32 com ESP-IDF; alcance de RTU em MQTT/Bluetooth aguarda confirmação |
| 29/10/2026 | 0.3.0 | STM32 confirmado como coletor e orquestrador central; ESP32 sempre como módulo de comunicação com possível armazenamento; registrados 16 MB disponíveis ao STM32, tipo a detalhar |
| 29/10/2026 | 0.3.1 | Explicitados módulos de comunicação externos e rede entre STM32 e broker MQTT; localização do cliente MQTT permanece a definir |
| 29/10/2026 | 0.3.2 | Incorporados componentes e flash SPI externa dos dois modelos analisados; referenciadas pendências de hardware e USB |
| 29/10/2026 | 0.3.3 | Parte elétrica registrada como resolvida por confirmação do responsável técnico; detalhes de implementação serão incorporados ao firmware conforme necessário |
| 30/10/2026 | 0.3.4 | Confirmada interoperabilidade Modbus RTU com equipamentos industriais e rede do cliente; registrados cenários mestre/escravo e modelo de dados padrão, sem definir arquitetura |
| 30/10/2026 | 0.3.5 | Usuário encerrou alinhamento conceitual Modbus: mesmos comandos nos meios seriais e de rede; diferenciados mestre/escravo RS-485 e publicação/assinatura MQTT com requisições/respostas Modbus |
| 30/10/2026 | 0.3.6 | Registrados brokers possíveis, criptografia, preferência por chave no STM32, prioridade/fallback por setup, UID e proposta de certificado por cliente; avaliada rotação remota e alternativa de credenciais individuais |
| 30/10/2026 | 0.3.7 | Usuário aceitou credenciais individuais por dispositivo e processo de provisionamento assistido pelo aplicativo; substituída proposta compartilhada por cliente, mantendo detalhes técnicos em aberto |
| 30/10/2026 | 0.3.8 | Definidos tópicos próprios sem comandos em grupo, confirmação de comandos/telemetria, cinco tentativas e retenção para posterior envio; confirmação do servidor para lote ou payload individual |
| 30/10/2026 | 0.3.9 | Usuário escolheu duas confirmações por comando: recebido e depois resultado da execução (executado ou falhou com motivo) |
| 30/10/2026 | 0.3.10 | Resumo consolidado do B3 confirmado pelo usuário; mantidos detalhes técnicos e validações para etapas posteriores |
| 31/10/2026 | 0.3.11 | Registradas verificações e recuperação da versão anterior na atualização; mecanismos permanecem em definição |
| 31/10/2026 | 0.3.12 | Usuário definiu dez tentativas de leitura Modbus a cada 30 segundos antes de informar falha de comunicação |
| 31/10/2026 | 0.3.13 | Usuário definiu continuar tentando e informar defeito se a condição persistir por uma hora |
| 31/10/2026 | 0.3.14 | Usuário definiu início da contagem de uma hora após as dez tentativas sem resposta |
| 31/10/2026 | 0.3.15 | Usuário confirmou mensagem ao sistema quando a comunicação Modbus for restabelecida |
| 01/11/2026 | 0.3.16 | Usuário definiu cinco leituras consecutivas bem-sucedidas para considerar a comunicação restabelecida |
| 01/11/2026 | 0.3.17 | Usuário confirmou aplicar ao Modbus as regras de controle e falha de sensores já definidas |
| 01/11/2026 | 0.3.18 | Usuário confirmou reavaliação automática da regra do sensor após recuperação Modbus, com tempo de partida da saída |
| 01/11/2026 | 0.3.21 | Usuário definiu timeout de 30 segundos por consulta Modbus, fixo no firmware |
| 01/11/2026 | 0.3.20 | Usuário definiu que a primeira consulta sem resposta conta nas dez tentativas, timeout fixo no firmware, e cinco leituras antes do minuto de retorno da saída em frequência |
| 01/11/2026 | 0.3.19 | Usuário confirmou aplicação das regras já definidas à saída em frequência associada a sinal Modbus |

