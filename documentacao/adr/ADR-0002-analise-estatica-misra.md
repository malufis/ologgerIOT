# ADR-0002 — Ferramenta de verificação estática MISRA C:2012

> **Criado em:** 04/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.0

**Estado:** Proposta, a decidir pelo responsável técnico.

## Contexto

- O padrão de código C usa a base MISRA C:2012 (`documentacao/padrao_codigo_c.md`). Os desvios devem ser registrados em `documentacao/desvios_misra.md`, que só é criado quando houver o primeiro desvio. No código há marcas `MISRA-DEV` em comentários (por exemplo, em `lib/adc/src/adc.c`).
- Hoje o projeto verifica: compilação com `-Wall -Wextra -Werror`, `tools/verifica_padrao.py` (tamanho de função e de arquivo, cabeçalho e alocação dinâmica) e Doxygen sem avisos. **Nenhuma ferramenta confere as regras MISRA.** O `FW-008` entregou só o verificador do projeto.

## Opções

1. **Sem ferramenta MISRA, só os avisos do compilador e o verificador do projeto.** Zero custo; a conformidade com MISRA fica por revisão (`firmware-reviewer`).
2. **cppcheck com o módulo MISRA.** Gratuito. A cobertura é parcial, e o módulo costuma exigir um arquivo com os textos das regras, que o usuário precisa obter do documento licenciado da MISRA. *A verificar na versão adotada.*
3. **Ferramenta comercial** (por exemplo, PC-lint Plus, Helix QAC, Polyspace, Coverity ou IAR C-STAT). Cobertura maior e relatórios de conformidade; tem custo de licença. *Preços e cobertura não foram levantados.*
4. **Analisador genérico (clang-tidy ou similar).** Sem conjunto MISRA oficial; ajuda em qualidade, não em conformidade.

## Consequências

- A decisão depende do requisito: se a conformidade MISRA precisa ser demonstrada a um cliente ou a uma certificação, a opção 3 é a que sustenta o relatório; se for boa prática interna, 1 ou 2 bastam.
- Qualquer ferramenta nova vira um item de backlog e uma etapa em `sprint-close` e `sprint-task`. Os desvios já marcados como `MISRA-DEV` passam a precisar de registro em `desvios_misra.md`.

## Decisão

Pendente. Recomendação do agente, a confirmar: opção 2 como verificação informativa (sem bloquear) no início, e reavaliar a opção 3 se houver exigência de conformidade.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.0 | Criado como proposta |
