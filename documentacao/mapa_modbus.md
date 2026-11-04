# Mapa Modbus do device oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.14

## Origem e estado

Mapa definido pelo usuário em 01/11/2026: seguir a norma Modbus, com float ocupando 2 registradores, e 15 posições reservadas para cada tipo de entrada e saída, mesmo que o device não tenha todas. O manual do CLP Raptor 4G (`sistema-init/modbus rtu CLP Raptor 4G.docx`) é referência do que o device deve expor (conectividade, sistema trifásico, configuração de sensores), mas os endereços são os deste mapa. Este documento define endereços e tipos. Ainda não há implementação.

## Regras gerais

- **Tipos de dado:**
  - Coils (0xxxx): saídas digitais, leitura e escrita.
  - Discrete inputs (1xxxx): entradas digitais e estados, somente leitura.
  - Input registers (3xxxx): medições e status, somente leitura.
  - Holding registers (4xxxx): configurações, leitura e escrita.
- **Float:** IEEE 754 de 32 bits, em 2 registradores consecutivos. O endereço do dado é o primeiro registrador. Ordem: big-endian (ABCD), palavra alta primeiro, confirmada em 01/11/2026.
- **Strings:** ASCII, 2 caracteres por registrador, preenchimento com zero. Tamanhos indicados em cada bloco.
- **Posições:** cada tipo de entrada ou saída reserva 15 posições. Posições não instaladas no modelo devem responder zero.
- **Endereçamento:** a tabela usa a convenção de 5 dígitos. No protocolo, o endereço enviado é o número menos 1.
- **Status fora da faixa 41xxx:** os status do manual Raptor (faixa 41xxx) foram colocados na faixa 3xxxx, por serem somente leitura.

## Entradas analógicas (15 posições)

| Endereço | Tipo | Descrição |
|---|---|---|
| 30001–30030 | input float | Valor de engenharia da entrada n. Entrada n = 30001 + 2 × (n − 1) |
| 30031–30060 | input float | Corrente medida em mA da entrada n. Mesma regra de posição |
| 40001–40030 | holding float | Span da entrada n (escala do sensor) |
| 40031–40060 | holding float | Zero da entrada n (escala inicial do sensor) |
| 40061–40090 | holding float | Offset da entrada n, de −100,00 a 100,00 |
| 10016–10030 | discrete | Falha do sensor analógico n (1 = com falha) |

## Saídas analógicas (reservado, 15 posições)

| Endereço | Tipo | Descrição |
|---|---|---|
| 30061–30090 | input float | Valor da saída analógica n |
| 40091–40120 | holding float | Span da saída analógica n |
| 40121–40150 | holding float | Zero da saída analógica n |

## Entradas digitais (15 posições)

| Endereço | Tipo | Descrição |
|---|---|---|
| 10001–10015 | discrete | Estado da entrada digital n (1 = ligada, 0 = desligada) |
| 40151–40165 | holding uint16 | Modo da entrada n: 0 = discreto, 1 = pulsado |

## Saídas digitais (15 posições)

