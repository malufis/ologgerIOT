/**
 * @file test_armazem.c
 * @brief Testes de unidade do armazém de configuração em dois espaços de flash (FW-032).
 *
 * Usa uma flash simulada de dois espaços de 2 KB, com injeção de falhas e de queda de energia.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include <string.h>

#include "armazem.h"
#include "teste.h"

#define ESPACO 2048U

/* Flash simulada. */
static uint8_t flash[ARMAZEM_ESPACOS][ESPACO];
static int32_t apagamentos;
/* Quantos bytes a próxima gravação consegue escrever antes de "cair a energia"; -1 = todos. */
static int32_t limite_de_escrita = -1;
static bool apagar_falha;

static const uint8_t *sim_endereco(uint8_t espaco)
{
  return flash[espaco];
}

static int32_t sim_apaga(uint8_t espaco)
{
  if (apagar_falha)
  {
    (void)memset(flash[espaco], 0x00, ESPACO / 2U); /* Apagamento interrompido: lixo. */
    return -1;
  }
  (void)memset(flash[espaco], 0xFF, ESPACO);
  apagamentos++;
  return 0;
}

static int32_t sim_grava(uint8_t espaco, const uint8_t *dados, size_t tamanho)
{
  size_t n = tamanho;

  if ((limite_de_escrita >= 0) && ((size_t)limite_de_escrita < n))
  {
    n = (size_t)limite_de_escrita;
    (void)memcpy(flash[espaco], dados, n);
    return -1; /* Caiu a energia no meio da gravação. */
  }
  (void)memcpy(flash[espaco], dados, n);
  return 0;
}

static const armazem_ops_t ops = {sim_endereco, sim_apaga, sim_grava, ESPACO};

static void zera(void)
{
  (void)memset(flash, 0xFF, sizeof(flash));
  apagamentos = 0;
  limite_de_escrita = -1;
  apagar_falha = false;
}

static void monta(uint8_t *carga, size_t n, uint8_t base)
{
  size_t i;

  for (i = 0U; i < n; i++)
  {
    carga[i] = (uint8_t)(base + i);
  }
}

static void rejeita_parametros_invalidos(void)
{
  armazem_t a;
  armazem_ops_t pequeno = {sim_endereco, sim_apaga, sim_grava, 100U};
  uint8_t carga[8] = {0};
  uint8_t lida[8];
  size_t n = 0U;

  zera();
  VERIFICA(armazem_inicia(NULL, &ops) == ARMAZEM_ERRO_PARAMETRO);
  VERIFICA(armazem_inicia(&a, NULL) == ARMAZEM_ERRO_PARAMETRO);
  VERIFICA(armazem_inicia(&a, &pequeno) == ARMAZEM_ERRO_PARAMETRO);
  (void)armazem_inicia(&a, &ops);
  VERIFICA(armazem_le(NULL, lida, 8U, &n) == ARMAZEM_ERRO_PARAMETRO);
  VERIFICA(armazem_le(&a, NULL, 8U, &n) == ARMAZEM_ERRO_PARAMETRO);
  VERIFICA(armazem_grava(&a, NULL, 8U) == ARMAZEM_ERRO_PARAMETRO);
  VERIFICA(armazem_grava(&a, carga, 0U) == ARMAZEM_ERRO_TAMANHO);
  VERIFICA(armazem_grava(&a, carga, ARMAZEM_CARGA_MAX + 1U) == ARMAZEM_ERRO_TAMANHO);
}

static void flash_apagada_devolve_vazio(void)
{
  armazem_t a;
  uint8_t lida[16];
  size_t n = 0U;

  zera();
  (void)armazem_inicia(&a, &ops);
  VERIFICA(armazem_le(&a, lida, sizeof(lida), &n) == ARMAZEM_ERRO_VAZIO);
}

static void grava_e_le_a_mesma_carga(void)
{
  armazem_t a;
  uint8_t carga[176];
  uint8_t lida[ARMAZEM_CARGA_MAX];
  size_t n = 0U;

  zera();
  monta(carga, sizeof(carga), 7U);
  (void)armazem_inicia(&a, &ops);
  VERIFICA(armazem_grava(&a, carga, sizeof(carga)) == ARMAZEM_OK);
  VERIFICA(armazem_le(&a, lida, sizeof(lida), &n) == ARMAZEM_OK);
  VERIFICA(n == sizeof(carga) && memcmp(lida, carga, n) == 0);
}

static void buffer_pequeno_devolve_erro_de_tamanho(void)
{
  armazem_t a;
  uint8_t carga[40];
  uint8_t lida[10];
  size_t n = 0U;

  zera();
  monta(carga, sizeof(carga), 1U);
  (void)armazem_inicia(&a, &ops);
  (void)armazem_grava(&a, carga, sizeof(carga));
  VERIFICA(armazem_le(&a, lida, sizeof(lida), &n) == ARMAZEM_ERRO_TAMANHO);
}

