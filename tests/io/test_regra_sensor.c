/**
 * @file test_regra_sensor.c
 * @brief Testes de unidade da regra do sensor (FW-063).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "regra_sensor.h"
#include "teste.h"

#include <math.h>
#include <stddef.h>

static void rejeita_nulos(void)
{
  regra_sensor_t r;

  VERIFICA(regra_sensor_limpa(NULL) == REGRA_SENSOR_ERRO_PARAMETRO);
  VERIFICA(regra_sensor_cadastra(NULL, 0U, 1.0F, 2.0F) == REGRA_SENSOR_ERRO_PARAMETRO);
  VERIFICA(regra_sensor_avalia(NULL, 1.0F) == CONTROLE_PEDIDO_NENHUM);
  VERIFICA(regra_sensor_limpa(&r) == REGRA_SENSOR_OK);
}

static void sem_cadastro_nao_pede_nada(void)
{
  regra_sensor_t r;

  (void)regra_sensor_limpa(&r);
  VERIFICA(r.definida == false);
  VERIFICA(regra_sensor_avalia(&r, 100.0F) == CONTROLE_PEDIDO_NENHUM);
  VERIFICA(regra_sensor_avalia(&r, -100.0F) == CONTROLE_PEDIDO_NENHUM);
}

static void cadastro_valida_sentido_e_limiares(void)
{
  regra_sensor_t r;

  (void)regra_sensor_limpa(&r);
  VERIFICA(regra_sensor_cadastra(&r, 2U, 4.0F, 5.0F) == REGRA_SENSOR_ERRO_SENTIDO);
  VERIFICA(regra_sensor_cadastra(&r, 1U, 5.0F, 5.0F) == REGRA_SENSOR_ERRO_LIMIARES);
  VERIFICA(regra_sensor_cadastra(&r, 1U, 6.0F, 5.0F) == REGRA_SENSOR_ERRO_LIMIARES);
  VERIFICA(regra_sensor_cadastra(&r, 1U, NAN, 5.0F) == REGRA_SENSOR_ERRO_LIMIARES);
  VERIFICA(regra_sensor_cadastra(&r, 1U, 4.0F, INFINITY) == REGRA_SENSOR_ERRO_LIMIARES);
  VERIFICA(r.definida == false);
  VERIFICA(regra_sensor_cadastra(&r, 1U, 4.0F, 5.0F) == REGRA_SENSOR_OK);
  VERIFICA(r.definida == true);
}

static void cadastro_invalido_mantem_a_regra_anterior(void)
{
  regra_sensor_t r;

  (void)regra_sensor_limpa(&r);
  (void)regra_sensor_cadastra(&r, 1U, 4.0F, 5.0F);
  VERIFICA(regra_sensor_cadastra(&r, 0U, 9.0F, 1.0F) == REGRA_SENSOR_ERRO_LIMIARES);
  VERIFICA(r.sentido == REGRA_SENTIDO_LIGA_NO_MAXIMO);
  VERIFICA(r.minimo == 4.0F && r.maximo == 5.0F);
}

static void sentido_1_liga_em_5_bar_e_desliga_em_4_bar(void)
{
  regra_sensor_t r;

  (void)regra_sensor_limpa(&r);
  (void)regra_sensor_cadastra(&r, 1U, 4.0F, 5.0F);
  VERIFICA(regra_sensor_avalia(&r, 4.5F) == CONTROLE_PEDIDO_NENHUM);
  VERIFICA(regra_sensor_avalia(&r, 5.0F) == CONTROLE_PEDIDO_LIGA);
  VERIFICA(regra_sensor_avalia(&r, 4.5F) == CONTROLE_PEDIDO_LIGA);
  VERIFICA(regra_sensor_avalia(&r, 4.1F) == CONTROLE_PEDIDO_LIGA);
  VERIFICA(regra_sensor_avalia(&r, 4.0F) == CONTROLE_PEDIDO_DESLIGA);
  VERIFICA(regra_sensor_avalia(&r, 4.5F) == CONTROLE_PEDIDO_DESLIGA);
  VERIFICA(regra_sensor_avalia(&r, 5.2F) == CONTROLE_PEDIDO_LIGA);
}

static void sentido_0_liga_no_minimo_e_desliga_no_maximo(void)
{
  regra_sensor_t r;

  (void)regra_sensor_limpa(&r);
  (void)regra_sensor_cadastra(&r, 0U, 4.0F, 5.0F);
  VERIFICA(regra_sensor_avalia(&r, 4.5F) == CONTROLE_PEDIDO_NENHUM);
  VERIFICA(regra_sensor_avalia(&r, 3.9F) == CONTROLE_PEDIDO_LIGA);
  VERIFICA(regra_sensor_avalia(&r, 4.5F) == CONTROLE_PEDIDO_LIGA);
  VERIFICA(regra_sensor_avalia(&r, 5.0F) == CONTROLE_PEDIDO_DESLIGA);
  VERIFICA(regra_sensor_avalia(&r, 4.5F) == CONTROLE_PEDIDO_DESLIGA);
  VERIFICA(regra_sensor_avalia(&r, 4.0F) == CONTROLE_PEDIDO_LIGA);
}

static void valor_nao_numerico_mantem_o_pedido(void)
{
  regra_sensor_t r;

  (void)regra_sensor_limpa(&r);
  (void)regra_sensor_cadastra(&r, 1U, 4.0F, 5.0F);
  (void)regra_sensor_avalia(&r, 5.5F);
  VERIFICA(regra_sensor_avalia(&r, NAN) == CONTROLE_PEDIDO_LIGA);
}

static void novo_cadastro_apaga_o_pedido(void)
{
  regra_sensor_t r;

  (void)regra_sensor_limpa(&r);
  (void)regra_sensor_cadastra(&r, 1U, 4.0F, 5.0F);
  (void)regra_sensor_avalia(&r, 5.5F);
  (void)regra_sensor_cadastra(&r, 1U, 4.0F, 5.0F);
  VERIFICA(r.pedido == CONTROLE_PEDIDO_NENHUM);
  VERIFICA(regra_sensor_avalia(&r, 4.5F) == CONTROLE_PEDIDO_NENHUM);
}

static void sensor_tem_prioridade_sobre_a_agenda_na_saida(void)
{
  regra_sensor_t r;
  controle_saida_t c;

  (void)regra_sensor_limpa(&r);
  (void)regra_sensor_cadastra(&r, 1U, 4.0F, 5.0F);
  (void)controle_saida_inicia(&c);
  controle_saida_define_regra_sensor(&c, r.definida, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_AGENDA, CONTROLE_PEDIDO_LIGA, 0U);
  (void)controle_saida_atualiza(&c, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, regra_sensor_avalia(&r, 5.1F), 20U);
  (void)controle_saida_atualiza(&c, 30U);
  VERIFICA(controle_saida_ligada(&c) == true);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, regra_sensor_avalia(&r, 3.9F), 40U);
  (void)controle_saida_atualiza(&c, 50U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

int main(void)
{
  EXECUTA(rejeita_nulos);
  EXECUTA(sem_cadastro_nao_pede_nada);
  EXECUTA(cadastro_valida_sentido_e_limiares);
  EXECUTA(cadastro_invalido_mantem_a_regra_anterior);
  EXECUTA(sentido_1_liga_em_5_bar_e_desliga_em_4_bar);
  EXECUTA(sentido_0_liga_no_minimo_e_desliga_no_maximo);
  EXECUTA(valor_nao_numerico_mantem_o_pedido);
  EXECUTA(novo_cadastro_apaga_o_pedido);
  EXECUTA(sensor_tem_prioridade_sobre_a_agenda_na_saida);
  return RESULTADO();
}
