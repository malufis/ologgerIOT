/**
 * @file flash_interna.c
 * @brief Implementação das operações do armazém na flash interna do STM32L412.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "flash_interna.h"

#include <string.h>

#include "hal_stm32.h"

/** Endereço do início da flash. */
#define FLASH_INICIO 0x08000000UL

/**
 * @brief Endereço de um espaço do armazém.
 *
 * @param espaco 0 ou 1.
 * @return Endereço do espaço, ou 0 se inválido.
 */
static uint32_t endereco_do_espaco(uint8_t espaco)
{
  if (espaco == 0U)
  {
    return FLASH_INTERNA_ESPACO_0;
  }
  return (espaco == 1U) ? FLASH_INTERNA_ESPACO_1 : 0UL;
}

static const uint8_t *flash_endereco(uint8_t espaco)
{
  uint32_t e = endereco_do_espaco(espaco);

  return (e == 0UL) ? NULL : (const uint8_t *)e;
}

static int32_t flash_apaga(uint8_t espaco)
{
  FLASH_EraseInitTypeDef apagamento = {0};
  uint32_t erro_pagina = 0U;
  uint32_t e = endereco_do_espaco(espaco);
  HAL_StatusTypeDef estado;

  if (e == 0UL)
  {
    return -1;
  }
  apagamento.TypeErase = FLASH_TYPEERASE_PAGES;
  apagamento.Banks = FLASH_BANK_1;
  apagamento.Page = (e - FLASH_INICIO) / FLASH_PAGE_SIZE;
  apagamento.NbPages = 1U;

  (void)HAL_FLASH_Unlock();
  estado = HAL_FLASHEx_Erase(&apagamento, &erro_pagina);
  (void)HAL_FLASH_Lock();
  return (estado == HAL_OK) ? 0 : -1;
}

static int32_t flash_grava(uint8_t espaco, const uint8_t *dados, size_t tamanho)
{
  uint32_t e = endereco_do_espaco(espaco);
  uint64_t palavra;
  size_t i;
  HAL_StatusTypeDef estado = HAL_OK;

  if ((e == 0UL) || (dados == NULL) || ((tamanho % ARMAZEM_ALINHAMENTO) != 0U) ||
      (tamanho > FLASH_INTERNA_TAMANHO))
  {
    return -1;
  }

  (void)HAL_FLASH_Unlock();
  for (i = 0U; (i < tamanho) && (estado == HAL_OK); i += ARMAZEM_ALINHAMENTO)
  {
    (void)memcpy(&palavra, &dados[i], sizeof(palavra));
    estado = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, e + (uint32_t)i, palavra);
  }
  (void)HAL_FLASH_Lock();
  return (estado == HAL_OK) ? 0 : -1;
}

const armazem_ops_t *flash_interna_ops(void)
{
  static const armazem_ops_t ops = {flash_endereco, flash_apaga, flash_grava, FLASH_INTERNA_TAMANHO};

  return &ops;
}
