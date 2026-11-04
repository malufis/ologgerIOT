/**
 * @file agenda_aplicador.c
 * @brief Implementação da aplicação das agendas pelo horário atual.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "agenda_aplicador.h"

int32_t agenda_aplicador_limpa(agenda_aplicador_t *ap)
{
  uint8_t i;

  if (ap == NULL)
  {
    return AGENDA_ERRO_PARAMETRO;
  }
  for (i = 0U; i < AGENDA_APLICADOR_SAIDAS; i++)
  {
    (void)agenda_limpa(&ap->agenda[i]);
    ap->indice[i] = AGENDA_INDICE_INVALIDO;
    ap->reavalia[i] = false;
  }
  return AGENDA_OK;
}

int32_t agenda_aplicador_carrega(agenda_aplicador_t *ap, uint8_t canal, const char *hex,
                                 size_t tamanho)
{
  int32_t resultado;

  if ((ap == NULL) || (hex == NULL) || (canal < 1U) || (canal > AGENDA_APLICADOR_SAIDAS))
  {
    return AGENDA_ERRO_PARAMETRO;
  }
  if (tamanho == 0U)
  {
    resultado = agenda_limpa(&ap->agenda[canal - 1U]);
  }
  else
  {
    resultado = agenda_carrega_hex(&ap->agenda[canal - 1U], hex, tamanho);
  }
  if (resultado == AGENDA_OK)
  {
    ap->reavalia[canal - 1U] = true;
  }
  return resultado;
}

uint8_t agenda_aplicador_atualiza(agenda_aplicador_t *ap, bool hora_valida, uint8_t dia_semana,
                                  uint8_t hora, uint8_t minuto,
                                  agenda_estado_t estados[AGENDA_APLICADOR_SAIDAS],
                                  bool pedir[AGENDA_APLICADOR_SAIDAS])
{
  uint8_t quantidade = 0U;
  uint8_t i;
  uint16_t k;

  if ((ap == NULL) || (estados == NULL) || (pedir == NULL))
  {
    return 0U;
  }
  k = agenda_indice(dia_semana, hora, minuto);
  for (i = 0U; i < AGENDA_APLICADOR_SAIDAS; i++)
  {
    pedir[i] = false;
    estados[i] = AGENDA_SEM_AGENDA;
    if (hora_valida && (k != AGENDA_INDICE_INVALIDO) && ((k != ap->indice[i]) || ap->reavalia[i]))
    {
      ap->indice[i] = k;
      ap->reavalia[i] = false;
      estados[i] = agenda_estado(&ap->agenda[i], dia_semana, hora, minuto);
      pedir[i] = true;
      quantidade++;
    }
  }
  return quantidade;
}
