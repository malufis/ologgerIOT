/**
 * @file test_agenda.c
 * @brief Testes de unidade da agenda semanal de uma saída digital (FW-032, formato do P-042).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include <string.h>

#include "agenda.h"
#include "teste.h"

/* Monta uma cadeia de 84 caracteres com o byte `valor` na posição `byte` e zeros nas demais. */
static void monta(char *hex, uint8_t byte, const char *dois_digitos)
{
  size_t i;

  for (i = 0U; i < AGENDA_HEX_TAMANHO; i++)
  {
    hex[i] = '0';
  }
  hex[2U * byte] = dois_digitos[0];
  hex[(2U * byte) + 1U] = dois_digitos[1];
}

static void indice_dos_instantes_conhecidos(void)
{
  VERIFICA(agenda_indice(0U, 0U, 0U) == 0U);    /* Segunda 00:00. */
  VERIFICA(agenda_indice(0U, 0U, 29U) == 0U);   /* Ainda no primeiro intervalo. */
  VERIFICA(agenda_indice(0U, 0U, 30U) == 1U);
  VERIFICA(agenda_indice(0U, 8U, 0U) == 16U);   /* 08:00 = 16 intervalos depois da meia-noite. */
  VERIFICA(agenda_indice(0U, 23U, 59U) == 47U);
  VERIFICA(agenda_indice(1U, 0U, 0U) == 48U);   /* Terça 00:00. */
  VERIFICA(agenda_indice(6U, 23U, 30U) == 335U); /* Domingo 23:30, o último intervalo. */
  VERIFICA(agenda_indice(6U, 23U, 59U) == 335U);
}

static void indice_rejeita_instante_invalido(void)
{
  VERIFICA(agenda_indice(7U, 0U, 0U) == AGENDA_INDICE_INVALIDO);
  VERIFICA(agenda_indice(0U, 24U, 0U) == AGENDA_INDICE_INVALIDO);
  VERIFICA(agenda_indice(0U, 0U, 60U) == AGENDA_INDICE_INVALIDO);
}

static void agenda_nova_nao_tem_definicao(void)
{
  agenda_t a;

  VERIFICA(agenda_limpa(&a) == AGENDA_OK);
  VERIFICA(a.definida == false);
  VERIFICA(agenda_estado(&a, 0U, 8U, 0U) == AGENDA_SEM_AGENDA);
  VERIFICA(agenda_limpa(NULL) == AGENDA_ERRO_PARAMETRO);
}

static void carrega_agenda_toda_desligada(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 0U, "00");
  VERIFICA(agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO) == AGENDA_OK);
  VERIFICA(a.definida == true);
  VERIFICA(agenda_estado(&a, 0U, 8U, 0U) == AGENDA_DESLIGADA);
  VERIFICA(agenda_estado(&a, 6U, 23U, 30U) == AGENDA_DESLIGADA);
}

static void ordem_dos_bits_primeiro_byte_comeca_pelo_bit_menos_significativo(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 0U, "01"); /* Bit 0 do byte 0: intervalo 0. */
  (void)agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO);
  VERIFICA(agenda_estado(&a, 0U, 0U, 0U) == AGENDA_LIGADA);
  VERIFICA(agenda_estado(&a, 0U, 0U, 30U) == AGENDA_DESLIGADA);

  monta(hex, 0U, "80"); /* Bit 7 do byte 0: intervalo 7, segunda 03:30. */
  (void)agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO);
  VERIFICA(agenda_estado(&a, 0U, 3U, 30U) == AGENDA_LIGADA);
  VERIFICA(agenda_estado(&a, 0U, 3U, 0U) == AGENDA_DESLIGADA);
  VERIFICA(agenda_estado(&a, 0U, 0U, 0U) == AGENDA_DESLIGADA);
}

static void segundo_byte_cobre_os_intervalos_8_a_15(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 1U, "01"); /* Bit 0 do byte 1: intervalo 8, segunda 04:00. */
  (void)agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO);
  VERIFICA(agenda_estado(&a, 0U, 4U, 0U) == AGENDA_LIGADA);
  VERIFICA(agenda_estado(&a, 0U, 3U, 30U) == AGENDA_DESLIGADA);
}

static void ultimo_intervalo_e_o_bit_mais_significativo_do_ultimo_byte(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 41U, "80"); /* Bit 7 do byte 41: intervalo 335. */
  (void)agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO);
  VERIFICA(agenda_estado(&a, 6U, 23U, 30U) == AGENDA_LIGADA);
  VERIFICA(agenda_estado(&a, 6U, 23U, 0U) == AGENDA_DESLIGADA);
}

static void exemplo_do_motor_de_segunda_das_8h_as_8h30(void)
{
  /* Intervalo 16 (segunda 08:00 a 08:30) = byte 2, bit 0. */
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 2U, "01");
  (void)agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO);
  VERIFICA(agenda_estado(&a, 0U, 8U, 0U) == AGENDA_LIGADA);
  VERIFICA(agenda_estado(&a, 0U, 8U, 29U) == AGENDA_LIGADA);
  VERIFICA(agenda_estado(&a, 0U, 8U, 30U) == AGENDA_DESLIGADA);
  VERIFICA(agenda_estado(&a, 1U, 8U, 0U) == AGENDA_DESLIGADA); /* Terça, não. */
}

