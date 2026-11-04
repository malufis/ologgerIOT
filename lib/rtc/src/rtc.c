/**
 * @file rtc.c
 * @brief Implementação do RTC do STM32L412 nos registradores.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "rtc.h"

#include <stddef.h>

#include "hal_stm32.h"
#include "tempo.h"

/** Tempo máximo para o cristal LSE subir, em milissegundos. */
#define LSE_TIMEOUT_MS      3000U
/** Voltas máximas de espera por uma flag do RTC (sincronismo e modo de inicialização). */
#define ESPERA_VOLTAS       200000U
/** Chaves de desbloqueio da proteção de escrita do RTC. */
#define RTC_CHAVE_1         0xCAU
#define RTC_CHAVE_2         0x53U
#define RTC_CHAVE_TRAVA     0xFFU
/** Primeiro ano que o RTC guarda; o registrador tem só dois dígitos. */
#define RTC_ANO_BASE        2000U
/** Último ano que o RTC guarda. */
#define RTC_ANO_FIM         2099U

/** true se o cristal LSE subiu na partida. */
static bool lse_ok;

/**
 * @brief Converte um valor de 0 a 99 em BCD.
 *
 * @param v Valor.
 * @return Valor em BCD, dezena no nibble alto.
 */
static uint32_t para_bcd(uint32_t v)
{
  return ((v / 10U) << 4U) | (v % 10U);
}

/**
 * @brief Converte um byte BCD em valor.
 *
 * @param b BCD, dezena no nibble alto.
 * @return Valor de 0 a 99.
 */
static uint32_t de_bcd(uint32_t b)
{
  return (((b >> 4U) & 0x0FU) * 10U) + (b & 0x0FU);
}

/** Libera a escrita nos registradores protegidos do RTC. */
static void libera_escrita(void)
{
  RTC->WPR = RTC_CHAVE_1;
  RTC->WPR = RTC_CHAVE_2;
}

/** Trava a escrita nos registradores protegidos do RTC. */
static void trava_escrita(void)
{
  RTC->WPR = RTC_CHAVE_TRAVA;
}

/**
 * @brief Espera a flag RSF, que indica que os registradores sombra refletem o calendário.
 *
 * @return true se sincronizou a tempo.
 */
static bool espera_sincronismo(void)
{
  uint32_t voltas = ESPERA_VOLTAS;

  RTC->ICSR &= ~(RTC_ICSR_INIT | RTC_ICSR_RSF);
  while (((RTC->ICSR & RTC_ICSR_RSF) == 0U) && (voltas > 0U))
  {
    voltas--;
  }
  return voltas > 0U;
}

/**
 * @brief Liga o LSE e espera ficar pronto.
 *
 * @return true se o LSE ficou pronto a tempo.
 */
static bool liga_lse(void)
{
  uint32_t inicio = HAL_GetTick();

  SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);
  while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0U)
  {
    if ((HAL_GetTick() - inicio) > LSE_TIMEOUT_MS)
    {
      return false;
    }
  }
  return true;
}

int32_t rtc_inicia(void)
{
  uint32_t fonte;

  __HAL_RCC_PWR_CLK_ENABLE();
  SET_BIT(PWR->CR1, PWR_CR1_DBP);
  SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_RTCAPBEN);
  (void)READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_RTCAPBEN); /* Atraso do clock antes do acesso. */

  lse_ok = liga_lse();
  if (!lse_ok)
  {
    return RTC_ERRO_LSE;
  }

  /* Só escolhe a fonte quando o domínio ainda não tem uma: trocar exigiria reiniciar o domínio
   * de backup, que apagaria o totalizador de pulsos. */
  fonte = (RCC->BDCR & RCC_BDCR_RTCSEL) >> RCC_BDCR_RTCSEL_Pos;
  if (fonte == 0U)
  {
    SET_BIT(RCC->BDCR, RCC_BDCR_RTCSEL_0);
    SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN);
  }
  else if (fonte != 1U)
  {
    return RTC_ERRO_CONFIG;
  }
  else
  {
    SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN);
  }
  return RTC_OK;
}

bool rtc_hora_valida(void)
{
  return lse_ok && ((RTC->ICSR & RTC_ICSR_INITS) != 0U);
}

int32_t rtc_le(uint32_t *epoch_s)
{
  tempo_local_t utc;
  uint32_t tr;
  uint32_t dr;

  if (epoch_s == NULL)
  {
    return RTC_ERRO_PARAMETRO;
  }
  if (!rtc_hora_valida() || !espera_sincronismo())
  {
    return RTC_ERRO_INVALIDA;
  }

  /* Ler TR trava DR até DR ser lido, então os dois formam um instante consistente. */
  tr = RTC->TR;
  dr = RTC->DR;

  utc.ano = (uint16_t)(RTC_ANO_BASE + de_bcd((dr >> 16U) & 0xFFU));
  utc.mes = (uint8_t)de_bcd((dr >> 8U) & 0x1FU);
  utc.dia_mes = (uint8_t)de_bcd(dr & 0x3FU);
  utc.dia_semana = 0U;
  utc.hora = (uint8_t)de_bcd((tr >> 16U) & 0x3FU);
  utc.minuto = (uint8_t)de_bcd((tr >> 8U) & 0x7FU);
  utc.segundo = (uint8_t)de_bcd(tr & 0x7FU);
  return (tempo_de_utc(&utc, epoch_s) == TEMPO_OK) ? RTC_OK : RTC_ERRO_INVALIDA;
}

int32_t rtc_define(uint32_t epoch_s)
{
  tempo_local_t utc;
  uint32_t voltas = ESPERA_VOLTAS;

  if (!lse_ok)
  {
    return RTC_ERRO_LSE;
  }
  if ((tempo_para_local(epoch_s, 0, &utc) != TEMPO_OK) || (utc.ano < RTC_ANO_BASE) ||
      (utc.ano > RTC_ANO_FIM))
  {
    return RTC_ERRO_DATA;
  }

  libera_escrita();
  RTC->ICSR |= RTC_ICSR_INIT;
  while (((RTC->ICSR & RTC_ICSR_INITF) == 0U) && (voltas > 0U))
  {
    voltas--;
  }
  if (voltas == 0U)
  {
    RTC->ICSR &= ~RTC_ICSR_INIT;
    trava_escrita();
    return RTC_ERRO_TEMPO;
  }

  RTC->TR = (para_bcd(utc.hora) << 16U) | (para_bcd(utc.minuto) << 8U) | para_bcd(utc.segundo);
  /* O dia da semana do RTC vai de 1 (segunda) a 7 (domingo). */
  RTC->DR = (para_bcd(utc.ano - RTC_ANO_BASE) << 16U) | ((uint32_t)(utc.dia_semana + 1U) << 13U) |
            (para_bcd(utc.mes) << 8U) | para_bcd(utc.dia_mes);
  RTC->ICSR &= ~RTC_ICSR_INIT;
  trava_escrita();
  return RTC_OK;
}
