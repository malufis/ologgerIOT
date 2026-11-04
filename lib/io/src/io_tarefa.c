/**
 * @file io_tarefa.c
 * @brief Implementação da tarefa de leitura das entradas digitais.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.2.0
 */

#include "io_tarefa.h"

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "io_backup.h"
#include "io_entrada.h"
#include "io_filtro.h"
#include "io_pulsos.h"
#include "io_totalizador.h"
#include "rtos.h"

/* Estado do módulo. As máscaras são lidas por SWD na bancada. */
static StaticTask_t tcb_io;
static StackType_t pilha_io[IO_TAREFA_PILHA_PALAVRAS];
static io_filtro_t filtros[IO_ENTRADAS_QTD];       /* Estado discreto, tempo padrão. */
static io_filtro_t filtros_pulso[IO_ENTRADAS_QTD]; /* Contagem de pulsos, tempo menor. */
static io_pulsos_t pulsos[IO_ENTRADAS_QTD];
static const io_totalizador_armazem_t armazem_backup = {io_backup_le, io_backup_grava};
static uint16_t amostras_discreto;
static uint16_t amostras_pulso;
static volatile uint8_t estado_filtrado;
static volatile uint8_t estado_bruto;

/**
 * @brief Instante atual em milissegundos, a partir do tick do FreeRTOS.
 *
 * @return Milissegundos desde o início do escalonador.
 */
