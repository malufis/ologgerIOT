# Agenda operacional offline — oLoggerIOT

> **Criado em:** 31/10/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.27

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
- ~~Durante sleep, ao chegar o horário de uma mudança pela agenda offline, o STM32 deve despertar, executar a regra e voltar a dormir.~~ **Substituído em 04/11/2026:** a agenda **só funciona com o modo sleep desabilitado**; com o sleep habilitado ela não é aplicada e não há despertar para ela.
- O usuário informou que o dispositivo dispõe de **relógio interno com bateria** para manter data e hora sem internet. **Atualização de 04/11/2026:** a bateria de backup é **opcional**, conforme a implantação; sem ela, a hora se perde na falta de energia e só volta pelo NTP, e a agenda não é aplicada enquanto a hora for inválida. O OloggerMulti tem o cristal de 32,768 kHz para o RTC; o y7080 não tem cristal. A hora deverá ser conferida com um **servidor NTP quatro vezes ao dia**, substituindo a frequência anterior de uma vez ao dia, e atualizada quando o sistema iniciar após boot total. A agenda utiliza o relógio mantido por bateria durante o período offline; política de ajuste, disponibilidade e recuperação caso a sincronização falhe ainda serão definidas. Não confundir conferência NTP indisponível com perda comprovada da hora correta.
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

## Classificação da agenda (04/11/2026)

- A agenda é um **comando automático local**: funciona sem conectividade, e por isso vale só com o device offline.
- Com conectividade, em modo automático, o comando que vale é o do **CCO**. A agenda não é aplicada enquanto o device está online.
- Ao ficar offline, a saída aplica na hora o estado do intervalo atual da agenda. Ao reconectar, mantém o último estado e espera um novo comando do CCO.
- Uma saída com **regra do sensor** cadastrada (entrada analógica para saída digital) ignora a agenda.
- O cadastro da agenda é feito por MQTT, pelo CCO (seção seguinte). O comportamento das saídas está em `documentacao/configuracao_do_dispositivo.md`.

## Cadastro da agenda por MQTT (P-042, 04/11/2026)

A agenda chega ao device **por MQTT, vinda do CCO, como configuração**. O formato abaixo é uma **proposta a confirmar**; o módulo `lib/agenda` já o implementa e testa.

- **Canal:** mensagem de configuração no tópico `COMANDO`. O device responde em `RESULTADO` com "recebido" e depois "executado" ou "falhou com motivo". A agenda **não tem endereço Modbus**: são 336 bits por saída, até 15 saídas, e a atualização precisa ser atômica. Só se recebe por MQTT.
- **Conteúdo:** uma cadeia de **84 caracteres hexadecimais por saída** (42 bytes, um bit por intervalo de 30 minutos; 1 = ligada, 0 = desligada). O intervalo k é `dia * 48 + hora * 2 + minuto / 30`, com o dia de 0 (segunda) a 6 (domingo). O bit k fica no byte `k / 8`, na posição `k % 8`, do menos significativo para o mais significativo. O byte 0 vem primeiro, em maiúsculas ou minúsculas.
- **Exemplo** (a saída 1 liga na segunda das 08h às 08h30, que é o intervalo 16, byte 2, bit 0):

```json
{
  "id": "12345",
  "agenda": {
    "versao": 7,
    "saidas": {
      "1": "000001000000000000000000000000000000000000000000000000000000000000000000000000000000",
      "2": ""
    }
  }
}
```

