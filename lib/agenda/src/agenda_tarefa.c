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
#include "agenda_config.h"
#include "armazem.h"
#include "flash_interna.h"
#include "energia.h"
#include "io_saidas.h"
#include "rtc.h"
#include "rtos.h"
#include "tempo.h"

static StaticTask_t tcb_agenda;
static StackType_t pilha_agenda[AGENDA_TAREFA_PILHA_PALAVRAS];

/** Agendas das saídas e o último intervalo avaliado. */
static agenda_aplicador_t aplicador;
/** Fuso da instalação, em minutos em relação ao UTC; começa em Brasília até o CCO enviar outro. */
static int16_t fuso_min = AGENDA_TAREFA_FUSO_PADRAO_MIN;
/** Armazém persistente da agenda e do fuso. */
static armazem_t armazem;
/** true se há mudança ainda não gravada. */
static bool pendente;
/** Segundos desde a última mudança, para gravar só depois de um intervalo sem novas mudanças. */
static uint32_t segundos_parado;

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
 * @brief Lê a hora local, se a agenda pode ser aplicada.
 *
 * A agenda só funciona com o modo sleep desabilitado (decidido em 04/11/2026): com o sleep
 * habilitado não há hora local e nada é pedido.
 *
 * @param local Recebe a data e hora locais.
 * @return true se o sleep está desabilitado e a hora local é confiável.
 */
static bool hora_local(tempo_local_t *local)
{
  uint32_t epoch_s = 0U;
  int16_t fuso;

  if (energia_habilitada())
  {
    return false;
  }
  taskENTER_CRITICAL();
  fuso = fuso_min;
  taskEXIT_CRITICAL();
  if (rtc_le(&epoch_s) != RTC_OK)
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
 * @brief Carrega a agenda e o fuso gravados, se houver registro válido.
 *
 * Sem registro (primeira partida) ou com registro inválido, ficam a agenda vazia e o fuso de
 * Brasília.
 */
static void carrega_gravados(void)
{
  uint8_t carga[ARMAZEM_CARGA_MAX];
  size_t tamanho = 0U;
  agenda_aplicador_t lido;
  int16_t fuso = 0;

  if (armazem_inicia(&armazem, flash_interna_ops()) != ARMAZEM_OK)
  {
    return;
  }
  if ((armazem_le(&armazem, carga, sizeof(carga), &tamanho) == ARMAZEM_OK) &&
      (agenda_config_desempacota(&lido, &fuso, carga, tamanho) == AGENDA_CONFIG_OK) &&
      (fuso >= TEMPO_FUSO_MIN) && (fuso <= TEMPO_FUSO_MAX))
  {
    aplicador = lido;
    fuso_min = fuso;
  }
}

/** Marca que há mudança a gravar e reinicia a contagem do intervalo sem mudanças. */
static void marca_pendente(void)
{
  pendente = true;
  segundos_parado = 0U;
}

/**
 * @brief Grava a agenda e o fuso na flash quando houve mudança e ela está parada há algum tempo.
 *
 * A espera junta várias mudanças seguidas do CCO (as quatro agendas, por exemplo) numa só
 * gravação, e poupa a flash. A gravação fica fora de seção crítica: ela bloqueia o processador.
 */
static void grava_se_pendente(void)
{
  uint8_t carga[AGENDA_CONFIG_TAMANHO];
  size_t tamanho = 0U;
  agenda_aplicador_t copia;
  int16_t fuso;
  bool gravar;

  taskENTER_CRITICAL();
  gravar = pendente && (segundos_parado >= AGENDA_TAREFA_ESPERA_GRAVAR_S);
  if (pendente && !gravar)
  {
    segundos_parado++;
  }
  copia = aplicador;
  fuso = fuso_min;
  if (gravar)
  {
    pendente = false;
  }
  taskEXIT_CRITICAL();

  if (gravar && (agenda_config_empacota(&copia, fuso, carga, sizeof(carga), &tamanho) == AGENDA_CONFIG_OK) &&
      (armazem_grava(&armazem, carga, tamanho) != ARMAZEM_OK))
  {
    taskENTER_CRITICAL();
    marca_pendente(); /* Tenta de novo depois de outra espera. */
    taskEXIT_CRITICAL();
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
    grava_se_pendente();
    vTaskDelayUntil(&ultimo, pdMS_TO_TICKS(AGENDA_TAREFA_PERIODO_MS));
  }
}

int32_t agenda_tarefa_inicia(void)
{
  TaskHandle_t tarefa;

  (void)agenda_aplicador_limpa(&aplicador);
  carrega_gravados();
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
  (void)agenda_aplicador_reavalia(&aplicador);
  marca_pendente();
  taskEXIT_CRITICAL();
  return AGENDA_TAREFA_OK;
}

int32_t agenda_tarefa_carrega(uint8_t canal, const char *hex, size_t tamanho)
{
  int32_t resultado;

  taskENTER_CRITICAL();
  resultado = agenda_aplicador_carrega(&aplicador, canal, hex, tamanho);
  if (resultado == AGENDA_OK)
  {
    marca_pendente();
  }
  taskEXIT_CRITICAL();
  return resultado;
}
