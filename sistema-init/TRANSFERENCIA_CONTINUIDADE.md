# Transferência de contexto — oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 01/11/2026 · **Versão:** 1.0.20

## Instruções para o próximo agente

Continue a entrevista em português do Brasil com Rodrigo Maluf, responsável técnico e decisor da Optmile. Leia `sistema-init/PROMPT_MESTRE.md` e `sistema-init/STATUS_PROJETO.md` antes de prosseguir. Os caminhos efetivamente usados são `sistema-init/` e `documentacao/`, mesmo onde o prompt mestre apresenta caminhos genéricos diferentes. Siga as instruções atuais do usuário quando divergirem do fluxo padrão.

O usuário quer **entender e levantar as funcionalidades antes de escolher como separar o projeto ou iniciar implementação**. Não apresente suposições como decisões confirmadas; faça perguntas pontuais e registre as respostas nos documentos correspondentes, incrementando versão e histórico. B0 a B8 estão confirmados no device. B9 a B17 são do sistema e foram confirmados como fora do device, sem perguntas de firmware. Regra do device: toda comunicação é por MQTT ou serial. Plano de implantação criado em `planejamento/PLANO_IMPLANTACAO_FIRMWARE.md` (fases 0 a 10). Próximo passo: Fase 0, base do projeto. Não reinicie a entrevista.

## Onde continuar

B4 foi fechado pelo usuário em 01/11/2026. As pendências de amostragem, falha e protótipo foram finalizadas pelo usuário na mesma data, e estão registradas em `documentacao/configuracao_do_dispositivo.md`:
- **Amostragem:** 10 a 1000 amostras por segundo por entrada; média simples por segundo; envio por média móvel com janela de um envio.
- **Falha por dente de serra:** inversão de sentido acima do percentual dentro de 5 segundos. Padrão de 2% sem inversor e 10% com inversor de frequência. Falha vai para 0 Hz, alerta e valor real sem suavização.
- **Escala logarítmica:** não é requisito.
- **Protótipo de bancada:** sensores 4–20 mA reais com a placa de destino, incluindo NB-IoT com módulo e operadora.

Valores definidos depois, em 01/11/2026: timeout Modbus de 30 segundos (fixo no firmware) e espaço de alertas de 300 registros.

B5 em andamento (01/11/2026): perfis Cliente, Integrador, Técnico de campo e Suporte/administrador; sessão única por dispositivo, sem mais de uma sessão no módulo, com bloqueio durante a configuração (P-008 concluído); hierarquia país > estado > cidade > empresa > unidade; várias unidades com restrição; acesso por horário e dia da semana por perfil. Pendências: restrição de unidades, janelas por perfil e ações exclusivas de cada perfil. Não rediscutir decisões já tomadas.

## Visão do produto

- Família de equipamentos para saneamento; dois projetos de placa, **OloggerMulti** e **y7080**, analisados nos arquivos de hardware. O usuário informou que a parte elétrica está resolvida. As capacidades e quantidades de sensores, portas e rádios variam conforme o modelo.
- STM32 em C é o coletor e orquestrador; ESP32 com ESP-IDF atua como módulo de comunicação subordinado e pode armazenar telemetria. O usuário imagina uma compilação e um setup que descreve os recursos do equipamento; a separação final será definida depois. Os esquemáticos indicam flash SPI externa de 16 MiB; distribuição de espaço ainda não definida.
- Protótipo para bancada foi solicitado em um mês, com referência registrada em 29/10/2026 e data derivada 29/11/2026. Isso não define sozinho o escopo do protótipo. Firmware não foi identificado nesta pasta: em 01/11/2026 a busca por `.c`, `.h`, `.ioc` e `CMakeLists.txt` não retornou arquivos. Não há implementação comprovada. A pasta local não foi reconhecida como repositório Git nessa verificação; não presumir branch, commit ou publicação.

## Regras fundamentais já levantadas

