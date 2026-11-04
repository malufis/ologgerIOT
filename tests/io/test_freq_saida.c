/**
 * @file test_freq_saida.c
 * @brief Testes de unidade da frequência da saída em frequência (FW-023).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "freq_saida.h"
#include "teste.h"

#include <math.h>
#include <stddef.h>

/* Compara floats com tolerância de 0,01 Hz. */
static bool quase(float a, float b)
{
  return fabsf(a - b) < 0.01F;
}

static freq_saida_t cadastrada(float hz4, float hz20)
{
  freq_saida_t f;

  (void)freq_saida_limpa(&f);
  (void)freq_saida_cadastra(&f, hz4, hz20);
  return f;
}

static void rejeita_nulos(void)
{
  freq_saida_t f;
  float hz = 1.0F;

  VERIFICA(freq_saida_limpa(NULL) == FREQ_SAIDA_ERRO_PARAMETRO);
  VERIFICA(freq_saida_cadastra(NULL, 0.0F, 100.0F) == FREQ_SAIDA_ERRO_PARAMETRO);
  f = cadastrada(0.0F, 100.0F);
  VERIFICA(freq_saida_calcula(NULL, 12.0F, false, &hz) == FREQ_SAIDA_ERRO_PARAMETRO);
  VERIFICA(freq_saida_calcula(&f, 12.0F, false, NULL) == FREQ_SAIDA_ERRO_PARAMETRO);
  VERIFICA(freq_saida_calcula(&f, NAN, false, &hz) == FREQ_SAIDA_ERRO_PARAMETRO);
}

static void sem_cadastro_devolve_erro_e_zero(void)
{
  freq_saida_t f;
  float hz = 5.0F;

  (void)freq_saida_limpa(&f);
  VERIFICA(freq_saida_calcula(&f, 12.0F, false, &hz) == FREQ_SAIDA_ERRO_SEM_CADASTRO);
  VERIFICA(hz == 0.0F);
}

static void cadastro_valida_as_frequencias(void)
{
  freq_saida_t f;

  (void)freq_saida_limpa(&f);
  VERIFICA(freq_saida_cadastra(&f, -1.0F, 100.0F) == FREQ_SAIDA_ERRO_FREQUENCIA);
  VERIFICA(freq_saida_cadastra(&f, 0.0F, 20000.1F) == FREQ_SAIDA_ERRO_FREQUENCIA);
  VERIFICA(freq_saida_cadastra(&f, NAN, 100.0F) == FREQ_SAIDA_ERRO_FREQUENCIA);
  VERIFICA(freq_saida_cadastra(&f, 0.0F, INFINITY) == FREQ_SAIDA_ERRO_FREQUENCIA);
  VERIFICA(f.definida == false);
  VERIFICA(freq_saida_cadastra(&f, 0.0F, 20000.0F) == FREQ_SAIDA_OK);
  VERIFICA(f.definida == true);
}

static void cadastro_invalido_mantem_o_anterior(void)
{
  freq_saida_t f = cadastrada(10.0F, 500.0F);

  VERIFICA(freq_saida_cadastra(&f, 10.0F, -5.0F) == FREQ_SAIDA_ERRO_FREQUENCIA);
  VERIFICA(f.hz_4ma == 10.0F && f.hz_20ma == 500.0F);
}

static void extremos_e_ponto_medio_sao_proporcionais(void)
{
  freq_saida_t f = cadastrada(0.0F, 100.0F);
  float hz = -1.0F;

  VERIFICA(freq_saida_calcula(&f, 4.0F, false, &hz) == FREQ_SAIDA_OK && quase(hz, 0.0F));
  VERIFICA(freq_saida_calcula(&f, 12.0F, false, &hz) == FREQ_SAIDA_OK && quase(hz, 50.0F));
  VERIFICA(freq_saida_calcula(&f, 20.0F, false, &hz) == FREQ_SAIDA_OK && quase(hz, 100.0F));
  VERIFICA(freq_saida_calcula(&f, 8.0F, false, &hz) == FREQ_SAIDA_OK && quase(hz, 25.0F));
}

static void frequencia_inicial_diferente_de_zero(void)
{
  freq_saida_t f = cadastrada(100.0F, 1100.0F);
  float hz = 0.0F;

  (void)freq_saida_calcula(&f, 4.0F, false, &hz);
  VERIFICA(quase(hz, 100.0F));
  (void)freq_saida_calcula(&f, 12.0F, false, &hz);
  VERIFICA(quase(hz, 600.0F));
}

static void relacao_invertida_quando_4ma_e_maior(void)
{
  freq_saida_t f = cadastrada(1000.0F, 0.0F);
  float hz = 0.0F;

  (void)freq_saida_calcula(&f, 4.0F, false, &hz);
  VERIFICA(quase(hz, 1000.0F));
  (void)freq_saida_calcula(&f, 12.0F, false, &hz);
  VERIFICA(quase(hz, 500.0F));
  (void)freq_saida_calcula(&f, 20.0F, false, &hz);
  VERIFICA(quase(hz, 0.0F));
}

static void corrente_fora_da_faixa_fica_presa_nos_extremos(void)
{
  freq_saida_t f = cadastrada(0.0F, 100.0F);
  float hz = 0.0F;

  (void)freq_saida_calcula(&f, 0.0F, false, &hz);
  VERIFICA(quase(hz, 0.0F));
  (void)freq_saida_calcula(&f, 25.0F, false, &hz);
  VERIFICA(quase(hz, 100.0F));
}

static void falha_do_sensor_leva_a_saida_a_zero_hz(void)
{
  freq_saida_t f = cadastrada(100.0F, 1100.0F);
  float hz = 7.0F;

  VERIFICA(freq_saida_calcula(&f, 12.0F, true, &hz) == FREQ_SAIDA_OK);
  VERIFICA(hz == 0.0F);
}

int main(void)
{
  EXECUTA(rejeita_nulos);
  EXECUTA(sem_cadastro_devolve_erro_e_zero);
  EXECUTA(cadastro_valida_as_frequencias);
  EXECUTA(cadastro_invalido_mantem_o_anterior);
  EXECUTA(extremos_e_ponto_medio_sao_proporcionais);
  EXECUTA(frequencia_inicial_diferente_de_zero);
  EXECUTA(relacao_invertida_quando_4ma_e_maior);
  EXECUTA(corrente_fora_da_faixa_fica_presa_nos_extremos);
  EXECUTA(falha_do_sensor_leva_a_saida_a_zero_hz);
  return RESULTADO();
}
