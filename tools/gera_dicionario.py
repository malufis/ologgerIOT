#!/usr/bin/env python3
"""Gera o dicionário de funções a partir do XML do Doxygen (FW-004).

Só entram as funções da interface pública, declaradas em arquivos .h e não
estáticas. A tabela é escrita em documentacao/dicionario_funcoes.md, entre os
marcadores DICIONARIO:INICIO e DICIONARIO:FIM. O resto do documento não muda.

Uso:
  doxygen Doxyfile
  python tools/gera_dicionario.py [--xml PASTA] [--doc ARQUIVO] [--check]

--check não escreve: sai com código 1 se o dicionário estiver desatualizado.

Criado em 02/11/2026. Última modificação: 02/11/2026. Versão: 0.1.0.
"""

import argparse
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

MARCA_INICIO = "<!-- DICIONARIO:INICIO -->"
MARCA_FIM = "<!-- DICIONARIO:FIM -->"
CABECALHO_TABELA = (
    "| Função | Módulo | Arquivo | Finalidade | Retorno | Status |\n"
    "|---|---|---|---|---|---|"
)
LINHA_VAZIA = "| — | — | — | Nenhuma função pública implementada ainda | — | — |"


def texto(no):
    """Texto de um nó XML, com os espaços normalizados."""
    if no is None:
        return ""
    return " ".join("".join(no.itertext()).split())


def retorno_da_funcao(membro):
    """Descrição do @return, ou o tipo de retorno quando não há descrição."""
    for secao in membro.iter("simplesect"):
        if secao.get("kind") == "return":
            return texto(secao)
    return texto(membro.find("type"))


def funcao_publica(membro):
    """Indica se o membro é função não estática."""
    return membro.get("kind") == "function" and membro.get("static") != "yes"


def le_arquivo_xml(caminho):
    """Lê as funções públicas de um arquivo .h do XML do Doxygen."""
    raiz = ET.parse(caminho).getroot()
    funcoes = []
    for comp in raiz.iter("compounddef"):
        if comp.get("kind") != "file":
            continue
        local = comp.find("location")
        arquivo = local.get("file", "") if local is not None else ""
        if not arquivo.endswith(".h"):
            continue
        modulo = Path(arquivo).stem
        for membro in comp.iter("memberdef"):
            if funcao_publica(membro):
                funcoes.append({
                    "nome": texto(membro.find("name")),
                    "modulo": modulo,
                    "arquivo": arquivo.replace("\\", "/"),
                    "finalidade": texto(membro.find("briefdescription")),
                    "retorno": retorno_da_funcao(membro),
                    "status": "obsoleta" if membro.find(".//xrefsect") is not None else "ativa",
                })
    return funcoes


def le_funcoes(pasta_xml):
    """Lê todas as funções públicas da pasta de XML, em ordem estável."""
    funcoes = []
    for caminho in sorted(Path(pasta_xml).glob("*_8h.xml")):
        funcoes.extend(le_arquivo_xml(caminho))
    return sorted(funcoes, key=lambda f: (f["modulo"], f["nome"]))


def escapa(valor):
    """Escapa o separador de coluna da tabela."""
    return valor.replace("|", "\\|")


def monta_tabela(funcoes):
    """Monta a tabela Markdown das funções."""
    linhas = CABECALHO_TABELA.split("\n")
    if not funcoes:
        linhas.append(LINHA_VAZIA)
    for f in funcoes:
        linhas.append(
            f"| `{f['nome']}` | {f['modulo']} | `{f['arquivo']}` | "
            f"{escapa(f['finalidade'])} | {escapa(f['retorno'])} | {f['status']} |"
        )
    return "\n".join(linhas)


def aplica_no_documento(conteudo, tabela):
    """Troca o trecho entre os marcadores pela tabela gerada."""
    inicio = conteudo.find(MARCA_INICIO)
    fim = conteudo.find(MARCA_FIM)
    if inicio < 0 or fim < 0 or fim < inicio:
        raise ValueError(f"marcadores {MARCA_INICIO} e {MARCA_FIM} não encontrados")
    return conteudo[: inicio + len(MARCA_INICIO)] + "\n" + tabela + "\n" + conteudo[fim:]


def main(argv):
    sys.stdout.reconfigure(encoding="utf-8")
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--xml", default="build/doxygen/xml", help="pasta do XML do Doxygen")
    ap.add_argument("--doc", default="documentacao/dicionario_funcoes.md")
    ap.add_argument("--check", action="store_true", help="só confere, sem escrever")
    args = ap.parse_args(argv[1:])

    if not Path(args.xml).is_dir():
        print(f"Pasta de XML não encontrada: {args.xml}. Rode 'doxygen Doxyfile' antes.")
        return 2

    funcoes = le_funcoes(args.xml)
    doc = Path(args.doc)
    atual = doc.read_text(encoding="utf-8")
    novo = aplica_no_documento(atual, monta_tabela(funcoes))

    if args.check:
        if novo != atual:
            print(f"{args.doc} está desatualizado. Rode tools/gera_dicionario.py.")
            return 1
        print(f"{args.doc} está atualizado ({len(funcoes)} função(ões)).")
        return 0

    doc.write_text(novo, encoding="utf-8")
    print(f"{args.doc} atualizado com {len(funcoes)} função(ões).")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
