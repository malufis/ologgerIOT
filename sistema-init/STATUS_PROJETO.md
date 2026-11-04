# Status do Projeto — oLoggerIOT

> **Criado em:** 15/10/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.3.167
> **Repositório:** https://github.com/malufis/ologgerIOT

## Fase atual
**Desenvolvimento do firmware** — entrevista encerrada (B0 a B8 confirmados; B9 a B17 registrados como fora do device, confirmado pelo usuário em 01/11/2026). Fase 0 do plano concluída em 04/11/2026 (SPRINT_00 fechada, o FW-007 foi para a SPRINT_07); fases 1, 1B e 4 em andamento (`planejamento/sprints/`).
**Progresso da entrevista:** concluída (9/18 blocos no device; B9 a B17 são do sistema e não têm perguntas de firmware). O estado do código está em `documentacao/ESTADO_IMPLEMENTACAO.md`.

## Concluído
- Kit de inicialização criado (`sistema-init/`)
- Rascunho analisado e itens repetidos consolidados (`docs/ANALISE_RASCUNHO.md`)
- Conceitos de lambda, auditoria, RLS, mascaramento e MQTT registrados no glossário da entrevista
- Entrevista reorganizada em 18 blocos, com IoT (B3), apps (B7), banco e LGPD (B11) e auditoria (B12)

## Pendências
| ID | Item | Bloco | Status |
|---|---|---|---|
| P-001 | Nome oficial e repositório definidos e confirmados no B0 | B0, B15 | CONCLUÍDO |
| P-002 | Definir onde o sistema roda: nuvem, on-premise ou híbrido | B13 | PENDENTE |
| P-003 | Brokers possíveis informados: AWS IoT Core, EMQX ou RabbitMQ; nenhum provedor único escolhido | B3 | OPÇÕES DEFINIDAS |
| P-004 | Conexão criptografada com certificado/chave definida; portas e parâmetros por ambiente a detalhar | B3 | PARCIAL — proteção definida |
| P-005 | RTU para comandos/respostas por RS-485, serial e rede/MQTT; rádios controlados por AT; detalhar parâmetros | B2, B3 | CONCEITO ALINHADO — detalhes posteriores |
| P-006 | Credencial individual e fluxo de provisionamento aceitos; detalhar geração, armazenamento, rotação e recuperação | B3 | PARCIAL — decisão confirmada |
| P-007 | WhatsApp definido para configuração em linguagem natural; detalhar comandos, permissões e confirmações | B4, B8 | PARCIAL — canal e finalidade definidos |
| P-008 | Sessão única por dispositivo, não por usuário nem por empresa; bloqueio durante a configuração | B5 | CONCLUÍDO em 01/11/2026 |
| P-009 | Gateway USB/serial do técnico é aplicação Python no notebook, com chaveiro serial; sem app de celular; configuração local de conectividade | B7 | CONCLUÍDO em 01/11/2026 |
| P-010 | Definir o modelo de isolamento de dados por cliente (RLS, schema ou banco por cliente) | B11 | PENDENTE |
| P-011 | Definir a lista de campos criptografados na aplicação e a matriz de mascaramento por perfil | B11 | PENDENTE |
| P-012 | Definir base legal, prazos de retenção e encarregado (DPO) para LGPD. Revisão jurídica recomendada | B11 | PENDENTE |
| P-013 | Definir o escopo da auditoria e estimar o volume diário de eventos | B12 | PENDENTE |
| P-014 | Escolher a estratégia de painéis: Grafana Server, Amazon Managed Grafana, sistema próprio ou combinação | B6 | PENDENTE |
| P-015 | Definir quais ações exigem lambda de pré-execução e quais regras cada uma executa | B9 | PENDENTE |
| P-016 | Definir backend, frontend, banco, broker e apps (decisão deferida para o agente) | B14, B17 | PENDENTE |
| P-017 | Definir inclusões e exclusões da primeira versão após levantar as funcionalidades | B1, B4 | ADIADO ATÉ B4 |
| P-018 | Orçamento considerado não relevante nesta etapa pelo usuário | B1 | DISPENSADO NESTA ETAPA |
| P-019 | WhatsApp como canal preferencial; MQTT com device conectado e gateway USB pelo app; detalhar comandos e entregas | B1, B3, B4, B7, B8 | PARCIAL — fluxos propostos |
| P-020 | Papéis RS-485 mestre/escravo e MQTT clientes/broker esclarecidos; detalhar encapsulamento e transações | B2, B3 | CONCEITO ALINHADO — detalhes posteriores |
| P-021 | HW-001: resolver gateway USB do y7080, cujo STM32L071 indicado não possui USB nativo | B3, B4, B7 | RESOLVIDO — confirmação do responsável técnico |
| P-022 | HW-002: verificar compartilhamento de VBUS USB e alimentação lógica nos Gerbers do OloggerMulti | B3, B13 | RESOLVIDO — confirmação do responsável técnico |
| P-023 | HW-003: reconciliar boot do ESP32-C6, GPIO9 no esquema e GPIO0 nos Gerbers | B3, B14 | RESOLVIDO — confirmação do responsável técnico |
| P-024 | HW-004: reconciliar conexão EN/VIN de U7 entre esquema e Gerbers do OloggerMulti | B13 | RESOLVIDO — confirmação do responsável técnico |
| P-025 | Confirmar modem montado no y7080 (SIMY7080/footprint SIM7080G), sensores PC11/K8 e revisão de cada placa | B2, B3 | PENDENTE — confirmar componentes |

| P-026 | Validar recepção de comandos NB-IoT em baixo consumo, modo do módulo/operadora, despertar do STM32 e caminho MQTT | B3 | DEFINIDO — validação por teste com módulo real e operadora; execução pendente |

| P-027 | Amostragem de 10 a 1000 amostras/s com média simples por segundo; envio por média móvel de janela de um envio; falha por dente de serra com inversão acima de 2% sem inversor e 10% com inversor, em 5 s; falha vai para 0 Hz; primeira leitura após zero sem comparação; escala logarítmica não é requisito | B4 | CONCLUÍDO em 01/11/2026 |
| P-028 | HW-007: soldar a ferrite L2 na placa de bancada, conferir o VDDA (3,25 V) e refazer a calibração em 4, 8, 12, 16 e 20 mA | Fase 1 | PENDENTE — bloqueia o FW-012 |
| P-029 | HW-008: entrada digital com D1 retirada e limite de 12 V na bancada; resistor de 4K7 em 0402 passa a 0603 no próximo layout, e 24 V ainda pede potência maior; avaliar supressor de ~30 V para campo industrial | Fase 1B | PARCIAL — 12 V na bancada; decidir o layout |
| P-030 | Bateria no pino VBAT e cristais do relógio | Fase 1B | RESOLVIDO em 04/11/2026 — a bateria é opcional, conforme a implantação; o OloggerMulti tem os dois cristais (LSE para o RTC) e o y7080 não tem cristal. Sem bateria, o totalizador e a hora se perdem na falta de energia (hora restaurada pelo NTP, totalizador recomeça); com bateria, sobrevivem |
| P-031 | Confirmar que os leitores geram pulsos mais largos que 20 ms (com folga) e a taxa de 5 a 10 pulsos por segundo | Fase 1B | PENDENTE — confirmar com os leitores |
| P-032 | Mapa Modbus: definir os modos da entrada digital (discreto, contador de pulsos e frequência) e o endereço do totalizador; hoje o mapa diz "discreto ou pulsado" | Fase 1B | PENDENTE |
| P-033 | Frequência em Hz (FW-054): limite do pino no datasheet e do optoacoplador TLP291, faixa de 0 a 20 kHz, janela de medição e filtro próprio do modo | Fase 1B | PENDENTE — ajuste fica para depois |
| P-034 | Saída digital "pulsada" (modo 1 do mapa Modbus) | Fase 4 | RESOLVIDO em 03/11/2026 — "pulsada" é a saída em frequência; o acionamento por tempo da saída discreta é o retardo de acionamento (relé de tempo) |
| P-035 | HW-009: saídas digitais sem pull-down na base do transistor; estado durante reset e bootloader não garantido | Fase 4 | ANOTADO em 03/11/2026 em `documentacao/melhorias_hardware.md` para a próxima revisão; pull-down de ~10 kΩ |
| P-036 | Saída em frequência (FW-023): PB12 não tem canal de timer e o temporizador de software não roda em Stop 2 | Fase 4 | RESOLVIDO em 03/11/2026 — a frequência só habilita com o sleep desligado e o sleep só liga sem saída em frequência (FW-061); falta implementar o lado das saídas |
| P-037 | Mapa Modbus: o tipo da saída (40166–40180) tinha 1 = pulsado e 2 = frequência, que são a mesma coisa | Fase 4 | RESOLVIDO em 03/11/2026 — 0 = discreto, 1 = frequência (pulsada), 2 reservado; 40271–40285 reservado (o tempo de acionamento é o retardo de 40256–40270) |
| P-038 | Espaço de flash do STM32L412C8: 64 KB, com a imagem atual em 25,8 KB | Fase 9 | RESOLVIDO em 04/11/2026 — ADR-0003: imagem na flash externa com bootloader; novo FW-062 reserva o espaço e mede a folga por fase |
| P-039 | ADR-0001 (versão do FreeRTOS nos dois builds) e ADR-0002 (ferramenta MISRA): propostas aguardando decisão | Fase 0 | PENDENTE |
| P-040 | Modo manual e automático das saídas (FW-025) | Fase 4 | RESOLVIDO em 04/11/2026 — o sensor volta a operar depois de 5 minutos estáveis; sem pedido válido mantém o último estado válido; depois de reinício a saída volta desligada e em automático. Resta o alerta ao CCO na chegada do comando manual, que depende do espaço de alertas (FW-021) |
| P-041 | Regra do sensor entrada analógica para saída digital (FW-063): cadastro e classificação da agenda | Fase 4 | RESOLVIDO em 04/11/2026 — os campos vêm por MQTT ou Modbus e têm endereço Modbus (limiares 40307–40366 e sentido 40367–40381, endereços propostos a confirmar); sentido 0 = liga no mínimo e desliga no máximo, sentido 1 = liga no máximo e desliga no mínimo; a agenda é comando local e só vale offline, e online vale o CCO |
| P-042 | Agenda semanal offline (FW-032): formato do cadastro dos 336 intervalos de 30 minutos | Fase 6 | RESOLVIDO em 04/11/2026 (proposta a confirmar) — vem por MQTT do CCO, sem endereço Modbus, em 84 caracteres hexadecimais por saída, validada por inteiro; implementado em `lib/agenda` |
| P-043 | Agenda: fuso, versão, momento depois de reinício, identificador do comando e tratamento de saída ausente e de cadeia vazia | Fase 6 | RESOLVIDO em 04/11/2026 — fuso horário configurado pelo CCO (40403, proposta); o device guarda só a última agenda, sem ordem de versão; depois de reinício a agenda assume o intervalo do horário atual; saída ausente mantém e cadeia vazia apaga (proposta mantida); identificador do comando em proposta (P-044) |
| P-044 | Identificador do comando MQTT | Fase 7 | RESOLVIDO em 04/11/2026 — aceito: campo `id` de até 36 caracteres, devolvido sem alteração em cada resposta, descarte de repetição com os últimos 8; nomes `etapa` e `motivo` e endereço 40403 do fuso seguem como proposta |
| P-045 | Agenda sem fuso configurado | Fase 6 | RESOLVIDO em 04/11/2026 — o fuso começa em Brasília (−180), e a agenda e o fuso gravados persistem ao reinício; a agenda só funciona com o sleep desabilitado |
| P-046 | Onde guardar a agenda e o fuso: proposta de usar as duas últimas páginas de 2 KB da flash interna (0x0800F000 a 0x0800FFFF), fora da área do programa de 60 KB. Confirmar com o layout do bootloader (FW-062, ADR-0003) e com o responsável técnico; a alternativa é a flash externa, que depende do driver FW-018 | Fase 6 | PENDENTE — confirmar |

