# Decisões de arquitetura (ADR) — firmware oLoggerIOT

> **Criado em:** 04/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.1

Cada ADR registra uma decisão técnica com contexto, opções e consequências. Um ADR começa como **Proposta** e só vira **Decidido** com a confirmação do responsável técnico. Os três primeiros foram escritos em 04/11/2026 a partir das pendências listadas em `documentacao/ESTADO_IMPLEMENTACAO.md`. O ADR-0003 foi decidido em 04/11/2026; os ADR-0001 e ADR-0002 seguem em proposta.

| ADR | Assunto | Estado |
|---|---|---|
| `ADR-0001-versao-freertos.md` | Versão do FreeRTOS nos dois builds STM32 | Proposta |
| `ADR-0002-analise-estatica-misra.md` | Ferramenta de verificação estática MISRA C:2012 | Proposta |
| `ADR-0003-atualizacao-e-protecao.md` | Atualização de firmware, espaço de flash e proteção do código e das credenciais | **Decidido em 04/11/2026** (imagem na flash externa e bootloader) |

## Modelo

```
# ADR-NNNN — Título
Estado: Proposta | Decidido em dd/mm/aaaa | Substituído por ADR-NNNN
## Contexto
## Opções
## Consequências
## Decisão
```

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.1 | ADR-0003 decidido em 04/11/2026 |
| 04/11/2026 | 0.1.0 | Criado com três ADRs em estado de proposta |
