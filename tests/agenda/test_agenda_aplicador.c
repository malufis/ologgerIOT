/**
 * @file test_agenda_aplicador.c
 * @brief Testes de unidade da aplicação das agendas pelo horário atual (FW-032).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include <string.h>

#include "agenda_aplicador.h"
#include "teste.h"

/* Quarta-feira é o dia 2. A cadeia liga só o intervalo das 09:30 de quarta (k = 2*48 + 19 = 115). */
#define QUARTA 2U

/* Monta uma cadeia de 84 caracteres com todos os intervalos desligados, menos o k pedido. */
static void so_o_intervalo(char *hex, uint16_t k)
{
  static const char digitos[] = "0123456789ABCDEF";
  uint8_t byte = (uint8_t)(1U << (k % 8U));
  size_t i;

  for (i = 0U; i < AGENDA_HEX_TAMANHO; i++)
  {
    hex[i] = '0';
  }
  hex[2U * (k / 8U)] = digitos[byte >> 4U];
  hex[(2U * (k / 8U)) + 1U] = digitos[byte & 0x0FU];
}

typedef struct
{
  agenda_estado_t estados[AGENDA_APLICADOR_SAIDAS];
  bool pedir[AGENDA_APLICADOR_SAIDAS];
} saida_t;

static uint8_t avalia(agenda_aplicador_t *ap, bool valida, uint8_t dia, uint8_t h, uint8_t m,
                      saida_t *s)
{
  return agenda_aplicador_atualiza(ap, valida, dia, h, m, s->estados, s->pedir);
}

static void rejeita_nulos_e_canais_invalidos(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, 0U);
  VERIFICA(agenda_aplicador_limpa(NULL) == AGENDA_ERRO_PARAMETRO);
  (void)agenda_aplicador_limpa(&ap);
  VERIFICA(agenda_aplicador_carrega(NULL, 1U, hex, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_PARAMETRO);
  VERIFICA(agenda_aplicador_carrega(&ap, 0U, hex, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_PARAMETRO);
  VERIFICA(agenda_aplicador_carrega(&ap, 5U, hex, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_PARAMETRO);
  VERIFICA(agenda_aplicador_carrega(&ap, 1U, NULL, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_PARAMETRO);
  VERIFICA(agenda_aplicador_atualiza(NULL, true, 0U, 0U, 0U, s.estados, s.pedir) == 0U);
  VERIFICA(agenda_aplicador_atualiza(&ap, true, 0U, 0U, 0U, NULL, s.pedir) == 0U);
  VERIFICA(agenda_aplicador_atualiza(&ap, true, 0U, 0U, 0U, s.estados, NULL) == 0U);
}

static void primeira_avaliacao_pede_o_estado_do_intervalo_atual(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  /* Reiniciou às 09:30 de quarta: a agenda assume o estado agendado para as 09:30. */
  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_limpa(&ap);
  VERIFICA(agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO) == AGENDA_OK);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 30U, &s) == 4U);
  VERIFICA(s.pedir[0] == true && s.estados[0] == AGENDA_LIGADA);
  VERIFICA(s.pedir[1] == true && s.estados[1] == AGENDA_SEM_AGENDA);
}

static void reiniciar_as_9h31_assume_o_mesmo_intervalo(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_limpa(&ap);
  (void)agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO);
  (void)avalia(&ap, true, QUARTA, 9U, 31U, &s);
  VERIFICA(s.estados[0] == AGENDA_LIGADA);
}

static void dentro_do_mesmo_intervalo_nao_pede_de_novo(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_limpa(&ap);
  (void)agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO);
  (void)avalia(&ap, true, QUARTA, 9U, 30U, &s);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 31U, &s) == 0U);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 59U, &s) == 0U);
  VERIFICA(s.pedir[0] == false);
}

static void a_virada_de_intervalo_pede_o_novo_estado(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_limpa(&ap);
  (void)agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO);
  (void)avalia(&ap, true, QUARTA, 9U, 30U, &s);
  VERIFICA(avalia(&ap, true, QUARTA, 10U, 0U, &s) == 4U);
  VERIFICA(s.pedir[0] == true && s.estados[0] == AGENDA_DESLIGADA);
}

static void a_virada_do_dia_e_da_semana_pede_o_novo_estado(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, 0U); /* Só segunda 00:00 ligada. */
  (void)agenda_aplicador_limpa(&ap);
  (void)agenda_aplicador_carrega(&ap, 2U, hex, AGENDA_HEX_TAMANHO);
  (void)avalia(&ap, true, 6U, 23U, 30U, &s); /* Domingo 23:30. */
  VERIFICA(s.estados[1] == AGENDA_DESLIGADA);
  VERIFICA(avalia(&ap, true, 0U, 0U, 0U, &s) == 4U);
  VERIFICA(s.estados[1] == AGENDA_LIGADA);
}

static void hora_invalida_nao_pede_nada(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_limpa(&ap);
  (void)agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO);
  VERIFICA(avalia(&ap, false, QUARTA, 9U, 30U, &s) == 0U);
  VERIFICA(s.pedir[0] == false && s.pedir[3] == false);
}

