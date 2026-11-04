# Entrevista de Inicialização de Projeto

> **Criado em:** 15/10/2026 · **Última modificação:** 04/11/2026 · **Versão:** 1.2.1

> **Nota de 04/11/2026:** os documentos de sistema citados abaixo (`documentacao/01_visao_do_projeto.md`, `02_requisitos.md`, `03_glossario.md`, `04_riscos_e_pendencias.md`, `05_arquitetura.md`, `apps_moveis.md`, `lgpd.md`, `seguranca_e_dados.md`, `adr/ADR-0001-backend.md`, `adr/ADR-0002-frontend.md` e `planejamento/ROADMAP.md`) pertencem ao kit genérico de inicialização do **sistema** e **não existem neste repositório do firmware**. As decisões do device estão em `sistema-init/STATUS_PROJETO.md` e em `documentacao/`. A pasta `documentacao/adr/` deste repositório trata das decisões de arquitetura do firmware, com outros assuntos.

## Como funciona

A entrevista tem 18 blocos (B0 a B17), respondidos na ordem. Cada bloco tem um **objetivo**, as **perguntas** e as **saídas** que alimentam os documentos. Respostas "não sei" geram item `PENDENTE` no estado do projeto. Blocos marcados com **(*)** são obrigatórios para avançar. O bloco B17 (stack) é sempre o último, porque depende de todos os anteriores.

## Glossário inicial

Termos usados nas perguntas. Serão expandidos em `documentacao/03_glossario.md` do projeto.

- **Lambda (gatilho de pré-execução):** função executada automaticamente quando um endpoint recebe uma requisição, **antes** de qualquer outro processamento. Exemplo: ao receber "ligar luz", a lambda verifica permissão e horário e só então executa o acionamento. O nome "lambda" é uma escolha do usuário e pode ser alterado.
- **Auditoria de alterações sensíveis:** registro de quem alterou algo sensível (configuração, permissão, usuário, comando enviado a dispositivo), com data, hora, IP, navegador, valor anterior e valor novo.
- **RLS (Row Level Security):** regra do banco de dados que limita quais linhas cada usuário ou serviço pode ler ou alterar, independentemente do código da aplicação.
- **Mascaramento:** exibição parcial de um dado (ex.: `***.456.789-**`), aplicada conforme o perfil de quem consulta.
- **Dado pessoal sensível (LGPD):** dado sobre origem racial, saúde, biometria, convicção, entre outros. Dados como CPF, e-mail e telefone são dados pessoais e exigem proteção, mas não entram nessa categoria.
- **MQTT 1883 / 8883:** porta padrão do protocolo MQTT sem criptografia (1883) e com TLS (8883). Em produção, use 8883.

---

## B0 — Identificação (*)
**Objetivo:** identificar o projeto, seus responsáveis e seu nome oficial.
1. Qual é o **nome do sistema**? Ele será usado em documentos, código e interface.
2. Existe **codinome ou nome interno** diferente do nome comercial?
3. Qual o **nome do repositório** Git?
4. Quem é o cliente ou patrocinador? Quem decide sobre o projeto?
5. Quem é o responsável técnico?
6. Descreva o objetivo em uma frase.
7. Qual problema o sistema resolve hoje?

**Saídas:** cabeçalho do projeto com nome oficial, visão inicial.

## B1 — Negócio e escopo (*)
**Objetivo:** delimitar o que será feito.
1. Qual o setor de atuação? (indústria, saúde, comércio, outro)
2. Quem são os usuários-alvo? Quantas pessoas, em quais funções?
3. Qual resultado esperado em 6 a 12 meses?
4. O que está **fora** do escopo desta versão?
5. Existe sistema atual que será substituído ou integrado?
6. Há prazo ou orçamento que limite o projeto?

**Saídas:** escopo, fora de escopo, público, restrições de prazo e orçamento.

