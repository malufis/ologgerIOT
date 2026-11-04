"""Testes do verificador de padrão (FW-008).

Rodar: python -I -m unittest discover -s tools -p "test_*.py"

Criado em 02/11/2026. Última modificação: 02/11/2026. Versão: 0.1.0.
"""

import tempfile
import unittest
from pathlib import Path

import verifica_padrao as vp

CABECALHO = """/**
 * @file teste.c
 * @brief Arquivo de teste.
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */
"""


def funcao_com_linhas(corpo_linhas, estilo_allman=True):
    """Gera uma função cujo corpo tem a quantidade pedida de linhas de código."""
    corpo = "\n".join("  contador++;" for _ in range(corpo_linhas))
    if estilo_allman:
        return f"int fn(void)\n{{\n{corpo}\n  return 0;\n}}\n"
    return f"int fn(void) {{\n{corpo}\n  return 0;\n}}\n"


class TesteVerificaPadrao(unittest.TestCase):
    def verifica(self, conteudo, nome="teste.c"):
        with tempfile.TemporaryDirectory() as pasta:
            caminho = Path(pasta) / nome
            caminho.write_text(conteudo, encoding="utf-8")
            return vp.verifica_arquivo(caminho)

    def test_arquivo_correto_nao_tem_violacao(self):
        self.assertEqual(self.verifica(CABECALHO + funcao_com_linhas(5)), [])

    def test_funcao_com_exatamente_40_linhas_passa(self):
        # '{' + 37 de corpo + 'return' + '}' = 40 linhas.
        self.assertEqual(self.verifica(CABECALHO + funcao_com_linhas(37)), [])

    def test_funcao_com_41_linhas_falha(self):
        # '{' + 38 de corpo + 'return' + '}' = 41 linhas.
        violacoes = self.verifica(CABECALHO + funcao_com_linhas(38))
        self.assertEqual(len(violacoes), 1)
        self.assertIn("função com 41 linhas", violacoes[0])

    def test_funcao_acima_do_limite_falha(self):
        violacoes = self.verifica(CABECALHO + funcao_com_linhas(45))
        self.assertEqual(len(violacoes), 1)
        self.assertIn("função com", violacoes[0])

    def test_funcao_acima_do_limite_falha_com_chave_na_mesma_linha(self):
        violacoes = self.verifica(CABECALHO + funcao_com_linhas(45, estilo_allman=False))
        self.assertEqual(len(violacoes), 1)

    def test_arquivo_sem_cabecalho_falha(self):
        violacoes = self.verifica("int x;\n")
        self.assertEqual(len(violacoes), len(vp.CAMPOS_CABECALHO))

    def test_cabecalho_sem_versao_falha(self):
        sem_versao = CABECALHO.replace(" * @version 0.1.0\n", "")
        violacoes = self.verifica(sem_versao + "int x;\n")
        self.assertEqual(len(violacoes), 1)
        self.assertIn("@version", violacoes[0])

    def test_alocacao_dinamica_falha(self):
        codigo = CABECALHO + "void f(void)\n{\n  char *p = malloc(10);\n  free(p);\n}\n"
        violacoes = self.verifica(codigo)
        self.assertEqual(len(violacoes), 2)

    def test_alocacao_em_comentario_ou_texto_nao_conta(self):
        codigo = CABECALHO + '/* nao usar malloc( */\nconst char *m = "free(x)";\n'
        self.assertEqual(self.verifica(codigo), [])

    def test_arquivo_acima_de_400_linhas_falha(self):
        codigo = CABECALHO + "int a;\n" * 400
        violacoes = self.verifica(codigo)
        self.assertEqual(len(violacoes), 1)
        self.assertIn("limite 400", violacoes[0])

    def test_inicializador_e_controle_nao_sao_funcao(self):
        codigo = CABECALHO + (
            "static const int tabela[] = {\n" + "  1,\n" * 60 + "};\n"
        )
        self.assertEqual(self.verifica(codigo), [])

    def test_pasta_coleta_so_c_e_h(self):
        with tempfile.TemporaryDirectory() as pasta:
            (Path(pasta) / "a.c").write_text(CABECALHO)
            (Path(pasta) / "b.h").write_text(CABECALHO)
            (Path(pasta) / "c.txt").write_text("nada")
            self.assertEqual(len(vp.coleta_arquivos([pasta])), 2)


if __name__ == "__main__":
    unittest.main()