static uint32_t agora_ms(void)
{
  return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

/**
 * @brief Lê as quatro entradas, atualiza filtros e contadores e guarda as máscaras.
 */
static void le_entradas(void)
{
  uint8_t bruto = 0U;
  uint8_t filtrado = 0U;
  uint8_t i;
  uint32_t antes;
  bool ativa = false;

  for (i = 0U; i < IO_ENTRADAS_QTD; i++)
  {
    (void)io_entrada_le((uint8_t)(i + 1U), &ativa);
    if (ativa)
    {
      bruto |= (uint8_t)(1U << i);
    }
    if (io_filtro_atualiza(&filtros[i], ativa))
    {
      filtrado |= (uint8_t)(1U << i);
    }
    antes = io_pulsos_contagem(&pulsos[i]);
    io_pulsos_atualiza(&pulsos[i], io_filtro_atualiza(&filtros_pulso[i], ativa), agora_ms());
    if (io_pulsos_contagem(&pulsos[i]) != antes)
    {
      (void)io_totalizador_soma((uint8_t)(i + 1U), io_pulsos_contagem(&pulsos[i]) - antes);
    }
  }
  estado_bruto = bruto;
  estado_filtrado = filtrado;
}

/**
 * @brief Calcula quantas amostras valem os tempos dos dois filtros.
 *
 * @return IO_TAREFA_OK ou IO_TAREFA_ERRO.
 */
static int32_t calcula_amostras(void)
{
  if ((io_filtro_amostras_do_tempo(IO_FILTRO_TEMPO_PADRAO_MS, IO_TAREFA_PERIODO_MS * 1000U,
                                   &amostras_discreto) != IO_FILTRO_OK) ||
      (io_filtro_amostras_do_tempo(IO_FILTRO_TEMPO_PULSO_MS, IO_TAREFA_PERIODO_MS * 1000U,
                                   &amostras_pulso) != IO_FILTRO_OK))
  {
    return IO_TAREFA_ERRO;
  }
  return IO_TAREFA_OK;
}

/**
 * @brief Ressincroniza os dois filtros de cada entrada com o estado lido agora.
 *
 * Evita uma mudança falsa na partida e ao acordar do sleep, quando o estado guardado
 * nos filtros está desatualizado.
 *
 * @param[out] ativas Estado de cada entrada lido agora, em máscara de bits.
 * @return IO_TAREFA_OK ou IO_TAREFA_ERRO.
 */
static int32_t sincroniza_filtros(uint8_t *ativas)
{
  uint8_t i;
  bool ativa = false;

  *ativas = 0U;
  for (i = 0U; i < IO_ENTRADAS_QTD; i++)
  {
    (void)io_entrada_le((uint8_t)(i + 1U), &ativa);
    if ((io_filtro_inicia(&filtros[i], amostras_discreto, ativa) != IO_FILTRO_OK) ||
        (io_filtro_inicia(&filtros_pulso[i], amostras_pulso, ativa) != IO_FILTRO_OK))
    {
      return IO_TAREFA_ERRO;
    }
    if (ativa)
    {
      *ativas |= (uint8_t)(1U << i);
    }
  }
  return IO_TAREFA_OK;
}

/**
 * @brief Corpo da tarefa: lê as entradas a cada período fixo.
 *
 * @param[in] argumento Não usado.
 */
static void tarefa_io(void *argumento)
{
  TickType_t ultimo = xTaskGetTickCount();

  (void)argumento;
  for (;;)
  {
    le_entradas();
    vTaskDelayUntil(&ultimo, pdMS_TO_TICKS(IO_TAREFA_PERIODO_MS));
  }
}

int32_t io_tarefa_inicia(void)
{
  TaskHandle_t tarefa;
  uint8_t ativas = 0U;
  uint8_t i;

  io_backup_inicia();
  if ((io_entrada_inicia() != IO_OK) || (calcula_amostras() != IO_TAREFA_OK) ||
      (io_totalizador_inicia(&armazem_backup) != IO_TOTALIZADOR_OK) ||
      (sincroniza_filtros(&ativas) != IO_TAREFA_OK))
  {
    return IO_TAREFA_ERRO;
  }
  for (i = 0U; i < IO_ENTRADAS_QTD; i++)
  {
    if (io_pulsos_inicia(&pulsos[i], IO_PULSOS_TIMEOUT_PADRAO_MS,
                         (ativas & (uint8_t)(1U << i)) != 0U, agora_ms()) != IO_PULSOS_OK)
    {
      return IO_TAREFA_ERRO;
    }
  }
  le_entradas();

  tarefa = xTaskCreateStatic(tarefa_io, "io", IO_TAREFA_PILHA_PALAVRAS, NULL,
                             RTOS_PRIO_SEGUNDO_PLANO, pilha_io, &tcb_io);
  return (tarefa == NULL) ? IO_TAREFA_ERRO : IO_TAREFA_OK;
}

uint32_t io_tarefa_pulsos(uint8_t canal)
{
  if ((canal < 1U) || (canal > IO_ENTRADAS_QTD))
  {
    return 0U;
  }
  return io_pulsos_contagem(&pulsos[canal - 1U]);
}

int32_t io_tarefa_define_inatividade(uint32_t timeout_ms)
{
  uint8_t i;

  if (timeout_ms == 0U)
  {
    return IO_TAREFA_ERRO;
  }
  for (i = 0U; i < IO_ENTRADAS_QTD; i++)
  {
    (void)io_pulsos_define_timeout(&pulsos[i], timeout_ms);
  }
  return IO_TAREFA_OK;
}

bool io_tarefa_pode_dormir(void)
{
  uint8_t i;

  for (i = 0U; i < IO_ENTRADAS_QTD; i++)
  {
    if (!io_pulsos_pode_dormir(&pulsos[i], agora_ms()))
    {
      return false;
    }
  }
  return true;
}

void io_tarefa_trata_despertar(void)
{
  uint8_t ativas = 0U;
  uint8_t i;

  /* Seção crítica: a tarefa de leitura usa os mesmos filtros e contadores. */
  taskENTER_CRITICAL();
  (void)sincroniza_filtros(&ativas);
  for (i = 0U; i < IO_ENTRADAS_QTD; i++)
  {
    io_pulsos_soma(&pulsos[i], io_entrada_coleta_pulsos((uint8_t)(i + 1U)),
                   (ativas & (uint8_t)(1U << i)) != 0U, agora_ms());
  }
  taskEXIT_CRITICAL();
}

uint32_t io_tarefa_total(uint8_t canal)
{
  return io_totalizador_le(canal);
}

int32_t io_tarefa_define_total(uint8_t canal, uint32_t valor)
{
  return (io_totalizador_define(canal, valor) == IO_TOTALIZADOR_OK) ? IO_TAREFA_OK
                                                                     : IO_TAREFA_ERRO;
}

void io_tarefa_prepara_sleep(void)
{
  (void)io_entrada_acordar_habilita();
}

void io_tarefa_retoma(void)
{
  io_entrada_acordar_desabilita();
  io_tarefa_trata_despertar();
}

uint8_t io_tarefa_estado(void)
{
  return estado_filtrado;
}

uint8_t io_tarefa_bruto(void)
{
  return estado_bruto;
}