static void aceita_maiusculas_e_minusculas(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 0U, "aF"); /* 1010 1111: bits 0 a 3, 5 e 7. */
  VERIFICA(agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO) == AGENDA_OK);
  VERIFICA(agenda_estado(&a, 0U, 0U, 0U) == AGENDA_LIGADA);   /* Bit 0. */
  VERIFICA(agenda_estado(&a, 0U, 2U, 0U) == AGENDA_DESLIGADA); /* Bit 4. */
  VERIFICA(agenda_estado(&a, 0U, 2U, 30U) == AGENDA_LIGADA);  /* Bit 5. */
  VERIFICA(agenda_estado(&a, 0U, 3U, 30U) == AGENDA_LIGADA);  /* Bit 7. */
}

static void rejeita_tamanho_errado(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO + 2U];

  (void)agenda_limpa(&a);
  (void)memset(hex, '0', sizeof(hex));
  VERIFICA(agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO - 1U) == AGENDA_ERRO_TAMANHO);
  VERIFICA(agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO + 1U) == AGENDA_ERRO_TAMANHO);
  VERIFICA(agenda_carrega_hex(&a, hex, 0U) == AGENDA_ERRO_TAMANHO);
  VERIFICA(a.definida == false);
}

static void rejeita_caractere_que_nao_e_hexadecimal(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 0U, "00");
  hex[40] = 'G';
  VERIFICA(agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_CARACTERE);
  hex[40] = ' ';
  VERIFICA(agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_CARACTERE);
  hex[40] = '0';
  hex[83] = 'x';
  VERIFICA(agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_CARACTERE);
  VERIFICA(a.definida == false);
}

static void rejeita_parametro_nulo(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 0U, "00");
  VERIFICA(agenda_carrega_hex(NULL, hex, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_PARAMETRO);
  VERIFICA(agenda_carrega_hex(&a, NULL, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_PARAMETRO);
}

static void erro_de_carga_deixa_a_agenda_anterior_intacta(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 0U, "01");
  (void)agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO);
  monta(hex, 0U, "FF");
  hex[83] = 'Z'; /* Inválida só no último caractere. */
  VERIFICA(agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO) == AGENDA_ERRO_CARACTERE);
  VERIFICA(a.definida == true);
  VERIFICA(agenda_estado(&a, 0U, 0U, 0U) == AGENDA_LIGADA);
  VERIFICA(agenda_estado(&a, 0U, 0U, 30U) == AGENDA_DESLIGADA); /* Os bits novos não entraram. */
}

static void nova_agenda_valida_substitui_a_anterior(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 0U, "01");
  (void)agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO);
  monta(hex, 0U, "02");
  VERIFICA(agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO) == AGENDA_OK);
  VERIFICA(agenda_estado(&a, 0U, 0U, 0U) == AGENDA_DESLIGADA);
  VERIFICA(agenda_estado(&a, 0U, 0U, 30U) == AGENDA_LIGADA);
}

static void limpar_a_agenda_volta_a_sem_agenda(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 0U, "FF");
  (void)agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO);
  (void)agenda_limpa(&a);
  VERIFICA(agenda_estado(&a, 0U, 0U, 0U) == AGENDA_SEM_AGENDA);
}

static void instante_invalido_devolve_sem_agenda(void)
{
  agenda_t a;
  char hex[AGENDA_HEX_TAMANHO];

  (void)agenda_limpa(&a);
  monta(hex, 0U, "FF");
  (void)agenda_carrega_hex(&a, hex, AGENDA_HEX_TAMANHO);
  VERIFICA(agenda_estado(&a, 7U, 0U, 0U) == AGENDA_SEM_AGENDA);
  VERIFICA(agenda_estado(&a, 0U, 24U, 0U) == AGENDA_SEM_AGENDA);
}

int main(void)
{
  EXECUTA(indice_dos_instantes_conhecidos);
  EXECUTA(indice_rejeita_instante_invalido);
  EXECUTA(agenda_nova_nao_tem_definicao);
  EXECUTA(carrega_agenda_toda_desligada);
  EXECUTA(ordem_dos_bits_primeiro_byte_comeca_pelo_bit_menos_significativo);
  EXECUTA(segundo_byte_cobre_os_intervalos_8_a_15);
  EXECUTA(ultimo_intervalo_e_o_bit_mais_significativo_do_ultimo_byte);
  EXECUTA(exemplo_do_motor_de_segunda_das_8h_as_8h30);
  EXECUTA(aceita_maiusculas_e_minusculas);
  EXECUTA(rejeita_tamanho_errado);
  EXECUTA(rejeita_caractere_que_nao_e_hexadecimal);
  EXECUTA(rejeita_parametro_nulo);
  EXECUTA(erro_de_carga_deixa_a_agenda_anterior_intacta);
  EXECUTA(nova_agenda_valida_substitui_a_anterior);
  EXECUTA(limpar_a_agenda_volta_a_sem_agenda);
  EXECUTA(instante_invalido_devolve_sem_agenda);
  return RESULTADO();
}