## B2 — Dados e origem (*)
**Objetivo:** entender quais dados entram no sistema e como são usados.
1. Quais são as fontes de dados? (dispositivos, sensores, APIs, planilhas, usuários)
2. Qual o formato dos dados? (JSON, CSV, binário, outro)
3. Qual a frequência e o volume estimado de dados?
4. Quais grandezas são medidas e em qual unidade? (energia, 4-20 mA, pulsos, frequência, temperatura, outras)
5. Por quanto tempo os dados brutos e os tratados devem ser mantidos?
6. Há dados pessoais ou sensíveis? (detalhado no B11)

**Saídas:** catálogo de fontes de dados, modelo de dados inicial, requisitos de retenção.

## B3 — Dispositivos IoT e comunicação (*)
**Objetivo:** definir se haverá dispositivos IoT, onde o broker MQTT vai rodar e como os dispositivos se comunicam com o sistema. Ao final deste bloco, o agente **propõe a estrutura de comunicação**, que o usuário confirma.

### 3.1 Dispositivos
1. Haverá dispositivos IoT conectados? Quais? (CLP, gateway, sensor com conectividade própria, outro)
2. Quantos dispositivos por cliente e no total, hoje e em 12 meses?
3. Qual a conectividade do dispositivo? (Ethernet, Wi-Fi, 4G/LTE, outra)
4. O dispositivo deve guardar dados localmente quando perder conexão e enviar depois? (buffer offline)

### 3.2 Broker e infraestrutura de mensagens
5. Onde o broker MQTT vai rodar?
   - **A)** AWS IoT Core (serviço gerenciado da Amazon)
   - **B)** Broker próprio em nuvem de sua escolha (Mosquitto, EMQX ou HiveMQ, hospedado por você)
   - **C)** Broker proprietário on-premise, na sua rede
   - **D)** Outro serviço gerenciado de terceiros
   - **E)** Ainda não sei: o agente compara as opções no B17
   
   Observação: o Google Cloud IoT Core, antigo serviço gerenciado de IoT do Google, foi descontinuado. Se a preferência for Google, o caminho é um broker próprio hospedado no Google Cloud (opção B).
6. Haverá mais de um broker (por exemplo, um por região ou por cliente)?

### 3.3 Portas e segurança de transporte
7. Quais portas serão usadas?
   - **1883**: MQTT sem criptografia. Aceitável apenas em rede isolada de laboratório.
   - **8883**: MQTT com TLS. Recomendada para produção.
   - **443** (MQTT sobre WebSocket): útil para navegadores e apps, se necessário.
   
   Quais portas são necessárias em cada ambiente (desenvolvimento, homologação, produção)?
8. Os dispositivos estão em rede local, com NAT, ou diretamente na internet? Há firewall ou VPN entre o campo e a nuvem?

### 3.4 Chaves e autenticação dos dispositivos
9. Como os dispositivos vão se autenticar?
   - **A)** Certificado X.509 por dispositivo (recomendado para produção)
   - **B)** Usuário e senha por dispositivo
   - **C)** Token ou chave de API
   - **D)** Combinação
10. Quem gera as chaves e certificados? Como são entregues ao dispositivo na instalação?
11. Com que frequência as chaves são rotacionadas? Como uma chave comprometida é revogada?
12. Onde as chaves do sistema são guardadas? (cofre de segredos, variáveis de ambiente, outro). Nunca em código versionado.
13. Cada dispositivo deve ter permissão apenas para os próprios tópicos? (controle de acesso por tópico)

### 3.5 Protocolo de comunicação
14. Para cada tipo de fluxo, qual protocolo será usado?
    | Fluxo | MQTT | REST (HTTP/HTTPS) | TCP puro |
    |---|---|---|---|
    | Telemetria (dispositivo envia leituras) | | | |
    | Comando (sistema envia ação ao dispositivo) | | | |
    | Configuração do dispositivo | | | |
    | Atualização de firmware | | | |
    | Consulta de histórico (usuário ou app) | | | |
