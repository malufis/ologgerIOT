# Análise do Rascunho Original (chatInicial.txt)

> **Criado em:** 15/10/2026 · **Última modificação:** 04/11/2026 · **Versão:** 1.2.1

> **Nota de 04/11/2026:** os documentos de sistema citados abaixo (`documentacao/01_visao_do_projeto.md`, `02_requisitos.md`, `03_glossario.md`, `04_riscos_e_pendencias.md`, `05_arquitetura.md`, `apps_moveis.md`, `lgpd.md`, `seguranca_e_dados.md`, `adr/ADR-0001-backend.md`, `adr/ADR-0002-frontend.md` e `planejamento/ROADMAP.md`) pertencem ao kit genérico de inicialização do **sistema** e **não existem neste repositório do firmware**. As decisões do device estão em `sistema-init/STATUS_PROJETO.md` e em `documentacao/`. A pasta `documentacao/adr/` deste repositório trata das decisões de arquitetura do firmware, com outros assuntos.

## 1. Onde cada tema do rascunho foi tratado

| Tema do rascunho | Bloco ou documento |
|---|---|
| Documentação em .md como base do projeto | `prompt/PROMPT_MESTRE.md` (Etapas 2 e 4) |
| Git e versionamento | `skills/SKILLS.md` e B15 |
| Pastas `aprendizado/`, `teste_automatizado/`, `debug/` | Etapa 4 do prompt mestre e `agentes/ARQUITETURA_AGENTES.md` |
| Agentes especialistas (MQTT, IoT, telas dinâmicas) | `agentes/ARQUITETURA_AGENTES.md` |
| Dados do CLP, 4-20 mA, pulsos, temperatura, energia | B2 |
| MQTT, broker, portas 1883/8883, chaves, REST ou TCP | B3 |
| Estrutura de containers | B13 |
| Níveis de acesso e hierarquia país/estado/cidade/empresa | B5 |
| Regras de acesso por horário e dia da semana | B5 |
| Sessão única e troca de turno | B5 |
| Auditoria por usuário, IP e navegador | B12 |
| Autenticação, hashing com salt e recuperação de senha | B10 |
| Segurança do banco, RLS, criptografia e LGPD | B11 |
| Dashboards arrastar e soltar; Grafana | B6 |
| Chat por WhatsApp, Telegram e app | B8 |
| Lambda de pré-execução | B9 e glossário |
| Apps Android, iOS e tablet | B7 |
| Idioma pt-BR | Regra 1 do prompt mestre |

## 2. Itens repetidos consolidados

| Grupo | Itens originais | Requisito consolidado | Onde fica |
|---|---|---|---|
| Agentes especialistas e robôs de análise | 8 e 14 | Agentes especialistas com instrução, conhecimento e responsabilidades | `agentes/ARQUITETURA_AGENTES.md` |
| Apps móveis e responsividade | 10 e 21 | Interfaces web, Android, iOS e tablet, todas responsivas | B6 e B7 |
| Autenticação por e-mail | 20 (citada duas vezes) | Autenticação por e-mail com recuperação de senha | B10 |

## 3. Conflitos e decisões

| # | Item do rascunho | Resolução |
|---|---|---|
| 1 | Item 5 ("python puro, sem frameworks") x itens 7 (FastAPI) e 4 (React) | Registrado como restrição no B14. A escolha de tecnologia é feita no B17. |
| 2 | Item 2 ("lambda em python") | Definido como função de pré-execução. Ex.: "ligar luz". Tratado no B9. |
| 3 | Item 9 (lambdas do WhatsApp, Telegram e app) | Canais no B8. A lambda de pré-execução se aplica a todos os canais de entrada. |
| 4 | Item 19 (sessão única) | Por usuário ou por empresa, no B5. Auditoria no B12. |
| 5 | Item 17 (Grafana) | Quatro caminhos no B6. |
| 6 | Item 3 (Supabase) | Confirmado no B11 como serviço já escolhido ou não. |
| 7 | Itens 9 e 19 com "lambda" | Mantidos como lambda de pré-execução, sem ambiguidade. |

## 4. Lacunas encontradas e incorporadas

O rascunho não tratava de alguns temas que agora têm bloco próprio:

- **Comunicação IoT:** escolha de broker (AWS IoT Core, broker próprio ou on-premise), portas 1883 e 8883, autenticação de dispositivos e protocolo por fluxo (B3). Observação: o Google Cloud IoT Core foi descontinuado; a opção Google exige broker próprio hospedado no Google Cloud.
- **Segurança de dados:** RLS, criptografia em repouso e em trânsito, criptografia de campos na aplicação, mascaramento por perfil e LGPD (B11).
- **Apps móveis:** nativo, framework multiplataforma ou PWA, offline, push, biometria e publicação (B7).

## 5. Perguntas em aberto

Estão no `estado/STATUS_PROJETO.md` e são feitas nos blocos indicados. As mais críticas para a arquitetura são: broker e portas (B3), isolamento de dados (B11), sessão única (B5), estratégia de painéis (B6) e estratégia de app (B7).

## 6. Sugestão de evolução

- **Versão 1.3 do kit:** bloco de **telemetria e tipos de sensor** (faixas de 4-20 mA, contadores de pulso, conversão de escala, qualidade do sinal) e bloco de **calendário de eventos** (falhas, alterações de configuração, acesso remoto).
- **Skills específicas:** `padroes_iot_sensores`, `padroes_mqtt` e `seguranca_lgpd`, quando o projeto entrar na fase de arquitetura.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 1.2.1 | Adicionada a nota sobre os documentos de sistema que não existem neste repositório e a tabela de histórico |
| 15/10/2026 | 1.2.0 | Versão anterior à tabela de histórico |
