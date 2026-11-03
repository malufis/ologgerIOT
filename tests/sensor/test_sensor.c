/**
 * @file test_sensor.c
 * @brief Testes de unidade do módulo sensor (FW-010 e FW-011).
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#include "sensor.h"
#include "teste.h"

/* Alimenta o acumulador com uma sequência e devolve a média do primeiro segundo. */
static float media_da_sequencia(const uint16_t *amostras, uint16_t quantidade, uint16_t taxa)
{
  sensor_media_t media;
  bool pronta = false;
  float resultado = -1.0f;
  uint16_t i;

  (void)sensor_media_inicia(&media, taxa);
  for (i = 0U; i < quantidade; i++)
  {
    (void)sensor_media_acumula(&media, amostras[i], &pronta, &resultado);
  }
  return pronta ? resultado : -1.0f;
}

static void media_de_valor_constante(void)
{
  uint16_t amostras[10];
  uint16_t i;

  for (i = 0U; i < 10U; i++)
  {
    amostras[i] = 2000U;
  }
  VERIFICA_PROXIMO(media_da_sequencia(amostras, 10U, 10U), 2000.0, 0.001);
}

static void media_de_rampa_sintetica(void)
{
  /* 0, 10, 20 ... 90 tem média 45. */
  uint16_t amostras[10];
  uint16_t i;

  for (i = 0U; i < 10U; i++)
  {
    amostras[i] = (uint16_t)(i * 10U);
  }
  VERIFICA_PROXIMO(media_da_sequencia(amostras, 10U, 10U), 45.0, 0.001);
}

static void media_mantem_parte_fracionaria(void)
{
  /* 1 e 2 em 10 amostras: cinco de cada tem média 1,5. */
  uint16_t amostras[10] = {1U, 2U, 1U, 2U, 1U, 2U, 1U, 2U, 1U, 2U};

  VERIFICA_PROXIMO(media_da_sequencia(amostras, 10U, 10U), 1.5, 0.001);
}

static void media_so_fecha_na_ultima_amostra(void)
{
  sensor_media_t media;
  bool pronta = true;
  float resultado = 0.0f;
  uint16_t i;

  (void)sensor_media_inicia(&media, 10U);
  for (i = 0U; i < 9U; i++)
  {
    (void)sensor_media_acumula(&media, 100U, &pronta, &resultado);
    VERIFICA(!pronta);
  }
  (void)sensor_media_acumula(&media, 100U, &pronta, &resultado);
  VERIFICA(pronta);
}

static void media_reinicia_depois_de_fechar_o_segundo(void)
{
  sensor_media_t media;
  bool pronta = false;
  float resultado = 0.0f;
  uint16_t i;

  (void)sensor_media_inicia(&media, 10U);
  for (i = 0U; i < 10U; i++)
  {
    (void)sensor_media_acumula(&media, 1000U, &pronta, &resultado);
  }
  /* Segundo segundo, com outro valor: não pode herdar a soma do primeiro. */
  for (i = 0U; i < 10U; i++)
  {
    (void)sensor_media_acumula(&media, 3000U, &pronta, &resultado);
  }
  VERIFICA(pronta);
  VERIFICA_PROXIMO(resultado, 3000.0, 0.001);
}

static void media_na_taxa_maxima_nao_estoura(void)
{
  /* 1000 amostras de 4095: soma 4.095.000, cabe em 32 bits. */
  sensor_media_t media;
  bool pronta = false;
  float resultado = 0.0f;
  uint16_t i;

  (void)sensor_media_inicia(&media, 1000U);
  for (i = 0U; i < 1000U; i++)
  {
    (void)sensor_media_acumula(&media, 4095U, &pronta, &resultado);
  }
  VERIFICA(pronta);
  VERIFICA_PROXIMO(resultado, 4095.0, 0.001);
}

static void taxa_aceita_somente_de_10_a_1000(void)
{
  sensor_media_t media;

  VERIFICA(sensor_media_inicia(&media, 9U) == SENSOR_ERRO_TAXA);
  VERIFICA(sensor_media_inicia(&media, 10U) == SENSOR_OK);
  VERIFICA(sensor_media_inicia(&media, 1000U) == SENSOR_OK);
  VERIFICA(sensor_media_inicia(&media, 1001U) == SENSOR_ERRO_TAXA);
  VERIFICA(sensor_media_inicia(&media, 0U) == SENSOR_ERRO_TAXA);
}