- **Saída que não vem na mensagem:** mantém a agenda que já tinha. **Cadeia vazia (`""`):** apaga a agenda da saída, que passa a "sem agenda".
- **Validação por inteiro:** se qualquer saída tiver número inexistente neste build, cadeia que não tenha exatamente 84 caracteres ou caractere fora de 0 a 9 e A a F, o device **recusa a mensagem inteira**, responde "falhou com motivo" e **mantém a agenda anterior**. Só uma mensagem válida troca as agendas, de uma vez.
- **Sessão de configuração:** vale a regra da sessão (`documentacao/integracao_sistema.md`): durante uma sessão ativa o device recusa o comando MQTT com aviso.
- **Versão e responsabilidade (decidido em 04/11/2026):** o **CCO é responsável pela agenda**. O device **guarda só a última agenda que o CCO enviar**: não guarda versões antigas, não compara versões e não recusa uma agenda por ser mais antiga. O campo `versao` é opcional, é guardado junto com a agenda e informado ao sistema, mas nunca é comparado.
- **Aprovação:** a aprovação pelo CCO ou integrador acontece no sistema, antes do envio. O device não a verifica.
- **Armazenamento:** a agenda precisa sobreviver a reinício. Ficará na memória não volátil, com CRC e gravação atômica, no mesmo estilo da configuração do mapeamento Modbus (FW-048). São 42 bytes por saída, 168 bytes no OloggerMulti, mais a versão.
- **Quando atua:** só com o device **offline**, em modo automático, **sem regra do sensor** na saída e com a **hora válida**. A agenda é aplicada **pelo horário atual**: a cada limite de 30 minutos o device pede à saída o estado do intervalo, e ao ficar offline pede o do intervalo atual. **Depois de um reinício, a agenda assume na hora o estado do intervalo do horário atual** (se reiniciou às 9h30, vale o que está agendado para as 9h30), sem esperar as tentativas de reconexão; se depois o device conecta, a saída mantém o estado e espera um novo comando do CCO. A agenda só funciona com o modo sleep desabilitado. **Implementado** (lógica) em `agenda_aplicador`: pede o estado na primeira avaliação, a cada virada de intervalo e depois de carregar ou apagar a agenda; sem hora válida não pede nada e as saídas mantêm o estado. A tarefa `agenda_tarefa` lê o RTC, aplica o fuso enviado pelo CCO e entrega o pedido às saídas; o fuso começa em **Brasília (UTC−3)** e vale até o CCO enviar outro (decidido em 04/11/2026). **A agenda e o fuso gravados persistem ao reinício**: ficam nas duas últimas páginas da flash interna e são carregados na partida.
- **Hora de referência (decidido em 04/11/2026):** o horário da instalação, o mesmo do `timestamp` do JSON. O **fuso horário é um parâmetro de configuração, enviado pelo CCO**, em minutos em relação ao UTC (endereço Modbus 40403 proposto, `documentacao/mapa_modbus.md`). O device não muda o fuso sozinho: se houver horário de verão, o CCO envia o novo valor.
- **No código:** `agenda_carrega_hex` valida e carrega; `agenda_estado(dia, hora, minuto)` devolve ligada, desligada ou sem agenda; a tarefa que lê o relógio e entrega o pedido a `controle_saida_pedido` com a fonte `CONTROLE_FONTE_AGENDA` é parte do FW-032.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.27 | Agenda só com o sleep desabilitado; fuso padrão de Brasília; persistência da agenda e do fuso |
| 04/11/2026 | 0.1.26 | Tarefa da agenda pelo RTC; agenda só com o fuso configurado (proposta) |
| 04/11/2026 | 0.1.25 | Bateria de backup opcional; cristais do OloggerMulti |
| 04/11/2026 | 0.1.24 | Aplicador da agenda pelo horário atual implementado (lógica) |
| 04/11/2026 | 0.1.23 | P-043: fuso horário configurado pelo CCO; sem ordem de versão, o device guarda só a última agenda; depois de reinício a agenda assume o intervalo do horário atual |
| 04/11/2026 | 0.1.22 | P-042: a agenda chega por MQTT do CCO, em 84 caracteres hexadecimais por saída, com validação por inteiro; formato proposto e implementado em `lib/agenda` |
| 04/11/2026 | 0.1.21 | A agenda é comando automático local e só vale offline; online vale o CCO; cadastro da agenda pendente (P-042) |
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