15. Qual a frequência de envio de telemetria e o tamanho máximo do payload?
16. Qual a garantia de entrega necessária? (QoS 0: no máximo uma vez; QoS 1: ao menos uma vez; QoS 2: exatamente uma vez)
17. Há necessidade de mensagens retidas (último valor conhecido disponível para quem se conectar depois)?

**Saídas:** inventário de dispositivos, decisão de broker, decisão de portas por ambiente, método de autenticação, matriz de protocolo por fluxo, regras de tópicos e QoS, e **proposta de arquitetura de comunicação** elaborada pelo agente, com confirmação do usuário.

## B4 — Funcionalidades principais (*)
**Objetivo:** listar o que o sistema faz.
Para cada funcionalidade citada pelo usuário, pergunte:
1. Quem usa?
2. Qual é a entrada e qual é a saída esperada?
3. Qual a prioridade? (Must, Should, Could, Won't)

**Saídas:** lista de requisitos funcionais com prioridade MoSCoW.

## B5 — Usuários, perfis, acessos e sessões (*)
**Objetivo:** definir quem pode fazer o quê e como as sessões funcionam.
1. Quais perfis de usuário existem? (ex.: cliente, suporte, integrador, administrador)
2. Qual a hierarquia da organização? (país, estado, cidade, empresa, filial, unidade)
3. Um usuário pode ver dados de várias unidades? Com que restrições?
4. Deve haver regras de acesso por horário ou dia da semana?
5. **Sessão única:** pode haver mais de uma sessão ativa por usuário?
6. Se a sessão for restrita, a regra vale **por usuário** ou **por empresa**? Há troca de turno com o mesmo acesso?
7. Quais ações são exclusivas de cada perfil?

**Saídas:** matriz de perfis e permissões, modelo de organização (multi-tenant), regra de sessão.

## B6 — Interfaces, telas e painéis (*)
**Objetivo:** definir os canais visuais e como os painéis serão construídos.
1. Quais interfaces são necessárias? (web, Android, iOS, tablet, desktop)
2. O usuário deve poder criar telas e painéis personalizados? Com quais elementos? (gráficos, tabelas, indicadores, mapas)
3. Deve haver arrastar e soltar (drag and drop) para montar os painéis?
4. **Estratégia de painéis.** Escolha uma opção:
   - **A)** Integrar com **Grafana Server** (instalação própria do Grafana).
   - **B)** Integrar com **Amazon Managed Grafana** (Grafana gerenciado na AWS).
   - **C)** Construir um **sistema próprio de dashboards** no frontend, com framework escolhido no B17.
   - **D)** Combinação: painéis próprios para o cliente final e Grafana para a equipe técnica.
5. Qual o idioma? Há requisitos de acessibilidade?
6. Existe identidade visual (logotipo, cores) a seguir?

**Saídas:** lista de telas, estratégia de painéis (A, B, C ou D), requisitos de responsividade e de personalização.

## B7 — Aplicativos móveis (*)
**Objetivo:** definir se haverá app, em quais plataformas e como será construído.

### 7.1 Escopo
1. Haverá aplicativo? Para quais plataformas? (Android, iOS, tablet, nenhum: apenas web responsivo)
2. Quais telas o app terá? (painel, alarmes, chat, comandos, histórico, configurações)
3. O app precisa funcionar **offline**? Se sim, quais dados ficam disponíveis sem conexão?
4. Haverá **notificações push** para alarmes?
5. Haverá **login por biometria** (impressão digital ou reconhecimento facial)?

### 7.2 Tecnologia
6. Como o app será construído?
   - **A)** Código **nativo e separado**: Kotlin para Android e Swift para iOS. Maior desempenho e acesso total ao sistema, mas dois códigos para manter.
   - **B)** **Framework multiplataforma**: um código para Android e iOS (ex.: React Native, Flutter, .NET MAUI, Kotlin Multiplatform). Menos esforço de manutenção, com algumas limitações de acesso a recursos nativos.
   - **C)** **PWA** (aplicativo web instalável), sem publicação em loja. Mais simples, com recursos limitados em iOS.
   - **D)** Não sei: o agente recomenda no B17.
