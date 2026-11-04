/**
 * @file config_tarefa.c
 * @brief Implementação da configuração do device no firmware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include "config_tarefa.h"

#include "FreeRTOS.h"
#include "task.h"

#include "agenda_config.h"
#include "agenda_tarefa.h"
#include "armazem.h"
#include "energia.h"
#include "flash_interna.h"
#include "io_frequencia.h"
#include "io_saidas.h"
#include "io_tarefa.h"
#include "rtos.h"

static StaticTask_t tcb_config;
static StackType_t pilha_config[CONFIG_TAREFA_PILHA_PALAVRAS];

/** Configuração em uso. */
static config_t cfg;
/** Armazém persistente. */
static armazem_t armazem;
/** Quantas escritas de configuração houve desde a partida. */
static uint32_t escritas;

/**
 * @brief Aplica o que mudou nos parâmetros de cada saída.
 *
 * @param antes Configuração anterior.
 * @param agora Configuração nova.
 * @param tudo true para aplicar tudo, mudando ou não (na partida).
 */
static void aplica_saidas(const config_t *antes, const config_t *agora, bool tudo)
{
  uint8_t i;
  uint8_t canal;

  for (i = 0U; i < CONFIG_QTD_SAIDAS; i++)
  {
    canal = (uint8_t)(i + 1U);
    if (tudo || config_mudou(antes, agora, CFG_SAI_TIPO, i))
    {
      (void)io_frequencia_define_tipo(canal, (uint16_t)config_inteiro(agora, CFG_SAI_TIPO, i),
                                      energia_habilitada());
    }
    if (tudo || config_mudou(antes, agora, CFG_SAI_F4MA, i) || config_mudou(antes, agora, CFG_SAI_F20MA, i))
    {
      (void)io_frequencia_cadastra(canal, config_float(agora, CFG_SAI_F4MA, i),
                                   config_float(agora, CFG_SAI_F20MA, i));
    }
    if (tudo || config_mudou(antes, agora, CFG_SAI_RETARDO, i))
    {
      (void)io_saidas_define_atraso(canal, (uint32_t)config_inteiro(agora, CFG_SAI_RETARDO, i));
    }
    if (tudo || config_mudou(antes, agora, CFG_SAI_ESTAB, i))
    {
      (void)io_saidas_define_estabilidade(canal, (uint32_t)config_inteiro(agora, CFG_SAI_ESTAB, i));
    }
  }
}

/**
 * @brief Aplica o que mudou nos parâmetros gerais: fuso, sleep e inatividade da entrada digital.
 *
 * O sleep é desligado antes de mexer nas saídas em frequência e ligado depois delas, para a
 * exclusão entre os dois valer em qualquer ordem de gravação.
 *
 * @param antes Configuração anterior.
 * @param agora Configuração nova.
 * @param tudo true para aplicar tudo (na partida).
 * @param depois true para a passada que vem depois das saídas.
 */
static void aplica_gerais(const config_t *antes, const config_t *agora, bool tudo, bool depois)
{
  bool sleep = (config_inteiro(agora, CFG_MODO_OPERACAO, 0U) == 1);

  if (!depois)
  {
    if ((tudo || config_mudou(antes, agora, CFG_MODO_OPERACAO, 0U)) && !sleep)
    {
      (void)energia_define_habilitada(false);
    }
    if (tudo || config_mudou(antes, agora, CFG_FUSO, 0U))
    {
      (void)agenda_tarefa_define_fuso((int16_t)config_inteiro(agora, CFG_FUSO, 0U));
    }
    if (tudo || config_mudou(antes, agora, CFG_INATIVIDADE, 0U))
    {
      (void)io_tarefa_define_inatividade(1000U * (uint32_t)config_inteiro(agora, CFG_INATIVIDADE, 0U));
    }
  }
  else if ((tudo || config_mudou(antes, agora, CFG_MODO_OPERACAO, 0U)) && sleep)
  {
    (void)energia_define_habilitada(true);
  }
}

/** Aplica aos módulos a diferença entre duas configurações. */
static void aplica(const config_t *antes, const config_t *agora, bool tudo)
{
  aplica_gerais(antes, agora, tudo, false);
  aplica_saidas(antes, agora, tudo);
  aplica_gerais(antes, agora, tudo, true);
}

/**
 * @brief Lê o registro gravado: a configuração e a agenda.
 *
 * Sem registro válido, a configuração fica nos padrões de fábrica e a agenda vazia.
 */
