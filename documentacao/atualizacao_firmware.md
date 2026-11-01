# Atualização de firmware — oLoggerIOT

> **Criado em:** 31/10/2026 · **Última modificação:** 31/10/2026 · **Versão:** 0.1.6

## Requisitos levantados

- CCO, integrador e técnico de campo poderão iniciar a atualização. É necessário identificar quem a enviou; autenticação e detalhamento da rastreabilidade ainda serão definidos.
- Antes da instalação, verificar a validade e a compatibilidade do firmware com o equipamento. Se não passar na verificação, preservar a versão em funcionamento.
- Se a atualização não for bem-sucedida, inclusive por interrupção de energia, o equipamento deverá recuperar a versão anterior. Após a volta da energia, essa recuperação deve permitir que volte a executar uma versão funcional.
- Durante a atualização e a reinicialização, todas as saídas digitais devem permanecer desligadas. Após a conclusão e o retorno do dispositivo, ele volta a aplicar os comandos e as regras que estavam ativos antes da atualização. A sequência de retomada e sua interação com modos de comando, falhas de sensor e partida temporizada precisam ser detalhadas.
- Se uma saída estava ligada antes da atualização e possui tempo de partida configurado, ela deve aguardar esse tempo antes de ligar novamente após o retorno do dispositivo.
- Enviar uma mensagem informando que o dispositivo está sendo atualizado. Destinatários, momento exato, confirmação de entrega e tratamento de falha de comunicação ainda precisam ser definidos.
- Ao concluir, informar ao sistema o resultado: atualização concluída ou falha com retorno à versão anterior, identificando o equipamento e quem iniciou a atualização. O tratamento de impossibilidade de recuperação ainda precisa ser definido.
- Sem conexão para comunicar o resultado, guardá-lo na fila persistente para envio posterior, removendo-o somente após confirmação de recebimento pelo servidor, conforme a regra de telemetria.

## Pontos a detalhar

- Autenticidade da imagem, identificação do responsável, permissões, transferência e comunicação do resultado.
- Como preservar e recuperar a versão anterior, incluindo capacidade de armazenamento, sequência de inicialização e testes de interrupção em cada etapa.
- Sequência de retomada das saídas, inclusive temporizadores e condições de falha; interação com sleep e tarefas em andamento. Verificar o estado elétrico das saídas durante todas as etapas, inclusive a reinicialização.
- Aplicação ao STM32 e ao módulo ESP32, conforme o componente a atualizar; detalhes de cada processo ainda não foram definidos.

São requisitos de levantamento; a implementação e a viabilidade em cada modelo ainda precisam ser verificadas.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 31/10/2026 | 0.1.0 | Registrados responsáveis, identificação, verificações e recuperação da versão anterior em falha ou interrupção de energia |
| 31/10/2026 | 0.1.1 | Usuário definiu desligamento de todas as saídas digitais durante atualização e reinicialização |
| 31/10/2026 | 0.1.2 | Usuário solicitou mensagem informando que o dispositivo está sendo atualizado |
| 31/10/2026 | 0.1.3 | Usuário escolheu retomar os comandos e regras anteriores após a atualização |
| 31/10/2026 | 0.1.4 | Usuário definiu que saída anteriormente ligada aguarda seu tempo de partida configurado antes de religar após atualização |
| 31/10/2026 | 0.1.5 | Usuário confirmou envio ao sistema do resultado da atualização com identificação do equipamento e responsável |
| 31/10/2026 | 0.1.6 | Usuário confirmou retenção e reenvio do resultado sem conexão, com exclusão após confirmação do servidor |
