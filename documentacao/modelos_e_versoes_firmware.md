# Modelos e versões do firmware — oLoggerIOT

> **Criado em:** 29/10/2026 · **Última modificação:** 29/10/2026 · **Versão:** 0.2.0

## Entendimento atual do projeto

Desenvolver o firmware para **os dois projetos: OloggerMulti e y7080**, dentro da família oLoggerIOT. O foco atual é entender o projeto e seus comportamentos. A forma de separar e organizar a implementação será definida posteriormente por Rodrigo Maluf.

## Ideia proposta pelo usuário

Ter **uma compilação única e um arquivo de setup que indique o que o dispositivo contém**. Esse setup descreve a composição do equipamento, incluindo os recursos presentes conforme o modelo: portas, sensores, rádios e interfaces. Origem: esclarecimento explícito de Rodrigo Maluf em 29/10/2026.

| Aspecto | Entendimento funcional | Detalhamento futuro |
|---|---|---|
| Hardware | O equipamento contém uma combinação de recursos | Quais informações cada setup precisará descrever |
| Firmware | Controla os recursos existentes no dispositivo | Como o setup será aplicado na implementação |
| Modelos | OloggerMulti e y7080 fazem parte do projeto | Organização do código, a ser definida pelo usuário |
| Versões | O software terá evolução e versionamento | Numeração, geração e publicação das versões |

## Decisões técnicas ainda abertas

Ainda não foram definidos:

- Formato, campos e local de armazenamento do setup.
- Momento de aplicação do setup: geração, gravação ou inicialização/operação.
- Possibilidade de alterar o setup e quem terá essa autorização.
- Organização de arquivos, separação de módulos e ferramentas de compilação.
- Realização técnica da compilação única nos MCUs utilizados.

Esses pontos serão avaliados posteriormente. A ideia de compilação única é registrada como intenção do usuário, sem declarar uma solução já implementada ou tecnicamente validada.

## Correção do registro anterior

A versão 0.1.0 deste documento antecipou imagens separadas por modelo a partir de uma interpretação do assistente. Essa interpretação foi substituída pelo esclarecimento acima. Os exemplos de nomes de dois binários não constituem decisão vigente do projeto.

## Estado da implementação

Há documentação e análise de hardware no workspace. Ainda não há firmware C, projeto de compilação ou binários produzidos. O levantamento de requisitos continua; esta atualização não confirma os blocos restantes da entrevista.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 29/10/2026 | 0.1.0 | Registrados os dois modelos como alvos do firmware e a seleção/identificação da imagem na geração e gravação |
| 29/10/2026 | 0.2.0 | Corrigida interpretação anterior: usuário propõe compilação única com arquivo de setup; separação e organização técnicas serão definidas depois; foco atual no entendimento do projeto |
