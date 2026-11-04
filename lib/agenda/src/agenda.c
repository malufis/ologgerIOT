/**
 * @file agenda.c
 * @brief Implementação da agenda semanal de uma saída digital.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "agenda.h"

#include <string.h>

/** Intervalos por dia e dias por semana. */
#define INTERVALOS_POR_DIA 48U
#define DIAS_POR_SEMANA    7U
/** Minutos de um intervalo. */
#define MINUTOS_INTERVALO  30U

/**
 * @brief Converte um caractere hexadecimal no seu valor.
 *
 * @param c Caractere, em maiúscula ou minúscula.
 * @return Valor de 0 a 15, ou -1 se não for dígito hexadecimal.
 */
static int32_t valor_hex(char c)
{
  if ((c >= '0') && (c <= '9'))
  {
    return (int32_t)(c - '0');
  }
  if ((c >= 'A') && (c <= 'F'))
  {
    return (int32_t)(c - 'A') + 10;
  }
  if ((c >= 'a') && (c <= 'f'))
  {
    return (int32_t)(c - 'a') + 10;
  }
  return -1;
}

int32_t agenda_limpa(agenda_t *agenda)
{
  if (agenda == NULL)
  {
    return AGENDA_ERRO_PARAMETRO;
  }

  (void)memset(agenda->bits, 0, AGENDA_BYTES);
  agenda->definida = false;
  return AGENDA_OK;
}

int32_t agenda_carrega_hex(agenda_t *agenda, const char *hex, size_t tamanho)
{
  uint8_t bits[AGENDA_BYTES];
  size_t i;
  int32_t alto;
  int32_t baixo;

  if ((agenda == NULL) || (hex == NULL))
  {
    return AGENDA_ERRO_PARAMETRO;
  }
  if (tamanho != AGENDA_HEX_TAMANHO)
  {
    return AGENDA_ERRO_TAMANHO;
  }

  /* Valida e converte numa cópia: a agenda anterior só é trocada se tudo for válido. */
  for (i = 0U; i < AGENDA_BYTES; i++)
  {
    alto = valor_hex(hex[2U * i]);
    baixo = valor_hex(hex[(2U * i) + 1U]);
    if ((alto < 0) || (baixo < 0))
    {
      return AGENDA_ERRO_CARACTERE;
    }
    bits[i] = (uint8_t)(((uint32_t)alto << 4) | (uint32_t)baixo);
  }

  (void)memcpy(agenda->bits, bits, AGENDA_BYTES);
  agenda->definida = true;
  return AGENDA_OK;
}

uint16_t agenda_indice(uint8_t dia, uint8_t hora, uint8_t minuto)
{
  if ((dia >= DIAS_POR_SEMANA) || (hora > 23U) || (minuto > 59U))
  {
    return AGENDA_INDICE_INVALIDO;
  }

  return (uint16_t)(((uint32_t)dia * INTERVALOS_POR_DIA) + ((uint32_t)hora * 2U) +
                    ((uint32_t)minuto / MINUTOS_INTERVALO));
}

agenda_estado_t agenda_estado(const agenda_t *agenda, uint8_t dia, uint8_t hora, uint8_t minuto)
{
  uint16_t k = agenda_indice(dia, hora, minuto);

  if (!agenda->definida || (k == AGENDA_INDICE_INVALIDO))
  {
    return AGENDA_SEM_AGENDA;
  }

  return ((agenda->bits[k / 8U] & (uint8_t)(1U << (k % 8U))) != 0U) ? AGENDA_LIGADA
                                                                      : AGENDA_DESLIGADA;
}
