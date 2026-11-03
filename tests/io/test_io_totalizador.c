/**
 * @file test_io_totalizador.c
 * @brief Testes de unidade do totalizador de pulsos (FW-060).
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#include "io_totalizador.h"
#include "teste.h"

/* Armazém simulado. limite_gravacoes simula a queda de energia no meio de uma gravação. */
static uint32_t memoria[IO_TOTALIZADOR_PALAVRAS];
static int32_t limite_gravacoes;

static uint32_t armazem_le(uint8_t palavra)
{
  return memoria[palavra];
}

static void armazem_grava(uint8_t palavra, uint32_t valor)
{
  if (limite_gravacoes != 0)
  {
    memoria[palavra] = valor;
    if (limite_gravacoes > 0)
    {
      limite_gravacoes--;
    }
  }
}

static const io_totalizador_armazem_t armazem = {armazem_le, armazem_grava};

/* Simula um reset: zera a memória do módulo recarregando do armazém. */
static void reinicia_modulo(void)
{
  limite_gravacoes = -1;
  (void)io_totalizador_inicia(&armazem);
}

static void zera_armazem(void)
{
  uint8_t i;

  for (i = 0U; i < IO_TOTALIZADOR_PALAVRAS; i++)
  {
    memoria[i] = 0U;
  }
  limite_gravacoes = -1;
}

static void inicia_rejeita_parametro_invalido(void)
{
  io_totalizador_armazem_t sem_le = {NULL, armazem_grava};

  VERIFICA(io_totalizador_inicia(NULL) == IO_TOTALIZADOR_ERRO_PARAMETRO);
  VERIFICA(io_totalizador_inicia(&sem_le) == IO_TOTALIZADOR_ERRO_PARAMETRO);
}

static void canal_invalido_e_recusado(void)
{
  zera_armazem();
  reinicia_modulo();
  VERIFICA(io_totalizador_soma(0U, 1U) == IO_TOTALIZADOR_ERRO_PARAMETRO);
  VERIFICA(io_totalizador_soma(5U, 1U) == IO_TOTALIZADOR_ERRO_PARAMETRO);
  VERIFICA(io_totalizador_define(0U, 1U) == IO_TOTALIZADOR_ERRO_PARAMETRO);
  VERIFICA(io_totalizador_le(0U) == 0U);
}

static void armazem_zerado_comeca_em_zero_e_fica_marcado(void)
{
  zera_armazem();
  reinicia_modulo();
  VERIFICA(io_totalizador_le(1U) == 0U);
  VERIFICA(io_totalizador_corrompido(1U) == true);
  /* Na carga seguinte já está gravado e válido. */
  reinicia_modulo();
  VERIFICA(io_totalizador_corrompido(1U) == false);
}

static void total_soma_e_sobrevive_ao_reinicio(void)
{
  zera_armazem();
  reinicia_modulo();
  (void)io_totalizador_soma(1U, 5U);
  (void)io_totalizador_soma(1U, 7U);
  (void)io_totalizador_soma(3U, 2U);
  VERIFICA(io_totalizador_le(1U) == 12U);
  reinicia_modulo();
  VERIFICA(io_totalizador_le(1U) == 12U);
  VERIFICA(io_totalizador_le(2U) == 0U);
  VERIFICA(io_totalizador_le(3U) == 2U);
  VERIFICA(io_totalizador_corrompido(1U) == false);
}

static void total_volta_a_zero_como_hodometro(void)
{
  zera_armazem();
  reinicia_modulo();
  (void)io_totalizador_define(2U, 0xFFFFFFFEU);
  (void)io_totalizador_soma(2U, 3U);
  VERIFICA(io_totalizador_le(2U) == 1U);
  reinicia_modulo();
  VERIFICA(io_totalizador_le(2U) == 1U);
}

static void define_iguala_a_leitura_do_medidor(void)
{
  zera_armazem();
  reinicia_modulo();
  (void)io_totalizador_define(4U, 123456U);
  (void)io_totalizador_soma(4U, 1U);
  reinicia_modulo();
  VERIFICA(io_totalizador_le(4U) == 123457U);
}

static void queda_de_energia_em_qualquer_gravacao_nao_perde_o_total(void)
{
  /* Total 100 gravado; cai a energia depois de k das 4 palavras da próxima gravação. */
  int32_t k;

  for (k = 0; k <= 4; k++)
  {
    zera_armazem();
    reinicia_modulo();
    (void)io_totalizador_define(1U, 100U);
    limite_gravacoes = k;
    (void)io_totalizador_soma(1U, 1U);
    reinicia_modulo();
    VERIFICA((io_totalizador_le(1U) == 100U) || (io_totalizador_le(1U) == 101U));
    VERIFICA(io_totalizador_corrompido(1U) == false);
  }
}

static void queda_no_meio_da_gravacao_com_total_que_volta_a_zero(void)
{
  int32_t k;

  for (k = 0; k <= 4; k++)
  {
    zera_armazem();
    reinicia_modulo();
    (void)io_totalizador_define(1U, 0xFFFFFFFFU);
    limite_gravacoes = k;
    (void)io_totalizador_soma(1U, 1U);
    reinicia_modulo();
    VERIFICA((io_totalizador_le(1U) == 0xFFFFFFFFU) || (io_totalizador_le(1U) == 0U));
  }
}

static void copia_corrompida_usa_a_outra(void)
{
  zera_armazem();
  reinicia_modulo();
  (void)io_totalizador_define(1U, 500U);
  memoria[0] = 0x12345678U; /* Estraga o valor da primeira cópia. */
  reinicia_modulo();
  VERIFICA(io_totalizador_le(1U) == 500U);
  VERIFICA(io_totalizador_corrompido(1U) == false);
}

static void duas_copias_corrompidas_zeram_e_marcam(void)
{
  zera_armazem();
  reinicia_modulo();
  (void)io_totalizador_define(1U, 500U);
  memoria[0] = 0x11111111U;
  memoria[2] = 0x22222222U;
  reinicia_modulo();
  VERIFICA(io_totalizador_le(1U) == 0U);
  VERIFICA(io_totalizador_corrompido(1U) == true);
}

int main(void)
{
  EXECUTA(inicia_rejeita_parametro_invalido);
  EXECUTA(canal_invalido_e_recusado);
  EXECUTA(armazem_zerado_comeca_em_zero_e_fica_marcado);
  EXECUTA(total_soma_e_sobrevive_ao_reinicio);
  EXECUTA(total_volta_a_zero_como_hodometro);
  EXECUTA(define_iguala_a_leitura_do_medidor);
  EXECUTA(queda_de_energia_em_qualquer_gravacao_nao_perde_o_total);
  EXECUTA(queda_no_meio_da_gravacao_com_total_que_volta_a_zero);
  EXECUTA(copia_corrompida_usa_a_outra);
  EXECUTA(duas_copias_corrompidas_zeram_e_marcam);
  return RESULTADO();
}
