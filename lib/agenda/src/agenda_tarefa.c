/**
 * @file agenda_tarefa.c
 * @brief Implementação da tarefa que aplica as agendas pelo horário do RTC.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "agenda_tarefa.h"

#include "FreeRTOS.h"
#include "task.h"

#include "agenda_aplicador.h"
#include "io_saidas.h"
#include "rtc.h"
#include "rtos.h"
#include "tempo.h"

static StaticTask_t tcb_agenda;
static StackType_t pilha_agenda[AGENDA_TAREFA_PILHA_PALAVRAS];

/** Agendas das saídas e o último intervalo avaliado. */
static agenda_aplicador_t aplicador;
/** Fuso da instalação, em minutos em relação ao UTC. */
static int16_t fuso_min;
/** true depois que o CCO enviou o fuso. */
static bool fuso_definido;

/**
 * @brief Traduz o estado da agenda no pedido da fonte agenda.
 *
 * @param estado Estado do intervalo na agenda.
 * @return Liga, desliga, ou nenhum pedido se a saída não tem agenda.
 */
static controle_pedido_t pedido_do_estado(agenda_estado_t estado)
{
  if (estado == AGENDA_LIGADA)
  {
    return CONTROLE_PEDIDO_LIGA;
  }
  if (estado == AGENDA_DESLIGADA)
  {
    return CONTROLE_PEDIDO_DESLIGA;
  }
  return CONTROLE_PEDIDO_NENHUM;
}

/**
 * @brief Lê a hora local, se houver hora válida e fuso.
 *
 * @param local Recebe a data e hora locais.
 * @return true se a hora local é confiável.
 */
static bool hora_local(tempo_local_t *local)
{
  uint32_t epoch_s = 0U;
  bool tem_fuso;
  int16_t fuso;

  taskENTER_CRITICAL();
  tem_fuso = fuso_definido;
  fuso = fuso_min;
  taskEXIT_CRITICAL();
  if (!tem_fuso || (rtc_le(&epoch_s) != RTC_OK))
  {
    return false;
  }
  return tempo_para_local(epoch_s, fuso, local) == TEMPO_OK;
}

/** Avalia as agendas no horário atual e entrega os pedidos às saídas. */
static void aplica_agendas(void)
{
  tempo_local_t local = {0};
  agenda_estado_t estados[AGENDA_APLICADOR_SAIDAS];
  bool pedir[AGENDA_APLICADOR_SAIDAS];
  bool valida = hora_local(&local);
  uint8_t i;

  taskENTER_CRITICAL();
  (void)agenda_aplicador_atualiza(&aplicador, valida, local.dia_semana, local.hora, local.minuto,
                                  estados, pedir);
  taskEXIT_CRITICAL();

  for (i = 0U; i < AGENDA_APLICADOR_SAIDAS; i++)
  {
    if (pedir[i])
    {
      (void)io_saidas_pedido((uint8_t)(i + 1U), CONTROLE_FONTE_AGENDA, pedido_do_estado(estados[i]));
    }
  }
}

/**
 * @brief Corpo da tarefa: aplica as agendas a cada período.
 *
 * @param[in] argumento Não usado.
 */
static void tarefa_agenda(void *argumento)
{
  TickType_t ultimo = xTaskGetTickCount();

  (void)argumento;
  for (;;)
  {
    aplica_agendas();
    vTaskDelayUntil(&ultimo, pdMS_TO_TICKS(AGENDA_TAREFA_PERIODO_MS));
  }
}

int32_t agenda_tarefa_inicia(void)
{
  TaskHandle_t tarefa;

  (void)agenda_aplicador_limpa(&aplicador);
  tarefa = xTaskCreateStatic(tarefa_agenda, "agenda", AGENDA_TAREFA_PILHA_PALAVRAS, NULL,
                             RTOS_PRIO_REGRAS, pilha_agenda, &tcb_agenda);
  return (tarefa == NULL) ? AGENDA_TAREFA_ERRO_TAREFA : AGENDA_TAREFA_OK;
}

int32_t agenda_tarefa_define_fuso(int16_t fuso)
{
  if ((fuso < TEMPO_FUSO_MIN) || (fuso > TEMPO_FUSO_MAX))
  {
    return AGENDA_TAREFA_ERRO_FUSO;
  }
  taskENTER_CRITICAL();
  fuso_min = fuso;
  fuso_definido = true;
  (void)agenda_aplicador_reavalia(&aplicador);
  taskEXIT_CRITICAL();
  return AGENDA_TAREFA_OK;
}

int32_t agenda_tarefa_carrega(uint8_t canal, const char *hex, size_t tamanho)
{
  int32_t resultado;

  taskENTER_CRITICAL();
  resultado = agenda_aplicador_carrega(&aplicador, canal, hex, tamanho);
  taskEXIT_CRITICAL();
  return resultado;
}