## Decisões tomadas
B0 — Identificação: respostas e resumo confirmados pelo usuário em 29/10/2026.

- **B0.1 — Nome oficial:** oLoggerIOT.
- **B0.2 — Codinome:** não possui.
- **B0.3 — Repositório:** ologgerIOT — https://github.com/malufis/ologgerIOT.
- **B0.4 — Cliente ou patrocinador:** Optmile. Responsável pelas decisões: Rodrigo Maluf.
- **B0.5 — Responsável técnico:** Rodrigo Maluf.
- **B0.6 — Objetivo:** construir o firmware para o oLoggerIOT.
- **B0.7 — Contexto e necessidade:** desenvolver o firmware de um dispositivo IoT com microcontrolador ARM, conexões seriais para módulos de rádio 4G, módulos Wi-Fi, módulos seriais e chips RS-485, comunicação com servidor via MQTT e entradas e saídas de sensores. Modelos de componentes e detalhes das interfaces ainda não informados.

## Respostas do B1 — Negócio e escopo (confirmado)
Resumo confirmado pelo usuário em 29/10/2026. Limites de cada entrega serão definidos após o levantamento de funcionalidades no B4; orçamento não relevante nesta etapa.
- **B1.1 — Setor:** saneamento.
- **B1.2 — Usuários-alvo:** integradores e técnicos do cliente; 2 pessoas inicialmente.
- **B1.3 — Resultado esperado em 6 a 12 meses:** piloto em campo.
- **B1.4 — Recorte informado pelo usuário:** firmware, atualização remota e configuração do dispositivo por serial, Bluetooth e internet. Ideia a detalhar: comandos por terminal ou WhatsApp com webhook e MQTT; fluxo, responsabilidades e inclusão no primeiro protótipo ainda não confirmados. Funcionalidades ainda não levantadas em detalhe. Inclusões e exclusões da primeira versão serão definidas após o B4.
- **B1.5 — Sistema relacionado:** sistema desenvolvido junto com o firmware. Não foi informada a existência de firmware anterior nem os detalhes da integração.
- **B1.6 — Prazo e orçamento:** primeiro protótipo para teste em bancada em um mês (referência: 29/10/2026; data derivada: 29/11/2026). Esse marco é distinto do piloto em campo mencionado em B1.3. Orçamento: não relevante nesta etapa, conforme orientação explícita do usuário.

## Premissa transversal — Família de modelos oLoggerIOT
- **Origem:** esclarecimento do usuário durante o B2, em 29/10/2026.
- O projeto abrange uma família de modelos de equipamentos e seus firmwares.
- O controlador principal é o STM32 (ARM), com firmware em C. Ele coleta informações, aciona portas/saídas, controla toda a telemetria e orquestra as comunicações por rádios, ESP32, Modbus e serial. Esquemáticos analisados identificam STM32L412C8Ux no OloggerMulti e STM32L071CBUx no y7080; a montagem e a revisão de referência precisam ser confirmadas. O ESP32 usa ESP-IDF e será sempre módulo de comunicação do STM32, podendo também armazenar dados de telemetria; não será o controlador principal.
- A composição de MCU, rádios, sensores analógicos, sensores digitais e interfaces de comunicação varia conforme o modelo do equipamento.
- Quantidades de portas, sensores e rádios devem ser definidas para cada modelo específico de equipamento/firmware; não há uma quantidade única fixada para toda a família.
- Os exemplos de módulos 4G, Wi-Fi e RS-485 registrados no B0 não implicam que todos estejam presentes em todos os modelos.
- O firmware atenderá aos dois projetos OloggerMulti e y7080. Ideia explícita do usuário: compilação única com arquivo de setup que indique o que o dispositivo contém. A separação e a organização técnicas serão definidas depois pelo usuário. A interpretação anterior do assistente de imagens separadas foi substituída; foco atual em entender o projeto.
- A entrevista deve distinguir capacidades comuns à família de características específicas de cada modelo.

## Responsabilidades e armazenamento — STM32 e ESP32
- **Origem:** esclarecimento explícito de Rodrigo Maluf durante o B2, em 29/10/2026.
- **STM32:** coletor de dados e orquestrador central; coleta informações, aciona portas/saídas e controla toda a telemetria e os meios de comunicação disponíveis no modelo.
- **ESP32:** sempre módulo de comunicação subordinado ao STM32; pode também guardar dados de telemetria. Firmware desenvolvido com ESP-IDF.
- **Memória:** esquemáticos dos dois modelos identificam GD25Q128ESIG externa por SPI, 128 Mbit = 16 MiB, coerente com os 16 MB mencionados pelo usuário. Não se trata de SRAM interna. Capacidade destinada à telemetria, retenção e partições ainda a definir.
- **Armazenamento offline confirmado:** STM32 continua coletando, guarda medições na flash e envia quando reconectar, respeitando programação de sleep a detalhar. Memória cheia: substituir o arquivo mais antigo. Retenção após envio, confirmação de entrega e distribuição STM32/ESP32 ainda a definir. Protocolo STM32 ↔ ESP32 pendente.

## Premissa transversal — Modbus RTU
- **Origem:** instrução explícita de Rodrigo Maluf durante o B2, em 29/10/2026.
- **Decisão esclarecida:** Modbus RTU para sensores e atuadores. Rádio ↔ MCU usa comandos AT fornecidos pelo fabricante, com integração específica para cada modelo; possíveis módulos incluem 4G, NB-IoT e categorias celulares a especificar. Substitui a interpretação anterior de RTU também para controle dos rádios.
- **Base documental:** especificações oficiais Modbus Application Protocol V1.1b3 e Serial Line V1.02 consultadas; referências e diretrizes em documentacao/comunicacao_iot.md.
- **Alcance esclarecido:** manter comandos e respostas RTU na RS-485, serial e rede por Wi-Fi, 4G, NB-IoT e outras tecnologias disponíveis; por MQTT os quadros seguem no payload. Detalhes de encapsulamento ainda a especificar.
- **Rádios:** controle por AT esclarecido; cada integração seguirá a documentação do módulo específico. No y7080, símbolo SIMY7080 e footprint SIM7080G; confirmar a variante montada antes de adotar manual AT. No OloggerMulti, módulo ESP32-C6-MINI-1 identificado no esquemático.
- Mapas de registradores por modelo, funções, papéis por interface, telemetria, atualização remota e controle de acesso continuam a detalhar.

## Configuração em linguagem natural e gateway USB
- **Origem:** proposta funcional de Rodrigo Maluf durante o B2, em 29/10/2026.
- Canal preferencial do integrador: WhatsApp em linguagem natural. O sistema em desenvolvimento gera comandos estruturados e retorna as respostas.
- Device com conectividade própria: WhatsApp ↔ sistema ↔ broker MQTT ↔ rede ↔ módulo de comunicação externo (rádio 4G, ESP32 com Wi-Fi ou outra tecnologia do modelo) ↔ STM32. Comandos e respostas atravessam essa camada de comunicação. Localização do cliente MQTT (STM32 ou módulo) ainda não definida.
- Gateway USB (decisão de 01/11/2026): aplicação Python no notebook do técnico, com chaveiro serial ligado ao STM32. Recebe comandos do servidor e os encaminha pela serial; encaminha as respostas para retorno via WhatsApp. Não há app de celular: o celular usa somente WhatsApp. Canal notebook ↔ servidor ainda não escolhido.
- Sem acesso ao servidor: a aplicação do notebook permite comandos locais seriais limitados a estabelecer conectividade do dispositivo. Após conexão, configuração pelo WhatsApp. Essa limitação local não restringe os comandos do servidor enviados via gateway USB.
- Catálogo de comandos, formato, identificação do device/integrador, permissões e confirmação de ações ainda serão detalhados.
- Registro dos fluxos e responsabilidades: documentacao/configuracao_do_dispositivo.md.

## Análise documental dos dois hardwares
- **Solicitação:** análise dos dois ZIPs de Gerbers e esquemáticos em nogit/gerbes/.
- **Fontes:** 10 folhas OloggerMulti e 9 y7080, sem projeto/PCB nativo completos. Arquivos originais preservados.
- **OloggerMulti:** STM32L412C8Ux, ESP32-C6-MINI-1, flash SPI externa de 16 MiB, quatro entradas 4–20 mA, quatro digitais, quatro saídas por transistor e RS-485.
- **y7080:** STM32L071CBUx, modem identificado como SIMY7080 com footprint SIM7080G, flash SPI externa de 16 MiB, uma entrada 4–20 mA, circuito diferencial PC11/K8 e RS-485.
- **Verificação:** ZIPs íntegros, 19 netlists exportadas; 445 pinos comparáveis no Multi com 49 divergências de nomes de rede; 519 no y7080 sem divergências. Comparação por atributos X2 não equivale a DRC/continuidade de trilhas.
- **ERC parcial:** executado nas folhas dos MCUs; 87/104 ocorrências, afetadas por folhas isoladas e bibliotecas ausentes; nenhum projeto completo declarado aprovado.
- **Saídas:** documentacao/analise_hardware_ologgeriot.md e nogit/analise_hardware/evidencias.json; documentos de comunicação e configuração atualizados.
- **Progresso à época da análise:** 2/18; a análise técnica não confirmou automaticamente B2 ou B3. O B2 foi confirmado posteriormente, conforme fase atual e histórico.

## Confirmação do responsável técnico — parte elétrica
- Rodrigo Maluf informou que a parte elétrica já está toda resolvida.
- P-021 a P-024 encerrados como bloqueios elétricos por essa confirmação; análise anterior preservada no histórico.
- Não foram informadas as alterações físicas específicas; não presumir troca de MCU, ponte USB ou correção de pino particular.
- Continuar o levantamento do firmware. Componentes e interfaces finais serão identificados quando necessários à implementação.

