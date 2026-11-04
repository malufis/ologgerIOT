/**
 * @file test_config.c
 * @brief Testes de unidade da tabela de configuração do device (mapa Modbus).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include <math.h>
#include <string.h>

#include "config.h"
#include "teste.h"

/* Escreve um float em 2 registradores, palavra alta primeiro. */
static void escreve_float(config_t *c, uint16_t endereco, float f, int32_t esperado)
{
  uint32_t b;
  uint16_t r[2];

  (void)memcpy(&b, &f, sizeof(b));
  r[0] = (uint16_t)(b >> 16U);
  r[1] = (uint16_t)(b & 0xFFFFU);
  VERIFICA(config_escreve_registradores(c, endereco, 2U, r) == esperado);
}

static void rejeita_parametros_invalidos(void)
{
  config_t c;
  uint16_t r[2] = {0U, 0U};

  VERIFICA(config_padroes(NULL) == CONFIG_ERRO_PARAMETRO);
  (void)config_padroes(&c);
  VERIFICA(config_le_registradores(NULL, 40001U, 1U, r) == CONFIG_ERRO_PARAMETRO);
  VERIFICA(config_le_registradores(&c, 40001U, 0U, r) == CONFIG_ERRO_PARAMETRO);
  VERIFICA(config_escreve_registradores(&c, 40001U, 1U, NULL) == CONFIG_ERRO_PARAMETRO);
  VERIFICA(config_inteiro(&c, CFG_QTD, 0U) == 0);
  VERIFICA(config_float(&c, CFG_QTD, 0U) == 0.0F);
}

static void valores_de_fabrica(void)
{
  config_t c;

  (void)config_padroes(&c);
  VERIFICA(config_float(&c, CFG_ENT_SPAN, 0U) == 100.0F);
  VERIFICA(config_float(&c, CFG_ENT_ZERO, 3U) == 0.0F);
  VERIFICA(config_inteiro(&c, CFG_SAI_TIPO, 1U) == 0);
  VERIFICA(config_inteiro(&c, CFG_SAI_ESTAB, 2U) == 300);
  VERIFICA(config_inteiro(&c, CFG_FUSO, 0U) == -180);
  VERIFICA(config_inteiro(&c, CFG_ENDERECO_ESCRAVO, 0U) == 1);
  VERIFICA(config_inteiro(&c, CFG_INATIVIDADE, 0U) == 60);
  VERIFICA(config_inteiro(&c, CFG_MODO_OPERACAO, 0U) == 0);
}

static void le_um_float_com_a_palavra_alta_primeiro(void)
{
  config_t c;
  uint16_t r[2] = {0U, 0U};

  (void)config_padroes(&c);
  /* 100,0 = 0x42C80000. */
  VERIFICA(config_le_registradores(&c, 40001U, 2U, r) == CONFIG_OK);
  VERIFICA(r[0] == 0x42C8U && r[1] == 0x0000U);
}

static void escreve_e_le_floats_de_instancias_diferentes(void)
{
  config_t c;
  uint16_t r[2] = {0U, 0U};

  (void)config_padroes(&c);
  escreve_float(&c, 40003U, 250.5F, CONFIG_OK); /* Span da entrada 2. */
  escreve_float(&c, 40037U, 4.0F, CONFIG_OK);   /* Zero da entrada 4. */
  VERIFICA(config_float(&c, CFG_ENT_SPAN, 1U) == 250.5F);
  VERIFICA(config_float(&c, CFG_ENT_SPAN, 0U) == 100.0F);
  VERIFICA(config_float(&c, CFG_ENT_ZERO, 3U) == 4.0F);
  VERIFICA(config_le_registradores(&c, 40003U, 2U, r) == CONFIG_OK);
  VERIFICA(r[0] == 0x437AU && r[1] == 0x8000U); /* 250,5 = 0x437A8000. */
}

static void escreve_inteiros_dentro_dos_limites(void)
{
  config_t c;
  uint16_t v;

  (void)config_padroes(&c);
  v = 7U;
  VERIFICA(config_escreve_registradores(&c, 40256U, 1U, &v) == CONFIG_OK); /* Retardo da saída 1. */
  VERIFICA(config_inteiro(&c, CFG_SAI_RETARDO, 0U) == 7);
  v = 1U;
  VERIFICA(config_escreve_registradores(&c, 40368U, 1U, &v) == CONFIG_OK); /* Sentido da saída 2. */
  VERIFICA(config_inteiro(&c, CFG_SAI_SENTIDO, 1U) == 1);
  v = 247U;
  VERIFICA(config_escreve_registradores(&c, 40402U, 1U, &v) == CONFIG_OK);
  VERIFICA(config_inteiro(&c, CFG_ENDERECO_ESCRAVO, 0U) == 247);
}