7. Se houver framework, há preferência ou restrição? (o agente considera isso no B17)
8. Quais versões mínimas de Android e iOS devem ser suportadas?
9. A equipe já tem experiência com alguma dessas tecnologias?

### 7.3 Publicação e operação
10. Quem terá as contas de desenvolvedor da Google Play e da Apple Developer? (há taxa anual da Apple)
11. Haverá distribuição interna (corporativa, sem loja pública)? Há gestão de dispositivos (MDM)?
12. Haverá atualizações forçadas ou obrigatórias?

**Saídas:** decisão de escopo mobile, estratégia de tecnologia (A, B, C ou D), requisitos de offline, push e biometria, lista de telas móveis, plano de publicação.

## B8 — Comunicação e chat agêntico (*)
**Objetivo:** definir se haverá conversa com o sistema por mensagens.
1. Haverá interação por chat? Por quais canais? (WhatsApp Business, Telegram, chat dentro do app/site, e-mail, SMS, nenhum)
2. O chat deve apenas **responder perguntas** sobre os dados, ou também **executar ações**? (ex.: "desligar bomba 2")
3. Quais ações podem ser executadas pelo chat? Quais exigem confirmação humana?
4. O chat deve identificar o usuário antes de responder? Com qual método?
5. Qual o idioma das respostas do agente?

**Saídas:** requisitos de canais, catálogo de intenções do agente, política de confirmação de ações críticas.

## B9 — Automação, lambdas, alarmes e análise (*)
**Objetivo:** definir o que o sistema decide e avisa sozinho, e quais verificações rodam antes de cada ação.
1. Existem ações que devem ser verificadas **antes** de executar qualquer comando? (ex.: antes de "ligar luz", checar permissão do usuário, horário e estado do dispositivo)
2. Para cada tipo de ação, quais regras de pré-execução (lambda) são obrigatórias?
3. Quais alarmes e alertas são necessários? Quem deve ser notificado e por qual canal?
4. Há regras de disparo? (limite, variação, ausência de dados, tempo sem comunicação)
5. Deve haver análises automáticas? (tendências, previsões, detecção de anomalias)
6. Quais decisões podem ser automatizadas e quais precisam de aprovação humana?

**Saídas:** catálogo de lambdas por ação, catálogo de regras de alarme, política de automação.

## B10 — Segurança e autenticação (*)
**Objetivo:** definir como o acesso é protegido.
1. Quais métodos de autenticação? (senha, e-mail, dois fatores, SSO)
2. Como as senhas serão armazenadas? Recomendado: hash com algoritmo de custo (Argon2id ou bcrypt) e salt individual por senha. Há objeção?
3. Quais políticas de senha (tamanho, expiração, bloqueio após tentativas)?
4. Qual o fluxo de recuperação de senha? (link por e-mail com validade curta)
5. Quanto tempo dura uma sessão? Há expiração por inatividade?
6. Há necessidade de limitar requisições por IP ou por usuário (proteção contra abuso)?
7. Há necessidade de criptografia de comunicação entre todos os serviços internos, ou apenas na entrada do sistema?
8. Como os segredos do sistema (chaves, senhas de serviços) serão guardados e rotacionados?

**Saídas:** requisitos de autenticação, política de senha, política de sessão, política de segredos.

## B11 — Banco de dados e proteção de dados (*)
**Objetivo:** definir como os dados são guardados, isolados, criptografados e protegidos conforme a LGPD.

### 11.1 Banco e isolamento
1. Qual banco de dados será usado? (PostgreSQL gerenciado, como o Supabase, ou PostgreSQL próprio)
2. Como os dados de cada cliente serão isolados?
   - **A)** Um banco com coluna de identificação da empresa em cada tabela, protegido por **RLS** (Row Level Security).
   - **B)** Um schema por cliente.
   - **C)** Um banco por cliente.
   - **D)** Não sei: o agente recomenda no B17.
