/**
 * @file armazem.h
 * @brief Armazém de configuração persistente em dois espaços da flash, seguro contra queda de energia.
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/armazem) com uma flash simulada. Guarda
 * um bloco de bytes (a "carga": a agenda das saídas e o fuso horário, por exemplo) em um de dois
 * espaços da flash. Cada gravação:
 *
 * 1. escolhe o espaço que **não** tem o registro mais recente;
 * 2. apaga esse espaço;
 * 3. grava o registro novo (cabeçalho com número de sequência e CRC-32, mais a carga);
 * 4. relê e confere o CRC antes de dar o registro como gravado.
 *
 * Uma queda de energia em qualquer ponto deixa o registro anterior intacto no outro espaço, e a
 * leitura devolve sempre o registro válido de maior sequência. Registro com CRC errado é ignorado.
 * Com a flash apagada (0xFF) ou sem nenhum registro válido, a leitura devolve ARMAZEM_ERRO_VAZIO e
 * quem chama usa os valores de fábrica.
 *
 * O desgaste da flash fica por conta de quem decide quando gravar: uma gravação a cada comando do
 * CCO é suficiente (a flash do STM32L4 aguenta 10 mil ciclos por página).
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef ARMAZEM_H
#define ARMAZEM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Número de espaços de flash do armazém. */
#define ARMAZEM_ESPACOS      2U
/** Tamanho do cabeçalho do registro, em bytes. */
#define ARMAZEM_CABECALHO    16U
/** Maior carga aceita, em bytes. */
#define ARMAZEM_CARGA_MAX    256U
/** A gravação na flash é em múltiplos deste tamanho, em bytes (palavra dupla do STM32L4). */
#define ARMAZEM_ALINHAMENTO  8U

/** Código de retorno: sucesso. */
#define ARMAZEM_OK              0
/** Código de retorno: parâmetro nulo ou inválido. */
#define ARMAZEM_ERRO_PARAMETRO  (-1)
/** Código de retorno: nenhum registro válido na flash. */
#define ARMAZEM_ERRO_VAZIO      (-2)
/** Código de retorno: a carga não cabe (no registro ou no buffer de quem lê). */
#define ARMAZEM_ERRO_TAMANHO    (-3)
/** Código de retorno: a flash falhou ao apagar ou gravar, ou a conferência da gravação falhou. */
#define ARMAZEM_ERRO_FLASH      (-4)

/** Operações da flash que o armazém usa; o driver do hardware ou uma flash simulada as fornece. */
typedef struct
{
  /** Endereço de leitura do espaço (a flash interna é mapeada na memória). */
  const uint8_t *(*endereco)(uint8_t espaco);
  /** Apaga o espaço inteiro; devolve 0 em sucesso. */
  int32_t (*apaga)(uint8_t espaco);
  /** Grava @p tamanho bytes (múltiplo de ARMAZEM_ALINHAMENTO) no início do espaço; 0 em sucesso. */
  int32_t (*grava)(uint8_t espaco, const uint8_t *dados, size_t tamanho);
  /** Tamanho de cada espaço, em bytes. */
  size_t tamanho_espaco;
} armazem_ops_t;

/** Estado do armazém. */
typedef struct
{
  const armazem_ops_t *ops; /**< Operações da flash. */
  uint32_t sequencia;       /**< Sequência do registro mais recente. */
  int8_t ultimo;            /**< Espaço do registro mais recente, ou -1 se não há. */
} armazem_t;

/**
 * @brief Inicia o armazém e procura o registro válido mais recente.
 *
 * @param armazem Armazém a iniciar.
 * @param ops Operações da flash; precisam durar enquanto o armazém existir.
 * @return ARMAZEM_OK, ou ARMAZEM_ERRO_PARAMETRO se algum ponteiro for nulo ou o espaço for pequeno.
 */
int32_t armazem_inicia(armazem_t *armazem, const armazem_ops_t *ops);

/**
 * @brief Lê a carga do registro válido mais recente.
 *
 * @param armazem Armazém iniciado.
 * @param carga Recebe a carga.
 * @param capacidade Tamanho de @p carga, em bytes.
 * @param tamanho Recebe o tamanho da carga lida.
 * @return ARMAZEM_OK, ARMAZEM_ERRO_PARAMETRO, ARMAZEM_ERRO_VAZIO ou ARMAZEM_ERRO_TAMANHO.
 */
int32_t armazem_le(const armazem_t *armazem, uint8_t *carga, size_t capacidade, size_t *tamanho);

/**
 * @brief Grava uma carga nova, sem apagar a anterior até a nova estar conferida.
 *
 * @param armazem Armazém iniciado.
 * @param carga Carga a gravar.
 * @param tamanho Tamanho da carga, de 1 a ARMAZEM_CARGA_MAX.
 * @return ARMAZEM_OK, ARMAZEM_ERRO_PARAMETRO, ARMAZEM_ERRO_TAMANHO ou ARMAZEM_ERRO_FLASH (a carga
 *         anterior continua valendo).
 */
int32_t armazem_grava(armazem_t *armazem, const uint8_t *carga, size_t tamanho);

#endif /* ARMAZEM_H */
