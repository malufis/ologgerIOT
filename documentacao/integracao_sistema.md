# Integração do device com o sistema — oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.3

## Para quem é

Equipe de desenvolvimento do sistema (plataforma, backend e integrações). Descreve o que o device faz e o que espera do sistema. Não descreve regras internas do sistema, como perfis, painéis, chat ou banco.

## Princípios

- Toda comunicação do device é por **MQTT 5.0 com TLS 1.3** ou pela **serial** (gateway no notebook do técnico).
- Cada device tem credencial individual, provisionada pelo sistema, e tópicos próprios. Não há comando em grupo de devices.
- O caminho base dos tópicos é cadastrado pelo sistema e enviado ao device. A hierarquia de cadastro (empresa, filial, cidade, local) é do sistema; o device só usa o caminho recebido como texto.
- O id do módulo está no tópico. Não entra no corpo das mensagens.
- Mensagens em JSON, separadas por grupo.

## Tópicos padrão

Caminho base: `<caminho_do_sistema>/<id_do_modulo>`. Nomes de tópico abaixo são relativos a esse caminho.

| Tópico | Sentido | Finalidade |
|---|---|---|
| `SETUP` | sistema → device | Envia a configuração do device. Formato a definir na implantação. |
| `INFO` | device → sistema | Responde ao pedido do sistema com identificação e versão. O sistema pede; o device responde. |
| `STATUS` | device → sistema | Heartbeat com dados de conectividade do módulo (id, IP, rede e operadora, entre outros). Campos a definir na implantação. |
| `COMANDO` | sistema → device | Recebe comandos de saída, configuração e modo. |
| `RESULTADO` | device → sistema | Confirma recebimento e resultado do comando: executado, ou falhou com motivo. |
| `DADOS` | device → sistema | Telemetria em lote, com confirmação de recebimento pelo servidor. |
| `ALERTA` | device → sistema | Alertas de alarme, falha de sensor, falha Modbus, hora inválida e resultado de atualização. |

## Confirmações

- Comando: primeiro recebido, depois executado ou falhou com motivo. Os dois retornos vão no mesmo `RESULTADO`, associados ao identificador do comando.
- Telemetria: o device só remove o lote após confirmação de recebimento e salvamento pelo sistema, identificado pelo lote.
- Sem confirmação após cinco tentativas, o device mantém os dados para envio posterior.

## Formato dos dados

- JSON com grupos: `analogico`, `digital`, `falhas`, `sistema` e `rede`. Nomes a confirmar na implantação.
- Chave de cada valor: endereço Modbus do dado, conforme `documentacao/mapa_modbus.md`.
- Valor: número JSON. Estado digital: `1` ou `0`.
- Campo `timestamp` com a hora do device, em ISO 8601. Dado sem hora válida é marcado como sem validade e não é tratado como medição.

Exemplo:

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
    "10031": 1
  }
}
```

## Mapa Modbus

O device expõe o mapa Modbus RTU de `documentacao/mapa_modbus.md`, com 15 posições por tipo de entrada e saída, float em dois registradores em big-endian (ABCD) e endereço escravo de 1 a 247. A configuração pode ser feita pelo Modbus e pelo MQTT, com a mesma regra de sessão.

## Sessão de configuração

- Iniciada pelo técnico, pelo gateway serial no notebook, ou pelo sistema, pelo MQTT.
- Enquanto a sessão está ativa, o device recusa comandos MQTT com aviso.
- A sessão termina após cinco minutos sem atividade.

## Gateway serial

Aplicação Python no notebook do técnico, com chaveiro serial ligado ao STM32. Envia apenas as configurações definidas no catálogo de comandos locais. Não há app de celular; o celular do integrador usa WhatsApp, pelo sistema.

## Cadastro da agenda das saídas (proposta de 04/11/2026)

A agenda semanal das saídas chega no tópico `COMANDO`, vinda do CCO, em JSON, e não tem endereço Modbus. Cada saída leva uma cadeia de 84 caracteres hexadecimais (336 intervalos de 30 minutos, um bit por intervalo). A mensagem é validada por inteiro: se qualquer saída for inválida, o device recusa tudo e responde em `RESULTADO` com o motivo, mantendo a agenda anterior. Formato completo, exemplo e regras em `documentacao/agenda_operacional_offline.md`.

## Pendências de integração

- Formato de `SETUP` e `INFO`.
- Campos do `STATUS` (heartbeat).
- Nomes dos grupos do JSON.
- Formato do identificador de comando e da confirmação de lote.
- Caminho base dos tópicos: formato e tamanho máximo.
- Cadastro da regra do sensor por MQTT (04/11/2026): os campos entrada associada, limiares máximo e mínimo, sentido e frequências de 4 mA e 20 mA chegam com as chaves iguais aos endereços Modbus (40181 a 40240, 40241 a 40255 e 40307 a 40381, estes últimos em proposta), sob a regra da sessão de configuração.
- Grupo `digital` do JSON (aberto em 03/11/2026): estado de cada entrada e, no modo contador de pulsos, o totalizador de 32 bits enviado como valor absoluto. O servidor calcula as diferenças entre duas leituras, o que torna uma transmissão perdida sem perda de contagem. Nomes dos campos a definir junto com o mapa Modbus (P-032).

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.3 | Registrada a agenda das saídas no tópico COMANDO (P-042) |
| 04/11/2026 | 0.1.2 | Registrado o cadastro da regra do sensor por MQTT, com chaves iguais aos endereços Modbus |
| 03/11/2026 | 0.1.1 | Registrada a pendência do grupo digital do JSON com o totalizador enviado como valor absoluto |
| 01/11/2026 | 0.1.0 | Criado contrato de integração com tópicos padrão, confirmações, formato de dados, mapa Modbus e sessão de configuração |
