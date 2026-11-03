/**
 * @file test_io_filtro.c
 * @brief Testes de unidade do filtro de ruído da entrada digital (FW-053).
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#include "io_filtro.h"
#include "teste.h"

static void inicia_rejeita_parametro_invalido(void)
{
  io_filtro_t filtro;

  VERIFICA(io_filtro_inicia(NULL, 3U, false) == IO_FILTRO_ERRO_PARAMETRO);
  VERIFICA(io_filtro_inicia(&filtro, 0U, false) == IO_FILTRO_ERRO_PARAMETRO);
}

static void inicia_guarda_estado_inicial(void)
{
  io_filtro_t filtro;

  VERIFICA(io_filtro_inicia(&filtro, 3U, true) == IO_FILTRO_OK);
  VERIFICA(io_filtro_atualiza(&filtro, true) == true);
  VERIFICA(io_filtro_inicia(&filtro, 3U, false) == IO_FILTRO_OK);
  VERIFICA(io_filtro_atualiza(&filtro, false) == false);
}

static void mudanca_so_aceita_apos_amostras_estaveis(void)
{
  io_filtro_t filtro;

  (void)io_filtro_inicia(&filtro, 3U, false);
  VERIFICA(io_filtro_atualiza(&filtro, true) == false);
  VERIFICA(io_filtro_atualiza(&filtro, true) == false);
  VERIFICA(io_filtro_atualiza(&filtro, true) == true);
}

static void mudanca_para_desligada_tambem_passa_pelo_filtro(void)
{
  io_filtro_t filtro;

  (void)io_filtro_inicia(&filtro, 2U, true);
  VERIFICA(io_filtro_atualiza(&filtro, false) == true);
  VERIFICA(io_filtro_atualiza(&filtro, false) == false);
}

static void pulso_curto_e_descartado(void)
{
  /* Dois níveis altos seguidos com filtro de três amostras: nada muda. */
  io_filtro_t filtro;

  (void)io_filtro_inicia(&filtro, 3U, false);
  VERIFICA(io_filtro_atualiza(&filtro, true) == false);
  VERIFICA(io_filtro_atualiza(&filtro, true) == false);
  VERIFICA(io_filtro_atualiza(&filtro, false) == false);
  VERIFICA(io_filtro_atualiza(&filtro, false) == false);
}

static void ruido_alternado_nao_muda_estado(void)
{
  io_filtro_t filtro;
  uint16_t i;

  (void)io_filtro_inicia(&filtro, 2U, false);
  for (i = 0U; i < 100U; i++)
  {
    VERIFICA(io_filtro_atualiza(&filtro, (i % 2U) == 0U) == false);
  }
}

static void contagem_reinicia_quando_nivel_volta(void)
{
  /* Dois altos, um baixo (zera a contagem), depois dois altos: ainda não muda. */
  io_filtro_t filtro;

  (void)io_filtro_inicia(&filtro, 3U, false);
  (void)io_filtro_atualiza(&filtro, true);
  (void)io_filtro_atualiza(&filtro, true);
  (void)io_filtro_atualiza(&filtro, false);
  VERIFICA(io_filtro_atualiza(&filtro, true) == false);
  VERIFICA(io_filtro_atualiza(&filtro, true) == false);
  VERIFICA(io_filtro_atualiza(&filtro, true) == true);
}

static void filtro_de_uma_amostra_aceita_na_hora(void)
{
  io_filtro_t filtro;

  (void)io_filtro_inicia(&filtro, 1U, false);
  VERIFICA(io_filtro_atualiza(&filtro, true) == true);
  VERIFICA(io_filtro_atualiza(&filtro, false) == false);
}

static void padrao_de_50_ms_com_leitura_de_1_ms(void)
{
  uint16_t amostras = 0U;

  VERIFICA(io_filtro_amostras_do_tempo(IO_FILTRO_TEMPO_PADRAO_MS, 1000U, &amostras) == IO_FILTRO_OK);
  VERIFICA(amostras == 50U);
}

