/**
 * @file main.c
 * @brief Ponto de entrada do firmware STM32 do oLoggerIOT.
 *
 * Inicializa o HAL, cria as tarefas e entrega o controle ao escalonador do
 * FreeRTOS. Tarefas atuais: vida (FW-046), aquisição analógica (fase 1) e
 * leitura das entradas digitais (fase 1B).
 *
 * @date criacao 02/11/2026
 * @date modificacao 03/11/2026
 * @version 0.4.0
 */

#include "hal_stm32.h"
#include "pinos.h"

#include "FreeRTOS.h"
#include "task.h"
#include "vida.h"

#ifdef OLOG_TEM_AQUISICAO
  #include "adc.h"
  #include "aquisicao.h"
#endif

#ifdef OLOG_TEM_ENTRADA_DIGITAL
  #include "io_tarefa.h"
#endif

#ifdef OLOG_TEM_ENERGIA
  #include "energia.h"
#endif

#if defined(OLOG_TEM_ENERGIA) && defined(OLOG_TEM_ENTRADA_DIGITAL)
/* Hoje só as entradas digitais decidem se o equipamento pode dormir: após o tempo de
 * inatividade sem pulso. Outros módulos entram aqui quando existirem. */
static void prepara_sleep(void)
{
#ifdef OLOG_TEM_AQUISICAO
  (void)aquisicao_para(); /* Sem isto o ADC e o DMA seguem ligados e o consumo não cai. */
#endif
  io_tarefa_prepara_sleep();
}

static void retoma_do_sleep(void)
{
  io_tarefa_retoma();
#ifdef OLOG_TEM_AQUISICAO
  (void)aquisicao_retoma();
#endif
}

static const energia_ganchos_t ganchos_energia = {
  io_tarefa_pode_dormir,
  prepara_sleep,
  retoma_do_sleep,
};
#endif

/**
 * @brief Cria as tarefas do firmware.
 *
 * @return 0 em sucesso, código negativo se alguma tarefa não puder ser criada.
 */
static int32_t cria_tarefas(void)
{
  if (vida_inicia() != 0)
  {
    return -1;
  }

#ifdef OLOG_TEM_AQUISICAO
  if (aquisicao_inicia(AQUISICAO_TAXA_PADRAO_SPS) != AQUISICAO_OK)
  {
    return -1;
  }
#endif

#ifdef OLOG_TEM_ENTRADA_DIGITAL
  if (io_tarefa_inicia() != IO_TAREFA_OK)
  {
    return -1;
  }
#endif

#if defined(OLOG_TEM_ENERGIA) && defined(OLOG_TEM_ENTRADA_DIGITAL)
  if (energia_inicia(&ganchos_energia) != ENERGIA_OK)
  {
    return -1;
  }
#endif

  return 0;
}

/**
 * @brief Função principal do firmware.
 *
 * @return Nunca retorna: o escalonador assume o controle. Se uma tarefa falhar
 *         na criação, a partida trava aqui, visível ao depurador.
 * @note Chamada pelo código de inicialização do startup, depois de SystemInit.
 */
int main(void)
{
  (void)HAL_Init();

#ifdef OLOG_TEM_AQUISICAO
  adc_diagnostico(); /* Antes das tarefas: o tick ainda corre. */
#endif

  if (cria_tarefas() == 0)
  {
    vTaskStartScheduler();
  }

  for (;;)
  {
  }
}
