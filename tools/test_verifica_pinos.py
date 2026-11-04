"""Testes do verificador de pinagem (FW-003).

Rodar: python -m unittest discover -s tools -p "test_*.py"

Criado em 02/11/2026. Última modificação: 02/11/2026. Versão: 0.1.0.
"""

import unittest

import verifica_pinos as vp

CORRETO = """
#define PINO_A_PORTA GPIOA
#define PINO_A_NUM   GPIO_PIN_1
#define PINO_B_PORTA GPIOB
#define PINO_B_NUM   GPIO_PIN_1
"""


class TesteVerificaPinos(unittest.TestCase):
    def test_pinagem_correta_nao_tem_problema(self):
        self.assertEqual(vp.verifica_texto("x.h", CORRETO), [])

    def test_mesmo_numero_em_portas_diferentes_nao_e_duplicado(self):
        # PA1 e PB1 são pinos físicos diferentes.
        self.assertEqual(vp.verifica_texto("x.h", CORRETO), [])

    def test_pino_duplicado_falha(self):
        texto = CORRETO + "#define PINO_C_PORTA GPIOA\n#define PINO_C_NUM GPIO_PIN_1\n"
        problemas = vp.verifica_texto("x.h", texto)
        self.assertEqual(len(problemas), 1)
        self.assertIn("PA1 usado por A e C", problemas[0])

    def test_falta_macro_num_falha(self):
        problemas = vp.verifica_texto("x.h", "#define PINO_A_PORTA GPIOA\n")
        self.assertEqual(len(problemas), 1)
        self.assertIn("sem a macro _NUM", problemas[0])

    def test_falta_macro_porta_falha(self):
        problemas = vp.verifica_texto("x.h", "#define PINO_A_NUM GPIO_PIN_3\n")
        self.assertEqual(len(problemas), 1)
        self.assertIn("sem a macro _PORTA", problemas[0])

    def test_pino_fora_da_faixa_falha(self):
        texto = "#define PINO_A_PORTA GPIOA\n#define PINO_A_NUM GPIO_PIN_16\n"
        problemas = vp.verifica_texto("x.h", texto)
        self.assertEqual(len(problemas), 1)
        self.assertIn("fora de 0 a 15", problemas[0])

    def test_nome_de_funcao_com_digitos_e_sublinhados(self):
        texto = (
            "#define PINO_ENTRADA_4_20MA_1_PORTA GPIOA\n"
            "#define PINO_ENTRADA_4_20MA_1_NUM   GPIO_PIN_1\n"
        )
        self.assertEqual(vp.verifica_texto("x.h", texto), [])


if __name__ == "__main__":
    unittest.main()
