# Dicionário de funções — firmware oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.3.1

## Como manter

Toda função criada ou alterada no firmware entra neste dicionário no mesmo item em que foi feita. A skill `doc-sync` cuida disso ao fechar o item.

A tabela é **gerada**, a partir dos comentários Doxygen. Não editar à mão o trecho entre os marcadores. Corrija o comentário no código e regenere:

```
doxygen Doxyfile
python tools/gera_dicionario.py
python tools/gera_dicionario.py --check
```

O `--check` não escreve: sai com código 1 se o dicionário estiver desatualizado. O Doxygen está configurado para falhar em função ou parâmetro sem documentação, então uma função pública sem Doxygen não chega ao dicionário.

Cada entrada informa: nome, módulo, arquivo, finalidade, parâmetros, retorno e status da função (`ativa` ou `obsoleta`).

Função `static` (interna ao arquivo) não entra aqui. Só entra a função da interface pública do módulo, definida no `.h`.

## Funções

<!-- DICIONARIO:INICIO -->
| Função | Módulo | Arquivo | Finalidade | Retorno | Status |
|---|---|---|---|---|---|
| `adc_diagnostico` | adc | `lib/adc/include/adc.h` | Diagnóstico de bancada: mede VREFINT e a entrada 4 com conversões simples. | void | ativa |
| `adc_inicia` | adc | `lib/adc/include/adc.h` | Inicia a aquisição contínua. | ADC_OK, ADC_ERRO_PARAMETRO ou ADC_ERRO_HAL. | ativa |
| `adc_para` | adc | `lib/adc/include/adc.h` | Para a aquisição para o equipamento dormir: timer, DMA, ADC e referência interna. | ADC_OK ou ADC_ERRO_HAL. | ativa |
| `adc_retoma` | adc | `lib/adc/include/adc.h` | Retoma a aquisição com a taxa e o callback de adc_inicia(). | ADC_OK, ADC_ERRO_PARAMETRO (se adc_inicia() nunca foi chamado) ou ADC_ERRO_HAL. | ativa |
| `aquisicao_contagens` | aquisicao | `lib/aquisicao/include/aquisicao.h` | Lê a média de contagens do ADC do último segundo de uma entrada. | AQUISICAO_OK, AQUISICAO_ERRO_PARAMETRO ou AQUISICAO_ERRO_SEM_DADO. | ativa |
| `aquisicao_inicia` | aquisicao | `lib/aquisicao/include/aquisicao.h` | Cria a tarefa de aquisição e inicia o ADC. | AQUISICAO_OK, AQUISICAO_ERRO_PARAMETRO ou AQUISICAO_ERRO_INICIO. | ativa |
| `aquisicao_ma` | aquisicao | `lib/aquisicao/include/aquisicao.h` | Lê a corrente média do último segundo de uma entrada. | AQUISICAO_OK, AQUISICAO_ERRO_PARAMETRO ou AQUISICAO_ERRO_SEM_DADO. | ativa |
| `aquisicao_para` | aquisicao | `lib/aquisicao/include/aquisicao.h` | Para a aquisição, o timer e o ADC, para o equipamento dormir. | AQUISICAO_OK ou AQUISICAO_ERRO_INICIO. | ativa |
| `aquisicao_retoma` | aquisicao | `lib/aquisicao/include/aquisicao.h` | Retoma a aquisição depois do sleep, com a mesma taxa. | AQUISICAO_OK ou AQUISICAO_ERRO_INICIO. | ativa |
| `aquisicao_segundos` | aquisicao | `lib/aquisicao/include/aquisicao.h` | Lê a quantidade de segundos fechados desde a partida. | Segundos fechados. | ativa |
| `energia_define_habilitada` | energia | `lib/energia/include/energia.h` | Liga ou desliga o modo sleep. Começa desligado. | ENERGIA_OK ou ENERGIA_ERRO_BLOQUEADO (só ao ligar). | ativa |
| `energia_habilitada` | energia | `lib/energia/include/energia.h` | Informa se o sleep está habilitado. | true se o equipamento pode entrar em sleep. | ativa |
| `energia_inicia` | energia | `lib/energia/include/energia.h` | Cria a tarefa de gerência de energia com memória estática. | ENERGIA_OK ou ENERGIA_ERRO. | ativa |
| `energia_solicita_acordar` | energia | `lib/energia/include/energia.h` | Pede para o equipamento sair do sleep na próxima oportunidade. | void | ativa |
| `io_backup_grava` | io_backup | `lib/io/include/io_backup.h` | Grava uma palavra de backup. | void | ativa |
| `io_backup_inicia` | io_backup | `lib/io/include/io_backup.h` | Liga o acesso ao domínio de backup e o clock do TAMP. | void | ativa |
| `io_backup_le` | io_backup | `lib/io/include/io_backup.h` | Lê uma palavra de backup. | Valor da palavra; zero se o índice for inválido. | ativa |
| `io_entrada_acordar_desabilita` | io_entrada | `lib/io/include/io_entrada.h` | Desliga a interrupção externa e volta as entradas ao modo de leitura simples. | void | ativa |
| `io_entrada_acordar_habilita` | io_entrada | `lib/io/include/io_entrada.h` | Liga a interrupção externa na borda de ativação das entradas, para contar dormindo. | IO_OK. | ativa |
| `io_entrada_coleta_pulsos` | io_entrada | `lib/io/include/io_entrada.h` | Lê e zera os pulsos contados em uma entrada pela interrupção externa. | Pulsos contados desde a última coleta; zero se o canal for inválido. | ativa |
| `io_entrada_inicia` | io_entrada | `lib/io/include/io_entrada.h` | Configura os pinos das entradas digitais como entrada, sem resistor interno. | IO_OK. | ativa |
| `io_entrada_irq` | io_entrada | `lib/io/include/io_entrada.h` | Trata a interrupção externa das entradas: soma um pulso em cada linha disparada. | void | ativa |
| `io_entrada_le` | io_entrada | `lib/io/include/io_entrada.h` | Lê o estado de uma entrada digital. | IO_OK ou IO_ERRO_PARAMETRO. | ativa |
| `io_filtro_amostras_do_tempo` | io_filtro | `lib/io/include/io_filtro.h` | Converte um tempo de estabilidade em número de amostras, arredondando para cima. | IO_FILTRO_OK ou IO_FILTRO_ERRO_PARAMETRO (nulo, zero ou número fora de uint16_t). | ativa |
| `io_filtro_atualiza` | io_filtro | `lib/io/include/io_filtro.h` | Recebe uma amostra do nível da entrada e devolve o estado aceito. | Estado aceito após esta amostra. | ativa |
| `io_filtro_inicia` | io_filtro | `lib/io/include/io_filtro.h` | Inicia o filtro com o estado aceito e o número de amostras de estabilidade. | IO_FILTRO_OK ou IO_FILTRO_ERRO_PARAMETRO. | ativa |
| `io_pulsos_atualiza` | io_pulsos | `lib/io/include/io_pulsos.h` | Recebe o estado filtrado da entrada e conta a transição para ativo. | void | ativa |
| `io_pulsos_contagem` | io_pulsos | `lib/io/include/io_pulsos.h` | Lê o total de pulsos contados. | Pulsos contados desde o início. | ativa |
| `io_pulsos_define_timeout` | io_pulsos | `lib/io/include/io_pulsos.h` | Altera o tempo sem pulso necessário para poder dormir. | IO_PULSOS_OK ou IO_PULSOS_ERRO_PARAMETRO. | ativa |
| `io_pulsos_inicia` | io_pulsos | `lib/io/include/io_pulsos.h` | Inicia o contador. O instante inicial conta como atividade. | IO_PULSOS_OK ou IO_PULSOS_ERRO_PARAMETRO. | ativa |
| `io_pulsos_pode_dormir` | io_pulsos | `lib/io/include/io_pulsos.h` | Informa se a entrada ficou sem pulso pelo tempo configurado. | true se passou ao menos o tempo de inatividade desde o último pulso. | ativa |
| `io_pulsos_soma` | io_pulsos | `lib/io/include/io_pulsos.h` | Soma pulsos contados enquanto o equipamento dormia. | void | ativa |
| `io_saida_define` | io_saida | `lib/io/include/io_saida.h` | Liga ou desliga uma saída no pino. | IO_SAIDA_OK ou IO_SAIDA_ERRO_PARAMETRO. | ativa |
| `io_saida_inicia` | io_saida | `lib/io/include/io_saida.h` | Configura os pinos das saídas, todos desligados. | IO_SAIDA_OK. | ativa |
| `io_saida_le` | io_saida | `lib/io/include/io_saida.h` | Lê o estado que o pino de uma saída está comandando. | IO_SAIDA_OK ou IO_SAIDA_ERRO_PARAMETRO. | ativa |
| `io_saidas_aguardando` | io_saidas | `lib/io/include/io_saidas.h` | Informa se uma saída está esperando o retardo de acionamento. | IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO. | ativa |
| `io_saidas_define_atraso` | io_saidas | `lib/io/include/io_saidas.h` | Define o retardo de acionamento (relé de tempo) de uma saída. | IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO. | ativa |
| `io_saidas_desliga` | io_saidas | `lib/io/include/io_saidas.h` | Desliga uma saída. | IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO. | ativa |
| `io_saidas_estado` | io_saidas | `lib/io/include/io_saidas.h` | Lê o estado de uma saída. | IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO. | ativa |
| `io_saidas_inicia` | io_saidas | `lib/io/include/io_saidas.h` | Cria a tarefa que liga as saídas ao fim do retardo, com memória estática. | IO_SAIDAS_OK ou IO_SAIDAS_ERRO_TAREFA. | ativa |
| `io_saidas_liga` | io_saidas | `lib/io/include/io_saidas.h` | Liga uma saída sem prazo, respeitando o retardo de acionamento configurado. | IO_SAIDAS_OK ou IO_SAIDAS_ERRO_PARAMETRO. | ativa |
| `io_saidas_prepara` | io_saidas | `lib/io/include/io_saidas.h` | Leva os pinos das saídas ao estado desligado e zera os estados. | IO_SAIDAS_OK. | ativa |
| `io_tarefa_bruto` | io_tarefa | `lib/io/include/io_tarefa.h` | Lê o estado das entradas sem filtro. | Máscara de bits, igual a io_tarefa_estado(), mas da última leitura direta. | ativa |
| `io_tarefa_define_inatividade` | io_tarefa | `lib/io/include/io_tarefa.h` | Altera o tempo sem pulso necessário para poder dormir, em todas as entradas. | IO_TAREFA_OK ou IO_TAREFA_ERRO. | ativa |
| `io_tarefa_define_total` | io_tarefa | `lib/io/include/io_tarefa.h` | Define o totalizador de uma entrada, por exemplo para igualar ao medidor. | IO_TAREFA_OK ou IO_TAREFA_ERRO. | ativa |
| `io_tarefa_estado` | io_tarefa | `lib/io/include/io_tarefa.h` | Lê o estado filtrado das entradas. | Máscara de bits: o bit 0 é a entrada D1 e o bit 3 é a D4; 1 = ativa (há tensão). | ativa |
| `io_tarefa_inicia` | io_tarefa | `lib/io/include/io_tarefa.h` | Inicia as entradas, o filtro e cria a tarefa com memória estática. | IO_TAREFA_OK ou IO_TAREFA_ERRO. | ativa |
| `io_tarefa_pode_dormir` | io_tarefa | `lib/io/include/io_tarefa.h` | Informa se todas as entradas ficaram sem pulso pelo tempo de inatividade. | true se nenhuma entrada recebeu pulso no tempo configurado. | ativa |
| `io_tarefa_prepara_sleep` | io_tarefa | `lib/io/include/io_tarefa.h` | Prepara as entradas para dormir: liga a contagem pela interrupção externa. | void | ativa |
| `io_tarefa_pulsos` | io_tarefa | `lib/io/include/io_tarefa.h` | Lê o total de pulsos contados em uma entrada desde a partida. | Pulsos contados; zero se o canal for inválido. | ativa |
| `io_tarefa_retoma` | io_tarefa | `lib/io/include/io_tarefa.h` | Retoma a leitura periódica depois do sleep. | void | ativa |
| `io_tarefa_total` | io_tarefa | `lib/io/include/io_tarefa.h` | Lê o totalizador (hodômetro) de uma entrada. | Total acumulado de pulsos; zero se o canal for inválido. | ativa |
| `io_tarefa_trata_despertar` | io_tarefa | `lib/io/include/io_tarefa.h` | Soma os pulsos contados dormindo e ressincroniza os filtros com o estado atual. | void | ativa |
| `io_totalizador_corrompido` | io_totalizador | `lib/io/include/io_totalizador.h` | Informa se o total de uma entrada foi perdido por corrupção no armazém. | true se nenhuma cópia era válida na última carga e o total recomeçou em zero. Uma entrada nunca gravada (armazém zerado) também aparece como corrompida. | ativa |
| `io_totalizador_define` | io_totalizador | `lib/io/include/io_totalizador.h` | Define o total de uma entrada, por exemplo para igualar à leitura do medidor. | IO_TOTALIZADOR_OK ou IO_TOTALIZADOR_ERRO_PARAMETRO. | ativa |
| `io_totalizador_inicia` | io_totalizador | `lib/io/include/io_totalizador.h` | Carrega os totais do armazém e guarda a referência para as gravações seguintes. | IO_TOTALIZADOR_OK ou IO_TOTALIZADOR_ERRO_PARAMETRO. | ativa |
| `io_totalizador_le` | io_totalizador | `lib/io/include/io_totalizador.h` | Lê o total de uma entrada. | Total acumulado; zero se o canal for inválido. | ativa |
| `io_totalizador_soma` | io_totalizador | `lib/io/include/io_totalizador.h` | Soma pulsos ao total de uma entrada e grava no armazém. | IO_TOTALIZADOR_OK ou IO_TOTALIZADOR_ERRO_PARAMETRO. | ativa |
| `rtos_tick_handler` | rtos | `lib/rtos/include/rtos.h` | Trata o tick do sistema, chamado pela interrupção do SysTick. | void | ativa |
| `saida_aguardando` | saida | `lib/io/include/saida.h` | Informa se a saída está esperando o retardo de acionamento. | true se há um retardo em curso; a saída continua desligada. | ativa |
| `saida_atualiza` | saida | `lib/io/include/saida.h` | Atualiza o estado com o instante atual e devolve se a saída está ligada. | true se a saída está ligada depois da atualização. | ativa |
| `saida_define_atraso` | saida | `lib/io/include/saida.h` | Define o retardo de acionamento da saída. | void | ativa |
| `saida_desliga` | saida | `lib/io/include/saida.h` | Comando de desligar. Cancela o retardo em curso e desliga a saída. | void | ativa |
| `saida_inicia` | saida | `lib/io/include/saida.h` | Inicia a saída desligada e sem retardo. | SAIDA_OK ou SAIDA_ERRO_PARAMETRO. | ativa |
| `saida_liga` | saida | `lib/io/include/saida.h` | Comando de ligar a saída, respeitando o retardo de acionamento. | void | ativa |
| `saida_ligada` | saida | `lib/io/include/saida.h` | Lê o estado da saída sem atualizar o retardo. | true se a saída está ligada. | ativa |
| `sensor_contagens_para_ma` | sensor | `lib/sensor/include/sensor.h` | Converte contagens do ADC em corrente, pela tensão no resistor shunt. | SENSOR_OK ou SENSOR_ERRO_PARAMETRO. | ativa |
| `sensor_ma_para_valor` | sensor | `lib/sensor/include/sensor.h` | Converte corrente 4-20 mA em valor de engenharia, com zero, span e offset. | SENSOR_OK ou SENSOR_ERRO_PARAMETRO. | ativa |
| `sensor_media_acumula` | sensor | `lib/sensor/include/sensor.h` | Acumula uma amostra e, ao fechar um segundo, entrega a média. | SENSOR_OK ou SENSOR_ERRO_PARAMETRO. | ativa |
| `sensor_media_inicia` | sensor | `lib/sensor/include/sensor.h` | Inicia o acumulador para uma taxa de amostragem. | SENSOR_OK, SENSOR_ERRO_PARAMETRO ou SENSOR_ERRO_TAXA. | ativa |
| `vida_contador` | vida | `lib/vida/include/vida.h` | Lê o contador de ciclos da tarefa de vida. | Quantidade de ciclos executados desde a partida. | ativa |
| `vida_inicia` | vida | `lib/vida/include/vida.h` | Cria a tarefa de vida com memória estática. | 0 em sucesso, código negativo em erro. | ativa |
| `vida_pilha_livre_palavras` | vida | `lib/vida/include/vida.h` | Lê a menor folga de pilha medida na tarefa de vida. | Folga mínima, em palavras de 32 bits. Zero antes do primeiro ciclo. | ativa |
<!-- DICIONARIO:FIM -->

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.3.1 | Tabela regenerada com 73 funções públicas: entram `saida`, `io_saida`, `io_saidas` e o retardo de acionamento; sai `io_saidas_aciona` |
| 03/11/2026 | 0.3.0 | Tabela regenerada com 56 funções públicas: `io`, `energia`, parada e retomada da aquisição e as demais |
| 02/11/2026 | 0.2.0 | Regra de manutenção antes da geração automática; tabela de funções com colunas definidas; só interface pública entra |
| 01/11/2026 | 0.1.0 | Criado esqueleto do dicionário de funções, gerado a partir do Doxygen |
