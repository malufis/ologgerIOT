# Mapeamento Modbus de equipamentos externos — oLoggerIOT

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.2.0
> **Status:** aprovado pelo responsável técnico em 02/11/2026, com execução em FreeRTOS. Itens em "Pendências" ainda abertos.

## Princípio

O **STM32 é o cérebro**: ele consulta os equipamentos externos, guarda os dados, aplica as regras e expõe o resultado. O ESP32 e o modem apenas transportam dados (MQTT). Nenhum módulo de comunicação interpreta registrador de equipamento externo.

O device recebe dados de equipamentos Modbus e os guarda em uma **tabela de variáveis na RAM**. O restante do firmware lê essa tabela, nunca o registrador bruto.

## Limites (constantes nomeadas)

| Constante | Valor | Origem |
|---|---|---|
| `MAX_EQUIPAMENTOS_MODBUS` | 8 | Decisão de 02/11/2026 |
| `MAX_VARIAVEIS_MODBUS` | 64 | Decisão de 02/11/2026, total entre todos os equipamentos |

As constantes ficam em `lib/tabela_variaveis/include/` e podem subir em tarefa própria, com nota no histórico.

## Camadas

| Módulo | Responsabilidade | Persistência |
|---|---|---|
| `lib/modbus` | Quadro RTU, CRC e funções de leitura | Nenhuma |
| `lib/modbus_mestre` | Task FreeRTOS que consulta os equipamentos, com timeouts e tentativas | RAM |
| `lib/tabela_variaveis` | Tabela de 64 variáveis, protegida por mutex FreeRTOS | RAM |
| `lib/mapeamento` | Configuração de equipamentos e entradas, validação e gravação | Flash, com versão e CRC |
| `lib/modbus_escravo` | Task FreeRTOS que atende o sistema pelo mapa fixo do device | Nenhuma |

Cada módulo tem interface pública pequena, conforme `documentacao/padrao_codigo_c.md`. A comunicação entre tasks segue a seção FreeRTOS desse padrão: filas e mutex estáticos, sem alocação dinâmica.

Escrita em equipamento externo **não faz parte do escopo** (decisão de 02/11/2026). O módulo `lib/modbus` implementa apenas leitura.

## Tabela de variáveis (RAM)

Cada variável tem:

- **ID** fixo, de 0 a 63.
- **Nome de código** para log e diagnóstico.
- **Tipo** e **unidade**.
- **Valor** atual.
- **Marca de tempo** da última leitura válida. Sem hora válida, o dado não tem validade (regra de B4).
- **Estado:** `NAO_LIDA`, `VALIDA`, `EXPIRADA` ou `FALHA`.
- **Contador de leituras válidas consecutivas**, usado na recuperação.

Regra de uso: regra de sensor e alarme só usa a variável com estado `VALIDA`. Para as outras, aplica a regra de falha já documentada em `documentacao/configuracao_do_dispositivo.md`.

Ao reiniciar, todas as variáveis voltam para `NAO_LIDA`. A configuração volta da flash, mas os valores não.

## Mapeamento configurável (flash)

Para cada equipamento:

| Campo | Regra |
|---|---|
| Endereço escravo | 1 a 247, único no barramento |
| Porta | RS-485 (`RS485_MODBUS`) |
| Período de consulta | Valor nomeado, em segundos |
| Entradas | Lista de até 64 no total, somando todos os equipamentos |

Cada entrada liga um endereço externo a uma variável:

| Campo | Exemplo |
|---|---|
| Função de leitura | Coil, entrada discreta, input register ou holding register |
| Endereço externo | Conforme manual do equipamento |
| Quantidade de registradores | 1 a 2 para formato numérico; limite do protocolo por função |
| Formato | uint16, int16, uint32, int32, float ABCD ou bool |
| Escala e offset | `valor = bruto × escala + offset` |
| ID da variável | Único na tabela |

Validação antes de gravar, em todo caminho de configuração:

- Endereço escravo dentro da faixa e sem repetição.
- ID de variável sem repetição entre entradas.
- Quantidade dentro do limite da função Modbus.
- Total de equipamentos e de variáveis dentro dos limites.
- CRC e versão do registro gravado.

Rejeita a configuração inteira se qualquer entrada falhar. Não grava parte dela.

## Fluxo de consulta

O barramento RS-485 tem um mestre e um quadro por vez. Por isso:

1. O agendador percorre os equipamentos ativos em ordem circular.
2. Cada consulta tem timeout de 30 s, fixo no firmware.
3. Sem resposta, o equipamento conta uma tentativa. Após dez tentativas, passa para `FALHA`.
4. Após uma hora em falha, registra defeito e envia alerta ao sistema.
5. Cinco leituras válidas consecutivas restabelecem a comunicação. A saída que depende da variável reavalia a regra, respeitando o tempo de partida.

**Risco a tratar:** dez tentativas de 30 s em um equipamento que não responde bloqueariam o barramento. O agendador precisa ser **não bloqueante**: a espera de cada equipamento vira um estado com marca de tempo, e os demais equipamentos continuam sendo consultados. Isso precisa de teste no simulador.

## Escravo Modbus do device

O device responde ao sistema pelo mapa de `documentacao/mapa_modbus.md`. Esse mapa é fixo e não muda quando um equipamento externo é trocado.

Decisão de 02/11/2026: adotada esta forma.

- Leitura de variável com estado `VALIDA` devolve o valor.
- Estado de cada variável fica em um registrador de status, para o sistema saber se o dado é confiável.
- Variável `EXPIRADA` ou `FALHA` devolve o último valor e o estado. Não devolve zero como se fosse medição.

## Configuração pelo sistema e pelo gateway

Decisão de 02/11/2026: a configuração chega pelo sistema (MQTT) e pelo gateway USB no notebook.

- Os dois caminhos usam a **mesma validação** e a **mesma sessão de configuração única** do device (ver `documentacao/configuracao_do_dispositivo.md`).
- Durante a sessão, comandos MQTT de outra origem são recusados com aviso.
- Gravação é atômica: a configuração inteira ou nada.

## Pendências

- Tratamento de valor inválido do equipamento (NaN de float, código de exceção Modbus): proposta é marcar a variável como `FALHA`, mas falta confirmação.
- Consulta de equipamentos com período diferente: valor por equipamento ou único no device.
- Limite de quantidade por consulta para cada função, a verificar no documento da especificação Modbus.
- Formato de transporte da configuração pelo sistema (tópico e JSON), a definir em `documentacao/integracao_sistema.md`.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 02/11/2026 | 0.1.0 | Proposta criada com tabela de variáveis na RAM, mapeamento configurável em flash, limites de 8 equipamentos e 64 variáveis, escravo com estado e configuração pelo sistema e pelo gateway |
| 02/11/2026 | 0.2.0 | Aprovada pelo responsável técnico: execução em FreeRTOS (tasks, filas e mutex estáticos); escravo com estado adotado; escrita em equipamento externo removida do escopo |
