/**
 * @file io_backup.c
 * @brief Implementação do armazém nos registradores de backup (TAMP).
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#include "io_backup.h"

#include "hal_stm32.h"

/**
 * @brief Endereço do registrador de backup de índice dado.
 *
 * @param palavra Índice de 0 a IO_BACKUP_PALAVRAS - 1.
 * @return Ponteiro para o registrador.
 */
static volatile uint32_t *registro(uint8_t palavra)
{
  volatile uint32_t *base = &TAMP->BKP0R;

  return &base[palavra];
}

void io_backup_inicia(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
  SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_RTCAPBEN);
  (void)READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_RTCAPBEN); /* Atraso do clock antes do acesso. */
}

uint32_t io_backup_le(uint8_t palavra)
{
  return (palavra < IO_BACKUP_PALAVRAS) ? *registro(palavra) : 0U;
}

void io_backup_grava(uint8_t palavra, uint32_t valor)
{
  if (palavra < IO_BACKUP_PALAVRAS)
  {
    *registro(palavra) = valor;
  }
}