static void sobrevive_ao_reinicio(void)
{
  armazem_t a;
  armazem_t b;
  uint8_t carga[50];
  uint8_t lida[ARMAZEM_CARGA_MAX];
  size_t n = 0U;

  zera();
  monta(carga, sizeof(carga), 3U);
  (void)armazem_inicia(&a, &ops);
  (void)armazem_grava(&a, carga, sizeof(carga));
  /* Reinício: um armazém novo sobre a mesma flash. */
  VERIFICA(armazem_inicia(&b, &ops) == ARMAZEM_OK);
  VERIFICA(armazem_le(&b, lida, sizeof(lida), &n) == ARMAZEM_OK);
  VERIFICA(n == sizeof(carga) && memcmp(lida, carga, n) == 0);
}

static void as_gravacoes_alternam_os_espacos_e_a_mais_nova_vale(void)
{
  armazem_t a;
  armazem_t b;
  uint8_t c1[20];
  uint8_t c2[30];
  uint8_t c3[40];
  uint8_t lida[ARMAZEM_CARGA_MAX];
  size_t n = 0U;

  zera();
  monta(c1, sizeof(c1), 1U);
  monta(c2, sizeof(c2), 2U);
  monta(c3, sizeof(c3), 3U);
  (void)armazem_inicia(&a, &ops);
  (void)armazem_grava(&a, c1, sizeof(c1));
  VERIFICA(a.ultimo == 0);
  (void)armazem_grava(&a, c2, sizeof(c2));
  VERIFICA(a.ultimo == 1);
  (void)armazem_grava(&a, c3, sizeof(c3));
  VERIFICA(a.ultimo == 0);
  (void)armazem_inicia(&b, &ops);
  VERIFICA(armazem_le(&b, lida, sizeof(lida), &n) == ARMAZEM_OK);
  VERIFICA(n == sizeof(c3) && memcmp(lida, c3, n) == 0);
}

static void queda_de_energia_durante_a_gravacao_mantem_a_carga_anterior(void)
{
  armazem_t a;
  armazem_t b;
  uint8_t antiga[60];
  uint8_t nova[60];
  uint8_t lida[ARMAZEM_CARGA_MAX];
  size_t n = 0U;
  int32_t corte;

  monta(antiga, sizeof(antiga), 10U);
  monta(nova, sizeof(nova), 99U);
  /* Corta a gravação em vários pontos: nenhum pode estragar a carga anterior. */
  for (corte = 0; corte < 80; corte += 4)
  {
    zera();
    (void)armazem_inicia(&a, &ops);
    (void)armazem_grava(&a, antiga, sizeof(antiga));
    limite_de_escrita = corte;
    VERIFICA(armazem_grava(&a, nova, sizeof(nova)) == ARMAZEM_ERRO_FLASH);
    limite_de_escrita = -1;
    (void)armazem_inicia(&b, &ops);
    VERIFICA(armazem_le(&b, lida, sizeof(lida), &n) == ARMAZEM_OK);
    /* Ou a carga anterior ou a nova inteira, nunca uma mistura. A nova só vale se todos os bytes
     * úteis chegaram à flash antes do corte (o resto do registro é preenchimento 0xFF). */
    VERIFICA(n == sizeof(antiga));
    VERIFICA((memcmp(lida, antiga, n) == 0) || (memcmp(lida, nova, n) == 0));
    if (corte < 76)
    {
      VERIFICA(memcmp(lida, antiga, n) == 0);
    }
  }
}

static void apagamento_interrompido_mantem_a_carga_anterior(void)
{
  armazem_t a;
  armazem_t b;
  uint8_t antiga[60];
  uint8_t nova[60];
  uint8_t lida[ARMAZEM_CARGA_MAX];
  size_t n = 0U;

  zera();
  monta(antiga, sizeof(antiga), 10U);
  monta(nova, sizeof(nova), 99U);
  (void)armazem_inicia(&a, &ops);
  (void)armazem_grava(&a, antiga, sizeof(antiga));
  apagar_falha = true;
  VERIFICA(armazem_grava(&a, nova, sizeof(nova)) == ARMAZEM_ERRO_FLASH);
  apagar_falha = false;
  (void)armazem_inicia(&b, &ops);
  VERIFICA(armazem_le(&b, lida, sizeof(lida), &n) == ARMAZEM_OK);
  VERIFICA(n == sizeof(antiga) && memcmp(lida, antiga, n) == 0);
}

static void falha_na_gravacao_nao_muda_o_estado_do_armazem(void)
{
  armazem_t a;
  uint8_t antiga[30];
  uint8_t nova[30];
  uint8_t lida[ARMAZEM_CARGA_MAX];
  size_t n = 0U;

  zera();
  monta(antiga, sizeof(antiga), 1U);
  monta(nova, sizeof(nova), 50U);
  (void)armazem_inicia(&a, &ops);
  (void)armazem_grava(&a, antiga, sizeof(antiga));
  limite_de_escrita = 20;
  VERIFICA(armazem_grava(&a, nova, sizeof(nova)) == ARMAZEM_ERRO_FLASH);
  limite_de_escrita = -1;
  VERIFICA(armazem_le(&a, lida, sizeof(lida), &n) == ARMAZEM_OK);
  VERIFICA(memcmp(lida, antiga, n) == 0);
  /* Gravar de novo depois da falha funciona. */
  VERIFICA(armazem_grava(&a, nova, sizeof(nova)) == ARMAZEM_OK);
  VERIFICA(armazem_le(&a, lida, sizeof(lida), &n) == ARMAZEM_OK);
  VERIFICA(memcmp(lida, nova, n) == 0);
}