static void media_recusa_parametro_nulo(void)
{
  sensor_media_t media;
  bool pronta = false;
  float resultado = 0.0f;

  VERIFICA(sensor_media_inicia(NULL, 10U) == SENSOR_ERRO_PARAMETRO);
  (void)sensor_media_inicia(&media, 10U);
  VERIFICA(sensor_media_acumula(NULL, 1U, &pronta, &resultado) == SENSOR_ERRO_PARAMETRO);
  VERIFICA(sensor_media_acumula(&media, 1U, NULL, &resultado) == SENSOR_ERRO_PARAMETRO);
  VERIFICA(sensor_media_acumula(&media, 1U, &pronta, NULL) == SENSOR_ERRO_PARAMETRO);
}

static void media_sem_inicio_e_recusada(void)
{
  sensor_media_t media = {0U, 0U, 0U};
  bool pronta = false;
  float resultado = 0.0f;

  VERIFICA(sensor_media_acumula(&media, 1U, &pronta, &resultado) == SENSOR_ERRO_PARAMETRO);
}

static void contagens_para_ma_nos_pontos_da_escala(void)
{
  /* Shunt de 150 ohms e referência de 3,3 V: 4 mA = 0,6 V, 20 mA = 3,0 V. */
  float ma = 0.0f;
  float contagens_4ma = (0.6f / 3.3f) * 4095.0f;
  float contagens_20ma = (3.0f / 3.3f) * 4095.0f;

  VERIFICA(sensor_contagens_para_ma(contagens_4ma, 3.3f, 150.0f, &ma) == SENSOR_OK);
  VERIFICA_PROXIMO(ma, 4.0, 0.001);
  VERIFICA(sensor_contagens_para_ma(contagens_20ma, 3.3f, 150.0f, &ma) == SENSOR_OK);
  VERIFICA_PROXIMO(ma, 20.0, 0.001);
}

static void contagens_para_ma_zero_e_fundo_de_escala(void)
{
  float ma = -1.0f;

  VERIFICA(sensor_contagens_para_ma(0.0f, 3.3f, 150.0f, &ma) == SENSOR_OK);
  VERIFICA_PROXIMO(ma, 0.0, 0.0001);
  /* 4095 contagens = 3,3 V sobre 150 ohms = 22 mA. */
  VERIFICA(sensor_contagens_para_ma(4095.0f, 3.3f, 150.0f, &ma) == SENSOR_OK);
  VERIFICA_PROXIMO(ma, 22.0, 0.001);
}

static void contagens_para_ma_com_shunt_de_120_ohms(void)
{
  /* Se o resistor montado for 120 ohms, 20 mA dá 2,4 V. */
  float ma = 0.0f;
  float contagens = (2.4f / 3.3f) * 4095.0f;

  VERIFICA(sensor_contagens_para_ma(contagens, 3.3f, 120.0f, &ma) == SENSOR_OK);
  VERIFICA_PROXIMO(ma, 20.0, 0.001);
}

static void contagens_para_ma_recusa_parametros_invalidos(void)
{
  float ma = 0.0f;

  VERIFICA(sensor_contagens_para_ma(100.0f, 3.3f, 150.0f, NULL) == SENSOR_ERRO_PARAMETRO);
  VERIFICA(sensor_contagens_para_ma(100.0f, 0.0f, 150.0f, &ma) == SENSOR_ERRO_PARAMETRO);
  VERIFICA(sensor_contagens_para_ma(100.0f, 3.3f, 0.0f, &ma) == SENSOR_ERRO_PARAMETRO);
  VERIFICA(sensor_contagens_para_ma(-1.0f, 3.3f, 150.0f, &ma) == SENSOR_ERRO_PARAMETRO);
  VERIFICA(sensor_contagens_para_ma(4096.0f, 3.3f, 150.0f, &ma) == SENSOR_ERRO_PARAMETRO);
}

static void valor_de_sensor_de_0_a_10_bar(void)
{
  /* Sensor 0-10 bar: zero 0, span 10. 4 mA = 0, 12 mA = 5, 20 mA = 10. */
  float valor = -1.0f;

  VERIFICA(sensor_ma_para_valor(4.0f, 0.0f, 10.0f, 0.0f, &valor) == SENSOR_OK);
  VERIFICA_PROXIMO(valor, 0.0, 0.0001);
  VERIFICA(sensor_ma_para_valor(12.0f, 0.0f, 10.0f, 0.0f, &valor) == SENSOR_OK);
  VERIFICA_PROXIMO(valor, 5.0, 0.0001);
  VERIFICA(sensor_ma_para_valor(20.0f, 0.0f, 10.0f, 0.0f, &valor) == SENSOR_OK);
  VERIFICA_PROXIMO(valor, 10.0, 0.0001);
}

