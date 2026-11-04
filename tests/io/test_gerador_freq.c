/**
 * @file test_gerador_freq.c
 * @brief Testes de unidade do gerador de frequência por acumulador de fase (FW-023).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "gerador_freq.h"
#include "teste.h"

#include <math.h>
#include <stddef.h>

/* Resultado de rodar o gerador por um número de ticks. */
typedef struct
{
  uint32_t subidas; /* Bordas de subida. */
  uint32_t altos;   /* Ticks em nível alto. */
} medida_t;

static medida_t roda(gerador_freq_t *g, uint32_t ticks)
{
  medida_t m = {0U, 0U};
  bool anterior = false;
  uint32_t i;

  for (i = 0U; i < ticks; i++)
  {
    bool nivel = gerador_freq_tick(g);

    if (nivel && !anterior)
    {
      m.subidas++;
    }
    if (nivel)
    {
      m.altos++;
    }
    anterior = nivel;
  }
  return m;
}

static void rejeita_nulo_e_frequencia_invalida(void)
{
  gerador_freq_t g;

  VERIFICA(gerador_freq_limpa(NULL) == GERADOR_FREQ_ERRO_PARAMETRO);
  VERIFICA(gerador_freq_define(NULL, 100.0F) == GERADOR_FREQ_ERRO_PARAMETRO);
  VERIFICA(gerador_freq_tick(NULL) == false);
  (void)gerador_freq_limpa(&g);
  VERIFICA(gerador_freq_define(&g, -1.0F) == GERADOR_FREQ_ERRO_FREQUENCIA);
  VERIFICA(gerador_freq_define(&g, 20000.5F) == GERADOR_FREQ_ERRO_FREQUENCIA);
  VERIFICA(gerador_freq_define(&g, NAN) == GERADOR_FREQ_ERRO_FREQUENCIA);
  VERIFICA(gerador_freq_define(&g, INFINITY) == GERADOR_FREQ_ERRO_FREQUENCIA);
  VERIFICA(g.passo == 0U);
}

static void zero_hz_fica_em_nivel_baixo(void)
{
  gerador_freq_t g;
  medida_t m;

  (void)gerador_freq_limpa(&g);
  (void)gerador_freq_define(&g, 0.0F);
  m = roda(&g, GERADOR_FREQ_TICK_HZ);
  VERIFICA(m.subidas == 0U && m.altos == 0U);
}

static void mil_hz_gera_mil_bordas_por_segundo_com_duty_de_50(void)
{
  gerador_freq_t g;
  medida_t m;

  (void)gerador_freq_limpa(&g);
  VERIFICA(gerador_freq_define(&g, 1000.0F) == GERADOR_FREQ_OK);
  m = roda(&g, GERADOR_FREQ_TICK_HZ);
  VERIFICA(m.subidas >= 999U && m.subidas <= 1001U);
  VERIFICA(m.altos >= 49900U && m.altos <= 50100U);
}

static void limite_de_20_khz(void)
{
  gerador_freq_t g;
  medida_t m;

  (void)gerador_freq_limpa(&g);
  VERIFICA(gerador_freq_define(&g, 20000.0F) == GERADOR_FREQ_OK);
  m = roda(&g, GERADOR_FREQ_TICK_HZ);
  VERIFICA(m.subidas >= 19999U && m.subidas <= 20001U);
  /* Com cinco ticks por período o duty só pode ser 40% ou 60%: aqui dá 60%. */
  VERIFICA(m.altos >= 39000U && m.altos <= 61000U);
}

static void frequencia_baixa_e_fracionaria(void)
{
  gerador_freq_t g;
  medida_t m;

  (void)gerador_freq_limpa(&g);
  VERIFICA(gerador_freq_define(&g, 0.5F) == GERADOR_FREQ_OK);
  m = roda(&g, GERADOR_FREQ_TICK_HZ * 10U);
  VERIFICA(m.subidas >= 4U && m.subidas <= 6U);
  VERIFICA(m.altos >= 499000U && m.altos <= 501000U);
}

static void frequencias_variadas_tem_media_exata(void)
{
  static const float freqs[] = {1.0F, 10.0F, 60.0F, 123.4F, 440.0F, 2500.0F, 9999.0F, 15000.0F};
  gerador_freq_t g;
  medida_t m;
  uint32_t i;

  for (i = 0U; i < (sizeof(freqs) / sizeof(freqs[0])); i++)
  {
    (void)gerador_freq_limpa(&g);
    (void)gerador_freq_define(&g, freqs[i]);
    m = roda(&g, GERADOR_FREQ_TICK_HZ * 10U);
    /* Em 10 s: freq * 10 bordas, com folga de uma. */
    VERIFICA(fabsf((float)m.subidas - (freqs[i] * 10.0F)) <= 1.5F);
  }
}

static void mudar_a_frequencia_mantem_a_fase_sem_salto(void)
{
  gerador_freq_t g;
  uint32_t fase;

  (void)gerador_freq_limpa(&g);
  (void)gerador_freq_define(&g, 1000.0F);
  (void)roda(&g, 37U);
  fase = g.fase;
  (void)gerador_freq_define(&g, 2000.0F);
  VERIFICA(g.fase == fase);
}

static void parar_leva_a_saida_a_nivel_baixo_e_zera_a_fase(void)
{
  gerador_freq_t g;

  (void)gerador_freq_limpa(&g);
  (void)gerador_freq_define(&g, 1000.0F);
  (void)roda(&g, 37U);
  (void)gerador_freq_define(&g, 0.0F);
  VERIFICA(gerador_freq_tick(&g) == false);
  VERIFICA(g.fase == 0U && g.passo == 0U);
}

static void frequencia_invalida_mantem_o_gerador_como_estava(void)
{
  gerador_freq_t g;
  uint32_t passo;

  (void)gerador_freq_limpa(&g);
  (void)gerador_freq_define(&g, 500.0F);
  passo = g.passo;
  VERIFICA(gerador_freq_define(&g, 30000.0F) == GERADOR_FREQ_ERRO_FREQUENCIA);
  VERIFICA(g.passo == passo);
}

static void geradores_sao_independentes(void)
{
  gerador_freq_t a;
  gerador_freq_t b;
  medida_t ma;
  medida_t mb;

  (void)gerador_freq_limpa(&a);
  (void)gerador_freq_limpa(&b);
  (void)gerador_freq_define(&a, 300.0F);
  (void)gerador_freq_define(&b, 7000.0F);
  ma = roda(&a, GERADOR_FREQ_TICK_HZ);
  mb = roda(&b, GERADOR_FREQ_TICK_HZ);
  VERIFICA(ma.subidas >= 299U && ma.subidas <= 301U);
  VERIFICA(mb.subidas >= 6999U && mb.subidas <= 7001U);
}

int main(void)
{
  EXECUTA(rejeita_nulo_e_frequencia_invalida);
  EXECUTA(zero_hz_fica_em_nivel_baixo);
  EXECUTA(mil_hz_gera_mil_bordas_por_segundo_com_duty_de_50);
  EXECUTA(limite_de_20_khz);
  EXECUTA(frequencia_baixa_e_fracionaria);
  EXECUTA(frequencias_variadas_tem_media_exata);
  EXECUTA(mudar_a_frequencia_mantem_a_fase_sem_salto);
  EXECUTA(parar_leva_a_saida_a_nivel_baixo_e_zera_a_fase);
  EXECUTA(frequencia_invalida_mantem_o_gerador_como_estava);
  EXECUTA(geradores_sao_independentes);
  return RESULTADO();
}