static void filtro_de_pulso_de_20_ms_com_leitura_de_2_ms(void)
{
  uint16_t amostras = 0U;

  VERIFICA(io_filtro_amostras_do_tempo(IO_FILTRO_TEMPO_PULSO_MS, 2000U, &amostras) == IO_FILTRO_OK);
  VERIFICA(amostras == 10U);
}

static void pulso_de_20_ms_passa_e_um_de_10_ms_nao(void)
{
  /* Leitura a cada 2 ms: 10 amostras seguidas aceitam; 5 amostras seguidas não. */
  io_filtro_t filtro;
  uint16_t i;

  (void)io_filtro_inicia(&filtro, 10U, false);
  for (i = 0U; i < 5U; i++)
  {
    (void)io_filtro_atualiza(&filtro, true);
  }
  VERIFICA(io_filtro_atualiza(&filtro, false) == false);
  for (i = 0U; i < 9U; i++)
  {
    (void)io_filtro_atualiza(&filtro, true);
  }
  VERIFICA(io_filtro_atualiza(&filtro, true) == true);
}

static void tempo_convertido_arredonda_para_cima(void)
{
  /* 50 ms com leitura a cada 3 ms: 16,67 amostras viram 17, para o filtro não ficar mais curto. */
  uint16_t amostras = 0U;

  VERIFICA(io_filtro_amostras_do_tempo(50U, 3000U, &amostras) == IO_FILTRO_OK);
  VERIFICA(amostras == 17U);
}

static void tempo_igual_ao_periodo_resulta_em_uma_amostra(void)
{
  uint16_t amostras = 0U;

  VERIFICA(io_filtro_amostras_do_tempo(50U, 50000U, &amostras) == IO_FILTRO_OK);
  VERIFICA(amostras == 1U);
}

static void conversao_rejeita_parametro_invalido(void)
{
  uint16_t amostras = 0U;

  VERIFICA(io_filtro_amostras_do_tempo(50U, 1000U, NULL) == IO_FILTRO_ERRO_PARAMETRO);
  VERIFICA(io_filtro_amostras_do_tempo(0U, 1000U, &amostras) == IO_FILTRO_ERRO_PARAMETRO);
  VERIFICA(io_filtro_amostras_do_tempo(50U, 0U, &amostras) == IO_FILTRO_ERRO_PARAMETRO);
}

static void conversao_rejeita_numero_que_nao_cabe_em_16_bits(void)
{
  uint16_t amostras = 0U;

  /* 100 s com leitura a cada 1 ms: 100 000 amostras, acima de uint16_t. */
  VERIFICA(io_filtro_amostras_do_tempo(100000U, 1000U, &amostras) == IO_FILTRO_ERRO_PARAMETRO);
  /* Tempo máximo de 32 bits não estoura na conversão. */
  VERIFICA(io_filtro_amostras_do_tempo(0xFFFFFFFFU, 1U, &amostras) == IO_FILTRO_ERRO_PARAMETRO);
}

int main(void)
{
  EXECUTA(inicia_rejeita_parametro_invalido);
  EXECUTA(inicia_guarda_estado_inicial);
  EXECUTA(mudanca_so_aceita_apos_amostras_estaveis);
  EXECUTA(mudanca_para_desligada_tambem_passa_pelo_filtro);
  EXECUTA(pulso_curto_e_descartado);
  EXECUTA(ruido_alternado_nao_muda_estado);
  EXECUTA(contagem_reinicia_quando_nivel_volta);
  EXECUTA(filtro_de_uma_amostra_aceita_na_hora);
  EXECUTA(padrao_de_50_ms_com_leitura_de_1_ms);
  EXECUTA(filtro_de_pulso_de_20_ms_com_leitura_de_2_ms);
  EXECUTA(pulso_de_20_ms_passa_e_um_de_10_ms_nao);
  EXECUTA(tempo_convertido_arredonda_para_cima);
  EXECUTA(tempo_igual_ao_periodo_resulta_em_uma_amostra);
  EXECUTA(conversao_rejeita_parametro_invalido);
  EXECUTA(conversao_rejeita_numero_que_nao_cabe_em_16_bits);
  return RESULTADO();
}
