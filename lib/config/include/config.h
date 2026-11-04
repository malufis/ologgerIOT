/**
 * @file config.h
 * @brief Configuração do device: tabela de parâmetros com endereço Modbus, limites e padrões.
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/config). É a **fonte única** de todos os
 * parâmetros de configuração e de setup do device (decidido em 04/11/2026): cada parâmetro tem o
 * endereço dos registradores de retenção (holding, 4xxxx) do mapa Modbus (`mapa_modbus.md`), o
 * tipo, os limites e o valor de fábrica. As chaves do JSON do MQTT seguem os mesmos endereços,
 * então a camada Modbus e a MQTT chamam as mesmas duas funções: config_le_registradores() e
 * config_escreve_registradores().
 *
 * - **Escrita atômica:** o pedido só vale se todos os valores forem válidos e a combinação não violar
 *   uma regra de exclusão (config_conflito()); um valor fora do limite, um float gravado pela metade
 *   ou um conflito recusa o pedido inteiro, sem alterar nada.
 * - **Float:** 2 registradores, IEEE 754 de 32 bits, palavra alta primeiro (ABCD).
 * - **Persistência:** config_empacota() e config_desempacota() levam a configuração toda para o
 *   armazém (armazem.h) e a trazem de volta. Valor gravado inválido volta ao padrão de fábrica.
 * - **Estado, e não configuração:** o modo manual ou automático das saídas, o totalizador de pulsos
 *   e a hora não entram aqui.
 *
 * Os endereços marcados como proposta em `mapa_modbus.md` (estabilidade do sensor e inatividade da
 * entrada digital) ainda dependem de confirmação.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Número de entradas analógicas, de entradas digitais e de saídas digitais do OloggerMulti. */
#define CONFIG_QTD_ENTRADAS_ANALOGICAS 4U
#define CONFIG_QTD_ENTRADAS_DIGITAIS   4U
#define CONFIG_QTD_SAIDAS              4U

/** Código de retorno: sucesso. */
#define CONFIG_OK               0
/** Código de retorno: parâmetro nulo. */
#define CONFIG_ERRO_PARAMETRO   (-1)
/** Código de retorno: endereço que não é de configuração, ou pedido que cruza o limite de um parâmetro. */
#define CONFIG_ERRO_ENDERECO    (-2)
/** Código de retorno: valor fora dos limites do parâmetro. */
#define CONFIG_ERRO_VALOR       (-3)
/** Código de retorno: o buffer de empacotamento é pequeno. */
#define CONFIG_ERRO_TAMANHO     (-4)
/** Código de retorno: versão do formato gravado diferente da esperada. */
#define CONFIG_ERRO_VERSAO      (-5)
/** Código de retorno: a combinação viola uma regra de exclusão (frequência na saída 1 ou com o sleep ligado). */
#define CONFIG_ERRO_CONFLITO    (-6)

/** Versão do formato da carga gravada. */
#define CONFIG_VERSAO           1U

/** Identificador de cada parâmetro (cada um tem uma instância por entrada ou saída). */
typedef enum
{
  CFG_ENT_SPAN = 0,      /**< Span da entrada analógica n (valor em 20 mA). */
  CFG_ENT_ZERO,          /**< Zero da entrada analógica n (valor em 4 mA). */
  CFG_ENT_OFFSET,        /**< Offset da entrada analógica n, de −100 a 100. */
  CFG_ENT_MODO_DIG,      /**< Modo da entrada digital n: 0 = discreto, 1 = pulsado. */
  CFG_SAI_TIPO,          /**< Tipo da saída n: 0 = discreto, 1 = frequência. */
  CFG_SAI_F4MA,          /**< Frequência em Hz em 4 mA da saída n. */
  CFG_SAI_F20MA,         /**< Frequência em Hz em 20 mA da saída n. */
  CFG_SAI_ENTRADA,       /**< Entrada analógica associada à saída n (0 = nenhuma). */
  CFG_SAI_RETARDO,       /**< Retardo de acionamento da saída n, em segundos. */
  CFG_SAI_LIM_MAX,       /**< Limiar máximo da regra do sensor da saída n. */
  CFG_SAI_LIM_MIN,       /**< Limiar mínimo da regra do sensor da saída n. */
  CFG_SAI_SENTIDO,       /**< Sentido da regra do sensor da saída n (0 liga no mínimo, 1 no máximo). */
  CFG_SAI_ESTAB,         /**< Estabilidade do sensor depois de uma falha, da saída n, em segundos. */
  CFG_VEL_RS485,         /**< Velocidade da RS-485: 0 = 2400, 1 = 9600, 2 = 57600, 3 = 115200 bps. */
  CFG_MODO_RS485,        /**< Modo da RS-485: 0 = mestre, 1 = escravo. */
  CFG_ENDERECO_ESCRAVO,  /**< Endereço do escravo, de 1 a 247. */
  CFG_FUSO,              /**< Fuso horário da instalação, em minutos em relação ao UTC. */
  CFG_INTERVALO_COLETA,  /**< Intervalo de coleta, em segundos. */
  CFG_INTERVALO_ENVIO,   /**< Intervalo de envio, em segundos. */
  CFG_MODO_OPERACAO,     /**< Modo de operação: 0 = contínuo, 1 = sleep. */
  CFG_INATIVIDADE,       /**< Tempo sem pulso para o STM32 dormir, em segundos. */
  CFG_QTD                /**< Quantidade de parâmetros (não é um parâmetro). */
} config_id_t;

/** Total de valores guardados (soma das instâncias de todos os parâmetros). */
#define CONFIG_VALORES 60U

/** Configuração do device. */
typedef struct
{
  uint32_t valor[CONFIG_VALORES]; /**< Valor de cada instância; float guardado pelos seus 32 bits. */
} config_t;

