/**
 * @file rtc.h
 * @brief Relógio de tempo real do STM32L412 (RTC com o cristal de 32,768 kHz).
 *
 * Mantém a data e a hora em **UTC**, em segundos desde 01/01/1970; o fuso da instalação é
 * aplicado por quem lê (tempo.h). O RTC usa o cristal LSE do OloggerMulti e continua correndo em
 * Stop 2, então a hora não se perde quando o tick do FreeRTOS para.
 *
 * - **Hora válida:** só depois de rtc_define() (por exemplo, pelo NTP). Depois de uma perda total de
 *   energia sem bateria no VBAT, o RTC volta sem data e a hora é inválida até a próxima
 *   atualização (FW-031). Com bateria opcional no VBAT, a hora sobrevive à falta de energia.
 * - **Cristal:** se o LSE não subir (cristal ausente ou com defeito), rtc_inicia() devolve erro e
 *   a hora fica inválida; o firmware segue funcionando sem relógio.
 * - **Backup:** não reinicia o domínio de backup, para não apagar o totalizador de pulsos
 *   (registradores TAMP). Só configura o RTC quando o domínio ainda não tem fonte de relógio.
 *
 * Só o STM32L412 (build `ologgermulti`); o y7080 não tem cristal. O RTC aceita anos de 2000 a
 * 2099. Sem teste em hardware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef RTC_H
#define RTC_H

#include <stdbool.h>
#include <stdint.h>

/** Código de retorno: sucesso. */
#define RTC_OK               0
/** Código de retorno: parâmetro nulo. */
#define RTC_ERRO_PARAMETRO   (-1)
/** Código de retorno: o cristal LSE não subiu a tempo. */
#define RTC_ERRO_LSE         (-2)
/** Código de retorno: o domínio de backup já usa outra fonte de relógio, que não foi alterada. */
#define RTC_ERRO_CONFIG      (-3)
/** Código de retorno: a hora não é válida. */
#define RTC_ERRO_INVALIDA    (-4)
/** Código de retorno: data fora de 2000 a 2099 ou inexistente. */
#define RTC_ERRO_DATA        (-5)
/** Código de retorno: o RTC não entrou em modo de inicialização. */
#define RTC_ERRO_TEMPO       (-6)

/**
 * @brief Liga o cristal LSE e o RTC, sem alterar a hora que o RTC já tem.
 *
 * @return RTC_OK, RTC_ERRO_LSE ou RTC_ERRO_CONFIG.
 * @note Chamar uma vez na partida, com o tick do HAL correndo. Demora até 3 s se o cristal não subir.
 */
int32_t rtc_inicia(void);

/**
 * @brief Informa se o RTC tem uma hora válida.
 *
 * @return true se o LSE subiu e o RTC já recebeu data e hora.
 */
bool rtc_hora_valida(void);

/**
 * @brief Lê a hora atual em UTC.
 *
 * @param epoch_s Recebe os segundos desde 01/01/1970 00:00:00 UTC.
 * @return RTC_OK, RTC_ERRO_PARAMETRO ou RTC_ERRO_INVALIDA.
 */
int32_t rtc_le(uint32_t *epoch_s);

/**
 * @brief Acerta o RTC com a hora UTC dada (por exemplo, a do servidor NTP).
 *
 * @param epoch_s Segundos desde 01/01/1970 00:00:00 UTC, de 2000 a 2099.
 * @return RTC_OK, RTC_ERRO_LSE (cristal parado), RTC_ERRO_DATA ou RTC_ERRO_TEMPO.
 */
int32_t rtc_define(uint32_t epoch_s);

#endif /* RTC_H */