static void quando_a_hora_fica_valida_avalia_o_intervalo_atual(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_limpa(&ap);
  (void)agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO);
  (void)avalia(&ap, false, QUARTA, 9U, 30U, &s);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 40U, &s) == 4U);
  VERIFICA(s.estados[0] == AGENDA_LIGADA);
}

static void carregar_agenda_reavalia_na_hora_sem_esperar_o_intervalo(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_aplicador_limpa(&ap);
  (void)avalia(&ap, true, QUARTA, 9U, 30U, &s);
  VERIFICA(s.estados[0] == AGENDA_SEM_AGENDA);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 35U, &s) == 0U);
  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 36U, &s) == 1U);
  VERIFICA(s.pedir[0] == true && s.estados[0] == AGENDA_LIGADA);
  VERIFICA(s.pedir[1] == false);
}

static void cadeia_vazia_apaga_a_agenda_e_pede_sem_agenda(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_limpa(&ap);
  (void)agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO);
  (void)avalia(&ap, true, QUARTA, 9U, 30U, &s);
  VERIFICA(agenda_aplicador_carrega(&ap, 1U, "", 0U) == AGENDA_OK);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 31U, &s) == 1U);
  VERIFICA(s.pedir[0] == true && s.estados[0] == AGENDA_SEM_AGENDA);
}

static void cadeia_invalida_mantem_a_agenda_e_nao_reavalia(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_limpa(&ap);
  (void)agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO);
  (void)avalia(&ap, true, QUARTA, 9U, 30U, &s);
  VERIFICA(agenda_aplicador_carrega(&ap, 1U, "12XY", 4U) == AGENDA_ERRO_TAMANHO);
  hex[10] = 'Z';
  VERIFICA(agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_CARACTERE);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 31U, &s) == 0U);
  VERIFICA(ap.agenda[0].definida == true);
}

static void so_a_ultima_agenda_enviada_vale(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_limpa(&ap);
  (void)agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO);
  so_o_intervalo(hex, (QUARTA * 48U) + 20U); /* Agora só as 10:00 ligam. */
  (void)agenda_aplicador_carrega(&ap, 1U, hex, AGENDA_HEX_TAMANHO);
  (void)avalia(&ap, true, QUARTA, 9U, 30U, &s);
  VERIFICA(s.estados[0] == AGENDA_DESLIGADA);
  (void)avalia(&ap, true, QUARTA, 10U, 0U, &s);
  VERIFICA(s.estados[0] == AGENDA_LIGADA);
}

static void saidas_tem_agendas_independentes(void)
{
  agenda_aplicador_t ap;
  saida_t s;
  char hex[AGENDA_HEX_TAMANHO];

  so_o_intervalo(hex, (QUARTA * 48U) + 19U);
  (void)agenda_aplicador_limpa(&ap);
  (void)agenda_aplicador_carrega(&ap, 3U, hex, AGENDA_HEX_TAMANHO);
  (void)avalia(&ap, true, QUARTA, 9U, 30U, &s);
  VERIFICA(s.estados[0] == AGENDA_SEM_AGENDA);
  VERIFICA(s.estados[2] == AGENDA_LIGADA);
  VERIFICA(s.estados[3] == AGENDA_SEM_AGENDA);
}

static void reavaliar_pede_o_estado_de_todas_as_saidas_no_mesmo_intervalo(void)
{
  agenda_aplicador_t ap;
  saida_t s;

  VERIFICA(agenda_aplicador_reavalia(NULL) == AGENDA_ERRO_PARAMETRO);
  (void)agenda_aplicador_limpa(&ap);
  (void)avalia(&ap, true, QUARTA, 9U, 30U, &s);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 40U, &s) == 0U);
  VERIFICA(agenda_aplicador_reavalia(&ap) == AGENDA_OK);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 41U, &s) == 4U);
  VERIFICA(avalia(&ap, true, QUARTA, 9U, 42U, &s) == 0U);
}

int main(void)
{
  EXECUTA(rejeita_nulos_e_canais_invalidos);
  EXECUTA(primeira_avaliacao_pede_o_estado_do_intervalo_atual);
  EXECUTA(reiniciar_as_9h31_assume_o_mesmo_intervalo);
  EXECUTA(dentro_do_mesmo_intervalo_nao_pede_de_novo);
  EXECUTA(a_virada_de_intervalo_pede_o_novo_estado);
  EXECUTA(a_virada_do_dia_e_da_semana_pede_o_novo_estado);
  EXECUTA(hora_invalida_nao_pede_nada);
  EXECUTA(quando_a_hora_fica_valida_avalia_o_intervalo_atual);
  EXECUTA(carregar_agenda_reavalia_na_hora_sem_esperar_o_intervalo);
  EXECUTA(cadeia_vazia_apaga_a_agenda_e_pede_sem_agenda);
  EXECUTA(cadeia_invalida_mantem_a_agenda_e_nao_reavalia);
  EXECUTA(so_a_ultima_agenda_enviada_vale);
  EXECUTA(saidas_tem_agendas_independentes);
  EXECUTA(reavaliar_pede_o_estado_de_todas_as_saidas_no_mesmo_intervalo);
  return RESULTADO();
}