## Modelos e versões do firmware
- **Orientação de Rodrigo Maluf:** desenvolver para OloggerMulti e y7080; ideia de compilação única com arquivo de setup que descreva a composição do dispositivo.
- Entender os comportamentos e os recursos de cada equipamento antes de decidir como organizar ou separar a implementação. Essa decisão será tomada posteriormente pelo usuário.
- Formato e aplicação do setup, ferramentas, versionamento e realização técnica da compilação única ainda não definidos. Não há decisão vigente de gerar binários separados por modelo.
- Registro: documentacao/modelos_e_versoes_firmware.md. Nenhum binário foi produzido e nenhum projeto de compilação foi criado.

## Interoperabilidade Modbus RTU — entendimento confirmado em 30/10/2026
- Seguir o padrão industrial para consultar CLPs, inversores, sensores e integrar o oLoggerIOT à rede Modbus RTU do cliente.
- Cenários funcionais requeridos: mestre/cliente para consultar equipamentos e escravo/servidor para responder ao mestre do cliente. Seleção por interface e eventual simultaneidade ainda a detalhar.
- Modelo de dados aceito pelo usuário: coils para estados/comandos binários; discrete inputs para entradas binárias; input registers para medições analógicas; holding registers para configurações numéricas graváveis.
- Sensores locais 4–20 mA e diferenciais: aquisição e conversão pelo STM32, exposição dos valores no mapa Modbus; escalas, endereços, calibração e estados ainda a detalhar.
- Mapas externos seguem documentação de cada fabricante; manter os quadros e o comportamento RTU padrão na rede de campo.
- Continuação do entendimento funcional; compilação única com setup permanece como ideia do usuário e a organização da implementação será definida depois.

## Encerramento do alinhamento conceitual Modbus RTU — 30/10/2026
- Usuário declarou finalizada a parte de entendimento do Modbus RTU.
- RS-485: mestre/escravos no barramento. Wi-Fi com MQTT: clientes publicam/assinam mensagens por um broker, sem a estrutura mestre/escravos do enlace serial.
- Mantêm-se mapa, funções e comandos/respostas Modbus; transporte de rede não elimina quem solicita e quem responde a cada operação.
- Detalhes de setup, endereços, tempos, tópicos e transações serão definidos depois. Nenhuma implementação foi realizada e B2/B3 não foram confirmados integralmente.

## Respostas do B2 — Telemetria e armazenamento (confirmado)
Resumo consolidado do B2 confirmado pelo usuário em 30/10/2026, incluindo nova medição sob comando e fila com exclusão após recebimento confirmado pelo servidor. Detalhes técnicos registrados permanecem para as etapas correspondentes.
- Intervalos definidos no setup; operação contínua ou sleep mode.
- Exemplos do usuário: equipamento na rede elétrica com intervalo de um minuto; equipamento a bateria com quatro envios diários e sleep no restante do dia. Coleta e envio têm regras configuráveis e despertares independentes.
- Envio programado em lotes; comandos MQTT solicitam uma nova medição do dado/canal individual ou grupo solicitado, seguida de envio.
- Alarmes podem despertar o equipamento e provocar envio fora dos horários normais. Um aviso na ativação e mensagem após tempo normal configurável por alarme, com mínimo de um minuto; detecção durante sleep ainda a detalhar.
- Dados capturados entram em fila na memória para envio; em falha de comunicação, permanecem para nova tentativa. Após recebimento confirmado pelo servidor, apagar o arquivo enviado. Espaço cheio: continuar coletando e substituir o arquivo mais antigo, mesmo que isso implique perda de dados não confirmados; registro ou aviso dessa perda não foram confirmados.
- Alertas terão espaço separado da telemetria, de tamanho pequeno; podem ser apagados após sincronização confirmada pelo servidor. Se o espaço ficar cheio, substituir o alerta mais antigo pelo novo, mesmo sem confirmação. Capacidade e classificação dos avisos ainda a definir.
- Coleta sem envio: despertar STM32 e manter rádio em sleep. Envio: ativar rádio, transmitir e voltar ao sleep.
- Requisito do usuário para NB-IoT: recepção de informação da rede, processamento, resposta e retorno ao sleep. Modo de economia, disponibilidade e sinal de despertar do STM32 serão validados; não presumir recepção imediata em PSM.
- Documento: documentacao/telemetria_e_armazenamento.md. Resumo consolidado do B2 confirmado em 30/10/2026.

## Respostas do B3 — Broker, conectividade e identidade
- Brokers possíveis: AWS IoT Core, EMQX e RabbitMQ; comunicação sempre criptografada com certificado/chave.
- Preferência por armazenamento seguro de credenciais no STM32, quando viável; depende das proteções e de onde o TLS será executado.
- Prioridade dos meios definida no setup, com fallback entre os módulos de comunicação disponíveis.
- Identidade individual do equipamento: UID do STM32. UID não é segredo de autenticação.
- Usuário aceitou certificado e chave privada individuais por dispositivo, vinculados ao cliente no sistema, substituindo a proposta anterior de compartilhamento por cliente. Renovação remota prevista, com teste/confirmação antes de retirar credencial antiga; implementação e periodicidade a definir.
- Processo funcional aceito: integrador autenticado usa o aplicativo para identificar UID e vincular ao cliente; sistema/equipamento provisionam credencial, armazenam com proteção, testam e confirmam ativação. Renovação/revogação individuais; detalhes técnicos e recuperação a definir.
- Resumo consolidado do B3 confirmado pelo usuário em 30/10/2026. Detalhes técnicos e validações registrados permanecem para etapas posteriores. Fontes e avaliação em documentacao/comunicacao_iot.md.

## Respostas do B3 — Tópicos e entrega
- Cada equipamento tem seus próprios tópicos; não haverá comandos em grupo de dispositivos. Isso não exclui solicitar grupo de canais do mesmo device.
- Comandos e telemetria exigem confirmação. Sem confirmação após cinco tentativas, manter os dados/mensagens na memória para envio posterior.
- Envio pode ser lote ou um payload individual da fila. Servidor confirma recebimento e salvamento com identificação; excluir apenas o dado/lote confirmado.
- Confirmação de comando definida pelo usuário: recebido e depois resultado da execução (executado ou falhou com motivo), associados ao mesmo comando. Validade dos comandos adiados, intervalos/timeouts de cada etapa e tratamento de duplicatas ainda a detalhar. QoS específico não escolhido.

## Respostas do B4 — Configuração e instalação (em andamento)
- Objetivo: mínimo esforço de integração/parametrização em campo, com configuração complexa controlada pelo CCO através do sistema.
- Optmile pretende operar com vários integradores e instaladores; a estimativa inicial de 2 usuários no B1 não é limite para o produto.
- Técnico de campo/instalador: somente o necessário para colocar o device online e habilitar o recebimento dos comandos de parametrização.
- Integrador: pode configurar todo o dispositivo. Funções explicitadas: calibração de sensores, parâmetros Modbus, rede Wi-Fi e APN celular.
- Demais configurações enviadas pelo sistema. Matriz detalhada de permissões será levantada no B5; não presumir exclusividade do CCO que contrarie o acesso completo do integrador.
- Fluxo esperado: instalação física, conectividade mínima, device online e parametrização via sistema. WhatsApp e gateway USB continuam válidos.
- Calibração: cadastrar parâmetros do sensor e enviar/disponibilizar tensão medida na entrada analógica. Usuário confirmou span como faixa de medição (exemplo 10 bar para 0–10 bar) e offset como ajuste do zero. Não fixar 10 bar para todos os sensores; detalhes de sinal, limites e precisão a especificar.
- Sensores digitais não terão configuração local; não inferir ausência de parâmetros remotos.
- Falha de sensor deve enviar alerta ao sistema identificando o sensor, a porta e a falha detectada, conforme confirmação do usuário. Leitura zero normal a 4 mA, sem condição de falha, indica sensor sem carga e não deve gerar alerta nem tratamento de falha. A saída em Hz segue o valor configurado para 4 mA.
- Usuário esclareceu que queria suavização de pequenas oscilações, por exemplo 5,25→5,29→5,20, sem mascarar falhas; não solicitou transformação matemática logarítmica. Comparação entre leituras consecutivas com percentual da anterior; considerar leituras acima de 1% da escala e zero da grandeza indica sensor sem carga. Leituras positivas até 1% mantêm valor medido e ficam fora da comparação percentual, por escolha do usuário; detalhes de referência após transições ainda a definir. Saída em Hz continua proporcional à corrente 4–20 mA.
- Saídas digitais podem operar como discreta ligada/desligada ou em frequência determinada em Hz, inclusive representando uma entrada 4–20 mA. Para essa conversão, usuário confirmou escolha da entrada analógica interligada à saída, frequências configuráveis em 4 mA e 20 mA e proporcionalidade intermediária; exemplo 0–100 Hz não é padrão obrigatório. Em falha do sensor, saída vai a 0 Hz; após voltar a funcionar, aguardar um minuto normal antes de retomar. Usuário definiu falha abaixo de 4 mA, acima de 20 mA ou salto entre leituras consecutivas acima de percentual configurável da leitura anterior. Janela anterior: menos de 5 segundos. Exemplo com limite de 50%: 5→8 bar corresponde a 60% e 5→1 bar a 80%; 50% não é padrão obrigatório definido. Discreta: ligar/desligar e acionar por um tempo, desligando ao final. Partida com atraso configurável por saída: o comando inicia a contagem e, ao terminar, liga a porta.
- Após retorno da energia, enviar mensagem de sistema normal e aguardar novo comando para ligar, sem religamento automático. O comando inicia o temporizador configurável por saída e a saída liga quando o tempo termina. Se um comando de desligar chegar durante a contagem, cancelar a partida e manter a saída desligada. A mensagem de normalização não aguarda o temporizador.
- Alarmes configuráveis por porta/sinal ativo, com até quatro condições por regra ligadas por até três operadores && / ||. Usuário escolheu avaliação da esquerda para a direita; A || B && C equivale a (A || B) && C.
- Comparações numéricas disponíveis nos alertas: <, >, <=, >= e ==. Portas digitais podem ser comparadas com ligada/desligada; associação de nível elétrico ao estado lógico ainda a definir conforme a instalação.
- Para cada ocorrência, disparar e enviar um aviso assim que a condição de alarme for detectada, sem espera adicional. Enviar mensagem após a condição permanecer normal pelo tempo configurado para o alarme, mínimo de um minuto. Se a condição de alarme retornar antes, reiniciar a contagem. Dois minutos foram exemplo inicial; valor padrão não definido. As tentativas de entrega continuam válidas.
- Exemplo do usuário: estado da bomba por entrada digital ou Modbus e pressão por entrada analógica, com alertas diferentes para bomba ligada e desligada. Regras reais e limiares ainda não definidos.
- Prioridades para protótipo/piloto ainda não informadas; detalhes de calibração diferencial e catálogo Modbus a levantar.
- Registro em documentacao/configuracao_do_dispositivo.md.

