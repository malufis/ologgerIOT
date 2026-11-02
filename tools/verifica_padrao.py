#!/usr/bin/env python3
"""Verifica o padrão de código C do firmware oLoggerIOT (FW-008).

Regras verificadas (documentacao/padrao_codigo_c.md):
  - Função com no máximo 40 linhas, contadas de '{' a '}'.
  - Arquivo com no máximo 400 linhas.
  - Cabeçalho de arquivo com @file, @brief, @date criacao,
    @date modificacao e @version.
  - Sem alocação dinâmica: malloc, calloc, realloc e free.

Uso:
  python tools/verifica_padrao.py [caminho ...]

Sem argumentos, verifica src/, lib/ e include/. Aceita arquivos e pastas.
Saída: uma linha 'arquivo:linha: mensagem' por violação.
Código de saída: 0 sem violações, 1 com violações.

Criado em 02/11/2026. Última modificação: 02/11/2026. Versão: 0.1.0.
"""

import re
import sys
from pathlib import Path

LIMITE_LINHAS_FUNCAO = 40
LIMITE_LINHAS_ARQUIVO = 400
LINHAS_PROCURA_CABECALHO = 40
CAMPOS_CABECALHO = ("@file", "@brief", "@date criacao", "@date modificacao", "@version")
EXTENSOES = {".c", ".h"}
PASTAS_PADRAO = ("src", "lib", "include")

RE_ALOCACAO = re.compile(r"\b(malloc|calloc|realloc|free)\s*\(")
# Linha de assinatura de função: termina em ')' (chave na linha seguinte)
# ou em ') {' (chave na mesma linha). Exclui inicializadores com '='.
RE_ASSINATURA = re.compile(r"^[A-Za-z_][^;=]*\)\s*(\{\s*)?$")
PALAVRAS_CONTROLE = ("if", "for", "while", "switch", "else", "do", "return")


def remove_comentarios_e_textos(linhas):
    """Troca comentários e literais de texto por espaços, mantendo as linhas."""
    texto = "\n".join(linhas)

    def em_branco(trecho):
        return re.sub(r"[^\n]", " ", trecho.group(0))

    texto = re.sub(r"/\*.*?\*/", em_branco, texto, flags=re.DOTALL)
    texto = re.sub(r"//[^\n]*", em_branco, texto)
    texto = re.sub(r'"(\\.|[^"\\\n])*"', em_branco, texto)
    return texto.split("\n")


def verifica_cabecalho(linhas):
    """Retorna os campos de cabeçalho que faltam nas primeiras linhas."""
    inicio = "\n".join(linhas[:LINHAS_PROCURA_CABECALHO])
    return [campo for campo in CAMPOS_CABECALHO if campo not in inicio]


def abre_funcao(limpas, i):
    """Indica se a linha i abre o corpo de uma função e onde está o '{'."""
    linha = limpas[i]
    if not RE_ASSINATURA.match(linha) or linha.split("(")[0].split()[:1] == []:
        return None
    primeira = linha.split()[0]
    if primeira in PALAVRAS_CONTROLE or primeira.startswith(("typedef", "#")):
        return None
    if linha.rstrip().endswith("{"):
        return i
    if i + 1 < len(limpas) and limpas[i + 1].strip() == "{":
        return i + 1
    return None


def mede_funcoes(limpas):
    """Lista (linha_da_chave, tamanho) de cada função em coluna zero."""
    funcoes = []
    i = 0
    while i < len(limpas):
        chave = abre_funcao(limpas, i) if limpas[i][:1].isalpha() or limpas[i][:1] == "_" else None
        if chave is None:
            i += 1
            continue
        fim = chave
        while fim < len(limpas) and limpas[fim].rstrip() != "}":
            fim += 1
        funcoes.append((chave + 1, fim - chave + 1))
        i = fim + 1
    return funcoes


def verifica_arquivo(caminho):
    """Retorna a lista de violações de um arquivo."""
    violacoes = []
    linhas = caminho.read_text(encoding="utf-8", errors="replace").splitlines()
    nome = str(caminho)

    for campo in verifica_cabecalho(linhas):
        violacoes.append(f"{nome}:1: cabeçalho sem {campo}")

    if len(linhas) > LIMITE_LINHAS_ARQUIVO:
        violacoes.append(
            f"{nome}:{len(linhas)}: arquivo com {len(linhas)} linhas, "
            f"limite {LIMITE_LINHAS_ARQUIVO}"
        )

    limpas = remove_comentarios_e_textos(linhas)

    for numero, linha in enumerate(limpas, start=1):
        achado = RE_ALOCACAO.search(linha)
        if achado:
            violacoes.append(f"{nome}:{numero}: alocação dinâmica proibida: {achado.group(1)}")

    for numero, tamanho in mede_funcoes(limpas):
        if tamanho > LIMITE_LINHAS_FUNCAO:
            violacoes.append(
                f"{nome}:{numero}: função com {tamanho} linhas, limite {LIMITE_LINHAS_FUNCAO}"
            )

    return violacoes


def coleta_arquivos(entradas):
    """Expande arquivos e pastas em uma lista ordenada de arquivos C e H."""
    arquivos = []
    for entrada in entradas:
        caminho = Path(entrada)
        if caminho.is_dir():
            arquivos.extend(p for p in caminho.rglob("*") if p.suffix in EXTENSOES)
        elif caminho.suffix in EXTENSOES and caminho.exists():
            arquivos.append(caminho)
    return sorted(set(arquivos))


def main(argv):
    # Saída em UTF-8, para os acentos saírem corretos no console do Windows.
    sys.stdout.reconfigure(encoding="utf-8")
    entradas = argv[1:] or [p for p in PASTAS_PADRAO if Path(p).exists()]
    arquivos = coleta_arquivos(entradas)
    violacoes = []
    for arquivo in arquivos:
        violacoes.extend(verifica_arquivo(arquivo))

    for violacao in violacoes:
        print(violacao)
    print(f"{len(arquivos)} arquivo(s) verificado(s), {len(violacoes)} violação(ões).")
    return 1 if violacoes else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
