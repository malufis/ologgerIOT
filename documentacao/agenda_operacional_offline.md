# Agenda operacional offline — oLoggerIOT

> **Criado em:** 31/10/2026 · **Última modificação:** 01/11/2026 · **Versão:** 0.1.20

## Objetivo informado

Rodrigo Maluf solicitou uma tabela de operação para o módulo continuar operando quando estiver sem conexão com a nuvem. A plataforma analisa o histórico de funcionamento do CLP por dia e hora, constrói uma agenda semanal e a disponibiliza à placa para uso offline.

O histórico é construído durante a operação do sistema. A semana compreende segunda a domingo, em intervalos de 30 minutos. Como ilustração, um motor poderia ter registrado funcionamento na segunda das 08h às 08h30 e na terça das 09h às 10h; esses horários são exemplos, não uma agenda aprovada.

## Entendimento funcional

1. A plataforma recebe o histórico de operação e analisa padrões por dia da semana e horário.
2. A plataforma prepara uma agenda operacional correspondente à instalação, submetida à aprovação do CCO ou integrador, e a envia ao dispositivo enquanto houver comunicação.
3. O dispositivo guarda a agenda localmente para poder consultá-la quando o módulo estiver offline.
4. **Somente durante a indisponibilidade de comunicação**, o dispositivo usa a agenda guardada como referência para a operação por horário. Ao reconhecer a perda de conexão, aplica imediatamente o estado previsto para o **intervalo atual**, mesmo que ele já tenha começado, respeitando o modo manual, a prioridade da regra do sensor local e o atraso de partida configurado por saída.
5. Ao recuperar a conectividade, o dispositivo **avisa que a conexão voltou**, a agenda local deixa de atuar e a porta **mantém seu último estado, aguardando um novo comando da plataforma**. A reconexão, por si só, não gera uma nova ordem automática. O modo manual e as regras locais de sensor continuam sujeitos às prioridades já definidas; esta regra não suspende a atuação do sensor. Os eventos e dados pendentes seguem as regras de comunicação e fila já definidas. O tratamento de ordens remotas pendentes e o formato do aviso ainda serão detalhados.

O usuário definiu que o dispositivo deve se considerar **offline após esgotar as tentativas de reconexão com o servidor MQTT**: são **cinco tentativas em cada rádio disponível, com intervalo de um minuto entre tentativas**, seguindo a prioridade de comunicação configurada. Se nenhum rádio conseguir reconectar após suas tentativas, ativar a agenda offline. Se uma conexão for estabelecida, não entrar offline por essa falha. Esta contagem é de reconexão e é distinta das cinco tentativas de entrega de mensagens já registradas. Timeouts, marco de contagem do intervalo, definição de rádio disponível e relação com sleep programado serão detalhados.

Uma semana contém **336 intervalos de 30 minutos**: 48 por dia durante sete dias. O usuário confirmou que cada intervalo registra o **estado desejado**, ligado ou desligado. A agenda precisa indicar a qual saída/equipamento e período cada regra se refere. O formato exato, a resolução da análise do histórico, o algoritmo de inferência e o armazenamento são decisões futuras.

## Relação com as regras já definidas