## B4 — Agenda operacional offline (em levantamento)
- Plataforma em nuvem analisa histórico por dia/hora e constrói agenda semanal de segunda a domingo, em intervalos de 30 minutos, para a placa guardar e operar offline.
- Uma semana contém 336 intervalos; cada intervalo registra estado ligado/desligado. Agenda gerada pelo histórico precisa de aprovação do CCO ou integrador; fluxo e atualização ainda a detalhar.
- Usuário confirmou que a agenda poderá acionar liga/desliga após retorno da energia. Considerar a ordem da agenda como origem possível de novo acionamento, respeitando atraso por saída; momento de aplicação após reinicialização ainda a definir.
- Agenda local atua somente offline: tentar reconexão MQTT cinco vezes em cada rádio disponível, a cada um minuto e pela prioridade configurada; após esgotar tentativas, entrar offline e aplicar imediatamente o estado do intervalo atual, respeitando modo manual, prioridade do sensor local e atraso de partida por saída. Ao reconectar, avisar, manter último estado da porta e aguardar novo comando da plataforma; regras locais e modo manual continuam aplicáveis. Timeouts, marco do intervalo, disponibilidade dos rádios, sleep e ordens pendentes ainda a detalhar.

- Em sleep, ao chegar horário de mudança da agenda offline, STM32 acorda, executa regra e volta a dormir.
- Usuário informou relógio interno com bateria; conferir com servidor NTP quatro vezes ao dia, substituindo a frequência anterior diária, e atualizar a hora em boot total. Hora correta é premissa inegociável para telemetria. Agenda exige hora sincronizada, mantida pelo relógio durante operação offline. Se perder a hora correta, cada saída mantém o último estado até a hora ser corrigida, inclusive em manual e diante de novas regras de sensor; novos comandos manuais aguardam a correção e agenda não decide novos horários. Tratamento de outros alertas e medições nesse estado ainda a esclarecer. Falha de sincronização não é automaticamente perda da hora.
- Dados sem horário válido não têm validade; ao detectar o problema, informar que o equipamento precisa de manutenção. Sem conexão, guardar o aviso e enviá-lo após reconexão, identificando o horário da ocorrência como desconhecido. Tratamento de outros eventos sem hora válida ainda a definir.
- Após correção da hora, executar automaticamente o último comando de cada saída que aguardava, inclusive desligamento conforme regras já definidas; detalhes de validade e interação com outras condições permanecem em aberto.

## B4 — Controle local por entrada analógica
- Regra configurada no device permite que uma entrada analógica ligue/desligue uma porta digital ao atingir limites. Exemplo do usuário: ligar a 5 bar e desligar a 4 bar.
- Configuração pode ser feita na instalação ou pelo CCO. Qualquer entrada analógica disponível pode ser associada a qualquer saída digital disponível; uma entrada pode controlar mais de uma saída.
- Em modo automático, regra do sensor prevalece sobre agenda; na falha do sensor, desligar a saída discreta controlada. Em modo manual, manter o comando manual mesmo na falha do sensor e informar a falha ao sistema. O modo é definido por porta de saída; comando manual alerta o CCO e retorno ao automático requer comando específico para a porta.
- Detalhamento em documentacao/configuracao_do_dispositivo.md.

## B4 — Modos de comando e prioridades
- Cada porta de saída opera em modo manual ou automático. Comando recebido deve identificar explicitamente se é manual ou automático; comando da plataforma pode ser de qualquer tipo.
- Comando manual prevalece sobre regras automáticas da porta destinatária; essa porta entra em modo manual e o dispositivo alerta CCO. Permanece manual até receber comando de modo automático para ela. Ao voltar ao automático, avaliar e executar imediatamente a regra válida, respeitando prioridade do sensor e regras de partida; dado indisponível/nova leitura ainda a especificar.
- Agenda, sensor local e regras da plataforma são fontes de comandos automáticos. Sensor local prevalece sobre agenda e regra automática da plataforma. Agenda local só atua offline; com conectividade prevalece a regra automática da plataforma, respeitada a prioridade do sensor local e do modo manual.
- Em falha do sensor que controla saída discreta, em automático a saída desliga; em manual mantém o comando manual. A falha continua gerando alerta. Interação com a regra de 0 Hz para saída em frequência ainda não especificada.

## B4 — Atualização de firmware
- CCO, integrador e técnico de campo podem iniciar atualização, identificando quem a enviou. Verificar validade e compatibilidade antes de instalar; se houver falha ou interrupção de energia, recuperar a versão anterior. Detalhes e pendências em [atualização de firmware](../documentacao/atualizacao_firmware.md).
- Todas as saídas digitais devem permanecer desligadas durante a atualização e a reinicialização. Após a conclusão e o retorno do dispositivo, retomar comandos e regras anteriormente ativos; sequência e interação com falhas e partida temporizada ainda a definir.
- Na retomada após atualização, saída que estava ligada e possui tempo de partida configurado aguarda esse tempo antes de ligar novamente.
- Enviar mensagem de dispositivo em atualização; destinatários, momento e confirmação de entrega ainda a definir.
- Informar ao sistema atualização concluída ou falha com retorno à versão anterior, identificando equipamento e responsável; tratamento de falha na recuperação ainda a definir.
- Sem conexão, guardar o resultado da atualização na fila persistente para envio posterior; remover somente após confirmação do servidor.

## B4 — Falha de leitura Modbus
- Diante de equipamento Modbus que não responde, realizar dez tentativas, uma a cada 30 segundos, antes de informar falha. Continuar tentando e informar defeito após uma hora de persistência, contada a partir do fim das dez tentativas sem resposta. Após cinco leituras consecutivas bem-sucedidas, considerar a comunicação restabelecida e enviar mensagem ao sistema. Contagem da primeira consulta, timeout e configuração ainda a esclarecer.
- Sinais Modbus seguem as regras de controle e falha já definidas para sensores: após dez tentativas sem resposta, desligar saídas discretas automáticas que dependam do sensor com falha; em manual, manter o comando manual. Saída em frequência que reproduz sinal Modbus vai a 0 Hz na falha e retoma após um minuto de funcionamento normal; recuperação da comunicação também exige cinco leituras consecutivas válidas, com relação entre contagens a detalhar.
- Após cinco leituras válidas consecutivas e recuperação da comunicação, reavaliar a regra do sensor para as saídas automáticas afetadas; se a regra determinar ligar, respeitar o tempo de partida configurado por saída.

## Respostas do B4 — Armazenamento de alertas e validade de comandos (01/11/2026)
- **Eventos no espaço de alertas:** alarmes de condição, falha de sensor e falha de comunicação Modbus. Aviso de hora inválida/relógio e demais eventos não foram selecionados nesta rodada; classificação pendente.
- **Capacidade:** número fixo de registros; ao lotar, substitui o mais antigo. Quantidade máxima ainda não informada.
- **Validade de comando adiado:** válido até ser substituído por novo comando da mesma saída.
- **Quantidade de alertas:** definida na implementação, sem número fixo nesta fase.
- **Aviso de hora inválida:** entra no espaço de alertas, com a mesma regra de substituição e apagamento.
- **Escopo do protótipo de bancada (primeiro mês):** coleta e fila offline; MQTT com credencial individual; entradas e saídas (4–20 mA, saídas digitais, controle por sensor e falha de sensor); Modbus RTU.
- Registro: documentacao/telemetria_e_armazenamento.md e documentacao/agenda_operacional_offline.md.

## Respostas do B4 — Modbus e retorno da saída em frequência (01/11/2026)
- **Contagem:** a primeira consulta sem resposta conta como uma das dez tentativas.
- **Timeout:** fixo no firmware, não configurável por equipamento; valor a definir.
- **Retorno da saída em frequência:** primeiro as cinco leituras válidas consecutivas; depois começa a contagem de um minuto de funcionamento normal.
- Registro: documentacao/comunicacao_iot.md.

## Respostas — pendências finalizadas (01/11/2026)
- **Amostragem:** 10 a 1000 amostras por segundo por entrada; média simples por segundo.
- **Envio:** média móvel com janela de um envio.
- **Dente de serra:** inversão de sentido acima do percentual, dentro de 5 segundos. Padrão de 2% sem inversor e 10% com inversor de frequência. Falha vai para 0 Hz, com alerta e valor real sem suavização.
- **Escala logarítmica:** não é requisito.
- **Protótipo de bancada:** sensores 4–20 mA reais, com NB-IoT incluído (módulo e operadora).
- **Timeout Modbus:** 30 segundos por consulta, fixo no firmware.
- **Espaço de alertas:** 300 alertas.
- Registros: documentacao/configuracao_do_dispositivo.md e documentacao/telemetria_e_armazenamento.md.

## Respostas do B5 — separação device x sistema (fechado pelo usuário em 01/11/2026)
- **Device:** sessão única por dispositivo, bloqueio durante a configuração, técnico de campo configura a conectividade, caminho do tópico MQTT enviado pelo sistema.
- **Sessão de configuração (01/11/2026):** iniciada pelo técnico na aplicação do notebook (serial) ou pelo sistema via MQTT. Durante a sessão, comandos MQTT são recusados com aviso. A sessão termina após 5 minutos sem atividade. A configuração vale também pelo Modbus, com a mesma regra de sessão única.
- **Tópicos padrão mínimos (01/11/2026):** SETUP e INFO; STATUS/heartbeat com dados de conectividade do módulo (id, IP, rede e operadora, entre outros a detalhar); COMANDO e resultado (recebido, executado ou falhou com motivo); telemetria e alertas.
- **INFO:** o sistema pede a troca; o device responde.
- **Configuração mínima para operar:** identidade (UID e credencial individual), caminho do tópico, conectividade (configurada pelo técnico de campo) e intervalos e modo de operação.
- Pendente: lista completa de campos do STATUS/heartbeat e formato das mensagens SETUP e INFO.
- **Formato das mensagens (01/11/2026):** JSON. O id do módulo vem no tópico e não entra no JSON. O JSON é separado por grupos (analógico, digital, rede, falhas, entre outros). Os nomes das chaves seguem o endereço Modbus do dado.
- **Valores e endereços (01/11/2026):** endereços criados seguindo a norma Modbus. Medições em input registers (3xxxx), configurações e escritas em holding registers (4xxxx), entradas binárias em discrete inputs (1xxxx) e saídas em coils (0xxxx). Valores como número JSON, não texto. O JSON inclui timestamp com a hora do device, que precisa ser válida (B4).
- Referência de padrão: `sistema-init/modbus rtu CLP Raptor 4G.docx`, manual de outro equipamento (CLP Raptor 4G/WIFI).
- **Mapa Modbus criado (01/11/2026):** `documentacao/mapa_modbus.md`. Float em 2 registradores, 15 posições por tipo de entrada e saída, conectividade, sistema trifásico reservado e configurações. O manual Raptor é referência de conteúdo, não de endereços.
- Pendências do mapa resolvidas em 01/11/2026: float big-endian (ABCD), tempo de acionamento em segundos, endereço escravo de 1 a 247, configuração pelo Modbus e trifásico reservado para integração com MGE e inversores. Restam os nomes dos grupos do JSON, a lista do heartbeat e o formato de SETUP e INFO.
- **Sistema (não é regra do device):** perfis, hierarquia, unidades e acesso por horário ou dia da semana. Registrados abaixo apenas como referência de requisito do sistema.
- **Perfis:** Cliente, Integrador, Técnico de campo e Suporte/administrador.
- **Sessão:** única por **dispositivo**, nunca mais de uma sessão no módulo. A regra não vale por usuário nem por empresa.
- **Modo de configuração:** enquanto o equipamento estiver em configuração, não recebe comandos. A sessão fica travada durante a configuração.
- **Hierarquia:** país > estado > cidade > empresa > unidade.
- **Várias unidades:** um usuário pode ver dados de várias unidades, com restrição. Detalhe da restrição pendente.
- **Horário e dia da semana:** sim, por perfil. Janelas por perfil pendentes.
- Pendentes do B5: restrição de unidades, janelas de horário por perfil e ações exclusivas de cada perfil.

