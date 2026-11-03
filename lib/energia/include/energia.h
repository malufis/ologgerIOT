/**
 * @file energia.h
 * @brief Gerência de energia: entra em Stop 2 e volta quando alguém pede.
 *
 * Uma tarefa de baixa prioridade verifica, a cada segundo, se o equipamento pode dormir.
 * Se puder e o sleep estiver habilitado, chama o gancho de preparação, entra em Stop 2 e
 * só sai quando energia_solicita_acordar() é chamada por uma interrupção: um pulso de entrada
 * digital ou, em outra sprint, o alarme do RTC. Depois de sair, o STM32 fica acordado e só
 * dorme de novo quando pode_dormir() voltar a ser verdadeiro, então um pulso constante não faz
 * o STM32 acordar e dormir sem parar. Uma interrupção que não pede para acordar é atendida e o
 * STM32 volta ao Stop 2 sem sair do ciclo. Ao sair, chama o gancho de retomada.
 *
 * O sleep começa desligado: é preciso habilitá-lo com energia_define_habilitada(). Enquanto
 * dorme, o tick do FreeRTOS e do HAL param: o tempo medido pelo tick não conta o sleep, e
 * a hora certa virá do RTC. O clock do sistema fica no MSI, que o Stop 2 preserva; se o
 * firmware passar a usar o PLL, a saída do sleep precisa restaurá-lo.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#ifndef ENERGIA_H
#define ENERGIA_H

#include <stdbool.h>
#include <stdint.h>

/** Período com que a tarefa verifica se pode dormir, em milissegundos. */
#define ENERGIA_PERIODO_MS     1000U
/** Tamanho da pilha da tarefa, em palavras de 32 bits. */
#define ENERGIA_PILHA_PALAVRAS 128U

/** Código de retorno: sucesso. */
#define ENERGIA_OK             0
/** Código de retorno: parâmetro nulo ou tarefa não criada. */
#define ENERGIA_ERRO           (-1)
/** Código de retorno: um módulo impede habilitar o sleep (por exemplo, saída em frequência ativa). */
#define ENERGIA_ERRO_BLOQUEADO (-2)

/** Ganchos que ligam a gerência de energia aos módulos do equipamento. */
typedef struct
{
  bool (*pode_dormir)(void); /**< Devolve true se nenhum módulo precisa do equipamento acordado. */
  void (*antes)(void);       /**< Prepara os módulos para dormir; pode ser nulo. */
  void (*depois)(void);      /**< Retoma os módulos ao acordar; pode ser nulo. */
  bool (*sleep_permitido)(void); /**< Devolve false se algum módulo impede habilitar o sleep; pode ser nulo. */
} energia_ganchos_t;

/**
 * @brief Cria a tarefa de gerência de energia com memória estática.
 *
 * @param ganchos Ganchos do equipamento; a estrutura deve continuar válida. pode_dormir é obrigatório.
 * @return ENERGIA_OK ou ENERGIA_ERRO.
 * @note Chamar antes de vTaskStartScheduler.
 */
int32_t energia_inicia(const energia_ganchos_t *ganchos);

/**
 * @brief Liga ou desliga o modo sleep. Começa desligado.
 *
 * Desligar sempre funciona. Ligar é recusado se o gancho `sleep_permitido` disser que algum
 * módulo impede: a saída em frequência só pode ser habilitada com o sleep desligado, e o sleep
 * só pode ser ligado se nenhuma saída estiver em frequência. Corresponde ao modo de operação do
 * mapa Modbus (40412: 0 = contínuo, 1 = sleep).
 *
 * @param valor true para ligar o sleep, false para desligar.
 * @return ENERGIA_OK ou ENERGIA_ERRO_BLOQUEADO (só ao ligar).
 */
int32_t energia_define_habilitada(bool valor);

/**
 * @brief Informa se o sleep está habilitado.
 *
 * @return true se o equipamento pode entrar em sleep.
 */
bool energia_habilitada(void);

/**
 * @brief Pede para o equipamento sair do sleep na próxima oportunidade.
 *
 * @note Pode ser chamada de dentro de uma interrupção. Não usa a API do FreeRTOS.
 */
void energia_solicita_acordar(void);

#endif /* ENERGIA_H */
