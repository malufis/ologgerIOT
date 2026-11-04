/**
 * @file test_agenda_config.c
 * @brief Testes de unidade do formato da configuração persistente da agenda (FW-032).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include <string.h>

#include "agenda_config.h"
#include "teste.h"

/* Quarta-feira 09:30: k = 2*48 + 19 = 115. */
#define K_QUARTA_0930 115U

static void liga_o_intervalo(agenda_aplicador_t *ap, uint8_t canal, uint16_t k)
{
  static const char digitos[] = "0123456789ABCDEF";
  char hex[AGENDA_HEX_TAMANHO];
  uint8_t byte = (uint8_t)(1U << (k % 8U));
  size_t i;

  for (i = 0U; i < AGENDA_HEX_TAMANHO; i++)
  {
    hex[i] = '0';
  }
  hex[2U * (k / 8U)] = digitos[byte >> 4U];
  hex[(2U * (k / 8U)) + 1U] = digitos[byte & 0x0FU];
  (void)agenda_aplicador_carrega(ap, canal, hex, AGENDA_HEX_TAMANHO);
}

static void rejeita_parametros_invalidos(void)
{
  agenda_aplicador_t ap;
  uint8_t carga[AGENDA_CONFIG_TAMANHO];
  size_t n = 0U;
  int16_t fuso = 0;

  (void)agenda_aplicador_limpa(&ap);
  VERIFICA(agenda_config_empacota(NULL, -180, carga, sizeof(carga), &n) == AGENDA_CONFIG_ERRO_PARAMETRO);
  VERIFICA(agenda_config_empacota(&ap, -180, NULL, sizeof(carga), &n) == AGENDA_CONFIG_ERRO_PARAMETRO);
  VERIFICA(agenda_config_empacota(&ap, -180, carga, sizeof(carga), NULL) == AGENDA_CONFIG_ERRO_PARAMETRO);
  VERIFICA(agenda_config_empacota(&ap, -180, carga, 100U, &n) == AGENDA_CONFIG_ERRO_PARAMETRO);
  VERIFICA(agenda_config_desempacota(NULL, &fuso, carga, sizeof(carga)) == AGENDA_CONFIG_ERRO_PARAMETRO);
  VERIFICA(agenda_config_desempacota(&ap, NULL, carga, sizeof(carga)) == AGENDA_CONFIG_ERRO_PARAMETRO);
  VERIFICA(agenda_config_desempacota(&ap, &fuso, NULL, sizeof(carga)) == AGENDA_CONFIG_ERRO_PARAMETRO);
}

static void empacota_com_o_tamanho_e_o_fuso_no_lugar(void)
{
  agenda_aplicador_t ap;
  uint8_t carga[AGENDA_CONFIG_TAMANHO];
  size_t n = 0U;

  (void)agenda_aplicador_limpa(&ap);
  VERIFICA(agenda_config_empacota(&ap, -180, carga, sizeof(carga), &n) == AGENDA_CONFIG_OK);
  VERIFICA(n == AGENDA_CONFIG_TAMANHO && n == 176U);
  VERIFICA(carga[0] == AGENDA_CONFIG_VERSAO && carga[1] == 0U);
  VERIFICA(carga[2] == 0x4CU && carga[3] == 0xFFU); /* -180 = 0xFF4C em little-endian. */
}

static void ida_e_volta_preserva_o_fuso_e_as_agendas(void)
{
  agenda_aplicador_t origem;
  agenda_aplicador_t destino;
  uint8_t carga[AGENDA_CONFIG_TAMANHO];
  size_t n = 0U;
  int16_t fuso = 0;
  uint8_t i;

  (void)agenda_aplicador_limpa(&origem);
  liga_o_intervalo(&origem, 1U, K_QUARTA_0930);
  liga_o_intervalo(&origem, 4U, 0U);
  (void)agenda_config_empacota(&origem, -180, carga, sizeof(carga), &n);

  (void)agenda_aplicador_limpa(&destino);
  VERIFICA(agenda_config_desempacota(&destino, &fuso, carga, n) == AGENDA_CONFIG_OK);
  VERIFICA(fuso == -180);
  for (i = 0U; i < AGENDA_APLICADOR_SAIDAS; i++)
  {
    VERIFICA(destino.agenda[i].definida == origem.agenda[i].definida);
    VERIFICA(memcmp(destino.agenda[i].bits, origem.agenda[i].bits, AGENDA_BYTES) == 0);
  }
  VERIFICA(agenda_estado(&destino.agenda[0], 2U, 9U, 30U) == AGENDA_LIGADA);
  VERIFICA(agenda_estado(&destino.agenda[1], 2U, 9U, 30U) == AGENDA_SEM_AGENDA);
  VERIFICA(agenda_estado(&destino.agenda[3], 0U, 0U, 0U) == AGENDA_LIGADA);
}