3. O RLS deve filtrar por empresa, por filial e por perfil? Quais exceções existem (ex.: suporte da plataforma acessa todos os clientes)?
4. Os serviços da aplicação acessam o banco com usuários separados, com o mínimo de permissões necessárias?

### 11.2 Criptografia
5. A criptografia em repouso do provedor é suficiente, ou há campos que devem ser criptografados também na aplicação (ex.: CPF, e-mail, telefone, dados de integração)?
6. Quem controla as chaves de criptografia? (provedor, cofre de segredos, equipe interna)
7. A criptografia em trânsito (TLS) é obrigatória em todos os trechos, inclusive banco e broker?

### 11.3 Mascaramento
8. Quais dados devem aparecer mascarados e para quais perfis? (ex.: CPF mostrado como `***.456.789-**` para suporte; dado completo apenas para administrador)
9. O mascaramento vale também para logs, exportações e auditoria?
10. Dados mascarados podem ser revelados por ação explícita, com registro na auditoria?

### 11.4 LGPD
11. Quais dados pessoais o sistema trata? (nome, e-mail, telefone, CPF, IP, localização, outros)
12. Qual a base legal do tratamento? (execução de contrato, legítimo interesse, obrigação legal, outra). Se não souber, o agente registra como pendência para revisão jurídica.
13. O sistema deve permitir ao titular acessar, corrigir, exportar e excluir seus dados? Como o pedido chega?
14. Qual o prazo de retenção de cada tipo de dado pessoal? Após esse prazo, o dado é excluído ou anonimizado?
15. Quem é o **encarregado de dados (DPO)** da organização?
16. Há necessidade de registro das operações de tratamento de dados pessoais?
17. Há procedimento de resposta a incidentes de segurança com dados pessoais?
18. Há contratos com terceiros que tratam dados (provedores de nuvem, serviços de e-mail, WhatsApp, Telegram)? Eles precisam de cláusulas de proteção de dados.

### 11.5 Backup e recuperação
19. Qual a frequência de backup e por quanto tempo ele é mantido?
20. Os backups são criptografados? Há teste periódico de restauração?

**Saídas:** modelo de isolamento (A, B, C ou D), política de RLS, lista de campos criptografados na aplicação, matriz de mascaramento por perfil, inventário de dados pessoais, políticas de retenção e exclusão, política de backup, pendências jurídicas.

## B12 — Auditoria de alterações sensíveis (*)
**Objetivo:** definir o que é auditado e verificar se a auditoria cabe na escala prevista.
1. Quais ações são sensíveis e devem ser auditadas? Marque as que se aplicam: alteração de usuário, de permissão, de regra de acesso, de configuração do sistema, de configuração do CLP, de alarme, de dashboard, envio de comando a dispositivo, login, logout, falha de login.
2. Para cada alteração, o sistema registra: quem fez, quando, IP, navegador, valor anterior e valor novo? Há outras informações necessárias?
3. Quem pode **consultar** a auditoria? Existe perfil auditor separado do administrador?
4. Os registros podem ser **alterados ou apagados**? (recomendado: não, com registros imutáveis)
5. Por quanto tempo a auditoria é mantida?
6. **Dimensionamento:** quantas alterações sensíveis ocorrem por dia, por empresa e no total? Isso define se a auditoria pode ficar no mesmo banco ou precisa de armazenamento separado.
7. Há necessidade de exportar a auditoria? (PDF, CSV, integração com outro sistema)
8. Dados mascarados (B11.3) aparecem na auditoria? Qual a regra?

**Saídas:** lista de eventos auditados, política de consulta, política de imutabilidade, retenção, estimativa de volume e decisão de armazenamento.