- **Modbus:** interoperabilidade industrial RTU em RS-485, com papéis mestre/escravo, e quadros RTU transportados por MQTT e demais canais definidos; comandos AT específicos do fabricante para rádios. Sensor analógico em input register; saídas discretas em coil. Encapsulamento, mapa, parâmetros e contratos ainda pendentes.
- **Comunicação:** rede e módulos externos ficam entre STM32 e MQTT; AWS IoT, EMQX ou RabbitMQ são possibilidades. Credencial individual por equipamento, conexão criptografada e rotação remota pretendida. Tópicos por dispositivo; sem comandos em grupo. Confirmações de recebimento e resultado de comando; dados só apagados após confirmação do servidor, exceto substituição por memória cheia.
- **Coleta:** intervalos configuráveis, modos contínuo ou sleep, envio em lote ou sob comando; falhas de envio são mantidas para envio posterior. Os despertares para coleta e transmissão são independentes; alarmes podem despertar o dispositivo. Até cinco tentativas de envio antes de guardar para depois. Reconexão MQTT: cinco tentativas por rádio, a cada minuto, antes de entrar offline.
- **Alarmes:** até quatro condições e três operadores `&&`/`||`, avaliação da esquerda para a direita; comparações `<`, `>`, `<=`, `>=`, `==` e estados digitais. Alarme ao atingir a condição, uma vez; retorno ao normal após tempo configurável por alarme de pelo menos um minuto. Sensor com falha identifica sensor, porta e tipo de falha.
- **Controle:** modo manual/automático por saída; manual prevalece até comando de automático e comunica o CCO. Sensor local prevalece sobre agenda e regra automática da plataforma; falha do sensor desliga a saída discreta em automático, mas mantém comando em manual. Saída em frequência pode representar entrada 4–20 mA; em falha vai a 0 Hz e retorna após um minuto normal. Ligar pode ter atraso configurável por saída; comando de desligar durante a espera cancela a partida.
- **Agenda e relógio:** agenda semanal offline em intervalos de 30 minutos, aprovada pelo CCO ou integrador, baseada em histórico da plataforma. Ao recuperar conexão avisa e mantém último estado aguardando comando da plataforma, com regras locais e manual ainda aplicáveis. RTC com bateria, conferência NTP quatro vezes ao dia e atualização em boot total. Hora correta é obrigatória: sem hora válida, medições não têm validade, saídas mantêm estado e comandos manuais esperam; depois da correção executar automaticamente o último comando de cada saída que aguardava, respeitando regras definidas. Informar falha e solicitar manutenção; offline guardar aviso e enviar após reconexão indicando ocorrência com horário desconhecido. Falha de uma consulta NTP, por si só, não comprova hora inválida.
- **Modbus sem resposta:** dez tentativas a cada 30 segundos, alerta de comunicação, continuar tentando; após uma hora contada a partir do fim das dez tentativas, informar defeito se persistir. Recuperação exige cinco leituras bem-sucedidas consecutivas e aviso. Após a recuperação, reavaliar regras das saídas automáticas e respeitar o atraso de partida. As regras de sensor também se aplicam ao Modbus. A relação entre cinco leituras e o minuto normal para saída em frequência está aberta.
- **Atualização:** CCO, integrador e técnico de campo podem iniciar com identificação de quem enviou; verificar validade e compatibilidade e recuperar versão anterior se falhar ou faltar energia. Informar início e resultado ao sistema; resultado offline fica guardado até confirmação. Saídas digitais desligadas durante atualização/reinício, depois retomar comandos e regras anteriores; saída antes ligada aguarda tempo de partida configurado. Mecanismos técnicos, autenticação, armazenamento e testes ainda não estão definidos.
- **Configuração:** WhatsApp em linguagem natural via sistema e MQTT; aplicação Python no notebook do técnico, com chaveiro serial, é o gateway USB/serial quando o dispositivo ainda não tem conexão; não há app de celular. Operação local limitada para conectividade, com o mínimo de trabalho para técnico; configuração complexa por CCO/integrador. Permissões exatas precisam de detalhamento.

## Documentos de referência

1. `sistema-init/STATUS_PROJETO.md`: estado, decisões, pendências e histórico; referência principal para continuidade.
2. `sistema-init/ENTREVISTA.md` e `sistema-init/PROMPT_MESTRE.md`: roteiro e método.
3. `documentacao/analise_hardware_ologgeriot.md` e `documentacao/modelos_e_versoes_firmware.md`: placas e modelos.
4. `documentacao/comunicacao_iot.md`: Modbus, MQTT, credenciais e falhas.
5. `documentacao/telemetria_e_armazenamento.md`: coleta, filas, sleep e alertas.
6. `documentacao/configuracao_do_dispositivo.md`: calibração, modos, saídas e alarmes.
7. `documentacao/agenda_operacional_offline.md`: agenda, relógio e operação offline.
8. `documentacao/atualizacao_firmware.md`: atualização, recuperação e saídas.

Estes documentos são requisitos e análises, não prova de implementação ou testes. Leia as seções pertinentes antes de tomar decisões; em caso de divergência, a orientação mais recente explicitamente confirmada pelo usuário prevalece e o conflito deve ser registrado.