## Respostas do B6 — interfaces e painéis (fechado pelo usuário em 01/11/2026)
- Tratado como requisito do sistema, conforme decisão do usuário.
- **Interface do device:** WhatsApp, conforme já definido. Sem conectividade, a interface é a porta serial, pela aplicação Python no notebook do técnico.
- **Painéis:** o device se comunica por MQTT e não tem painéis próprios. Não há telas personalizadas no device.
- Pendências de sistema (idioma, acessibilidade, identidade visual, escolha de Grafana ou painel próprio) ficam no requisito do sistema, fora do firmware.

## Respostas do B7 — aplicativos móveis (fechado pelo usuário em 01/11/2026)
- Tratado como fora do device, conforme decisão do usuário. O B7 do roteiro é do sistema.
- **Sem app de celular:** o celular funciona sempre pelo WhatsApp.
- **Gateway do técnico:** aplicação Python no notebook, com chaveiro serial ligado ao STM32. Substitui o app de celular descrito antes.
- Registros de configuração e de gateway atualizados em `documentacao/configuracao_do_dispositivo.md`.

## Respostas do B8 — comunicação e chat (fechado pelo usuário em 01/11/2026)
- **Regra do device:** toda a comunicação do device é por MQTT ou serial. Não há outro canal de rede.
- **Canais:** WhatsApp e Telegram.
- **Ações pelo chat:** responder e executar comandos nos equipamentos.
- **Coordenação:** os comandos são coordenados pelo sistema, pelo servidor MQTT. O device só se comunica com o mundo externo pela plataforma, via MQTT.
- **Serial:** a aplicação Python no notebook do técnico é restrita a configurações definidas.
- **Confirmação humana:** somente a atualização de firmware exige confirmação pelo chat.
- **Identificação:** número cadastrado (WhatsApp ou Telegram vinculado ao usuário no sistema).
- **Idioma das respostas:** português e inglês.

## Definições do device (01/11/2026)
Regra de processo: a entrevista do firmware só pergunta o que é do device. O que depende do sistema não entra aqui.
- **Comunicação:** toda a comunicação do device é por MQTT ou serial.
- **Linguagens:** C para o STM32 (coletor e orquestrador) e ESP-IDF para o ESP32 (módulo de comunicação).
- **Documentação do código:** Doxygen em português.
- **Protocolos:** MQTT 5.0 e TLS 1.3. Validar no módulo de comunicação e no broker, porque a memória do STM32L4 e do L0 pode limitar o TLS.
- **Versionamento:** data e build.
- **Normas:** Modbus Application Protocol v1.1b3 e Modbus Serial Line v1.02 (RTU em RS-485).
- **Build do STM32:** CMake com toolchain ARM e HAL da ST.
- **Testes:** no PC (unitários da lógica, sem hardware), simulador em Python (broker MQTT e comportamento offline e reconexão) e bancada com a placa real.
- Pendentes do device: build do ESP32 (ESP-IDF), lista de campos do heartbeat, formato de SETUP e INFO, nomes dos grupos do JSON.

## Processo de desenvolvimento (01/11/2026)
- **Git:** repositório local iniciado em `firmware/`, com remoto `origin` para `https://github.com/malufis/ologgerIOT.git`. Em 03/11/2026 o branch `feat/fw-002-cmake-raiz` tem commits enviados ao remoto e o PR #1 está aberto para `main`; commit e push seguem dependendo de ordem explícita do usuário.
- **Documentação:** índice em `documentacao/README.md`. Dicionário de funções, padrão de código C (base MISRA C:2012), contrato de integração e manual de operação criados como esqueletos.
- **Backlog:** `planejamento/BACKLOG.md`, com 63 itens (FW-001 a FW-063) em 12 fases, incluindo a fase 1B. Sprints de uma semana, de SPRINT_00 a SPRINT_09, em `planejamento/sprints/`.
- **Skills:** `c-firmware-standards`, `doc-sync`, `device-profile-build` e `sprint-task` em `firmware/.claude/skills/`.
- **Agentes:** `firmware-dev` e `doc-keeper` em `firmware/.claude/agents/`.
- **Scripts prontos (02/11/2026):** `tools/verifica_padrao.py` e `tools/gera_dicionario.py` (FW-004 e FW-008), citados pelas skills.
- **Risco de prazo:** o escopo do protótipo de 29/11/2026 não cabe no cronograma de uma semana por sprint. Reavaliar antes da SPRINT_03.

## Decisões de desenvolvimento (02/11/2026)
- **Um projeto, um build por tipo de device:** `ologgermulti` (STM32L412 e ESP32-C6) e `y7080` (STM32L071). Todas as funcionalidades ficam no mesmo código; o arquivo `builds/composicao.cmake` diz o que entra em cada build. Pinagem por placa em `devices/`. Esclarecido pelo responsável técnico em 02/11/2026, e a composição de cada build foi confirmada no mesmo dia (detalhes em `documentacao/composicao_builds.md`). ESP-IDF de referência: v5.4.4, em `C:\Espressif`.
- **Código:** pequenos passos, uma função ou módulo por vez, com teste no mesmo passo. Boas práticas de C em `documentacao/padrao_codigo_c.md` e de ESP-IDF em `documentacao/padrao_esp_idf.md`.
- **Documentação viva:** `CHANGELOG.md`, `documentacao/ESTADO_IMPLEMENTACAO.md`, `documentacao/dicionario_funcoes.md`, `documentacao/INDICE_AGENTES.md`, `INICIAR.md` e `README.md` na raiz. Sprint só fecha com a skill `sprint-close`.
- **Agentes e skills:** `esp-idf-dev` e `firmware-reviewer` criados; skills `esp-idf-standards` e `sprint-close` criadas; `firmware-dev`, `doc-keeper`, `c-firmware-standards`, `device-profile-build`, `doc-sync` e `sprint-task` atualizados.

## Decisões de dados Modbus e rádio externo (02/11/2026)
- **MQTT do y7080:** cliente roda no modem SIM7080G, por comandos AT.
- **Rádio externo do OloggerMulti:** UART1 do STM32 (PA9/PA10), repassa bytes sem interpretar. Integrador por tipo de rádio é fase futura.
- **Dados de equipamentos Modbus:** tabela de variáveis só na RAM, com 64 variáveis no total e 8 equipamentos. O mapeamento de cada equipamento é configurado pelo sistema e pelo gateway USB e gravado em flash. Proposta detalhada em `documentacao/modbus_mapeamento_dados.md`.
- **Princípio:** o STM32 é o cérebro; ESP32 e modem só transportam.
- **FreeRTOS em todo o sistema** (STM32 e ESP32), com alocação estática. Regras em `documentacao/padrao_codigo_c.md`, seção FreeRTOS.
- **Modbus aprovado** conforme `documentacao/modbus_mapeamento_dados.md` (v0.2.0). Escrita em equipamento externo fora do escopo. Backlog: FW-046 na SPRINT_00 e FW-047 a FW-050 na fase de Modbus.

## Decisões de entrada digital e contador de pulsos (03/11/2026)
- **Uso:** o device pode ser contador de pulsos, com até quatro leitores ao mesmo tempo, pulsos sem repique de 5 a 10 por segundo. Tensão de campo de 12 a 24 V, a mesma fonte da entrada do CLP; na bancada, até 12 V e sem a D1 (HW-008).
- **Filtros:** 50 ms no estado discreto e 20 ms nos pulsos; leitura a cada 2 ms.
- **Sleep:** Stop 2. O pulso acorda o STM32, que fica acordado e só dorme de novo depois de 60 s sem pulso (configurável); com pulso constante não acorda e dorme a cada pulso. O ESP32 só é acordado para transmitir (1, 2 ou 5 vezes ao dia, ou por entrada analógica), em outra sprint. O sleep começa desabilitado.
- **Totalizador:** cada entrada tem um total de 32 bits que só soma, como um hodômetro, guardado nos registradores de backup (duas cópias com complemento) e gravado a cada pulso. Quem lê calcula a diferença. Nenhum pulso pode ser perdido antes da transmissão.
- **Frequência em Hz:** faixa de projeto de 0 a 20 kHz; o ajuste fica para depois.
- Detalhes em `documentacao/configuracao_do_dispositivo.md` e `planejamento/sprints/SPRINT_01B.md`.

## Saídas digitais (03/11/2026)
- **Circuito:** NPN BC847 em coletor aberto por saída; pino em nível alto liga e puxa `OUT-n` ao terra; base com 1k e sem pull-down (HW-009).
- **Implementado (FW-022, FW-024 e FW-025):** ligar, desligar e retardo de acionamento (relé de tempo), com as saídas desligadas na partida. O "acionamento por tempo" é o retardo; não há desligamento automático. Todo comando de ligar espera o retardo; com a saída ligada ou com o retardo em andamento, outro ligar é descartado; desligar cancela a partida. Modo manual e automático por saída, com prioridade do sensor local e falha do sensor (FW-025), em código. Falta a saída em frequência (FW-023) e o alerta ao CCO. "Pulsada" é a saída em frequência.
- **Sleep e frequência:** a saída em frequência só habilita com o sleep desligado, e o sleep só liga sem saída em frequência (FW-061). Diodo de roda livre no relé externo. Melhorias de hardware em `documentacao/melhorias_hardware.md`.
- Detalhes em `planejamento/sprints/SPRINT_04.md` e `documentacao/configuracao_do_dispositivo.md`.

- **Comandos automáticos (04/11/2026):** os **comandos automáticos locais** funcionam sem conectividade e são a **regra do sensor** (entrada analógica para saída digital, cadastrada com máximo, mínimo e frequência) e a **agenda**, que só vale offline. O **comando automático remoto** é o do CCO e vale online. Com regra do sensor, a agenda e o CCO são ignorados. Os campos do cadastro vêm por MQTT ou Modbus, com endereço Modbus. Falha do sensor: a saída só volta a operar pelo sensor depois de 5 minutos estáveis. Sem pedido válido, mantém o último estado válido do comando. Depois de reinício, desligada, automática e offline.