## B13 — Disponibilidade, infraestrutura e operação (*)
**Objetivo:** definir onde e como o sistema roda.
1. Onde o sistema vai rodar? (nuvem, servidor local, híbrido)
2. Qual o nível de disponibilidade esperado? (ex.: 99% ou 99,9%)
3. Qual o tempo aceitável para restaurar o sistema após falha? (RTO) e quanto dado pode ser perdido? (RPO)
4. Há ambientes separados? (desenvolvimento, homologação, produção)
5. Há restrições de hardware ou de rede no local de instalação?
6. Como serão feitos monitoramento, alertas técnicos e logs de aplicação?
7. Há serviços já contratados? (banco, nuvem, mensageria, Grafana, broker)

**Saídas:** documento de infraestrutura, requisitos não funcionais de disponibilidade.

## B14 — Restrições técnicas e preferências (*)
**Objetivo:** registrar o que é obrigatório, proibido ou preferido. **A escolha final de tecnologia não acontece aqui**, e sim no B17.
1. Há linguagens obrigatórias ou proibidas?
2. Há regras sobre frameworks ou bibliotecas? Registre a regra como foi dita; o agente verifica conflitos no B17.
3. Há restrição de licença de software ou custo de serviços em nuvem?
4. Quais tecnologias a equipe já domina?
5. Há serviços que já devem ser usados?

**Saídas:** lista de restrições e preferências, marcadas como "obrigatória", "preferência" ou "a confirmar".

## B15 — Equipe, processo e repositório (*)
**Objetivo:** definir como o trabalho será organizado.
1. Quantas pessoas trabalharão no projeto e em quais papéis?
2. Qual metodologia? (Scrum, Kanban, outra)
3. Qual a duração desejada de cada sprint? (1 a 4 semanas)
4. Qual a URL do repositório Git e a estratégia de branches?
5. Qual ferramenta de gestão será usada? (ou manter em arquivos Markdown)

**Saídas:** processo de trabalho, cadência de sprints, estratégia de branches.

## B16 — Métricas de sucesso e riscos
**Objetivo:** medir o sucesso e antecipar problemas.
1. Como saberemos que o projeto deu certo? (indicadores e metas)
2. Quais riscos você já enxerga? (técnicos, de prazo, de adoção, de dependências externas)
3. Há dependências de terceiros? (fornecedores de hardware, APIs, certificações)

**Saídas:** indicadores de sucesso, registro de riscos.

## B17 — Sugestão de arquitetura e stack (*)
**Objetivo:** recomendar tecnologias e arquitetura com base em todos os blocos anteriores.
1. O agente cruza as respostas de B3 (comunicação IoT), B6 (painéis), B7 (apps), B10 e B11 (segurança e dados), B12 (auditoria), B13 (infraestrutura) e B14 (restrições).
2. O agente apresenta, para cada camada, 2 a 3 opções com justificativa, e aponta conflitos com as restrições declaradas:
   - backend (incluindo a regra sobre frameworks);
   - frontend e painéis;
   - apps móveis;
   - banco de dados e isolamento;
   - broker e comunicação IoT.
3. O usuário escolhe ou pede outras opções.
4. Cada decisão é registrada em ADR.

**Saídas:** ADRs de backend, frontend, apps, banco e comunicação IoT.

---

## Checklist de encerramento da entrevista

- [ ] Nome oficial do projeto definido no B0
- [ ] Blocos obrigatórios (*) respondidos ou marcados como PENDENTE
- [ ] Comunicação IoT confirmada no B3 (broker, portas, autenticação, protocolo por fluxo)
- [ ] Estratégia de app definida no B7
- [ ] Isolamento, criptografia, mascaramento e LGPD definidos no B11
- [ ] Estratégia de painéis escolhida (A, B, C ou D) no B6
- [ ] Regras de sessão e auditoria definidas no B5 e B12, com estimativa de volume
- [ ] Conflitos entre respostas resolvidos ou registrados
- [ ] Stack e arquitetura decididas no B17 e registradas em ADR
- [ ] Estado do projeto atualizado

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 1.2.1 | Adicionada a nota sobre os documentos de sistema que não existem neste repositório e a tabela de histórico |
| 15/10/2026 | 1.2.0 | Versão anterior à tabela de histórico |