| Endereço | Tipo | Descrição |
|---|---|---|
| 00001–00015 | coil | Comando e estado da saída n (1 = ligada, 0 = desligada) |
| 30091–30120 | input float | Frequência atual da saída n em Hz (0 quando não está em modo frequência) |
| 40166–40180 | holding uint16 | Tipo da saída n: 0 = discreto, 1 = frequência (a saída "pulsada"), 2 = reservado (ajustado em 03/11/2026). Só as saídas 2 a 4 aceitam 1: a saída 1 não tem canal de timer (04/11/2026). Gravar 1 com o sleep ligado é recusado |
| 40181–40210 | holding float | Frequência em Hz correspondente a 4 mA da saída n |
| 40211–40240 | holding float | Frequência em Hz correspondente a 20 mA da saída n |
| 40241–40255 | holding uint16 | Entrada analógica associada à saída n (0 = nenhuma) |
| 40256–40270 | holding uint16 | Retardo de acionamento da saída n (relé de tempo), em segundos; 0 = liga na hora. É também o "atraso de partida" e o "tempo de acionamento" |
| 40271–40285 | holding uint16 | Reservado: o tempo de acionamento é o mesmo retardo de acionamento (40256–40270), decisão de 03/11/2026 |
| 40286–40300 | holding uint16 | Modo manual/automático da saída n: 0 = automático, 1 = manual |
| 40307–40336 | holding float | Limiar máximo da regra do sensor da saída n, na unidade de engenharia da entrada associada (**proposta de 04/11/2026**) |
| 40337–40366 | holding float | Limiar mínimo da regra do sensor da saída n, na unidade de engenharia da entrada associada (**proposta de 04/11/2026**) |
| 40367–40381 | holding uint16 | Sentido da regra do sensor da saída n: 0 = liga no mínimo e desliga no máximo, 1 = liga no máximo e desliga no mínimo (**confirmado em 04/11/2026**; endereços ainda a confirmar) |
| 40382–40396 | holding uint16 | Tempo de estabilidade do sensor da saída n depois de uma falha, em segundos; padrão 300 (5 minutos) (**confirmado em 04/11/2026**) |

## Sistema trifásico (reservado, sem hardware identificado)

O hardware analisado (OloggerMulti e y7080) não tem medição trifásica. O bloco é reservado para manter o mapa do manual Raptor.

| Endereço | Tipo | Descrição |
|---|---|---|
| 30131–30136 | input float × 2 | Tensões RMS das fases R, S e T (V) |
| 30137–30142 | input float × 2 | Correntes RMS das fases R, S e T (A) |
| 30143–30148 | input float × 2 | Fator de potência das fases R, S e T |
| 30149–30154 | input float × 2 | Potência ativa das fases R, S e T (W) |
| 30155–30160 | input float × 2 | Potência reativa das fases R, S e T (VAr) |
| 30161–30166 | input float × 2 | Potência aparente das fases R, S e T (VA) |
| 30167–30168 | input float | Fator de potência total |
| 30169–30170 | input float | Potência ativa total (W) |
| 30171–30172 | input float | Potência reativa total (VAr) |
| 30173–30174 | input float | Consumo total (kWh) |
| 30175–30176 | input float | Potência fundamental harmônica (W) |
| 30177–30182 | input float × 2 | Ângulo das fases R, S e T (graus) |
| 30183–30184 | input float | Temperatura (°C) |
| 40301–40303 | holding uint16 | Ganho de tensão das fases R, S e T (0 a 30000) |
| 40304–40306 | holding uint16 | Ganho de corrente das fases R, S e T: 0 = sem ganho, 1 = 2×, 2 = 4× |

## Estado do sistema e relógio

| Endereço | Tipo | Descrição |
|---|---|---|
| 10031 | discrete | Relógio com hora válida (1 = válida) |
| 10032 | discrete | MQTT conectado |
| 10033 | discrete | Celular conectado |
| 10034 | discrete | Wi-Fi conectado |
| 10035 | discrete | Rádio externo conectado |
| 10036 | discrete | Sessão de configuração ativa (comandos bloqueados) |
| 10037 | discrete | Atualização de firmware em andamento |
| 30121–30126 | input uint16 × 6 | Data e hora do device: ano, mês, dia, hora, minuto, segundo |
| 30127–30128 | input float | Tensão de alimentação DC (V) |
| 30129–30130 | input float | Temperatura interna (°C) |

## Conectividade

### Celular (30200–30283)