## Mensagem inicial sugerida para outro agente

> Leia `sistema-init/TRANSFERENCIA_CONTINUIDADE.md`, `sistema-init/PROMPT_MESTRE.md` e `sistema-init/STATUS_PROJETO.md` e continue a entrevista do oLoggerIOT a partir do B4. Não reinicie as perguntas já respondidas. Preserve as decisões do Rodrigo e atualize a documentação conforme as novas respostas.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 01/11/2026 | 1.0.20 | Processo de desenvolvimento criado: Git local com remoto (sem commit ou push), índice `documentacao/README.md`, backlog e sprints, padrão C, integração, manual, skills e agentes. Próximo passo: SPRINT_00 |
| 01/11/2026 | 1.0.19 | Criado plano de implantação do firmware (planejamento/PLANO_IMPLANTACAO_FIRMWARE.md), com fases 0 a 10, aceite e testes |
| 01/11/2026 | 1.0.18 | Definições do device registradas (Doxygen em português, MQTT 5.0 e TLS 1.3, versionamento por data e build, build CMake com toolchain ARM, testes em PC, simulador Python e bancada); B9 a B17 confirmados como fora do device |
| 01/11/2026 | 1.0.17 | B8 fechado (9 de 18); toda comunicação do device é por MQTT ou serial |
| 01/11/2026 | 1.0.16 | B7 fechado como fora do device (8 de 18); gateway do técnico é aplicação Python no notebook com chaveiro serial; sem app de celular |
| 01/11/2026 | 1.0.15 | B6 fechado pelo usuário (7 de 18); device com interface por WhatsApp e serial, sem painéis próprios; próximo B7 |
| 01/11/2026 | 1.0.14 | B5 fechado pelo usuário (6 de 18); próximo B6; pendências de implantação: heartbeat, SETUP/INFO e grupos do JSON |
| 01/11/2026 | 1.0.13 | Criado mapa Modbus (`documentacao/mapa_modbus.md`) com a norma, 15 posições por tipo e float em 2 registradores; pendências listadas no mapa |
| 01/11/2026 | 1.0.12 | Definidos tópicos padrão mínimos do device (SETUP, INFO, STATUS/heartbeat, COMANDO e resultado, telemetria e alertas) e configuração mínima; perfis, hierarquia e horários movidos para requisito do sistema |
| 01/11/2026 | 1.0.11 | B5 em andamento: perfis, sessão única por dispositivo (P-008), hierarquia, várias unidades com restrição e acesso por horário por perfil |
| 01/11/2026 | 1.0.10 | Definidos timeout Modbus de 30 s e espaço de alertas de 300 registros; início do B5 |
| 01/11/2026 | 1.0.9 | Finalizadas as pendências de amostragem, média móvel, dente de serra, escala logarítmica e protótipo; valores de timeout Modbus e quantidade de alertas ficam para a implementação |
| 01/11/2026 | 1.0.8 | Critério de falha definido como dente de serra (inversão de sentido acima do percentual); 2% padrão sem inversor e percentual configurado pelo sistema com inversor; variação normal não é falha |
| 01/11/2026 | 1.0.7 | Percentual de 1% da escala reinterpretado como variação normal do sensor, com proposta de 2%; aplicação a confirmar; rótulo "salto de 1%" corrigido |
| 01/11/2026 | 1.0.6 | Separadas integração por segundo (média simples) e média móvel de envio; salto de 1% sobre o valor de 1 segundo; janela da média móvel pendente |
| 01/11/2026 | 1.0.5 | Registradas amostragem de 10 a 1000 amostras/s e primeira leitura após zero sem comparação (P-027); validação NB-IoT por teste com módulo e operadora (P-026) |
| 01/11/2026 | 1.0.4 | B4 confirmado pelo usuário; progresso 5/18 e próximo passo B5 |
| 01/11/2026 | 1.0.3 | Decididas pendências do B4 (timeout na implementação, eventos de atualização e comando no espaço de alertas, referência do P-027); B4 aguarda confirmação final do resumo |
| 01/11/2026 | 1.0.2 | Registrados aviso de hora inválida no espaço de alertas, quantidade de alertas na implementação e escopo inicial do protótipo de bancada |
| 01/11/2026 | 1.0.1 | Registradas respostas de alertas (eventos, capacidade, validade de comando adiado), contagem Modbus, timeout fixo e retorno da saída em frequência; atualizada a lista de pendências |
| 01/11/2026 | 1.0.0 | Criado documento de transferência para continuidade por outro agente |