static void valor_com_offset_ajusta_o_zero(void)
{
  float valor = 0.0f;

  /* Offset de +0,2 bar desloca toda a faixa. */
  (void)sensor_ma_para_valor(4.0f, 0.0f, 10.0f, 0.2f, &valor);
  VERIFICA_PROXIMO(valor, 0.2, 0.0001);
  (void)sensor_ma_para_valor(20.0f, 0.0f, 10.0f, 0.2f, &valor);
  VERIFICA_PROXIMO(valor, 10.2, 0.0001);
}

static void valor_com_zero_diferente_de_zero(void)
{
  /* Sensor de nível de 2 a 12 m: 4 mA = 2, 20 mA = 12, 12 mA = 7. */
  float valor = 0.0f;

  (void)sensor_ma_para_valor(4.0f, 2.0f, 12.0f, 0.0f, &valor);
  VERIFICA_PROXIMO(valor, 2.0, 0.0001);
  (void)sensor_ma_para_valor(12.0f, 2.0f, 12.0f, 0.0f, &valor);
  VERIFICA_PROXIMO(valor, 7.0, 0.0001);
  (void)sensor_ma_para_valor(20.0f, 2.0f, 12.0f, 0.0f, &valor);
  VERIFICA_PROXIMO(valor, 12.0, 0.0001);
}

static void valor_nao_limita_fora_da_faixa(void)
{
  /* 3 mA e 21 mA saem da escala: a conversão não corta, a falha é tratada fora. */
  float valor = 0.0f;

  (void)sensor_ma_para_valor(3.0f, 0.0f, 10.0f, 0.0f, &valor);
  VERIFICA_PROXIMO(valor, -0.625, 0.0001);
  (void)sensor_ma_para_valor(21.0f, 0.0f, 10.0f, 0.0f, &valor);
  VERIFICA_PROXIMO(valor, 10.625, 0.0001);
}

static void valor_recusa_saida_nula(void)
{
  VERIFICA(sensor_ma_para_valor(12.0f, 0.0f, 10.0f, 0.0f, NULL) == SENSOR_ERRO_PARAMETRO);
}

static void cadeia_completa_contagens_ate_valor(void)
{
  /* 12 mA no shunt de 150 ohms = 1,8 V = 2234 contagens (3,3 V): 5 bar. */
  float ma = 0.0f;
  float valor = 0.0f;
  float contagens = (1.8f / 3.3f) * 4095.0f;

  (void)sensor_contagens_para_ma(contagens, 3.3f, 150.0f, &ma);
  (void)sensor_ma_para_valor(ma, 0.0f, 10.0f, 0.0f, &valor);
  VERIFICA_PROXIMO(valor, 5.0, 0.001);
}

int main(void)
{
  EXECUTA(media_de_valor_constante);
  EXECUTA(media_de_rampa_sintetica);
  EXECUTA(media_mantem_parte_fracionaria);
  EXECUTA(media_so_fecha_na_ultima_amostra);
  EXECUTA(media_reinicia_depois_de_fechar_o_segundo);
  EXECUTA(media_na_taxa_maxima_nao_estoura);
  EXECUTA(taxa_aceita_somente_de_10_a_1000);
  EXECUTA(media_recusa_parametro_nulo);
  EXECUTA(media_sem_inicio_e_recusada);
  EXECUTA(contagens_para_ma_nos_pontos_da_escala);
  EXECUTA(contagens_para_ma_zero_e_fundo_de_escala);
  EXECUTA(contagens_para_ma_com_shunt_de_120_ohms);
  EXECUTA(contagens_para_ma_recusa_parametros_invalidos);
  EXECUTA(valor_de_sensor_de_0_a_10_bar);
  EXECUTA(valor_com_offset_ajusta_o_zero);
  EXECUTA(valor_com_zero_diferente_de_zero);
  EXECUTA(valor_nao_limita_fora_da_faixa);
  EXECUTA(valor_recusa_saida_nula);
  EXECUTA(cadeia_completa_contagens_ate_valor);
  return RESULTADO();
}