## Próximo passo recomendado
A entrevista está encerrada e o firmware está em desenvolvimento. Os dois builds STM32 compilam sem avisos. Próximo: executar o roteiro de bancada da entrada digital (FW-056) quando o device estiver disponível, soldar a L2 e fechar o FW-012. O FW-007 (ESP32) e a parte ESP32 do FW-046 foram para a SPRINT_07 e só começam depois de finalizados os testes do STM32 e de o STM32 estar validado (decisão de 03/11/2026). O estado de cada módulo está em `documentacao/ESTADO_IMPLEMENTACAO.md`; o backlog e as sprints, em `planejamento/`. As pendências de heartbeat, formato de SETUP e INFO e nomes dos grupos do JSON ficam para a fase 7. Execução do teste NB-IoT (P-026) fica para a fase de bancada.

## Histórico de atualizações
| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.3.167 | P-045 resolvida; P-046 aberta (local da persistência); persistência da agenda escrita |
| 04/11/2026 | 0.3.166 | FW-032: tarefa da agenda pelo RTC escrita; P-045 aberta (agenda sem fuso) |
| 04/11/2026 | 0.3.165 | FW-030: driver do RTC escrito, sem teste em hardware |
| 04/11/2026 | 0.3.164 | P-030 resolvida: bateria VBAT opcional; cristais só no OloggerMulti |
| 04/11/2026 | 0.3.163 | FW-032: aplicador da agenda pelo horário atual pronto |
| 04/11/2026 | 0.3.162 | FW-030 parcial: módulo `tempo` pronto |
| 04/11/2026 | 0.3.161 | Saída em frequência: o sensor manda; teste na bancada |
| 04/11/2026 | 0.3.160 | FW-023: gerador ligado ao TIM6 e aos pinos (sem teste em hardware) |
| 04/11/2026 | 0.3.159 | FW-023: gerador de frequência pronto e testado |
| 04/11/2026 | 0.3.158 | FW-061 pronto nos dois lados; FW-023 com as decisões de 0 Hz, duty de 50% e canais |
| 04/11/2026 | 0.3.157 | FW-023 parcial (freq_saida pronto) |
| 04/11/2026 | 0.3.156 | P-044 resolvida; FW-063 parcial (regra_sensor pronto) |
| 04/11/2026 | 0.3.155 | P-043 resolvida (fuso pelo CCO, só a última agenda, aplicação pelo horário atual); P-044 aberta (identificador do comando) |
| 04/11/2026 | 0.3.154 | P-041 e P-042 resolvidas (sentido confirmado; agenda por MQTT); P-043 aberta |
| 04/11/2026 | 0.3.153 | P-041 resolvida (cadastro por MQTT ou Modbus; agenda é comando local); P-042 aberta (cadastro da agenda) |
| 04/11/2026 | 0.3.152 | Comandos automáticos locais e remotos; P-040 resolvida; P-041 aberta; novo FW-063 |
| 04/11/2026 | 0.3.151 | FW-025 em código pronto; pendência P-040 com o que o B4 não especifica |
| 04/11/2026 | 0.3.150 | ADR-0003 decidido (imagem na flash externa com bootloader); P-038 resolvida; novo FW-062 |
| 04/11/2026 | 0.3.149 | SPRINT_00 concluída em 04/11/2026 (changelog 0.2.0); cinco regras do padrão de código confirmadas; ADR-0003 escolhido como o primeiro a decidir |
| 04/11/2026 | 0.3.148 | Correções da verificação do doc-keeper antes do fechamento da SPRINT_00 (números, nomes dos registradores, concordância e referências) |
| 04/11/2026 | 0.3.147 | Pendências P-038 (espaço de flash do STM32L412C8) e P-039 (ADRs de FreeRTOS e MISRA); criada a pasta `documentacao/adr/` |
| 03/11/2026 | 0.3.146 | O "acionamento por tempo" é o retardo de acionamento; ligar com a saída ligada ou aguardando é descartado; mapa Modbus das saídas ajustado (P-037) |
| 03/11/2026 | 0.3.145 | Saídas digitais: pulsada é a saída em frequência, retardo de acionamento, sleep e frequência não coexistem, melhorias de hardware anotadas; P-034 e P-036 resolvidas, P-035 anotada e P-037 aberta |
| 03/11/2026 | 0.3.144 | SPRINT_04 começou: saídas digitais discretas (FW-022) em código pronto; pendências P-034 a P-036 |
| 03/11/2026 | 0.3.143 | Decisão do responsável técnico: o ESP32 (FW-007) fica para depois de finalizados os testes do STM32 e de ele estar validado; movido para a SPRINT_07 |
| 03/11/2026 | 0.3.142 | Fase de desenvolvimento: aquisição analógica, entrada digital, contador de pulsos, totalizador e gerência de energia em código; decisões de 03/11/2026 registradas; pendências P-028 a P-033; backlog com situação e sprints 01 a 09 criadas |
| 15/10/2026 | 0.1.0 | Criação do status inicial a partir do rascunho |
| 15/10/2026 | 0.2.0 | Removido nome provisório; decisões de stack deferidas; lambda e auditoria incorporados |
| 15/10/2026 | 0.3.0 | Entrevista em 18 blocos: IoT/MQTT, apps móveis, banco/RLS/LGPD e auditoria separada |
| 29/10/2026 | 0.3.1 | Início do B0; registrados nome oficial, ausência de codinome e repositório informados pelo usuário |
| 29/10/2026 | 0.3.2 | Registrados Optmile como cliente ou patrocinador, Rodrigo Maluf como responsável técnico e construção do firmware como objetivo |
| 29/10/2026 | 0.3.3 | Registrado o contexto do dispositivo: ARM, interfaces seriais, módulos 4G e Wi-Fi, RS-485, MQTT e entradas e saídas de sensores |
| 29/10/2026 | 0.3.4 | Registrado Rodrigo Maluf como responsável pelas decisões na Optmile; B0 respondido, aguardando confirmação do resumo |
| 29/10/2026 | 0.3.5 | Resumo do B0 confirmado pelo usuário; identificação concluída e início do B1 |
| 29/10/2026 | 0.3.6 | Registradas respostas B1.1 a B1.3: saneamento, integradores e técnicos do cliente, piloto em campo |
| 29/10/2026 | 0.3.7 | Registrados sistema desenvolvido junto ao firmware e protótipo de bancada em um mês; exclusões de escopo marcadas como pendentes |
| 29/10/2026 | 0.3.8 | Registrados 2 usuários iniciais, atualização remota e configuração por serial, Bluetooth e internet; ideia de terminal ou WhatsApp com webhook e MQTT; orçamento pendente |
| 29/10/2026 | 0.3.9 | Usuário considera orçamento não relevante agora; limites da primeira versão adiados até levantamento das funcionalidades no B4 |
| 29/10/2026 | 0.3.10 | B1 confirmado pelo usuário com limites das entregas adiados até B4 e orçamento dispensado nesta etapa; início do B2 |
| 29/10/2026 | 0.3.11 | Registrada premissa transversal de família de modelos ARM, atualmente STM32, com composição e quantidades de portas, sensores e rádios por modelo de equipamento/firmware |
| 29/10/2026 | 0.3.12 | Modbus RTU adotado por instrução do usuário para comunicação interna e externa; consultadas especificações oficiais e criado documentacao/comunicacao_iot.md; adaptações por canal pendentes |
| 29/10/2026 | 0.3.13 | Esclarecidos RTU para sensores/atuadores e AT por modelo de rádio; registrados C no STM32 e ESP-IDF no ESP32; papel do ESP32 pendente |
| 29/10/2026 | 0.3.14 | STM32 confirmado como coletor e orquestrador central, ESP32 sempre como módulo de comunicação com possível armazenamento; registrados 16 MB disponíveis ao STM32, tipo a detalhar |
| 29/10/2026 | 0.3.15 | Registrados WhatsApp em linguagem natural, comandos/respostas MQTT com device conectado, app gateway USB com comandos do servidor e modo local restrito à conectividade; criado documento de configuração |
| 29/10/2026 | 0.3.16 | Corrigidos fluxos para explicitar a camada de rádios, ESP32 ou outros módulos de comunicação externos e rede entre STM32 e broker MQTT |
| 29/10/2026 | 0.3.17 | Analisados esquemas e Gerbers de OloggerMulti/y7080; identificados componentes, flash SPI externa e mapas de pinos; registradas pendências USB, alimentação, boot ESP32, EN do regulador e variante do modem |
| 29/10/2026 | 0.3.18 | Responsável técnico confirmou parte elétrica toda resolvida; encerrados P-021 a P-024 como bloqueios elétricos e retomado levantamento de firmware |
| 29/10/2026 | 0.3.19 | Usuário definiu firmware para OloggerMulti e y7080, com seleção do modelo na geração/gravação; criada documentação de modelos e identificação das versões |
| 29/10/2026 | 0.3.20 | Usuário corrigiu interpretação de imagens separadas: ideia de compilação única com setup dos recursos do device; organização a definir depois pelo usuário e foco atual em entender o projeto |
| 30/10/2026 | 0.3.21 | Confirmada interoperabilidade industrial Modbus RTU: leitura de equipamentos externos e integração como escravo na rede do cliente; registrados modelo de dados e aquisição analógica |
| 30/10/2026 | 0.3.22 | Encerrado entendimento conceitual Modbus por orientação do usuário; mantido RTU nos meios seriais e de rede e esclarecida distinção entre papéis RS-485 e MQTT |
| 30/10/2026 | 0.3.23 | Registrados modos contínuo/sleep e intervalos por setup, envio em lotes ou sob comando MQTT, alarmes, armazenamento offline e substituição do arquivo mais antigo; B2 em andamento |
| 30/10/2026 | 0.3.24 | Esclarecidos despertares de STM32 para coleta, rádio para transmissão, recepção NB-IoT desejada e despertar por alarme; criada P-026 para validar disponibilidade de rede em baixo consumo |
| 30/10/2026 | 0.3.25 | Usuário confirmou o comportamento de coleta, envio, sleep, alarmes e armazenamento; B2 continua com seleção de dados sob comando e retenção após envio a esclarecer |
| 30/10/2026 | 0.3.26 | Usuário definiu nova medição sob comando MQTT e fila em memória: manter em falha, reenviar posteriormente e apagar o arquivo após recebimento confirmado pelo servidor |
| 30/10/2026 | 0.3.27 | Usuário confirmou o resumo consolidado do B2; progresso atualizado para 3/18 blocos e início do B3 pelas decisões de comunicação ainda abertas |
| 30/10/2026 | 0.3.28 | Registrados AWS IoT/EMQX/RabbitMQ, criptografia e preferência de chave no STM32, fallback por setup, UID e proposta de certificado por cliente; avaliada rotação remota e recomendação de credenciais individuais |
| 30/10/2026 | 0.3.29 | Usuário aceitou credencial individual por dispositivo e processo de provisionamento assistido pelo aplicativo, com vínculo ao cliente e renovação/revogação individuais |
| 30/10/2026 | 0.3.30 | Definidos tópicos por equipamento sem comandos em grupo, confirmação de comandos e telemetria, cinco tentativas e retenção para envio posterior; lote/payload só excluído após confirmação do servidor |
| 30/10/2026 | 0.3.31 | Usuário definiu dois retornos para comandos: recebido e depois executado ou falhou com motivo; próximo passo é confirmação do resumo B3 |
| 30/10/2026 | 0.3.32 | Usuário confirmou o resumo consolidado do B3; progresso atualizado para 4/18 blocos e início do B4 para levantar funcionalidades e prioridades |
| 30/10/2026 | 0.3.33 | B4 definiu instalação com esforço mínimo, instalador limitado à conectividade, integrador com acesso completo e CCO na configuração complexa; citadas calibração, Modbus, Wi-Fi e APN |
| 30/10/2026 | 0.3.34 | B4 definiu cadastro de parâmetros do sensor, leitura/envio de tensão analógica para calibração, span e offset no 4–20 mA e nenhuma configuração local de sensores digitais |
| 30/10/2026 | 0.3.35 | Usuário confirmou span como faixa física do sensor, exemplo 10 bar, e offset para ajustar o zero; significado registrado sem fixar fórmula ou faixa única |
| 30/10/2026 | 0.3.36 | B4 registrou alertas por sinais ativos e até quatro concatenações E/OU, com exemplo de estado da bomba via digital/Modbus combinado à pressão analógica; contagem e agrupamento pendentes |
| 30/10/2026 | 0.3.37 | B4 definiu operadores de comparação numérica <, >, <=, >= e == e comparações ligada/desligada para portas digitais |
| 30/10/2026 | 0.3.38 | Usuário esclareceu que cada regra de alerta pode ter até quatro condições ligadas por até três operadores E/OU |
| 30/10/2026 | 0.3.39 | Usuário definiu avaliação de regras de alerta da esquerda para a direita, sem precedência automática de E sobre OU |
| 30/10/2026 | 0.3.40 | Usuário definiu um aviso quando o alarme ativa e mensagem quando retorna ao normal; tentativas de entrega permanecem conforme política de comunicação |
| 30/10/2026 | 0.3.41 | Definida histerese temporal de dois minutos em condição normal antes de notificar normalização do alarme |
| 30/10/2026 | 0.3.42 | Usuário definiu histerese de normalização configurável por alarme, com mínimo de um minuto; dois minutos mantidos como exemplo |
| 30/10/2026 | 0.3.43 | Usuário definiu que o alerta dispara assim que a condição for detectada; histerese de tempo permanece no retorno ao normal |
| 30/10/2026 | 0.3.44 | Usuário definiu ligar/desligar, acionamento temporizado e relé de partida com atraso após falta de energia; tempo e religamento após espera pendentes |
| 30/10/2026 | 0.3.45 | Usuário definiu atraso de partida configurável, novo comando obrigatório para religar e mensagem de sistema normal após retorno de energia; abrangência e momento exato a esclarecer |
| 30/10/2026 | 0.3.46 | Usuário corrigiu partida: mensagem de sistema normal após retorno; comando de ligar inicia contagem e porta liga ao fim do tempo configurado |
| 30/10/2026 | 0.3.47 | Usuário confirmou que o tempo de partida é configurável individualmente por saída |
| 30/10/2026 | 0.3.48 | Usuário definiu cancelamento de partida por comando de desligar e saída digital em modo discreto ou frequência, capaz de representar entrada 4–20 mA em Hz |
| 30/10/2026 | 0.3.49 | Usuário confirmou frequências configuráveis em 4 mA e 20 mA e conversão proporcional para saída digital em Hz |
| 30/10/2026 | 0.3.50 | Usuário definiu seleção da interligação analógica/digital, saída em 0 Hz na falha e retomada após um minuto de funcionamento normal |
| 30/10/2026 | 0.3.51 | Usuário definiu falha abaixo de 4 mA, acima de 20 mA ou histerese superior a 50% em menos de 5 s; exigiu escala logarítmica e ausência de dente de serra; criada P-027 para esclarecer termos |
| 30/10/2026 | 0.3.52 | Usuário confirmou variação de 50% da faixa total, saída em Hz proporcional e remoção de oscilações sem mascarar falhas; escala logarítmica e oscilações abruptas repetidas ainda a detalhar |
| 30/10/2026 | 0.3.53 | Usuário esclareceu comparação entre leituras consecutivas e suavização de pequenas oscilações; registrada diferença entre exemplos 5→8/1 bar e o limiar anterior de 50% da faixa |
| 30/10/2026 | 0.3.54 | Usuário escolheu limite percentual configurável da leitura anterior entre leituras consecutivas, substituindo a referência à faixa total; tratamento de zero a esclarecer |
| 30/10/2026 | 0.3.55 | Usuário definiu considerar leituras acima de 1% da escala e que grandeza zero indica sensor sem carga; tratamento das leituras positivas até 1% e transições a esclarecer |
| 30/10/2026 | 0.3.56 | Usuário escolheu preservar leituras positivas até 1% da escala, excluindo-as apenas da comparação percentual de saltos |
| 30/10/2026 | 0.3.57 | Usuário confirmou envio de alerta ao sistema quando houver falha de sensor e ausência de alerta para leitura zero normal a 4 mA |
| 30/10/2026 | 0.3.58 | Usuário confirmou que alerta de falha deve identificar o sensor, a porta e a falha detectada |
| 31/10/2026 | 0.3.59 | Registrada proposta de agenda semanal offline gerada pela nuvem a partir do histórico, com intervalos de 30 minutos; pendentes semântica, aprovação e interação com retorno de energia |
| 31/10/2026 | 0.3.60 | Confirmados estado da agenda por intervalo, aprovação CCO/integrador e acionamento pela agenda; definido controle local de saída por entrada analógica com múltiplas saídas por entrada |
| 31/10/2026 | 0.3.61 | Usuário definiu modos manual/automático, classificação dos comandos, alerta ao CCO, prioridade do sensor sobre agenda e desligamento da saída na falha do sensor |
| 31/10/2026 | 0.3.62 | Usuário definiu modo manual/automático por porta e manutenção do comando manual quando o sensor falha; em automático a saída controlada desliga |
| 31/10/2026 | 0.3.63 | Usuário definiu que o retorno da porta ao modo automático executa imediatamente a regra válida |
| 31/10/2026 | 0.3.64 | Usuário definiu prioridade do sensor local sobre comando automático por regra da plataforma em conflito na mesma saída |
| 31/10/2026 | 0.3.65 | Usuário definiu agenda local somente offline e prioridade da regra automática da plataforma quando houver conectividade; prioridade do sensor local preservada |
| 31/10/2026 | 0.3.66 | Usuário definiu aplicação imediata do estado da agenda para o intervalo atual quando a conexão cair |
| 31/10/2026 | 0.3.67 | Usuário definiu aviso de reconexão, manutenção do último estado da porta e espera por novo comando da plataforma |
| 31/10/2026 | 0.3.68 | Usuário definiu estado offline após esgotar as tentativas de reconexão com o servidor MQTT; quantidade e intervalos a esclarecer |
| 31/10/2026 | 0.3.69 | Usuário definiu cinco tentativas de reconexão MQTT por rádio antes da entrada em modo offline |
| 31/10/2026 | 0.3.70 | Usuário definiu intervalo de um minuto entre tentativas de reconexão MQTT |
| 31/10/2026 | 0.3.71 | Usuário definiu despertar para agenda durante sleep, relógio interno com bateria, atualização diária/no boot total e desligamento se perder a hora correta; alcance a esclarecer |
| 31/10/2026 | 0.3.72 | Usuário corrigiu comportamento quando relógio perde a hora correta: manter último estado de cada saída conforme último comando, inclusive manual |
| 31/10/2026 | 0.3.73 | Usuário definiu manutenção do último estado até correção da hora, inclusive diante de regra local do sensor; agenda depende de relógio sincronizado |
| 31/10/2026 | 0.3.74 | Usuário definiu conferência do relógio interno com servidor NTP quatro vezes ao dia, substituindo frequência anterior diária; atualização no boot total mantida |
| 31/10/2026 | 0.3.75 | Usuário autorizou atualização pelos três perfis com identificação do responsável, verificação e recuperação da versão anterior em falha ou interrupção de energia |
| 31/10/2026 | 0.3.76 | Usuário definiu desligamento de todas as saídas digitais durante atualização e reinicialização |
| 31/10/2026 | 0.3.77 | Usuário solicitou mensagem de dispositivo em atualização |
| 31/10/2026 | 0.3.78 | Usuário escolheu retomar comandos e regras anteriores após a atualização |
| 31/10/2026 | 0.3.79 | Usuário definiu esperar o tempo de partida configurado antes de religar saída anteriormente ligada após atualização |
| 31/10/2026 | 0.3.80 | Usuário confirmou envio ao sistema do resultado da atualização com identificação do equipamento e responsável |
| 31/10/2026 | 0.3.81 | Usuário confirmou retenção e reenvio do resultado da atualização sem conexão, com exclusão após confirmação do servidor |
| 31/10/2026 | 0.3.82 | Usuário definiu hora correta como premissa inegociável e espera de novos comandos manuais até correção do relógio |
| 31/10/2026 | 0.3.83 | Usuário definiu dados sem horário válido como sem validade e solicitou informar problema e necessidade de manutenção |
| 31/10/2026 | 0.3.84 | Usuário confirmou guardar aviso de falha do relógio sem conexão e enviá-lo após reconexão com horário da ocorrência desconhecido |
| 31/10/2026 | 0.3.85 | Usuário definiu execução automática do último comando após correção da hora, inclusive regras de desligamento já definidas |
| 31/10/2026 | 0.3.86 | Usuário definiu dez tentativas de leitura Modbus a cada 30 segundos antes de informar falha de comunicação |
| 31/10/2026 | 0.3.87 | Usuário definiu continuar tentativas Modbus e informar defeito se a condição persistir por uma hora |
| 31/10/2026 | 0.3.88 | Usuário definiu início da contagem de uma hora após as dez tentativas sem resposta |
| 31/10/2026 | 0.3.89 | Usuário confirmou mensagem ao sistema quando a comunicação Modbus for restabelecida |
| 01/11/2026 | 0.3.90 | Usuário definiu cinco leituras consecutivas bem-sucedidas para considerar a comunicação restabelecida |
| 01/11/2026 | 0.3.91 | Usuário confirmou aplicar ao Modbus as regras de controle e falha de sensores já definidas |
| 01/11/2026 | 0.3.92 | Usuário confirmou reavaliação automática da regra do sensor após recuperação Modbus, com tempo de partida da saída |
| 01/11/2026 | 0.3.93 | Usuário confirmou aplicação das regras já definidas à saída em frequência associada a sinal Modbus |
| 01/11/2026 | 0.3.94 | Usuário confirmou continuar coletando e substituir os dados mais antigos com memória cheia |
| 01/11/2026 | 0.3.95 | Usuário definiu espaço separado e pequeno para alertas, apagados após sincronização |
| 01/11/2026 | 0.3.96 | Usuário confirmou substituir o alerta mais antigo quando o espaço reservado estiver cheio |
| 02/11/2026 | 0.3.141 | FW-046 concluído no STM32: FreeRTOS com alocação estática nos dois builds, tarefa de vida executando no L412 real com o tick de 1 ms conferido (9,9 ciclos/s para 10 esperados). Parte ESP32 adiada |
| 02/11/2026 | 0.3.140 | FW-003 concluído: pinagem das duas placas conferida contra as netlists e compilada com o HAL real; corrigido aviso do linker no y7080. Decisão: testar todo o STM32 antes do ESP32 |
| 02/11/2026 | 0.3.139 | OloggerMulti: BOOT0 do U1 sem ligação no esquema (HW-006), contornado por bits de opção sem intervenção física; imagem vazia executando no STM32L412 real (PC em `0x0800....`, SysTick contando). Criado FW-051 para o procedimento de gravação |
| 02/11/2026 | 0.3.138 | FW-004, FW-005 e FW-008 concluídos: Doxygen com dicionário gerado, modelo de arquivo C e verificador de padrão, com 20 testes das ferramentas. OloggerMulti gravado por SWD, mas o BOOT0 em nível alto impede a execução; correção de hardware adiada |
| 02/11/2026 | 0.3.137 | FW-006 concluído: imagens vazias dos dois builds STM32 compilam e linkam com `-Werror` (HAL L4 v1.18.2 para OloggerMulti e HAL L0 v1.12.4 para y7080) |
| 02/11/2026 | 0.3.136 | FW-006 parcial: imagem vazia do OloggerMulti (STM32L412C8) compila e linka com HAL L4 v1.18.2 e `-Werror`; y7080 aguarda o pacote STM32CubeL0 |
| 02/11/2026 | 0.3.135 | FW-002 concluído: CMake raiz com seleção de build por device, toolchain ARM e opções de CPU por MCU; configura os dois builds e recusa device inválido. Alvos STM32 pendentes de HAL (FW-006) |
| 02/11/2026 | 0.3.134 | FreeRTOS aprovado para todo o sistema, com alocação estática; mapeamento Modbus aprovado com escravo de status; escrita em equipamento externo fora do escopo; backlog FW-046 a FW-050 |
| 02/11/2026 | 0.3.133 | Decisões: MQTT do y7080 no modem; rádio externo do OloggerMulti na UART1 do STM32 com repasse transparente; dados Modbus só na RAM, 8 equipamentos e 64 variáveis; configuração pelo sistema e pelo gateway USB. Proposta em `documentacao/modbus_mapeamento_dados.md` |
| 02/11/2026 | 0.3.132 | Composição confirmada pelo responsável técnico: OloggerMulti com saída em frequência, ESP32-C6 como único rádio e UART de rádio externo transparente (`RADIO_UART_EXTERNO`); y7080 com entrada diferencial, celular e MQTT, sem entradas nem saídas digitais; ESP-IDF v5.4.4 de referência |
| 02/11/2026 | 0.3.131 | Esclarecido pelo responsável técnico: um build por tipo de device (`ologgermulti` e `y7080`), com todas as funcionalidades no mesmo projeto e o arquivo `builds/composicao.cmake` indicando o que entra em cada build; setup em runtime descartado; composição proposta validada no CMake |
| 02/11/2026 | 0.3.130 | Padrões C e ESP-IDF, composição dos builds, skills e agentes de desenvolvimento, changelog, README, INICIAR, índice para agentes e estado da implementação criados |
| 02/11/2026 | 0.3.129 | FW-001 concluído na SPRINT_00: pastas `src/`, `include/`, `lib/`, `devices/`, `tests/` e `tools/` com README; próximo item FW-002 |
| 01/11/2026 | 0.3.128 | Criado processo de desenvolvimento: Git local com remoto, índice de documentação, backlog FW-001 a FW-045, sprints de uma semana, padrão C (MISRA C:2012), dicionário de funções, contrato de integração, manual de operação, skills e agentes |
| 01/11/2026 | 0.3.127 | Criado plano de implantação do firmware em planejamento/PLANO_IMPLANTACAO_FIRMWARE.md, com 11 fases, aceite e testes |
| 01/11/2026 | 0.3.126 | Confirmado pelo usuário: B9 a B17 registrados como fora do device; próximo passo é o plano de implantação do firmware |
| 01/11/2026 | 0.3.125 | Definidos build do STM32 em CMake com toolchain ARM e HAL, e testes no PC, simulador Python e bancada |
| 01/11/2026 | 0.3.124 | Definidas regras do device: documentação em Doxygen em português, MQTT 5.0 e TLS 1.3, versionamento por data e build; processo: entrevista só sobre o device |
| 01/11/2026 | 0.3.123 | B8 fechado pelo usuário (9 de 18); regra do device: toda comunicação por MQTT ou serial |
| 01/11/2026 | 0.3.122 | B8 respondido: canais WhatsApp e Telegram; responder e executar pelo sistema via MQTT; confirmação só para atualização de firmware; identificação pelo número cadastrado; idioma português e inglês |
| 01/11/2026 | 0.3.121 | B7 fechado como fora do device; sem app de celular; gateway do técnico passa a ser aplicação Python no notebook com chaveiro serial; progresso 8/18 |
| 01/11/2026 | 0.3.120 | B6 fechado pelo usuário; progresso 7/18 e próximo passo B7 |
| 01/11/2026 | 0.3.119 | B6 tratado como requisito do sistema; device com interface pelo WhatsApp e serial sem conectividade, sem painéis próprios, comunicação por MQTT |
| 01/11/2026 | 0.3.118 | B5 fechado pelo usuário; progresso 6/18 e início do B6. Pendências de heartbeat, SETUP/INFO e grupos do JSON transferidas para a implantação |
| 01/11/2026 | 0.3.117 | Resolvidas as pendências do mapa Modbus: float ABCD, tempo de acionamento em segundos, endereço escravo de 1 a 247; trifásico mantido reservado |
| 01/11/2026 | 0.3.116 | B5 device: sessão de configuração iniciada pelo técnico (app local) ou pelo sistema (MQTT); comandos MQTT recusados durante a sessão; término após 5 minutos sem atividade; configuração válida pelo Modbus e pelo MQTT |
| 01/11/2026 | 0.3.115 | Criado mapa Modbus em documentacao/mapa_modbus.md, com 15 posições por tipo, float em 2 registradores, conectividade e configurações; manual Raptor usado como referência de conteúdo |
| 01/11/2026 | 0.3.114 | Endereços pela norma Modbus (3xxxx medições, 4xxxx configuração), valores numéricos no JSON e timestamp com hora do device; float de 32 bits em dois registradores a confirmar |
| 01/11/2026 | 0.3.113 | Definido formato JSON por grupo, sem id no corpo (id no tópico) e chaves pelo endereço Modbus; analisado manual do CLP Raptor 4G como referência |
| 01/11/2026 | 0.3.112 | Definidos tópicos padrão mínimos (SETUP, INFO, STATUS/heartbeat, COMANDO e resultado, telemetria e alertas), configuração mínima do device e INFO pedido pelo sistema |
| 01/11/2026 | 0.3.111 | Separados requisitos do device e do sistema no B5; técnico configura conectividade; caminho do tópico MQTT vem do sistema; tópicos padrão de configuração (SETUP, INFO) a definir |
| 01/11/2026 | 0.3.110 | B5: hierarquia país > estado > cidade > empresa > unidade; usuário vê várias unidades com restrição; acesso por horário e dia da semana por perfil |
| 01/11/2026 | 0.3.109 | B5 iniciado: perfis definidos (Cliente, Integrador, Técnico de campo, Suporte/administrador); sessão única por dispositivo, com bloqueio durante a configuração (P-008) |
| 01/11/2026 | 0.3.108 | Definidos timeout Modbus de 30 s, fixo no firmware, e espaço de alertas de 300 registros; início da leitura do B5 |
| 01/11/2026 | 0.3.107 | Finalizadas pendências do B4: janela de 5 s, padrão de 10% com inversor, média móvel de um envio, escala logarítmica não é requisito e protótipo com sensores reais e NB-IoT; timeout Modbus e quantidade de alertas ficam para a implementação |
| 01/11/2026 | 0.3.106 | Usuário confirmou saída em 0 Hz na falha por dente de serra, com retorno após um minuto normal |
| 01/11/2026 | 0.3.105 | Usuário definiu dente de serra como inversão de sentido acima do percentual: 2% padrão sem inversor e percentual configurado pelo sistema com inversor de frequência; variação linear ou logarítmica é normal; alerta e valor real sem suavização nem zeragem |
| 01/11/2026 | 0.3.104 | Usuário explicou 1% da escala como variação normal do sensor e propôs 2% para margem de segurança; falha por salto segue com limite configurável. Corrigido o rótulo "salto de 1%" dos registros 0.3.103 e P-027 |
| 01/11/2026 | 0.3.103 | Usuário separou a integração por segundo (média simples das amostras) da média móvel de envio; salto de 1% sobre o valor de 1 segundo; janela da média móvel pendente |
| 01/11/2026 | 0.3.102 | Usuário definiu amostragem de 10 a 1000 amostras por segundo por entrada, integração por segundo, primeira leitura após zero sem comparação (P-027) e validação NB-IoT por teste com módulo real e operadora (P-026) |
| 01/11/2026 | 0.3.101 | Usuário confirmou o fechamento do B4; progresso atualizado para 5/18 blocos e próximo passo para o B5 |
| 01/11/2026 | 0.3.100 | Usuário definiu timeout Modbus na implementação, resultado de atualização e comando com falha no espaço de alertas, e última leitura acima de 1% como referência (P-027); B4 aguarda confirmação final |
| 01/11/2026 | 0.3.99 | Usuário definiu quantidade de alertas na implementação, aviso de hora inválida no espaço de alertas e escopo inicial do protótipo de bancada |
| 01/11/2026 | 0.3.98 | Usuário definiu contagem da primeira consulta Modbus nas dez tentativas, timeout fixo no firmware e cinco leituras antes do minuto de retorno da saída em frequência |
| 01/11/2026 | 0.3.97 | Usuário definiu eventos do espaço de alertas (alarme, falha de sensor, falha Modbus), capacidade por número fixo de registros e validade do comando adiado até substituição |

