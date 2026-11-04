/**
 * @file test_saida.c
 * @brief Testes de unidade do estado da saída digital discreta com retardo (FW-022 e FW-024).
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.3.0
 */

#include "saida.h"
#include "teste.h"

static void inicia_rejeita_nulo(void)
{
  VERIFICA(saida_inicia(NULL) == SAIDA_ERRO_PARAMETRO);
}

static void saida_comeca_desligada(void)
{
  saida_t s;

  VERIFICA(saida_inicia(&s) == SAIDA_OK);
  VERIFICA(saida_ligada(&s) == false);
  VERIFICA(saida_aguardando(&s) == false);
  VERIFICA(saida_atualiza(&s, 123456U) == false);
}

static void saida_desligada_nao_liga_sozinha(void)
{
  saida_t s;

  (void)saida_inicia(&s);
  VERIFICA(saida_atualiza(&s, 0U) == false);
  VERIFICA(saida_atualiza(&s, 0xFFFFFFFFU) == false);
}

static void sem_retardo_liga_na_hora_e_fica_ligada(void)
{
  saida_t s;

  (void)saida_inicia(&s);
  saida_liga(&s, 500U);
  VERIFICA(saida_ligada(&s) == true);
  VERIFICA(saida_aguardando(&s) == false);
  VERIFICA(saida_atualiza(&s, 1000000U) == true); /* Nunca desliga sozinha. */
}

static void desliga_por_comando(void)
{
  saida_t s;

  (void)saida_inicia(&s);
  saida_liga(&s, 0U);
  saida_desliga(&s);
  VERIFICA(saida_ligada(&s) == false);
  VERIFICA(saida_atualiza(&s, 600000U) == false);
}

static void com_retardo_espera_e_liga_no_fim(void)
{
  saida_t s;

  (void)saida_inicia(&s);
  saida_define_atraso(&s, 3000U);
  saida_liga(&s, 1000U);
  VERIFICA(saida_ligada(&s) == false);
  VERIFICA(saida_aguardando(&s) == true);
  VERIFICA(saida_atualiza(&s, 3999U) == false); /* Falta 1 ms. */
  VERIFICA(saida_aguardando(&s) == true);
  VERIFICA(saida_atualiza(&s, 4000U) == true);  /* Exatamente o retardo: liga. */
  VERIFICA(saida_aguardando(&s) == false);
  VERIFICA(saida_atualiza(&s, 600000U) == true);
}

static void desligar_durante_a_espera_cancela_a_partida(void)
{
  saida_t s;

  (void)saida_inicia(&s);
  saida_define_atraso(&s, 3000U);
  saida_liga(&s, 0U);
  saida_desliga(&s);
  VERIFICA(saida_aguardando(&s) == false);
  VERIFICA(saida_atualiza(&s, 3000U) == false); /* O retardo venceria aqui, mas foi cancelado. */
  VERIFICA(saida_atualiza(&s, 600000U) == false);
}

static void ligar_durante_a_espera_e_descartado_e_a_contagem_continua(void)
{
  saida_t s;

  (void)saida_inicia(&s);
  saida_define_atraso(&s, 3000U);
  saida_liga(&s, 0U);
  saida_liga(&s, 2000U); /* Descartado: a contagem que começou em 0 continua. */
  VERIFICA(saida_atualiza(&s, 2999U) == false);
  VERIFICA(saida_atualiza(&s, 3000U) == true);
}

static void ligar_com_a_saida_ja_ligada_e_descartado(void)
{
  saida_t s;

  (void)saida_inicia(&s);
  saida_liga(&s, 0U);
  saida_define_atraso(&s, 3000U);
  saida_liga(&s, 1000U); /* Descartado: não reaplica o retardo e não desliga. */
  VERIFICA(saida_ligada(&s) == true);
  VERIFICA(saida_aguardando(&s) == false);
  VERIFICA(saida_atualiza(&s, 600000U) == true);
}

static void desligar_e_ligar_de_novo_reaplica_o_retardo(void)
{
  saida_t s;

  (void)saida_inicia(&s);
  saida_define_atraso(&s, 3000U);
  saida_liga(&s, 0U);
  (void)saida_atualiza(&s, 3000U);
  saida_desliga(&s);
  saida_liga(&s, 10000U);
  VERIFICA(saida_ligada(&s) == false);
  VERIFICA(saida_aguardando(&s) == true);
  VERIFICA(saida_atualiza(&s, 13000U) == true);
}

static void mudar_o_retardo_nao_altera_a_espera_em_curso(void)
{
  saida_t s;

  (void)saida_inicia(&s);
  saida_define_atraso(&s, 3000U);
  saida_liga(&s, 0U);
  saida_define_atraso(&s, 10000U);
  VERIFICA(saida_atualiza(&s, 3000U) == false); /* Passa a valer 10 s: a espera segue até lá. */
  VERIFICA(saida_atualiza(&s, 9999U) == false);
  VERIFICA(saida_atualiza(&s, 10000U) == true);
}

static void retardo_zero_depois_de_um_retardo_liga_na_hora(void)
{
  saida_t s;

  (void)saida_inicia(&s);
  saida_define_atraso(&s, 3000U);
  saida_define_atraso(&s, 0U);
  saida_liga(&s, 0U);
  VERIFICA(saida_ligada(&s) == true);
}

static void volta_do_contador_durante_o_retardo(void)
{
  /* Retardo de 4 s começando 1 s antes da volta do contador: liga 3 s depois da volta. */
  saida_t s;

  (void)saida_inicia(&s);
  saida_define_atraso(&s, 4000U);
  saida_liga(&s, 0xFFFFFFFFU - 999U);
  VERIFICA(saida_atualiza(&s, 2999U) == false);
  VERIFICA(saida_atualiza(&s, 3000U) == true);
}

int main(void)
{
  EXECUTA(inicia_rejeita_nulo);
  EXECUTA(saida_comeca_desligada);
  EXECUTA(saida_desligada_nao_liga_sozinha);
  EXECUTA(sem_retardo_liga_na_hora_e_fica_ligada);
  EXECUTA(desliga_por_comando);
  EXECUTA(com_retardo_espera_e_liga_no_fim);
  EXECUTA(desligar_durante_a_espera_cancela_a_partida);
  EXECUTA(ligar_durante_a_espera_e_descartado_e_a_contagem_continua);
  EXECUTA(ligar_com_a_saida_ja_ligada_e_descartado);
  EXECUTA(desligar_e_ligar_de_novo_reaplica_o_retardo);
  EXECUTA(mudar_o_retardo_nao_altera_a_espera_em_curso);
  EXECUTA(retardo_zero_depois_de_um_retardo_liga_na_hora);
  EXECUTA(volta_do_contador_durante_o_retardo);
  return RESULTADO();
}
