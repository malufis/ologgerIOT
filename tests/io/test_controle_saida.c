/**
 * @file test_controle_saida.c
 * @brief Testes de unidade do modo manual e automático de uma saída digital (FW-025).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.3.0
 */

#include "controle_saida.h"
#include "teste.h"

/** Cinco minutos em milissegundos. */
#define CINCO_MIN 300000U

/* Inicia o controle com o device online, para os testes de comando do CCO. */
static void inicia_online(controle_saida_t *c)
{
  (void)controle_saida_inicia(c);
  controle_saida_define_conectividade(c, true, 0U);
}

static void inicia_rejeita_nulo(void)
{
  VERIFICA(controle_saida_inicia(NULL) == CONTROLE_SAIDA_ERRO_PARAMETRO);
}

static void comeca_automatica_desligada_offline_e_sem_regra_do_sensor(void)
{
  controle_saida_t c;

  VERIFICA(controle_saida_inicia(&c) == CONTROLE_SAIDA_OK);
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_AUTOMATICO);
  VERIFICA(controle_saida_ligada(&c) == false);
  VERIFICA(c.regra_sensor == false && c.online == false);
  VERIFICA(controle_saida_sensor_indisponivel(&c) == false);
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

  inicia_online(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  (void)controle_saida_comando_manual(&c, false, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 10U);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 20U);
  VERIFICA(controle_saida_ligada(&c) == false); /* Os pedidos ficam guardados, sem efeito. */
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_MANUAL);
}

/* ------------------------------------------------------------- volta ao automático */

static void volta_ao_automatico_executa_a_regra_valida_na_hora(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  (void)controle_saida_comando_manual(&c, false, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
  controle_saida_modo_automatico(&c, 20U);
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_AUTOMATICO);
  VERIFICA(controle_saida_ligada(&c) == true); /* Sem esperar nova leitura ou novo pedido. */
}

static void volta_ao_automatico_sem_pedido_mantem_o_ultimo_estado_valido(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  (void)controle_saida_comando_manual(&c, true, 0U);
  controle_saida_modo_automatico(&c, 10U);
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_AUTOMATICO);
  VERIFICA(controle_saida_ligada(&c) == true);
  /* Idem com a saída desligada pelo comando manual. */
  (void)controle_saida_comando_manual(&c, false, 20U);
  controle_saida_modo_automatico(&c, 30U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void volta_ao_automatico_com_pedido_de_desligar_desliga(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_DESLIGA, 0U);
  (void)controle_saida_comando_manual(&c, true, 10U);
  controle_saida_modo_automatico(&c, 20U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

/* --------------------------------------------- comando local e comando remoto (CCO) */

static void sem_regra_do_sensor_vale_o_comando_remoto_do_cco(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 0U);
  VERIFICA(controle_saida_ligada(&c) == true);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_DESLIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void com_regra_do_sensor_o_comando_remoto_e_ignorado(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false); /* Há regra do sensor: o CCO não manda. */
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 20U);
  VERIFICA(controle_saida_ligada(&c) == true);
}

static void regra_do_sensor_que_pede_desligar_vence_o_remoto_que_pede_ligar(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_DESLIGA, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void regra_do_sensor_sem_pedido_mantem_o_estado_e_nao_libera_o_remoto(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 0U);
  VERIFICA(controle_saida_ligada(&c) == true);
  controle_saida_define_regra_sensor(&c, true, 10U); /* O remoto deixa de valer; o local ainda não pede. */
  VERIFICA(controle_saida_ligada(&c) == true);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_DESLIGA, 20U);
  VERIFICA(controle_saida_ligada(&c) == true);
}

static void apagar_a_regra_do_sensor_devolve_o_comando_ao_cco(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
  controle_saida_define_regra_sensor(&c, false, 20U);
  VERIFICA(controle_saida_ligada(&c) == true); /* O pedido do CCO, guardado, passa a valer. */
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
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 0U);
  VERIFICA(controle_saida_ligada(&c) == true);
  controle_saida_sensor_falha(&c, true, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
  VERIFICA(controle_saida_sensor_indisponivel(&c) == true);
}

