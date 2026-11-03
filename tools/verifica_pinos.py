#!/usr/bin/env python3
"""Verifica os cabeçalhos de pinagem das placas (FW-003).

Para cada devices/<build>/pinos.h confere:
  - cada função tem o par PINO_<F>_PORTA e PINO_<F>_NUM;
  - o número do pino está entre 0 e 15;
  - nenhum pino físico é usado por duas funções.

Uso:
  python tools/verifica_pinos.py [pinos.h ...]

Sem argumentos, verifica devices/*/pinos.h.
Código de saída: 0 sem problemas, 1 com problemas.

Criado em 02/11/2026. Última modificação: 02/11/2026. Versão: 0.1.0.
"""

import re
import sys
from pathlib import Path

RE_PORTA = re.compile(r"#define\s+PINO_(\w+?)_PORTA\s+GPIO([A-Z])\b")
RE_NUM = re.compile(r"#define\s+PINO_(\w+?)_NUM\s+GPIO_PIN_(\d+)\b")
PINOS_POR_PORTA = 16


def le_pinagem(texto):
    """Lê as macros de pinagem. Retorna (portas, numeros) por função."""
    portas = dict(RE_PORTA.findall(texto))
    numeros = {nome: int(num) for nome, num in RE_NUM.findall(texto)}
    return portas, numeros


def verifica_texto(nome_arquivo, texto):
    """Retorna a lista de problemas de um cabeçalho de pinagem."""
    portas, numeros = le_pinagem(texto)
    problemas = []

    for funcao in sorted(set(portas) ^ set(numeros)):
        falta = "_NUM" if funcao in portas else "_PORTA"
        problemas.append(f"{nome_arquivo}: {funcao} sem a macro {falta}")

    usados = {}
    for funcao in sorted(set(portas) & set(numeros)):
        numero = numeros[funcao]
        if not 0 <= numero < PINOS_POR_PORTA:
            problemas.append(f"{nome_arquivo}: {funcao} com pino {numero} fora de 0 a 15")
        pino = f"P{portas[funcao]}{numero}"
        if pino in usados:
            problemas.append(f"{nome_arquivo}: {pino} usado por {usados[pino]} e {funcao}")
        usados[pino] = funcao

    return problemas


def main(argv):
    sys.stdout.reconfigure(encoding="utf-8")
    arquivos = [Path(a) for a in argv[1:]] or sorted(Path("devices").glob("*/pinos.h"))
    problemas = []
    for arquivo in arquivos:
        texto = arquivo.read_text(encoding="utf-8")
        problemas.extend(verifica_texto(str(arquivo), texto))
    for problema in problemas:
        print(problema)
    print(f"{len(arquivos)} cabeçalho(s) verificado(s), {len(problemas)} problema(s).")
    return 1 if problemas else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
