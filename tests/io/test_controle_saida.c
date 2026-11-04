/**
 * @file test_controle_saida.c
 * @brief Testes de unidade do modo manual e automático de uma saída digital (FW-025).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "controle_saida.h"
#include "teste.h"

static void inicia_rejeita_nulo(void)
{
  VERIFICA(controle_saida_inicia(NULL) == CONTROLE_SAIDA_ERRO_PARAMETRO);
}

static void comeca_automatica_e_desligada(void)
{
  controle_saida_t c;

  VERIFICA(controle_saida_inicia(&c) == CONTROLE_SAIDA_OK);
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_AUTOMATICO);
  VERIFICA(controle_saida_ligada(&c) == false);
  VERIFICA(controle_saida_atualiza(&c, 1000U) == false);
}

/* ------------------------------------------------------------------ modo manual */

static void comando_manual_passa_a_manual_e_liga(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  VERIFICA(controle_saida_comando_manual(&c, true, 0U) == true); /* Mudou de modo. */
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_MANUAL);
  VERIFICA(controle_saida_ligada(&c) == true);
}

static void novo_comando_manual_nao_muda_de_modo_de_novo(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  (void)controle_saida_comando_manual(&c, true, 0U);
  VERIFICA(controle_saida_comando_manual(&c, false, 10U) == false); /* Já era manual. */
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void manual_respeita_o_retardo_de_acionamento(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_atraso(&c, 3000U);
  (void)controle_saida_comando_manual(&c, true, 1000U);
  VERIFICA(controle_saida_ligada(&c) == false);
  VERIFICA(controle_saida_aguardando(&c) == true);
  VERIFICA(controle_saida_atualiza(&c, 4000U) == true);
}

static void manual_desligar_cancela_a_partida(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_atraso(&c, 3000U);
  (void)controle_saida_comando_manual(&c, true, 0U);
  (void)controle_saida_comando_manual(&c, false, 1000U);
  VERIFICA(controle_saida_aguardando(&c) == false);
  VERIFICA(controle_saida_atualiza(&c, 3000U) == false);
}

static void manual_prevalece_sobre_pedidos_automaticos(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  (void)controle_saida_comando_manual(&c, false, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 10U);
  controle_saida_pedido(&c, CONTROLE_FONTE_EXTERNA, CONTROLE_PEDIDO_LIGA, 20U);
  VERIFICA(controle_saida_ligada(&c) == false); /* Os pedidos ficam guardados, sem efeito. */
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_MANUAL);
}

/* ------------------------------------------------------------- volta ao automático */

static void volta_ao_automatico_executa_a_regra_valida_na_hora(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  (void)controle_saida_comando_manual(&c, false, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
  controle_saida_modo_automatico(&c, 20U);
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_AUTOMATICO);
  VERIFICA(controle_saida_ligada(&c) == true); /* Sem esperar nova leitura ou novo pedido. */
}

static void volta_ao_automatico_sem_pedido_mantem_o_estado(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  (void)controle_saida_comando_manual(&c, true, 0U);
  controle_saida_modo_automatico(&c, 10U);
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_AUTOMATICO);
  VERIFICA(controle_saida_ligada(&c) == true);
}

static void volta_ao_automatico_com_pedido_de_desligar_desliga(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_EXTERNA, CONTROLE_PEDIDO_DESLIGA, 0U);
  (void)controle_saida_comando_manual(&c, true, 10U);
  controle_saida_modo_automatico(&c, 20U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

/* ------------------------------------------------------------ prioridade das fontes */

static void pedido_automatico_liga_e_desliga_a_saida(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_EXTERNA, CONTROLE_PEDIDO_LIGA, 0U);
  VERIFICA(controle_saida_ligada(&c) == true);
  controle_saida_pedido(&c, CONTROLE_FONTE_EXTERNA, CONTROLE_PEDIDO_DESLIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void sensor_prevalece_sobre_a_fonte_externa(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_DESLIGA, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_EXTERNA, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false); /* A agenda ou a plataforma não vence o sensor. */
}

static void externa_vale_quando_o_sensor_nao_pede_nada(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_NENHUM, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_EXTERNA, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == true);
}