- Os dados e eventos produzidos durante o período offline devem seguir a fila persistente, tentativas e confirmação do servidor já registradas em [telemetria e armazenamento](telemetria_e_armazenamento.md).
- As saídas já têm comandos de ligar/desligar, atraso de partida configurável por saída e cancelamento de partida pendente por desligamento. Falta definir como esses comportamentos se aplicam aos eventos gerados pela agenda.
- Após retorno de energia, o usuário confirmou que a **agenda poderá acionar ligar e desligar**. Considerar a ordem da agenda como origem possível de um novo acionamento, respeitando o atraso de partida por saída já definido. A regra anterior de não restaurar automaticamente o último estado continua registrada; o momento de aplicação da agenda após reinicialização ainda precisa ser definido.
- O usuário definiu controle de saídas por entrada analógica configurada localmente: **o sensor prevalece sobre a agenda** em conflito no modo automático. Comando manual prevalece sobre as regras automáticas e mantém **a saída destinatária** no modo manual até comando de modo automático para ela; o CCO é alertado. Outras saídas não mudam automaticamente de modo. Comandos de regras do sistema também são classificados como automáticos. Ver [configuração do dispositivo](configuracao_do_dispositivo.md).
- Ao retornar uma saída ao modo automático, avaliar e executar imediatamente a regra válida naquele momento, respeitando a prioridade do sensor sobre a agenda e as regras de partida definidas. A necessidade de nova leitura quando não houver dado válido permanece a especificar.
- Durante sleep, ao chegar o horário de uma mudança pela agenda offline, o STM32 deve despertar, executar a regra e voltar a dormir, conforme definição do usuário.
- O usuário informou que o dispositivo dispõe de **relógio interno com bateria** para manter data e hora sem internet. A hora deverá ser conferida com um **servidor NTP quatro vezes ao dia**, substituindo a frequência anterior de uma vez ao dia, e atualizada quando o sistema iniciar após boot total. A agenda utiliza o relógio mantido por bateria durante o período offline; política de ajuste, disponibilidade e recuperação caso a sincronização falhe ainda serão definidas. Não confundir conferência NTP indisponível com perda comprovada da hora correta.
- Se o relógio **perder a hora correta**, o usuário corrigiu a orientação anterior: **cada saída permanece no último estado até a hora ser corrigida**, inclusive em modo manual; novas ações por regra local de sensor não alteram esse estado durante esse período. Novos comandos manuais aguardam a correção da hora. A perda da hora por si só não determina o desligamento. Como a hora deixa de ser confiável, a agenda não deve decidir novos horários até haver hora válida. O tratamento de alertas nesse estado ainda não foi especificado.
- Após a correção da hora, executar automaticamente o último comando de cada saída que estava aguardando, incluindo comandos de desligar e respeitando as regras de desligamento já definidas. Em 01/11/2026, o usuário definiu que um comando adiado para uma saída permanece válido até ser substituído por novo comando para a mesma saída; o novo comando substitui o anterior. A interação com outras condições de operação ainda precisa ser detalhada.
- O usuário definiu hora correta como premissa inegociável para a telemetria: dados sem horário válido não têm validade. O equipamento deve informar o problema e solicitar manutenção quando a hora estiver inválida. Sem conexão, guardar o aviso para envio após a reconexão, identificando que o horário da ocorrência é desconhecido. O tratamento de outros eventos ainda precisa ser definido.

## Decisões em aberto

- Momento em que o estado do intervalo será aplicado depois de retorno de energia. A perda de conexão no meio de um intervalo já tem regra definida: aplicar imediatamente o estado vigente quando a perda for reconhecida.
- Como lidar com funcionamento que começa ou termina entre os limites de meia hora; a precisão da agenda é de 30 minutos conforme informado, sem presumir arredondamento.
- Fluxo específico de aprovação pelo CCO ou integrador e momento de envio/atualização da agenda.
- Timeouts das tentativas de reconexão MQTT, marco do intervalo de um minuto, definição de rádio disponível, relação com sleep, tratamento de ordens pendentes e formato do aviso. São cinco tentativas por rádio disponível com intervalo de um minuto, seguindo a prioridade configurada; ao esgotá-las, o dispositivo entra offline. Na reconexão, avisa, mantém o último estado e aguarda novo comando; as regras locais continuam aplicáveis.
- Regras de partida e desligamento, particularmente após perda de energia; tratamento de alarmes e condições que impeçam uma operação.
- Endereço/seleção do servidor NTP, horários das quatro conferências, política de ajuste, fuso da instalação, critérios de hora inválida, comportamento se a sincronização falhar, validade dos comandos aguardando correção, tratamento de alertas com hora inválida, ausência de agenda e falha de armazenamento. Conferência NTP quatro vezes ao dia, atualização no boot total e manutenção do relógio com bateria já informadas; saída mantém último estado até a hora ser corrigida, inclusive diante de novas regras de sensor.
- Período de histórico, variações entre semanas, exceções/feriados e formas de ajuste pelo usuário.

