# tools/ — ferramentas de apoio

> **Criado em:** 02/11/2026 · **Última modificação:** 02/11/2026 · **Versão:** 0.2.0

Scripts de apoio ao desenvolvimento, sem código embarcado:

- `verifica_padrao.py`: verifica função de até 40 linhas, arquivo de até 400, cabeçalho de arquivo e ausência de alocação dinâmica (FW-008). Sem argumentos, verifica `src/`, `lib/` e `include/`. Sai com 1 se houver violação.
- `gera_dicionario.py`: gera o dicionário de funções a partir do XML do Doxygen (FW-004). Com `--check`, só confere.
- `test_verifica_padrao.py` e `test_gera_dicionario.py`: testes das duas ferramentas.
- `modelos/`: modelos de arquivo C novo (FW-005).
- Simulador Python: broker MQTT local, comportamento offline e reconexão (fase 7), ainda a criar.

Rodar os testes, na raiz do repositório:

```
python -m unittest discover -s tools -p "test_*.py"
```

Os scripts usam só a biblioteca padrão do Python. Não incluir artefatos de build aqui.