| Endereço | Tamanho | Descrição |
|---|---|---|
| 30200–30215 | 16 registradores | IMEI do módulo |
| 30216–30223 | 8 registradores | Fabricante do módulo |
| 30224–30231 | 8 registradores | Modelo do módulo |
| 30232–30239 | 8 registradores | Operadora do chip 1 |
| 30240–30247 | 8 registradores | Operadora do chip 2 |
| 30248–30263 | 16 registradores | APN do chip 1 |
| 30264–30279 | 16 registradores | APN do chip 2 |
| 30280 | uint16 | Chip ativo (1 ou 2) |
| 30281 | uint16 | Nível de sinal |
| 30282 | int16 | RSSI (dBm) |
| 30283 | uint16 | Status da conexão |

### Wi-Fi (30300–30343)

| Endereço | Tamanho | Descrição |
|---|---|---|
| 30300–30308 | 9 registradores | MAC do módulo |
| 30309–30316 | 8 registradores | Fabricante do módulo |
| 30317–30324 | 8 registradores | Modelo do módulo |
| 30325–30340 | 16 registradores | SSID conectado |
| 30341 | uint16 | Nível de sinal |
| 30342 | int16 | RSSI (dBm) |
| 30343 | uint16 | Status da conexão |

### Rádio externo (30400–30443)

Mesmo layout do Wi-Fi, a partir de 30400: MAC 30400–30408, fabricante 30409–30416, modelo 30417–30424, SSID 30425–30440, sinal 30441, RSSI 30442, status 30443.

### Modbus, envio e identificação

| Endereço | Tipo | Descrição |
|---|---|---|
| 30500 | input uint16 | Modo Modbus: 0 = mestre, 1 = escravo |
| 30501 | input uint16 | Velocidade da porta, em código (ver 40400) |
| 30520–30535 | input string, 16 registradores | Endereço do servidor de envio |
| 30536 | input uint16 | Intervalo de envio em segundos |
| 30600–30615 | input string, 16 registradores | Número de série |
| 30616–30623 | input string, 8 registradores | Versão do firmware |
| 30624–30631 | input string, 8 registradores | Modelo do equipamento (OloggerMulti ou y7080) |

## Configurações gerais (holding)

| Endereço | Tipo | Descrição |
|---|---|---|
| 40400 | uint16 | Velocidade da porta RS-485: 0 = 2400 bps, 1 = 9600 bps, 2 = 57600 bps, 3 = 115200 bps |
| 40401 | uint16 | Modo RS-485: 0 = mestre, 1 = escravo |
| 40402 | uint16 | Endereço do escravo, pela norma de 1 a 247 (confirmado em 01/11/2026; o manual Raptor usa 0 a 99) |
| 40403 | int16 | Fuso horário da instalação, em minutos em relação ao UTC, em complemento de dois (por exemplo, −180 para o horário de Brasília), configurado pelo CCO (**proposta de 04/11/2026**, endereço a confirmar) |
| 40410 | uint16 | Intervalo de coleta em segundos |
| 40411 | uint16 | Intervalo de envio em segundos |
| 40412 | uint16 | Modo de operação: 0 = contínuo, 1 = sleep |
| 40413 | uint16 | Tempo sem pulso para o STM32 dormir, em segundos, de 1 a 65535; padrão 60 (**confirmado em 04/11/2026**) |

## Persistência dos registradores de configuração (decidido em 04/11/2026)

**Todos os registradores de retenção (holding, 4xxxx) deste mapa são configuração e setup, e ficam na memória flash do device:** o que o CCO ou o Modbus gravam sobrevive ao reinício e à falta de energia, sem depender de bateria. A gravação ocorre 5 segundos depois da última escrita. Valem os padrões de fábrica quando não há registro válido, e um valor gravado inválido volta ao padrão.

- **Escrita atômica:** o pedido só vale se todos os valores estiverem nos limites; um float escrito pela metade, um endereço fora da configuração ou uma combinação proibida (frequência na saída 1, ou frequência com o sleep ligado) recusa o pedido inteiro.
- **Não persistem** (são estado, e não configuração): o comando e o estado das saídas (coils), o modo manual ou automático (40286–40300, volta a automático e desligada depois de um reinício), o totalizador de pulsos e a hora. A agenda semanal é gravada junto, mas não tem endereço Modbus (vem por MQTT).
- **Implementação:** `lib/config` (tabela de parâmetros, com os endereços, limites e padrões) e `lib/armazem` (dois espaços de flash com CRC-32, seguro contra queda de energia).