static void falha_mantem_a_saida_desligada_apesar_de_novo_pedido(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_sensor_falha(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void depois_da_falha_so_volta_a_operar_apos_5_minutos_estaveis(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_sensor_falha(&c, true, 10U);
  controle_saida_sensor_falha(&c, false, 1000U); /* A falha terminou. */
  VERIFICA(controle_saida_ligada(&c) == false);
  VERIFICA(controle_saida_sensor_indisponivel(&c) == true);
  VERIFICA(controle_saida_atualiza(&c, 1000U + CINCO_MIN - 1U) == false);
  VERIFICA(controle_saida_atualiza(&c, 1000U + CINCO_MIN) == true); /* 5 minutos estáveis. */
  VERIFICA(controle_saida_sensor_indisponivel(&c) == false);
}

static void nova_falha_dentro_dos_5_minutos_reinicia_a_contagem(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_sensor_falha(&c, true, 0U);
  controle_saida_sensor_falha(&c, false, 1000U);
  controle_saida_sensor_falha(&c, true, 100000U);  /* Nova falha aos 100 s. */
  controle_saida_sensor_falha(&c, false, 200000U); /* Termina aos 200 s: a contagem recomeça. */
  VERIFICA(controle_saida_atualiza(&c, 1000U + CINCO_MIN) == false); /* A contagem antiga não vale. */
  VERIFICA(controle_saida_atualiza(&c, 200000U + CINCO_MIN - 1U) == false);
  VERIFICA(controle_saida_atualiza(&c, 200000U + CINCO_MIN) == true);
}

static void estabilidade_configuravel(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_estabilidade(&c, 60000U);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_sensor_falha(&c, true, 0U);
  controle_saida_sensor_falha(&c, false, 1000U);
  VERIFICA(controle_saida_atualiza(&c, 60999U) == false);
  VERIFICA(controle_saida_atualiza(&c, 61000U) == true);
}

static void sem_falha_anterior_nao_ha_espera(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_sensor_falha(&c, false, 0U); /* Nunca houve falha. */
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == true);
  VERIFICA(controle_saida_sensor_indisponivel(&c) == false);
}

static void falha_do_sensor_em_manual_mantem_o_comando_manual(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  (void)controle_saida_comando_manual(&c, true, 0U);
  controle_saida_sensor_falha(&c, true, 10U);
  VERIFICA(controle_saida_ligada(&c) == true);
  VERIFICA(controle_saida_modo(&c) == CONTROLE_MODO_MANUAL);
  controle_saida_sensor_falha(&c, false, 20U);
  VERIFICA(controle_saida_atualiza(&c, 20U + CINCO_MIN) == true); /* O fim da espera não mexe no manual. */
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

static void voltar_ao_automatico_durante_a_espera_de_estabilidade_desliga(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 0U);
  (void)controle_saida_comando_manual(&c, true, 10U);
  controle_saida_sensor_falha(&c, true, 20U);
  controle_saida_sensor_falha(&c, false, 30U);
  controle_saida_modo_automatico(&c, 40U); /* Ainda esperando os 5 minutos. */
  VERIFICA(controle_saida_ligada(&c) == false);
  VERIFICA(controle_saida_atualiza(&c, 30U + CINCO_MIN) == true);
}

/* ------------------------------------------------------------- retardo nos pedidos */

static void pedido_automatico_de_ligar_respeita_o_retardo(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_define_atraso(&c, 2000U);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 1000U);
  VERIFICA(controle_saida_aguardando(&c) == true);
  VERIFICA(controle_saida_atualiza(&c, 2999U) == false);
  VERIFICA(controle_saida_atualiza(&c, 3000U) == true);
}

static void falha_durante_o_retardo_cancela_a_partida(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_atraso(&c, 2000U);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_SENSOR, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_sensor_falha(&c, true, 500U);
  VERIFICA(controle_saida_aguardando(&c) == false);
  VERIFICA(controle_saida_atualiza(&c, 2000U) == false);
}

static void pedido_repetido_de_ligar_nao_reinicia_o_retardo(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_define_atraso(&c, 2000U);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 1500U);
  VERIFICA(controle_saida_atualiza(&c, 2000U) == true);
}

/* ------------------------------------------------- agenda local, CCO remoto e conectividade */

static void agenda_vale_offline_sem_regra_do_sensor(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c); /* Começa offline. */
  controle_saida_pedido(&c, CONTROLE_FONTE_AGENDA, CONTROLE_PEDIDO_LIGA, 0U);
  VERIFICA(controle_saida_ligada(&c) == true);
  controle_saida_pedido(&c, CONTROLE_FONTE_AGENDA, CONTROLE_PEDIDO_DESLIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void agenda_e_ignorada_com_o_device_online(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_AGENDA, CONTROLE_PEDIDO_LIGA, 0U);
  VERIFICA(controle_saida_ligada(&c) == false); /* Online, vale o CCO. */
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == true);
}

static void cco_e_ignorado_com_o_device_offline(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 0U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void ficar_offline_aplica_a_agenda_na_hora(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_AGENDA, CONTROLE_PEDIDO_DESLIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == true);
  controle_saida_define_conectividade(&c, false, 20U);
  VERIFICA(controle_saida_ligada(&c) == false); /* A agenda assume no mesmo instante. */
}

static void ficar_online_mantem_o_estado_e_espera_novo_comando_do_cco(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_AGENDA, CONTROLE_PEDIDO_DESLIGA, 5U);
  controle_saida_define_conectividade(&c, false, 10U); /* Offline: a agenda desliga. */
  VERIFICA(controle_saida_ligada(&c) == false);
  controle_saida_define_conectividade(&c, true, 20U);  /* Reconexão. */
  VERIFICA(controle_saida_ligada(&c) == false);        /* O LIGA antigo do CCO não volta. */
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 30U);
  VERIFICA(controle_saida_ligada(&c) == true);         /* Só vale o novo comando. */
}