static void fuso_negativo_em_complemento_de_dois(void)
{
  config_t c;
  uint16_t v = (uint16_t)(int16_t)-120;
  uint16_t lido = 0U;

  (void)config_padroes(&c);
  VERIFICA(config_escreve_registradores(&c, 40403U, 1U, &v) == CONFIG_OK);
  VERIFICA(config_inteiro(&c, CFG_FUSO, 0U) == -120);
  VERIFICA(config_le_registradores(&c, 40403U, 1U, &lido) == CONFIG_OK && lido == v);
}

static void valor_fora_do_limite_e_recusado(void)
{
  config_t c;
  uint16_t v;

  (void)config_padroes(&c);
  v = 2U;
  VERIFICA(config_escreve_registradores(&c, 40166U, 1U, &v) == CONFIG_ERRO_VALOR); /* Tipo reservado. */
  v = 0U;
  VERIFICA(config_escreve_registradores(&c, 40402U, 1U, &v) == CONFIG_ERRO_VALOR); /* Escravo 0. */
  v = 248U;
  VERIFICA(config_escreve_registradores(&c, 40402U, 1U, &v) == CONFIG_ERRO_VALOR);
  v = 5U;
  VERIFICA(config_escreve_registradores(&c, 40241U, 1U, &v) == CONFIG_ERRO_VALOR); /* Entrada 5. */
  v = (uint16_t)(int16_t)-721;
  VERIFICA(config_escreve_registradores(&c, 40403U, 1U, &v) == CONFIG_ERRO_VALOR);
  v = 841U;
  VERIFICA(config_escreve_registradores(&c, 40403U, 1U, &v) == CONFIG_ERRO_VALOR);
  VERIFICA(config_inteiro(&c, CFG_SAI_TIPO, 0U) == 0 && config_inteiro(&c, CFG_ENDERECO_ESCRAVO, 0U) == 1);
}

static void float_invalido_e_recusado(void)
{
  config_t c;

  (void)config_padroes(&c);
  escreve_float(&c, 40181U, -1.0F, CONFIG_ERRO_VALOR);     /* Frequência negativa. */
  escreve_float(&c, 40181U, 20001.0F, CONFIG_ERRO_VALOR);  /* Acima de 20 kHz. */
  escreve_float(&c, 40181U, NAN, CONFIG_ERRO_VALOR);
  escreve_float(&c, 40061U, 100.5F, CONFIG_ERRO_VALOR);    /* Offset acima de 100. */
  escreve_float(&c, 40307U, INFINITY, CONFIG_ERRO_VALOR);
  VERIFICA(config_float(&c, CFG_SAI_F4MA, 0U) == 0.0F);
}

static void float_gravado_pela_metade_e_recusado(void)
{
  config_t c;
  uint16_t v = 0x4120U;

  (void)config_padroes(&c);
  VERIFICA(config_escreve_registradores(&c, 40001U, 1U, &v) == CONFIG_ERRO_ENDERECO);
  VERIFICA(config_escreve_registradores(&c, 40002U, 1U, &v) == CONFIG_ERRO_ENDERECO);
  VERIFICA(config_float(&c, CFG_ENT_SPAN, 0U) == 100.0F);
}

static void endereco_fora_da_configuracao_e_recusado(void)
{
  config_t c;
  uint16_t r[2] = {0U, 0U};
  uint16_t v = 1U;

  (void)config_padroes(&c);
  VERIFICA(config_le_registradores(&c, 40005U, 1U, r) == CONFIG_OK); /* Span da entrada 3, 1a palavra. */
  VERIFICA(config_le_registradores(&c, 40009U, 1U, r) == CONFIG_ERRO_ENDERECO); /* Entrada 5 não existe. */
  VERIFICA(config_le_registradores(&c, 30001U, 1U, r) == CONFIG_ERRO_ENDERECO); /* Input register. */
  VERIFICA(config_escreve_registradores(&c, 40271U, 1U, &v) == CONFIG_ERRO_ENDERECO); /* Reservado. */
}