/** Tamanho da carga empacotada, em bytes (versão, quantidade e os valores). */
#define CONFIG_TAMANHO_CARGA (4U + (CONFIG_VALORES * 4U))

/**
 * @brief Põe todos os parâmetros nos valores de fábrica.
 *
 * @param cfg Configuração.
 * @return CONFIG_OK, ou CONFIG_ERRO_PARAMETRO se @p cfg for nulo.
 */
int32_t config_padroes(config_t *cfg);

/**
 * @brief Lê registradores de retenção (holding), a partir de um endereço do mapa Modbus.
 *
 * O pedido pode cruzar vários parâmetros, desde que todos os endereços sejam de configuração.
 *
 * @param cfg Configuração.
 * @param endereco Primeiro endereço, na convenção de 5 dígitos (40001 a 49999).
 * @param quantidade Quantidade de registradores.
 * @param registradores Recebe os registradores (16 bits cada).
 * @return CONFIG_OK, CONFIG_ERRO_PARAMETRO ou CONFIG_ERRO_ENDERECO.
 */
int32_t config_le_registradores(const config_t *cfg, uint16_t endereco, uint16_t quantidade,
                                uint16_t *registradores);

/**
 * @brief Grava registradores de retenção, só se o pedido inteiro for válido.
 *
 * Cada parâmetro tocado precisa vir inteiro (um float tem 2 registradores) e dentro dos limites.
 *
 * @param cfg Configuração.
 * @param endereco Primeiro endereço, na convenção de 5 dígitos.
 * @param quantidade Quantidade de registradores.
 * @param registradores Valores a gravar.
 * @return CONFIG_OK, CONFIG_ERRO_PARAMETRO, CONFIG_ERRO_ENDERECO ou CONFIG_ERRO_VALOR (nada muda).
 */
int32_t config_escreve_registradores(config_t *cfg, uint16_t endereco, uint16_t quantidade,
                                     const uint16_t *registradores);

/**
 * @brief Quantas entradas ou saídas tem um parâmetro.
 *
 * @param id Parâmetro.
 * @return Quantidade de instâncias, ou 0 se @p id for inválido.
 */
uint8_t config_instancias(config_id_t id);

/**
 * @brief Posição de uma instância no vetor de valores.
 *
 * @param id Parâmetro.
 * @param instancia Entrada ou saída, de 0 em diante.
 * @return A posição, ou CONFIG_VALORES se os argumentos forem inválidos.
 */
uint16_t config_indice(config_id_t id, uint8_t instancia);

/**
 * @brief Diz se os 32 bits guardados são um valor aceito pelo parâmetro.
 *
 * @param id Parâmetro.
 * @param bits Valor guardado (float pelos seus 32 bits).
 * @return true se estiver nos limites.
 */
bool config_bits_validos(config_id_t id, uint32_t bits);

/**
 * @brief Diz se a configuração viola uma regra de exclusão.
 *
 * As regras: a saída 1 não tem modo frequência (sem canal de timer) e o modo sleep não pode estar
 * ligado com alguma saída em frequência.
 *
 * @param cfg Configuração.
 * @return true se há conflito.
 */
bool config_conflito(const config_t *cfg);

/**
 * @brief Diz se uma instância tem valor diferente nas duas configurações.
 *
 * @param a Primeira configuração.
 * @param b Segunda configuração.
 * @param id Parâmetro.
 * @param instancia Entrada ou saída, de 0 em diante.
 * @return true se o valor mudou. Falso se os argumentos forem inválidos.
 */
bool config_mudou(const config_t *a, const config_t *b, config_id_t id, uint8_t instancia);

/**
 * @brief Lê o valor inteiro de uma instância (para parâmetros uint16 e int16).
 *
 * @param cfg Configuração.
 * @param id Parâmetro.
 * @param instancia Entrada ou saída, de 0 em diante (0 para parâmetros únicos).
 * @return O valor, ou 0 se os argumentos forem inválidos.
 */
int32_t config_inteiro(const config_t *cfg, config_id_t id, uint8_t instancia);

/**
 * @brief Lê o valor de uma instância de ponto flutuante.
 *
 * @param cfg Configuração.
 * @param id Parâmetro.
 * @param instancia Entrada ou saída, de 0 em diante.
 * @return O valor, ou 0 se os argumentos forem inválidos.
 */
float config_float(const config_t *cfg, config_id_t id, uint8_t instancia);

/**
 * @brief Empacota a configuração numa carga para o armazém.
 *
 * @param cfg Configuração.
 * @param carga Recebe a carga.
 * @param capacidade Tamanho de @p carga; pelo menos CONFIG_TAMANHO_CARGA.
 * @param tamanho Recebe o tamanho da carga.
 * @return CONFIG_OK, CONFIG_ERRO_PARAMETRO ou CONFIG_ERRO_TAMANHO.
 */
int32_t config_empacota(const config_t *cfg, uint8_t *carga, size_t capacidade, size_t *tamanho);

/**
 * @brief Carrega a configuração de uma carga gravada.
 *
 * Valor inválido volta ao padrão de fábrica; valores que faltam (carga de uma versão anterior com
 * menos parâmetros) ficam no padrão. Versão diferente é recusada e deixa a configuração como estava.
 *
 * @param cfg Configuração.
 * @param carga Carga lida do armazém.
 * @param tamanho Tamanho da carga.
 * @return CONFIG_OK, CONFIG_ERRO_PARAMETRO, CONFIG_ERRO_TAMANHO ou CONFIG_ERRO_VERSAO.
 */
int32_t config_desempacota(config_t *cfg, const uint8_t *carga, size_t tamanho);

#endif /* CONFIG_H */