## Implementação do escravo na RS-485 (04/11/2026)

O escravo atende as funções **01, 02, 03, 04, 05, 06, 15 e 16**, com as exceções 01 (função ilegal), 02 (endereço ilegal), 03 (valor ilegal) e 04 (falha no escravo). O endereço de protocolo é o número do mapa menos a base da tabela menos 1 (retenção 40001 é o endereço 0). Difusão (endereço 0) só vale para escrita e não é respondida. Leituras de até 125 registradores e 256 bobinas; escritas de até 123 registradores e 256 bobinas.

| Tabela | Números atendidos | Conteúdo |
|---|---|---|
| Bobinas | 00001 a 00004 | Estado das saídas; escrever liga ou desliga em modo manual |
| Entradas discretas | 10001 a 10004, 10016 a 10019, 10031 | Entradas digitais, falha do sensor analógico n, hora válida |
| Registradores de entrada | 30001 a 30008, 30031 a 30038, 30091 a 30098, 30121 a 30126 | Valor de engenharia (float), corrente em mA (float), frequência atual da saída (float), data e hora local |
| Registradores de retenção | 40001 a 40413 (os da tabela de configuração) e 40286 a 40289 | Toda a configuração, e o modo manual ou automático das saídas (estado) |

- **Formato serial:** 8 bits, sem paridade, 1 stop bit (8N1), **confirmado em 04/11/2026** (P-048); velocidade pelo registrador 40400 (0 = 2400, 1 = 9600, 2 = 57600, 3 = 115200 bps), padrão 9600; endereço pelo 40402, padrão 1.
- **Fim do quadro:** silêncio de 3,5 caracteres, medido pelo próprio USART.
- **Retenção:** a escrita é atômica (valor fora do limite, float pela metade, endereço fora da configuração ou combinação proibida recusam o pedido inteiro). Um pedido não pode misturar os registradores de estado (40286 a 40289) com os de configuração.
- **Só com o sleep desabilitado:** o USART não recebe em Stop 2.
- **Ainda sem endereço:** o totalizador de pulsos, a tensão de alimentação, a temperatura e as cadeias de texto (número de série, versão e modelo).

## Exemplo de JSON

Regras de envio: o id do módulo vai no tópico, não no corpo. O JSON é separado por grupo. A chave de cada valor é o primeiro endereço Modbus do dado. Valores são números JSON, e estados digitais são 1 ou 0.

```json
{
  "timestamp": "2026-11-01T14:30:00-03:00",
  "analogico": {
    "30001": 1.25,
    "30031": 4.87
  },
  "digital": {
    "10001": 1,
    "00001": 0
  },
  "falhas": {
    "10016": 0
  },
  "sistema": {
    "10031": 1,
    "30127": 12.1
  },
  "rede": {
    "30281": 18,
    "30283": 1
  }
}
```

Os nomes dos grupos (analogico, digital, falhas, sistema, rede) são uma proposta, a confirmar.

## Pendências