static void registro_corrompido_e_ignorado_e_vale_o_anterior(void)
{
  armazem_t a;
  armazem_t b;
  uint8_t c1[40];
  uint8_t c2[40];
  uint8_t lida[ARMAZEM_CARGA_MAX];
  size_t n = 0U;

  zera();
  monta(c1, sizeof(c1), 1U);
  monta(c2, sizeof(c2), 2U);
  (void)armazem_inicia(&a, &ops);
  (void)armazem_grava(&a, c1, sizeof(c1));
  (void)armazem_grava(&a, c2, sizeof(c2)); /* No espaço 1. */
  flash[1][ARMAZEM_CABECALHO + 5U] ^= 0x01U; /* Um bit errado na carga mais nova. */
  (void)armazem_inicia(&b, &ops);
  VERIFICA(b.ultimo == 0);
  VERIFICA(armazem_le(&b, lida, sizeof(lida), &n) == ARMAZEM_OK);
  VERIFICA(n == sizeof(c1) && memcmp(lida, c1, n) == 0);
}

static void os_dois_registros_corrompidos_devolvem_vazio(void)
{
  armazem_t a;
  uint8_t c[20];
  uint8_t lida[ARMAZEM_CARGA_MAX];
  size_t n = 0U;

  zera();
  monta(c, sizeof(c), 1U);
  (void)armazem_inicia(&a, &ops);
  (void)armazem_grava(&a, c, sizeof(c));
  (void)armazem_grava(&a, c, sizeof(c));
  flash[0][ARMAZEM_CABECALHO] ^= 0xFFU;
  flash[1][ARMAZEM_CABECALHO] ^= 0xFFU;
  (void)armazem_inicia(&a, &ops);
  VERIFICA(armazem_le(&a, lida, sizeof(lida), &n) == ARMAZEM_ERRO_VAZIO);
}

static void carga_de_tamanho_maximo_cabe(void)
{
  armazem_t a;
  uint8_t carga[ARMAZEM_CARGA_MAX];
  uint8_t lida[ARMAZEM_CARGA_MAX];
  size_t n = 0U;

  zera();
  monta(carga, sizeof(carga), 5U);
  (void)armazem_inicia(&a, &ops);
  VERIFICA(armazem_grava(&a, carga, sizeof(carga)) == ARMAZEM_OK);
  VERIFICA(armazem_le(&a, lida, sizeof(lida), &n) == ARMAZEM_OK);
  VERIFICA(n == sizeof(carga) && memcmp(lida, carga, n) == 0);
}

static void a_gravacao_e_alinhada_em_8_bytes(void)
{
  armazem_t a;
  uint8_t carga[21];

  zera();
  monta(carga, sizeof(carga), 5U);
  (void)armazem_inicia(&a, &ops);
  (void)armazem_grava(&a, carga, sizeof(carga));
  /* 16 de cabeçalho + 21 de carga = 37, completado a 40 com 0xFF; o resto fica apagado. */
  VERIFICA(flash[0][39] == 0xFFU);
  VERIFICA(flash[0][40] == 0xFFU);
}

static void cada_gravacao_apaga_um_espaco_so(void)
{
  armazem_t a;
  uint8_t c[20];

  zera();
  monta(c, sizeof(c), 1U);
  (void)armazem_inicia(&a, &ops);
  (void)armazem_grava(&a, c, sizeof(c));
  (void)armazem_grava(&a, c, sizeof(c));
  (void)armazem_grava(&a, c, sizeof(c));
  VERIFICA(apagamentos == 3);
}

int main(void)
{
  EXECUTA(rejeita_parametros_invalidos);
  EXECUTA(flash_apagada_devolve_vazio);
  EXECUTA(grava_e_le_a_mesma_carga);
  EXECUTA(buffer_pequeno_devolve_erro_de_tamanho);
  EXECUTA(sobrevive_ao_reinicio);
  EXECUTA(as_gravacoes_alternam_os_espacos_e_a_mais_nova_vale);
  EXECUTA(queda_de_energia_durante_a_gravacao_mantem_a_carga_anterior);
  EXECUTA(apagamento_interrompido_mantem_a_carga_anterior);
  EXECUTA(falha_na_gravacao_nao_muda_o_estado_do_armazem);
  EXECUTA(registro_corrompido_e_ignorado_e_vale_o_anterior);
  EXECUTA(os_dois_registros_corrompidos_devolvem_vazio);
  EXECUTA(carga_de_tamanho_maximo_cabe);
  EXECUTA(a_gravacao_e_alinhada_em_8_bytes);
  EXECUTA(cada_gravacao_apaga_um_espaco_so);
  return RESULTADO();
}