Os pontos listados nesta seção ainda não foram decididos ou implementados. As regras são requisitos a esclarecer no B4 e no bloco de automação; a organização do firmware continua para definição posterior pelo usuário.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 31/10/2026 | 0.1.0 | Registrada e analisada a proposta de agenda semanal offline derivada do histórico, em intervalos de 30 minutos, e sua relação com regras existentes |
| 31/10/2026 | 0.1.1 | Usuário confirmou estado por intervalo, aprovação pelo CCO/integrador e possibilidade de a agenda acionar saídas após retorno de energia; prioridades ainda pendentes |
| 31/10/2026 | 0.1.2 | Usuário definiu prioridade de comando manual e sensor sobre agenda, classificação manual/automático e aviso ao CCO |
| 31/10/2026 | 0.1.3 | Usuário esclareceu que modo manual/automático vale para a porta destinatária do comando |
| 31/10/2026 | 0.1.4 | Usuário definiu execução imediata da regra válida ao retornar ao modo automático |
| 31/10/2026 | 0.1.5 | Usuário definiu prioridade do sensor local sobre regra automática da plataforma; relação entre agenda e regra da plataforma permanece a definir |
| 31/10/2026 | 0.1.6 | Usuário definiu agenda local somente offline e regra automática da plataforma com conectividade, preservada a prioridade do sensor local |
| 31/10/2026 | 0.1.7 | Usuário definiu aplicação imediata do estado previsto no intervalo atual ao perder conectividade |
| 31/10/2026 | 0.1.8 | Usuário definiu aviso de reconexão, manutenção do último estado da porta e espera por novo comando da plataforma |
| 31/10/2026 | 0.1.9 | Usuário definiu entrada no estado offline após esgotar tentativas de reconexão ao servidor MQTT; número e intervalos a detalhar |
| 31/10/2026 | 0.1.10 | Usuário definiu cinco tentativas de reconexão MQTT em cada rádio disponível antes de entrar offline |
| 31/10/2026 | 0.1.11 | Usuário definiu intervalo de um minuto entre tentativas de reconexão MQTT |
| 31/10/2026 | 0.1.12 | Usuário definiu despertar para agenda e retorno ao sleep, relógio interno com bateria, atualização diária/no boot total e desligamento se perder a hora correta |
| 31/10/2026 | 0.1.13 | Usuário corrigiu comportamento sem hora correta: manter cada saída no último estado conforme último comando, inclusive manual |
| 31/10/2026 | 0.1.14 | Usuário esclareceu que as saídas mantêm estado até correção da hora, mesmo diante de regra do sensor; agenda depende de relógio sincronizado e mantido offline |
| 31/10/2026 | 0.1.15 | Usuário definiu conferência do relógio interno com servidor NTP quatro vezes ao dia, substituindo a frequência anterior diária; mantida atualização no boot total |
| 31/10/2026 | 0.1.16 | Usuário definiu hora correta como premissa inegociável e espera de novos comandos manuais até a correção |
| 01/11/2026 | 0.1.20 | Usuário definiu que comando adiado para uma saída vale até ser substituído por novo comando da mesma saída |
| 31/10/2026 | 0.1.17 | Usuário definiu dados sem horário válido como sem validade e solicitou informar problema e necessidade de manutenção |
| 31/10/2026 | 0.1.18 | Usuário confirmou guardar aviso de falha do relógio sem conexão e enviá-lo após reconexão com horário da ocorrência desconhecido |
| 31/10/2026 | 0.1.19 | Usuário definiu execução automática do último comando após correção da hora, inclusive regras de desligamento já definidas |