static void fusos_positivos_e_negativos_voltam_iguais(void)
{
  static const int16_t fusos[] = {-720, -180, 0, 330, 840};
  agenda_aplicador_t ap;
  uint8_t carga[AGENDA_CONFIG_TAMANHO];
  size_t n = 0U;
  int16_t fuso = 99;
  size_t i;

  (void)agenda_aplicador_limpa(&ap);
  for (i = 0U; i < (sizeof(fusos) / sizeof(fusos[0])); i++)
  {
    (void)agenda_config_empacota(&ap, fusos[i], carga, sizeof(carga), &n);
    VERIFICA(agenda_config_desempacota(&ap, &fuso, carga, n) == AGENDA_CONFIG_OK);
    VERIFICA(fuso == fusos[i]);
  }
}

static void depois_de_carregar_todas_as_saidas_sao_reavaliadas(void)
{
  agenda_aplicador_t origem;
  agenda_aplicador_t destino;
  agenda_estado_t estados[AGENDA_APLICADOR_SAIDAS];
  bool pedir[AGENDA_APLICADOR_SAIDAS];
  uint8_t carga[AGENDA_CONFIG_TAMANHO];
  size_t n = 0U;
  int16_t fuso = 0;

  (void)agenda_aplicador_limpa(&origem);
  liga_o_intervalo(&origem, 1U, K_QUARTA_0930);
  (void)agenda_config_empacota(&origem, -180, carga, sizeof(carga), &n);
  (void)agenda_aplicador_limpa(&destino);
  (void)agenda_config_desempacota(&destino, &fuso, carga, n);
  /* Reiniciou às 09:30 de quarta: assume o estado agendado para as 09:30. */
  VERIFICA(agenda_aplicador_atualiza(&destino, true, 2U, 9U, 30U, estados, pedir) == 4U);
  VERIFICA(estados[0] == AGENDA_LIGADA);
}

static void formato_errado_e_recusado_sem_alterar_nada(void)
{
  agenda_aplicador_t origem;
  agenda_aplicador_t ap;
  uint8_t carga[AGENDA_CONFIG_TAMANHO];
  size_t n = 0U;
  int16_t fuso = 77;

  (void)agenda_aplicador_limpa(&origem);
  liga_o_intervalo(&origem, 1U, K_QUARTA_0930);
  (void)agenda_config_empacota(&origem, -180, carga, sizeof(carga), &n);
  (void)agenda_aplicador_limpa(&ap);

  VERIFICA(agenda_config_desempacota(&ap, &fuso, carga, n - 1U) == AGENDA_CONFIG_ERRO_FORMATO);
  carga[0] = 2U;
  VERIFICA(agenda_config_desempacota(&ap, &fuso, carga, n) == AGENDA_CONFIG_ERRO_FORMATO);
  VERIFICA(fuso == 77 && ap.agenda[0].definida == false);
}

static void campo_invalido_e_recusado_por_inteiro(void)
{
  agenda_aplicador_t origem;
  agenda_aplicador_t ap;
  uint8_t carga[AGENDA_CONFIG_TAMANHO];
  size_t n = 0U;
  int16_t fuso = 77;

  (void)agenda_aplicador_limpa(&origem);
  liga_o_intervalo(&origem, 1U, K_QUARTA_0930);
  (void)agenda_config_empacota(&origem, -180, carga, sizeof(carga), &n);
  carga[4U + (2U * 43U)] = 5U; /* "Definida" da saída 3 com valor inválido. */
  (void)agenda_aplicador_limpa(&ap);
  VERIFICA(agenda_config_desempacota(&ap, &fuso, carga, n) == AGENDA_CONFIG_ERRO_CAMPO);
  VERIFICA(fuso == 77);
  VERIFICA(ap.agenda[0].definida == false && ap.agenda[2].definida == false);
}

int main(void)
{
  EXECUTA(rejeita_parametros_invalidos);
  EXECUTA(empacota_com_o_tamanho_e_o_fuso_no_lugar);
  EXECUTA(ida_e_volta_preserva_o_fuso_e_as_agendas);
  EXECUTA(fusos_positivos_e_negativos_voltam_iguais);
  EXECUTA(depois_de_carregar_todas_as_saidas_sao_reavaliadas);
  EXECUTA(formato_errado_e_recusado_sem_alterar_nada);
  EXECUTA(campo_invalido_e_recusado_por_inteiro);
  return RESULTADO();
}