static void pedido_com_um_valor_ruim_nao_altera_nada(void)
{
  config_t c;
  uint16_t v[3];

  (void)config_padroes(&c);
  /* Retardo das saídas 1 e 2 válidos e o da 3... o terceiro registrador é o retardo da saída 3 = 65535 ok;
   * usa o tipo da saída 1 e 2 (40166, 40167) com o segundo inválido. */
  v[0] = 1U;
  v[1] = 2U;
  VERIFICA(config_escreve_registradores(&c, 40167U, 2U, v) == CONFIG_ERRO_VALOR); /* Saída 2 ok, 3 inválida. */
  VERIFICA(config_inteiro(&c, CFG_SAI_TIPO, 1U) == 0);
  VERIFICA(config_inteiro(&c, CFG_SAI_TIPO, 2U) == 0);
}

static void pedido_que_cruza_varios_parametros_vale(void)
{
  config_t c;
  uint16_t v[4] = {1U, 2U, 3U, 4U};
  uint16_t lidos[4] = {0U, 0U, 0U, 0U};

  (void)config_padroes(&c);
  VERIFICA(config_escreve_registradores(&c, 40256U, 4U, v) == CONFIG_OK); /* Retardos das saídas 1 a 4. */
  VERIFICA(config_le_registradores(&c, 40256U, 4U, lidos) == CONFIG_OK);
  VERIFICA(memcmp(v, lidos, sizeof(v)) == 0);
  VERIFICA(config_inteiro(&c, CFG_SAI_RETARDO, 3U) == 4);
}

static void ida_e_volta_pela_persistencia(void)
{
  config_t a;
  config_t b;
  uint8_t carga[CONFIG_TAMANHO_CARGA];
  size_t n = 0U;
  uint16_t v = 12U;

  (void)config_padroes(&a);
  (void)config_escreve_registradores(&a, 40258U, 1U, &v);
  escreve_float(&a, 40313U, 5.0F, CONFIG_OK);
  v = (uint16_t)(int16_t)-60;
  (void)config_escreve_registradores(&a, 40403U, 1U, &v);
  VERIFICA(config_empacota(&a, carga, sizeof(carga), &n) == CONFIG_OK && n == CONFIG_TAMANHO_CARGA);
  (void)config_padroes(&b);
  VERIFICA(config_desempacota(&b, carga, n) == CONFIG_OK);
  VERIFICA(memcmp(&a, &b, sizeof(a)) == 0);
  VERIFICA(config_inteiro(&b, CFG_SAI_RETARDO, 2U) == 12);
  VERIFICA(config_float(&b, CFG_SAI_LIM_MAX, 3U) == 5.0F);
  VERIFICA(config_inteiro(&b, CFG_FUSO, 0U) == -60);
}

static void empacota_exige_buffer_grande_o_bastante(void)
{
  config_t c;
  uint8_t carga[CONFIG_TAMANHO_CARGA];
  size_t n = 0U;

  (void)config_padroes(&c);
  VERIFICA(config_empacota(&c, carga, CONFIG_TAMANHO_CARGA - 1U, &n) == CONFIG_ERRO_TAMANHO);
  VERIFICA(config_empacota(NULL, carga, sizeof(carga), &n) == CONFIG_ERRO_PARAMETRO);
}

static void valor_gravado_invalido_volta_ao_padrao(void)
{
  config_t a;
  config_t b;
  uint8_t carga[CONFIG_TAMANHO_CARGA];
  size_t n = 0U;

  (void)config_padroes(&a);
  (void)config_empacota(&a, carga, sizeof(carga), &n);
  /* Estraga o endereço do escravo (índice 54) e o span da entrada 1 (índice 0). */
  carga[4U + (54U * 4U)] = 0xFFU;
  carga[5U + (54U * 4U)] = 0xFFU;
  carga[4U + 0U] = 0xFFU;
  carga[5U + 0U] = 0xFFU;
  carga[6U + 0U] = 0xFFU;
  carga[7U + 0U] = 0x7FU; /* NaN. */
  (void)config_padroes(&b);
  VERIFICA(config_desempacota(&b, carga, n) == CONFIG_OK);
  VERIFICA(config_inteiro(&b, CFG_ENDERECO_ESCRAVO, 0U) == 1);
  VERIFICA(config_float(&b, CFG_ENT_SPAN, 0U) == 100.0F);
}