static void carrega_gravados(void)
{
  uint8_t carga[ARMAZEM_CARGA_MAX];
  size_t tamanho = 0U;

  (void)config_padroes(&cfg);
  if (armazem_inicia(&armazem, flash_interna_ops()) != ARMAZEM_OK)
  {
    return;
  }
  if ((armazem_le(&armazem, carga, sizeof(carga), &tamanho) == ARMAZEM_OK) &&
      (tamanho == (CONFIG_TAMANHO_CARGA + AGENDA_CONFIG_TAMANHO)))
  {
    (void)config_desempacota(&cfg, carga, CONFIG_TAMANHO_CARGA);
    (void)agenda_tarefa_importa(&carga[CONFIG_TAMANHO_CARGA], AGENDA_CONFIG_TAMANHO);
  }
}

/**
 * @brief Grava a configuração e a agenda num só registro.
 *
 * @return true se gravou.
 */
static bool grava_registro(void)
{
  uint8_t carga[ARMAZEM_CARGA_MAX];
  config_t copia;
  size_t n_config = 0U;
  size_t n_agenda = 0U;

  config_tarefa_copia(&copia);
  if ((config_empacota(&copia, carga, sizeof(carga), &n_config) != CONFIG_OK) ||
      (agenda_tarefa_exporta(&carga[n_config], sizeof(carga) - n_config, &n_agenda) != AGENDA_CONFIG_OK))
  {
    return false;
  }
  return armazem_grava(&armazem, carga, n_config + n_agenda) == ARMAZEM_OK;
}

/**
 * @brief Corpo da tarefa: grava o registro 5 segundos depois da última mudança.
 *
 * @param[in] argumento Não usado.
 */
static void tarefa_config(void *argumento)
{
  TickType_t ultimo = xTaskGetTickCount();
  uint32_t visto_config = 0U;
  uint32_t visto_agenda = 0U;
  uint32_t parado_s = 0U;
  bool pendente = false;
  uint32_t atual_config;
  uint32_t atual_agenda;

  (void)argumento;
  for (;;)
  {
    taskENTER_CRITICAL();
    atual_config = escritas;
    taskEXIT_CRITICAL();
    atual_agenda = agenda_tarefa_alteracoes();
    if ((atual_config != visto_config) || (atual_agenda != visto_agenda))
    {
      visto_config = atual_config;
      visto_agenda = atual_agenda;
      pendente = true;
      parado_s = 0U;
    }
    else if (pendente && (++parado_s >= CONFIG_TAREFA_ESPERA_GRAVAR_S))
    {
      pendente = !grava_registro(); /* Se falhar, tenta de novo depois de outra espera. */
      parado_s = 0U;
    }
    vTaskDelayUntil(&ultimo, pdMS_TO_TICKS(1000U));
  }
}

int32_t config_tarefa_inicia(void)
{
  TaskHandle_t tarefa;
  config_t padroes;

  carrega_gravados();
  (void)config_padroes(&padroes);
  aplica(&padroes, &cfg, true);
  tarefa = xTaskCreateStatic(tarefa_config, "config", CONFIG_TAREFA_PILHA_PALAVRAS, NULL,
                             RTOS_PRIO_SEGUNDO_PLANO, pilha_config, &tcb_config);
  return (tarefa == NULL) ? CONFIG_TAREFA_ERRO_TAREFA : CONFIG_TAREFA_OK;
}

int32_t config_tarefa_le(uint16_t endereco, uint16_t quantidade, uint16_t *registradores)
{
  int32_t resultado;

  taskENTER_CRITICAL();
  resultado = config_le_registradores(&cfg, endereco, quantidade, registradores);
  taskEXIT_CRITICAL();
  return resultado;
}

int32_t config_tarefa_escreve(uint16_t endereco, uint16_t quantidade, const uint16_t *registradores)
{
  config_t antes;
  config_t nova;
  int32_t resultado;

  taskENTER_CRITICAL();
  antes = cfg;
  nova = cfg;
  taskEXIT_CRITICAL();

  resultado = config_escreve_registradores(&nova, endereco, quantidade, registradores);
  if (resultado != CONFIG_OK)
  {
    return resultado;
  }
  taskENTER_CRITICAL();
  cfg = nova;
  escritas++;
  taskEXIT_CRITICAL();
  aplica(&antes, &nova, false);
  return CONFIG_OK;
}

void config_tarefa_copia(config_t *copia)
{
  taskENTER_CRITICAL();
  *copia = cfg;
  taskEXIT_CRITICAL();
}
