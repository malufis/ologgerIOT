/**
 * @file test_io_pulsos.c
 * @brief Testes de unidade do contador de pulsos e da inatividade.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#include "io_pulsos.h"
#include "teste.h"

static void inicia_rejeita_parametro_invalido(void)
{
  io_pulsos_t pulsos;

  VERIFICA(io_pulsos_inicia(NULL, 1000U, false, 0U) == IO_PULSOS_ERRO_PARAMETRO);
  VERIFICA(io_pulsos_inicia(&pulsos, 0U, false, 0U) == IO_PULSOS_ERRO_PARAMETRO);
}

static void conta_cada_transicao_para_ativo(void)
{
  io_pulsos_t pulsos;

  (void)io_pulsos_inicia(&pulsos, 60000U, false, 0U);
  io_pulsos_atualiza(&pulsos, true, 100U);
  io_pulsos_atualiza(&pulsos, false, 200U);
  io_pulsos_atualiza(&pulsos, true, 300U);
  io_pulsos_atualiza(&pulsos, false, 400U);
  VERIFICA(io_pulsos_contagem(&pulsos) == 2U);
}

static void entrada_mantida_ativa_conta_uma_vez(void)
{
  io_pulsos_t pulsos;
  uint32_t t;

  (void)io_pulsos_inicia(&pulsos, 60000U, false, 0U);
  for (t = 0U; t < 1000U; t += 10U)
  {
    io_pulsos_atualiza(&pulsos, true, t);
  }
  VERIFICA(io_pulsos_contagem(&pulsos) == 1U);
}

static void entrada_ativa_na_partida_nao_conta(void)
{
  io_pulsos_t pulsos;

  (void)io_pulsos_inicia(&pulsos, 60000U, true, 0U);
  io_pulsos_atualiza(&pulsos, true, 10U);
  VERIFICA(io_pulsos_contagem(&pulsos) == 0U);
}

static void pulsos_dormindo_somam_ao_total(void)
{
  io_pulsos_t pulsos;

  (void)io_pulsos_inicia(&pulsos, 60000U, false, 0U);
  io_pulsos_atualiza(&pulsos, true, 100U);
  io_pulsos_atualiza(&pulsos, false, 200U);
  io_pulsos_soma(&pulsos, 37U, false, 5000U);
  VERIFICA(io_pulsos_contagem(&pulsos) == 38U);
}

static void pulso_ainda_ativo_ao_acordar_nao_conta_de_novo(void)
{
  /* O pulso foi contado dormindo e ainda está ativo: a leitura seguinte não o conta. */
  io_pulsos_t pulsos;

  (void)io_pulsos_inicia(&pulsos, 60000U, false, 0U);
  io_pulsos_soma(&pulsos, 1U, true, 500U);
  io_pulsos_atualiza(&pulsos, true, 510U);
  VERIFICA(io_pulsos_contagem(&pulsos) == 1U);
}

static void pulso_terminado_ao_acordar_permite_novo_pulso(void)
{
  io_pulsos_t pulsos;

  (void)io_pulsos_inicia(&pulsos, 60000U, false, 0U);
  io_pulsos_soma(&pulsos, 1U, false, 500U);
  io_pulsos_atualiza(&pulsos, true, 800U);
  VERIFICA(io_pulsos_contagem(&pulsos) == 2U);
}

static void pulsos_dormindo_reiniciam_a_inatividade_e_zero_nao(void)
{
  io_pulsos_t pulsos;

  (void)io_pulsos_inicia(&pulsos, 60000U, false, 0U);
  io_pulsos_soma(&pulsos, 0U, false, 50000U);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 60000U) == true);
  io_pulsos_soma(&pulsos, 3U, false, 50000U);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 60000U) == false);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 110000U) == true);
}

static void so_dorme_depois_do_tempo_sem_pulso(void)
{
  io_pulsos_t pulsos;

  (void)io_pulsos_inicia(&pulsos, IO_PULSOS_TIMEOUT_PADRAO_MS, false, 1000U);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 1000U) == false);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 60999U) == false);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 61000U) == true);
}

static void pulso_reinicia_a_contagem_de_inatividade(void)
{
  io_pulsos_t pulsos;

  (void)io_pulsos_inicia(&pulsos, 60000U, false, 0U);
  io_pulsos_atualiza(&pulsos, true, 50000U);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 60000U) == false);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 109999U) == false);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 110000U) == true);
}

static void volta_do_contador_de_milissegundos_nao_confunde(void)
{
  /* Último pulso 10 s antes de o contador voltar a zero: aos 50 s depois da volta, passaram 60 s. */
  io_pulsos_t pulsos;

  (void)io_pulsos_inicia(&pulsos, 60000U, false, 0xFFFFFFFFU - 9999U);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 49999U) == false);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 50000U) == true);
}

static void timeout_configuravel_muda_o_limite_de_inatividade(void)
{
  io_pulsos_t pulsos;

  (void)io_pulsos_inicia(&pulsos, IO_PULSOS_TIMEOUT_PADRAO_MS, false, 0U);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 30000U) == false);
  VERIFICA(io_pulsos_define_timeout(&pulsos, 30000U) == IO_PULSOS_OK);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 30000U) == true);
  VERIFICA(io_pulsos_define_timeout(&pulsos, 0U) == IO_PULSOS_ERRO_PARAMETRO);
  VERIFICA(io_pulsos_define_timeout(NULL, 1000U) == IO_PULSOS_ERRO_PARAMETRO);
  VERIFICA(io_pulsos_pode_dormir(&pulsos, 29999U) == false);
}

int main(void)
{
  EXECUTA(inicia_rejeita_parametro_invalido);
  EXECUTA(conta_cada_transicao_para_ativo);
  EXECUTA(entrada_mantida_ativa_conta_uma_vez);
  EXECUTA(entrada_ativa_na_partida_nao_conta);
  EXECUTA(pulsos_dormindo_somam_ao_total);
  EXECUTA(pulso_ainda_ativo_ao_acordar_nao_conta_de_novo);
  EXECUTA(pulso_terminado_ao_acordar_permite_novo_pulso);
  EXECUTA(pulsos_dormindo_reiniciam_a_inatividade_e_zero_nao);
  EXECUTA(so_dorme_depois_do_tempo_sem_pulso);
  EXECUTA(pulso_reinicia_a_contagem_de_inatividade);
  EXECUTA(volta_do_contador_de_milissegundos_nao_confunde);
  EXECUTA(timeout_configuravel_muda_o_limite_de_inatividade);
  return RESULTADO();
}