static void versao_diferente_e_recusada_sem_alterar(void)
{
  config_t a;
  config_t b;
  uint8_t carga[CONFIG_TAMANHO_CARGA];
  size_t n = 0U;
  uint16_t v = 9U;

  (void)config_padroes(&a);
  (void)config_empacota(&a, carga, sizeof(carga), &n);
  carga[0] = 2U;
  (void)config_padroes(&b);
  (void)config_escreve_registradores(&b, 40256U, 1U, &v);
  VERIFICA(config_desempacota(&b, carga, n) == CONFIG_ERRO_VERSAO);
  VERIFICA(config_inteiro(&b, CFG_SAI_RETARDO, 0U) == 9);
  VERIFICA(config_desempacota(&b, carga, 2U) == CONFIG_ERRO_PARAMETRO);
}

static void carga_com_menos_parametros_completa_com_padroes(void)
{
  config_t a;
  config_t b;
  uint8_t carga[CONFIG_TAMANHO_CARGA];
  size_t n = 0U;
  uint16_t v = 33U;

  (void)config_padroes(&a);
  (void)config_escreve_registradores(&a, 40256U, 1U, &v); /* Retardo da saída 1, índice 32. */
  (void)config_empacota(&a, carga, sizeof(carga), &n);
  /* Simula uma versão antiga com só 40 valores: ajusta a quantidade e o tamanho. */
  carga[2] = 40U;
  carga[3] = 0U;
  (void)config_padroes(&b);
  VERIFICA(config_desempacota(&b, carga, 4U + (40U * 4U)) == CONFIG_OK);
  VERIFICA(config_inteiro(&b, CFG_SAI_RETARDO, 0U) == 33);   /* Índice 32 < 40: carregado. */
  VERIFICA(config_inteiro(&b, CFG_FUSO, 0U) == -180);        /* Índice 55 >= 40: padrão. */
  VERIFICA(config_desempacota(&b, carga, 4U + (41U * 4U)) == CONFIG_ERRO_TAMANHO);
}

static void saida_1_nao_aceita_frequencia(void)
{
  config_t c;
  uint16_t v = 1U;

  (void)config_padroes(&c);
  VERIFICA(config_escreve_registradores(&c, 40166U, 1U, &v) == CONFIG_ERRO_CONFLITO);
  VERIFICA(config_inteiro(&c, CFG_SAI_TIPO, 0U) == 0);
  VERIFICA(config_escreve_registradores(&c, 40167U, 1U, &v) == CONFIG_OK); /* Saída 2. */
  VERIFICA(config_inteiro(&c, CFG_SAI_TIPO, 1U) == 1);
}

static void frequencia_e_sleep_se_excluem_nos_dois_sentidos(void)
{
  config_t c;
  uint16_t v = 1U;

  (void)config_padroes(&c);
  (void)config_escreve_registradores(&c, 40168U, 1U, &v); /* Saída 3 em frequência. */
  VERIFICA(config_escreve_registradores(&c, 40412U, 1U, &v) == CONFIG_ERRO_CONFLITO); /* Sleep: recusado. */
  VERIFICA(config_inteiro(&c, CFG_MODO_OPERACAO, 0U) == 0);
  v = 0U;
  (void)config_escreve_registradores(&c, 40168U, 1U, &v); /* Volta a discreta. */
  v = 1U;
  VERIFICA(config_escreve_registradores(&c, 40412U, 1U, &v) == CONFIG_OK); /* Sleep: aceito. */
  VERIFICA(config_escreve_registradores(&c, 40168U, 1U, &v) == CONFIG_ERRO_CONFLITO); /* Frequência: recusada. */
  VERIFICA(config_inteiro(&c, CFG_SAI_TIPO, 2U) == 0);
}

static void pedido_unico_que_troca_sleep_e_frequencia_na_ordem_certa_vale(void)
{
  config_t c;
  uint16_t v[2];

  (void)config_padroes(&c);
  v[0] = 1U;
  (void)config_escreve_registradores(&c, 40412U, 1U, v); /* Sleep ligado. */
  /* Uma só escrita que desliga o sleep e liga a frequência na saída 2 não é possível (endereços
   * distantes), mas escrever só o sleep desligado e depois a frequência é. */
  v[0] = 0U;
  VERIFICA(config_escreve_registradores(&c, 40412U, 1U, v) == CONFIG_OK);
  v[0] = 1U;
  VERIFICA(config_escreve_registradores(&c, 40167U, 1U, v) == CONFIG_OK);
  VERIFICA(config_conflito(&c) == false);
}

