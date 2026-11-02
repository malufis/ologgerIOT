# Dicionário de funções — firmware oLoggerIOT

> **Criado em:** 01/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.2.0

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
| — | — | — | Nenhuma função pública implementada ainda | — | — |
<!-- DICIONARIO:FIM -->

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 01/11/2026 | 0.1.0 | Criado esqueleto do dicionário de funções, gerado a partir do Doxygen |
| 02/11/2026 | 0.2.0 | Regra de manutenção antes da geração automática; tabela de funções com colunas definidas; só interface pública entra |