- Resolvidas em 01/11/2026: ordem do float (ABCD), unidade do tempo de acionamento (segundos), faixa do endereço escravo (1 a 247) e configuração pelo Modbus.
- Bloco trifásico mantido como reservado, pois o device poderá integrar com MGE ou inversores pelo Modbus.
- Lista final de campos do heartbeat e formato das mensagens SETUP e INFO.
- Entrada digital (aberto em 03/11/2026): o registrador de modo da entrada (40151–40165) hoje diz "0 = discreto, 1 = pulsado". Falta definir os modos (discreto, contador de pulsos e frequência em Hz) e o endereço do totalizador de 32 bits de cada entrada, que será lido como valor absoluto (o servidor calcula as diferenças). Item P-032 em `STATUS_PROJETO.md`.
- Saída digital (resolvido em 03/11/2026): "pulsada" é a saída em frequência, então o tipo da saída (40166–40180) ficou 0 = discreto, 1 = frequência e 2 reservado. O "acionamento por tempo" é o retardo de acionamento (40256–40270), e o registrador 40271–40285 ficou reservado. O modo de operação (40412: 0 = contínuo, 1 = sleep) só pode ser 1 se nenhuma saída estiver em frequência, e a saída só pode ir para frequência com o modo de operação em 0.
- Regra do sensor entrada analógica para saída digital (04/11/2026): o cadastro vem por MQTT ou Modbus, com endereço Modbus para cada campo; as chaves do JSON seguem o endereço. Foram acrescentados os limiares máximo (40307–40336) e mínimo (40337–40366) e o sentido (40367–40381), com endereços **propostos a confirmar**. O **sentido foi confirmado**: 0 = liga no mínimo e desliga no máximo; 1 = liga no máximo e desliga no mínimo. A saída tem regra do sensor quando a entrada associada (40241–40255) é diferente de zero. Sem regra, valem a agenda offline e o CCO online.
- Agenda semanal das saídas (04/11/2026): **não tem registradores Modbus**; é cadastrada por MQTT, pelo CCO (`documentacao/agenda_operacional_offline.md`).

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.14 | Confirmados 40382–40396, 40413 e o formato serial 8N1 |
| 04/11/2026 | 0.1.13 | Implementação do escravo na RS-485: funções, tabelas atendidas e formato serial |
| 04/11/2026 | 0.1.12 | Persistência dos registradores de configuração; propostos 40382–40396 (estabilidade do sensor) e 40413 (inatividade da entrada digital) |
| 04/11/2026 | 0.1.11 | Tipo da saída: só as saídas 2 a 4 aceitam frequência e a gravação é recusada com o sleep ligado |
| 04/11/2026 | 0.1.10 | Proposto o endereço 40403 para o fuso horário da instalação, configurado pelo CCO |
| 04/11/2026 | 0.1.9 | Sentido da regra do sensor confirmado (0 liga no mínimo, 1 liga no máximo); registrado que a agenda não tem registradores Modbus |
| 04/11/2026 | 0.1.8 | Propostos os endereços dos limiares máximo e mínimo e do sentido da regra do sensor (40307 a 40381) |
| 04/11/2026 | 0.1.7 | Pendência: limiares máximo e mínimo da regra local para a saída discreta |
| 04/11/2026 | 0.1.6 | Correções da verificação do doc-keeper antes do fechamento da SPRINT_00 (números, nomes dos registradores, concordância e referências) |
| 03/11/2026 | 0.1.5 | O "acionamento por tempo" é o retardo de acionamento; ligar com a saída ligada ou aguardando é descartado; mapa Modbus das saídas ajustado (P-037) |
| 03/11/2026 | 0.1.4 | Registrada a pendência do modo da saída (pulsada é frequência) e a exclusão mútua com o sleep |
| 03/11/2026 | 0.1.3 | Registrada a pendência dos modos da entrada digital (discreto, contador de pulsos e frequência) e do endereço do totalizador |
| 01/11/2026 | 0.1.2 | Confirmados float big-endian (ABCD), tempo de acionamento em segundos e endereço escravo de 1 a 247; trifásico mantido reservado para integração com MGE e inversores |
| 01/11/2026 | 0.1.1 | Configuração válida pelo Modbus e pelo MQTT, com a mesma regra de sessão única; sessão de configuração termina após 5 minutos sem atividade |
| 01/11/2026 | 0.1.0 | Criado mapa Modbus com 15 posições por tipo, float em 2 registradores, conectividade e configurações, com base na norma e no manual Raptor |