static void conflito_na_carga_gravada_desliga_o_sleep(void)
{
  config_t a;
  config_t b;
  uint8_t carga[CONFIG_TAMANHO_CARGA];
  size_t n = 0U;

  (void)config_padroes(&a);
  (void)config_empacota(&a, carga, sizeof(carga), &n);
  carga[4U + (17U * 4U)] = 1U; /* Saída 2 em frequência (índice 17). */
  carga[4U + (58U * 4U)] = 1U; /* Sleep ligado (índice 58). */
  (void)config_padroes(&b);
  VERIFICA(config_desempacota(&b, carga, n) == CONFIG_OK);
  VERIFICA(config_inteiro(&b, CFG_SAI_TIPO, 1U) == 1 && config_inteiro(&b, CFG_MODO_OPERACAO, 0U) == 0);
  VERIFICA(config_conflito(&b) == false);
}

static void saida_1_em_frequencia_na_carga_gravada_descarta_tudo(void)
{
  config_t a;
  config_t b;
  uint8_t carga[CONFIG_TAMANHO_CARGA];
  size_t n = 0U;
  uint16_t v = 9U;

  (void)config_padroes(&a);
  (void)config_escreve_registradores(&a, 40256U, 1U, &v);
  (void)config_empacota(&a, carga, sizeof(carga), &n);
  carga[4U + (16U * 4U)] = 1U; /* Saída 1 em frequência (índice 16). */
  (void)config_padroes(&b);
  VERIFICA(config_desempacota(&b, carga, n) == CONFIG_OK);
  VERIFICA(config_inteiro(&b, CFG_SAI_TIPO, 0U) == 0);
  VERIFICA(config_inteiro(&b, CFG_SAI_RETARDO, 0U) == 0); /* Tudo voltou ao padrão. */
}

static void mudou_compara_uma_instancia(void)
{
  config_t a;
  config_t b;
  uint16_t v = 4U;

  (void)config_padroes(&a);
  b = a;
  VERIFICA(config_mudou(&a, &b, CFG_SAI_RETARDO, 1U) == false);
  (void)config_escreve_registradores(&b, 40257U, 1U, &v);
  VERIFICA(config_mudou(&a, &b, CFG_SAI_RETARDO, 1U) == true);
  VERIFICA(config_mudou(&a, &b, CFG_SAI_RETARDO, 0U) == false);
  VERIFICA(config_mudou(&a, &b, CFG_SAI_RETARDO, 9U) == false);
  VERIFICA(config_mudou(NULL, &b, CFG_SAI_RETARDO, 0U) == false);
}

int main(void)
{
  EXECUTA(rejeita_parametros_invalidos);
  EXECUTA(valores_de_fabrica);
  EXECUTA(le_um_float_com_a_palavra_alta_primeiro);
  EXECUTA(escreve_e_le_floats_de_instancias_diferentes);
  EXECUTA(escreve_inteiros_dentro_dos_limites);
  EXECUTA(fuso_negativo_em_complemento_de_dois);
  EXECUTA(valor_fora_do_limite_e_recusado);
  EXECUTA(float_invalido_e_recusado);
  EXECUTA(float_gravado_pela_metade_e_recusado);
  EXECUTA(endereco_fora_da_configuracao_e_recusado);
  EXECUTA(pedido_com_um_valor_ruim_nao_altera_nada);
  EXECUTA(pedido_que_cruza_varios_parametros_vale);
  EXECUTA(ida_e_volta_pela_persistencia);
  EXECUTA(empacota_exige_buffer_grande_o_bastante);
  EXECUTA(valor_gravado_invalido_volta_ao_padrao);
  EXECUTA(versao_diferente_e_recusada_sem_alterar);
  EXECUTA(carga_com_menos_parametros_completa_com_padroes);
  EXECUTA(saida_1_nao_aceita_frequencia);
  EXECUTA(frequencia_e_sleep_se_excluem_nos_dois_sentidos);
  EXECUTA(pedido_unico_que_troca_sleep_e_frequencia_na_ordem_certa_vale);
  EXECUTA(conflito_na_carga_gravada_desliga_o_sleep);
  EXECUTA(saida_1_em_frequencia_na_carga_gravada_descarta_tudo);
  EXECUTA(mudou_compara_uma_instancia);
  return RESULTADO();
}
