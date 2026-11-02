"""Testes do gerador do dicionário de funções (FW-004).

Rodar: python -m unittest discover -s tools -p "test_*.py"

Criado em 02/11/2026. Última modificação: 02/11/2026. Versão: 0.1.0.
"""

import tempfile
import unittest
from pathlib import Path

import gera_dicionario as gd

# XML no formato real do Doxygen, reduzido ao necessário.
XML_HEADER = """<?xml version='1.0' encoding='UTF-8'?>
<doxygen>
  <compounddef id="modelo_8h" kind="file" language="C++">
    <compoundname>modelo.h</compoundname>
    <sectiondef kind="func">
      <memberdef kind="function" id="a" prot="public" static="no">
        <type>int32_t</type>
        <argsstring>(uint32_t entrada, uint32_t *saida)</argsstring>
        <name>modelo_executa</name>
        <briefdescription><para>Resumo da função. </para></briefdescription>
        <detaileddescription><para>
          <simplesect kind="return"><para>0 em sucesso | negativo em erro. </para></simplesect>
        </para></detaileddescription>
        <location file="lib/modelo/include/modelo.h" line="30"/>
      </memberdef>
      <memberdef kind="function" id="b" prot="public" static="yes">
        <type>void</type><name>interna</name>
        <briefdescription><para>Não deve aparecer.</para></briefdescription>
        <detaileddescription/>
        <location file="lib/modelo/include/modelo.h" line="40"/>
      </memberdef>
    </sectiondef>
    <location file="lib/modelo/include/modelo.h"/>
  </compounddef>
</doxygen>
"""

XML_FONTE = XML_HEADER.replace("modelo.h", "modelo.c").replace("_8h", "_8c")

DOC = f"""# Dicionário

{gd.MARCA_INICIO}
tabela antiga
{gd.MARCA_FIM}

## Histórico
"""


class TesteGeraDicionario(unittest.TestCase):
    def xml_em_pasta(self, pasta, nome, conteudo):
        (Path(pasta) / nome).write_text(conteudo, encoding="utf-8")

    def test_le_funcao_publica_do_header(self):
        with tempfile.TemporaryDirectory() as pasta:
            self.xml_em_pasta(pasta, "modelo_8h.xml", XML_HEADER)
            funcoes = gd.le_funcoes(pasta)
        self.assertEqual(len(funcoes), 1)
        f = funcoes[0]
        self.assertEqual(f["nome"], "modelo_executa")
        self.assertEqual(f["modulo"], "modelo")
        self.assertEqual(f["arquivo"], "lib/modelo/include/modelo.h")
        self.assertEqual(f["finalidade"], "Resumo da função.")
        self.assertEqual(f["retorno"], "0 em sucesso | negativo em erro.")
        self.assertEqual(f["status"], "ativa")

    def test_funcao_estatica_nao_entra(self):
        with tempfile.TemporaryDirectory() as pasta:
            self.xml_em_pasta(pasta, "modelo_8h.xml", XML_HEADER)
            nomes = [f["nome"] for f in gd.le_funcoes(pasta)]
        self.assertNotIn("interna", nomes)

    def test_arquivo_c_nao_entra(self):
        with tempfile.TemporaryDirectory() as pasta:
            self.xml_em_pasta(pasta, "modelo_8c.xml", XML_FONTE)
            self.assertEqual(gd.le_funcoes(pasta), [])

    def test_tabela_escapa_barra_vertical(self):
        with tempfile.TemporaryDirectory() as pasta:
            self.xml_em_pasta(pasta, "modelo_8h.xml", XML_HEADER)
            tabela = gd.monta_tabela(gd.le_funcoes(pasta))
        self.assertIn("0 em sucesso \\| negativo em erro.", tabela)
        self.assertIn("`modelo_executa`", tabela)

    def test_tabela_sem_funcoes_tem_linha_vazia(self):
        self.assertIn(gd.LINHA_VAZIA, gd.monta_tabela([]))

    def test_aplica_troca_so_entre_marcadores(self):
        novo = gd.aplica_no_documento(DOC, "NOVA TABELA")
        self.assertIn("NOVA TABELA", novo)
        self.assertNotIn("tabela antiga", novo)
        self.assertTrue(novo.startswith("# Dicionário"))
        self.assertTrue(novo.rstrip().endswith("## Histórico"))

    def test_sem_marcadores_levanta_erro(self):
        with self.assertRaises(ValueError):
            gd.aplica_no_documento("# sem marcas\n", "x")

    def test_check_detecta_documento_desatualizado(self):
        with tempfile.TemporaryDirectory() as pasta:
            xml = Path(pasta) / "xml"
            xml.mkdir()
            self.xml_em_pasta(xml, "modelo_8h.xml", XML_HEADER)
            doc = Path(pasta) / "dic.md"
            doc.write_text(DOC, encoding="utf-8")
            args = ["x", "--xml", str(xml), "--doc", str(doc), "--check"]
            self.assertEqual(gd.main(args), 1)
            self.assertEqual(gd.main(["x", "--xml", str(xml), "--doc", str(doc)]), 0)
            self.assertEqual(gd.main(args), 0)


if __name__ == "__main__":
    unittest.main()
