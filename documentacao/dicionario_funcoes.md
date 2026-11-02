# Dicionário de funções — firmware oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.2.0

## Como manter

Toda função criada ou alterada no firmware entra neste dicionário no mesmo item em que foi feita. A skill `doc-sync` cuida disso ao fechar o item.

- **Até o FW-004 e o FW-008:** a tabela é mantida à mão pelo desenvolvedor ou pelo agente que criou a função. Cada linha é conferida contra o código.
- **Depois do FW-008:** a tabela é gerada por `tools/gera_dicionario.py`, a partir dos comentários Doxygen (FW-004). Não editar à mão. Corrija o comentário no código e regenere.

Cada entrada informa: nome, módulo, arquivo, finalidade, parâmetros, retorno e status da função (`ativa` ou `obsoleta`).

Função `static` (interna ao arquivo) não entra aqui. Só entra a função da interface pública do módulo, definida no `.h`.

## Funções

| Função | Módulo | Arquivo | Finalidade | Retorno | Status |
|---|---|---|---|---|---|
| — | — | — | Nenhuma função implementada ainda | — | — |

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 01/11/2026 | 0.1.0 | Criado esqueleto do dicionário de funções, gerado a partir do Doxygen |
| 02/11/2026 | 0.2.0 | Regra de manutenção antes da geração automática; tabela de funções com colunas definidas; só interface pública entra |