static void repetir_online_nao_descarta_o_pedido_do_cco(void)
{
  controle_saida_t c;

  inicia_online(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 0U);
  controle_saida_define_conectividade(&c, true, 10U);
  VERIFICA(controle_saida_ligada(&c) == true);
}

static void regra_do_sensor_ignora_agenda_e_cco(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_define_regra_sensor(&c, true, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_AGENDA, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false); /* Offline com regra do sensor: a agenda não vale. */
  controle_saida_define_conectividade(&c, true, 20U);
  controle_saida_pedido(&c, CONTROLE_FONTE_CCO, CONTROLE_PEDIDO_LIGA, 30U);
  VERIFICA(controle_saida_ligada(&c) == false); /* Online com regra do sensor: o CCO não vale. */
}

static void falha_do_sensor_desliga_mesmo_com_agenda_pedindo_ligar(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  controle_saida_pedido(&c, CONTROLE_FONTE_AGENDA, CONTROLE_PEDIDO_LIGA, 0U);
  VERIFICA(controle_saida_ligada(&c) == true);
  controle_saida_sensor_falha(&c, true, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
}

static void manual_prevalece_sobre_a_agenda_offline(void)
{
  controle_saida_t c;

  (void)controle_saida_inicia(&c);
  (void)controle_saida_comando_manual(&c, false, 0U);
  controle_saida_pedido(&c, CONTROLE_FONTE_AGENDA, CONTROLE_PEDIDO_LIGA, 10U);
  VERIFICA(controle_saida_ligada(&c) == false);
  controle_saida_modo_automatico(&c, 20U); /* A agenda guardada passa a valer na volta ao automático. */
  VERIFICA(controle_saida_ligada(&c) == true);
}

int main(void)
{
  EXECUTA(inicia_rejeita_nulo);
  EXECUTA(comeca_automatica_desligada_offline_e_sem_regra_do_sensor);
  EXECUTA(comando_manual_passa_a_manual_e_liga);
  EXECUTA(novo_comando_manual_nao_muda_de_modo_de_novo);
  EXECUTA(manual_respeita_o_retardo_de_acionamento);
  EXECUTA(manual_desligar_cancela_a_partida);
  EXECUTA(manual_prevalece_sobre_pedidos_automaticos);
  EXECUTA(volta_ao_automatico_executa_a_regra_valida_na_hora);
  EXECUTA(volta_ao_automatico_sem_pedido_mantem_o_ultimo_estado_valido);
  EXECUTA(volta_ao_automatico_com_pedido_de_desligar_desliga);
  EXECUTA(sem_regra_do_sensor_vale_o_comando_remoto_do_cco);
  EXECUTA(com_regra_do_sensor_o_comando_remoto_e_ignorado);
  EXECUTA(regra_do_sensor_que_pede_desligar_vence_o_remoto_que_pede_ligar);
  EXECUTA(regra_do_sensor_sem_pedido_mantem_o_estado_e_nao_libera_o_remoto);
  EXECUTA(apagar_a_regra_do_sensor_devolve_o_comando_ao_cco);
  EXECUTA(fonte_invalida_e_ignorada);
  EXECUTA(falha_do_sensor_em_automatico_desliga);
  EXECUTA(falha_mantem_a_saida_desligada_apesar_de_novo_pedido);
  EXECUTA(depois_da_falha_so_volta_a_operar_apos_5_minutos_estaveis);
  EXECUTA(nova_falha_dentro_dos_5_minutos_reinicia_a_contagem);
  EXECUTA(estabilidade_configuravel);
  EXECUTA(sem_falha_anterior_nao_ha_espera);
  EXECUTA(falha_do_sensor_em_manual_mantem_o_comando_manual);
  EXECUTA(voltar_ao_automatico_com_falha_desliga_na_hora);
  EXECUTA(voltar_ao_automatico_durante_a_espera_de_estabilidade_desliga);
  EXECUTA(pedido_automatico_de_ligar_respeita_o_retardo);
  EXECUTA(falha_durante_o_retardo_cancela_a_partida);
  EXECUTA(pedido_repetido_de_ligar_nao_reinicia_o_retardo);
  EXECUTA(agenda_vale_offline_sem_regra_do_sensor);
  EXECUTA(agenda_e_ignorada_com_o_device_online);
  EXECUTA(cco_e_ignorado_com_o_device_offline);
  EXECUTA(ficar_offline_aplica_a_agenda_na_hora);
  EXECUTA(ficar_online_mantem_o_estado_e_espera_novo_comando_do_cco);
  EXECUTA(repetir_online_nao_descarta_o_pedido_do_cco);
  EXECUTA(regra_do_sensor_ignora_agenda_e_cco);
  EXECUTA(falha_do_sensor_desliga_mesmo_com_agenda_pedindo_ligar);
  EXECUTA(manual_prevalece_sobre_a_agenda_offline);
  return RESULTADO();
}
