/**
 * @file test_tipo_saida.c
 * @brief Testes de unidade do tipo das saídas e da exclusão com o sleep (FW-061, FW-023).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "tipo_saida.h"
#include "teste.h"

#include <stddef.h>

static void rejeita_nulos_e_canais_invalidos(void)
{
  tipos_saida_t t;
  uint16_t tipo = 9U;

  VERIFICA(tipos_saida_limpa(NULL) == TIPO_SAIDA_ERRO_PARAMETRO);
  (void)tipos_saida_limpa(&t);
  VERIFICA(tipos_saida_define(NULL, 2U, 1U, false) == TIPO_SAIDA_ERRO_PARAMETRO);
  VERIFICA(tipos_saida_define(&t, 0U, 0U, false) == TIPO_SAIDA_ERRO_PARAMETRO);
  VERIFICA(tipos_saida_define(&t, 5U, 0U, false) == TIPO_SAIDA_ERRO_PARAMETRO);
  VERIFICA(tipos_saida_tipo(&t, 2U, NULL) == TIPO_SAIDA_ERRO_PARAMETRO);
  VERIFICA(tipos_saida_tipo(NULL, 2U, &tipo) == TIPO_SAIDA_ERRO_PARAMETRO);
  VERIFICA(tipos_saida_tipo(&t, 0U, &tipo) == TIPO_SAIDA_ERRO_PARAMETRO);
  VERIFICA(tipos_saida_algum_em_frequencia(NULL) == false);
}

static void todas_comecam_discretas(void)
{
  tipos_saida_t t;
  uint16_t tipo = 9U;
  uint8_t canal;

  (void)tipos_saida_limpa(&t);
  for (canal = 1U; canal <= TIPO_SAIDA_QTD; canal++)
  {
    VERIFICA(tipos_saida_tipo(&t, canal, &tipo) == TIPO_SAIDA_OK);
    VERIFICA(tipo == TIPO_SAIDA_DISCRETO);
  }
  VERIFICA(tipos_saida_algum_em_frequencia(&t) == false);
}

static void tipo_reservado_e_recusado(void)
{
  tipos_saida_t t;

  (void)tipos_saida_limpa(&t);
  VERIFICA(tipos_saida_define(&t, 2U, 2U, false) == TIPO_SAIDA_ERRO_TIPO);
  VERIFICA(tipos_saida_define(&t, 2U, 65535U, false) == TIPO_SAIDA_ERRO_TIPO);
  VERIFICA(tipos_saida_algum_em_frequencia(&t) == false);
}

static void saida_1_nao_tem_modo_frequencia(void)
{
  tipos_saida_t t;
  uint16_t tipo = 9U;

  (void)tipos_saida_limpa(&t);
  VERIFICA(tipos_saida_define(&t, 1U, TIPO_SAIDA_FREQUENCIA, false) == TIPO_SAIDA_ERRO_SEM_TIMER);
  (void)tipos_saida_tipo(&t, 1U, &tipo);
  VERIFICA(tipo == TIPO_SAIDA_DISCRETO);
  VERIFICA(tipos_saida_define(&t, 1U, TIPO_SAIDA_DISCRETO, false) == TIPO_SAIDA_OK);
}

static void saidas_2_3_e_4_aceitam_frequencia_com_o_sleep_desligado(void)
{
  tipos_saida_t t;
  uint16_t tipo = 9U;
  uint8_t canal;

  (void)tipos_saida_limpa(&t);
  for (canal = 2U; canal <= 4U; canal++)
  {
    VERIFICA(tipos_saida_define(&t, canal, TIPO_SAIDA_FREQUENCIA, false) == TIPO_SAIDA_OK);
    (void)tipos_saida_tipo(&t, canal, &tipo);
    VERIFICA(tipo == TIPO_SAIDA_FREQUENCIA);
  }
  VERIFICA(tipos_saida_algum_em_frequencia(&t) == true);
}

static void frequencia_com_o_sleep_ligado_e_recusada(void)
{
  tipos_saida_t t;
  uint16_t tipo = 9U;

  (void)tipos_saida_limpa(&t);
  VERIFICA(tipos_saida_define(&t, 3U, TIPO_SAIDA_FREQUENCIA, true) == TIPO_SAIDA_ERRO_SLEEP_ATIVO);
  (void)tipos_saida_tipo(&t, 3U, &tipo);
  VERIFICA(tipo == TIPO_SAIDA_DISCRETO);
  VERIFICA(tipos_saida_algum_em_frequencia(&t) == false);
}

static void voltar_ao_discreto_e_sempre_permitido(void)
{
  tipos_saida_t t;

  (void)tipos_saida_limpa(&t);
  (void)tipos_saida_define(&t, 2U, TIPO_SAIDA_FREQUENCIA, false);
  VERIFICA(tipos_saida_define(&t, 2U, TIPO_SAIDA_DISCRETO, true) == TIPO_SAIDA_OK);
  VERIFICA(tipos_saida_algum_em_frequencia(&t) == false);
}

static void sleep_so_e_permitido_sem_saida_em_frequencia(void)
{
  tipos_saida_t t;

  (void)tipos_saida_limpa(&t);
  VERIFICA(tipos_saida_algum_em_frequencia(&t) == false);
  (void)tipos_saida_define(&t, 4U, TIPO_SAIDA_FREQUENCIA, false);
  VERIFICA(tipos_saida_algum_em_frequencia(&t) == true);
  (void)tipos_saida_define(&t, 4U, TIPO_SAIDA_DISCRETO, false);
  VERIFICA(tipos_saida_algum_em_frequencia(&t) == false);
}

int main(void)
{
  EXECUTA(rejeita_nulos_e_canais_invalidos);
  EXECUTA(todas_comecam_discretas);
  EXECUTA(tipo_reservado_e_recusado);
  EXECUTA(saida_1_nao_tem_modo_frequencia);
  EXECUTA(saidas_2_3_e_4_aceitam_frequencia_com_o_sleep_desligado);
  EXECUTA(frequencia_com_o_sleep_ligado_e_recusada);
  EXECUTA(voltar_ao_discreto_e_sempre_permitido);
  EXECUTA(sleep_so_e_permitido_sem_saida_em_frequencia);
  return RESULTADO();
}