static void sensor_que_pede_ligar_vence_externa_que_pede_desligar(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_EXTERNA, CONTROLE_PEDIDO_DESLIGA, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == true);
}

static void fonte_invalida_e_ignorada(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_pedido(&c, (controle_fonte_t)7, CONTROLE_PEDIDO_LIGA, 0U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

/* ------------------------------------------------------------------ falha do sensor */

static void falha_do_sensor_em_automatico_desliga(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 0U);
  VERIFICA(controle_saida_ligada(&c) == true);
  controle_saida_sensor_falha(&c, true, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void falha_mantem_a_saida_desligada_apesar_de_novo_pedido(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_sensor_falha(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_EXTERNA, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void fim_da_falha_volta_a_aplicar_o_pedido_valido(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_sensor_falha(&c, true, 10U);
  controle_saida_sensor_falha(&c, false, 20U);
  VERIFICA(controle_saida_ligada(&c) == true);
}

static void falha_do_sensor_em_manual_mantem_o_comando_manual(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  (void)controle_saida_comando_manual(&c, true, 0U);
  controle_saida_sensor_falha(&c, true, 10U);
  VERIFICA(controle_saida_ligada(&c) == true);
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_MANUAL);
}

static void voltar_ao_automatico_com_falha_desliga_na_hora(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  (void)controle_saida_comando_manual(&c, true, 0U);
  controle_saida_sensor_falha(&c, true, 10U);
  controle_saida_modo_automatico(&c, 20U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

/* ------------------------------------------------------------- retardo nos pedidos */

static void pedido_automatico_de_ligar_respeita_o_retardo(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_atraso(&c, 2000U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 1000U);
  VERIFICA(controle_saida_aguardando(&c) == true);
  VERIFICA(controle_saida_atualiza(&c, 2999U) == false);
  VERIFICA(controle_saida_atualiza(&c, 3000U) == true);
}

static void falha_durante_o_retardo_cancela_a_partida(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_atraso(&c, 2000U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_sensor_falha(&c, true, 500U);
  VERIFICA(controle_saida_aguardando(&c) == false);
  VERIFICA(controle_saida_atualiza(&c, 2000U) == false);
}

static void pedido_repetido_de_ligar_nao_reinicia_o_retardo(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_atraso(&c, 2000U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 1500U);
  VERIFICA(controle_saida_atualiza(&c, 2000U) == true);
}

int main(void)
{
  EXECUTA(inicia_rejeita_nulo);
  EXECUTA(comeca_automatica_e_desligada);
  EXECUTA(comando_manual_passa_a_manual_e_liga);
  EXECUTA(novo_comando_manual_nao_muda_de_modo_de_novo);
  EXECUTA(manual_respeita_o_retardo_de_acionamento);
  EXECUTA(manual_desligar_cancela_a_partida);
  EXECUTA(manual_prevalece_sobre_pedidos_automaticos);
  EXECUTA(volta_ao_automatico_executa_a_regra_valida_na_hora);
  EXECUTA(volta_ao_automatico_sem_pedido_mantem_o_estado);
  EXECUTA(volta_ao_automatico_com_pedido_de_desligar_desliga);
  EXECUTA(pedido_automatico_liga_e_desliga_a_saida);
  EXECUTA(sensor_prevalece_sobre_a_fonte_externa);
  EXECUTA(externa_vale_quando_o_sensor_nao_pede_nada);
  EXECUTA(sensor_que_pede_ligar_vence_externa_que_pede_desligar);
  EXECUTA(fonte_invalida_e_ignorada);
  EXECUTA(falha_do_sensor_em_automatico_desliga);
  EXECUTA(falha_mantem_a_saida_desligada_apesar_de_novo_pedido);
  EXECUTA(fim_da_falha_volta_a_aplicar_o_pedido_valido);
  EXECUTA(falha_do_sensor_em_manual_mantem_o_comando_manual);
  EXECUTA(voltar_ao_automatico_com_falha_desliga_na_hora);
  EXECUTA(pedido_automatico_de_ligar_respeita_o_retardo);
  EXECUTA(falha_durante_o_retardo_cancela_a_partida);
  EXECUTA(pedido_repetido_de_ligar_nao_reinicia_o_retardo);
  return RESULTADO();
}
