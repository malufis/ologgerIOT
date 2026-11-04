/**
 * @file modbus_rtu.h
 * @brief Escravo Modbus RTU: validação do quadro, execução das funções e montagem da resposta.
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/modbus). Recebe um quadro RTU completo (já
 * separado pelo silêncio de 3,5 caracteres), confere o endereço e o CRC-16, executa a função pelo
 * conjunto de operações da aplicação (modbus_ops_t) e monta o quadro de resposta.
 *
 * Funções atendidas: 01 (ler bobinas), 02 (ler entradas discretas), 03 (ler registradores de
 * retenção), 04 (ler registradores de entrada), 05 (escrever uma bobina), 06 (escrever um registrador),
 * 15 (escrever várias bobinas) e 16 (escrever vários registradores). Outra função responde com a
 * exceção 01 (função ilegal).
 *
 * - **Endereço:** o do escravo ou 0 (difusão). Na difusão só as funções de escrita são executadas e
 *   nada é respondido. Quadro com outro endereço ou com CRC errado é descartado sem resposta.
 * - **Endereçamento:** a aplicação recebe o endereço de protocolo (começando em 0); o número do mapa é o
 *   do protocolo mais 1, somado à base da tabela (por exemplo, retenção 40001 é o endereço 0).
 * - **Limites:** até 125 registradores e 256 bobinas por leitura, 123 registradores e 256 bobinas por
 *   escrita (o protocolo permite 2000 e 1968, mas o device tem só 4 saídas); fora disso, exceção 03 (valor
 *   ilegal). O quadro tem no máximo 256 bytes.
 * - **Exceções:** 01 função ilegal, 02 endereço ilegal, 03 valor ilegal, 04 falha no escravo.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Tamanho máximo de um quadro RTU, em bytes. */
#define MODBUS_QUADRO_MAX     256U
/** Mais registradores por leitura. */
#define MODBUS_LER_REG_MAX    125U
/** Mais bobinas por leitura (o device tem poucas; limite menor que o do protocolo para poupar a pilha). */
#define MODBUS_LER_BIT_MAX    256U
/** Mais registradores por escrita. */
#define MODBUS_ESCREVER_REG_MAX 123U
/** Mais bobinas por escrita. */
#define MODBUS_ESCREVER_BIT_MAX 256U

/** Resultado de uma operação da aplicação: sucesso. */
#define MODBUS_OK              0
/** Exceção 01: função ilegal. */
#define MODBUS_EXC_FUNCAO      1
/** Exceção 02: endereço ilegal. */
#define MODBUS_EXC_ENDERECO    2
/** Exceção 03: valor ilegal. */
#define MODBUS_EXC_VALOR       3
/** Exceção 04: falha no escravo. */
#define MODBUS_EXC_FALHA       4

/**
 * Operações da aplicação. Cada uma devolve MODBUS_OK ou o código de exceção (MODBUS_EXC_*), e recebe
 * o endereço de protocolo (começando em 0). Uma operação nula responde com exceção 01 (função ilegal).
 */
typedef struct
{
  void *contexto; /**< Repassado a todas as operações. */
  int32_t (*le_bobinas)(void *contexto, uint16_t endereco, uint16_t quantidade, bool *valores);
  int32_t (*escreve_bobinas)(void *contexto, uint16_t endereco, uint16_t quantidade, const bool *valores);
  int32_t (*le_discretas)(void *contexto, uint16_t endereco, uint16_t quantidade, bool *valores);
  int32_t (*le_entrada)(void *contexto, uint16_t endereco, uint16_t quantidade, uint16_t *registradores);
  int32_t (*le_retencao)(void *contexto, uint16_t endereco, uint16_t quantidade, uint16_t *registradores);
  int32_t (*escreve_retencao)(void *contexto, uint16_t endereco, uint16_t quantidade,
                              const uint16_t *registradores);
} modbus_ops_t;

/**
 * @brief Calcula o CRC-16 do Modbus (polinômio 0xA001, valor inicial 0xFFFF).
 *
 * @param dados Bytes.
 * @param tamanho Número de bytes.
 * @return CRC; no quadro vai o byte baixo primeiro.
 */
uint16_t modbus_crc16(const uint8_t *dados, size_t tamanho);

/**
 * @brief Processa um quadro recebido e monta a resposta.
 *
 * @param ops Operações da aplicação.
 * @param escravo Endereço deste escravo, de 1 a 247.
 * @param pedido Quadro recebido, com o CRC.
 * @param tamanho Tamanho do quadro.
 * @param resposta Recebe o quadro de resposta, com o CRC; pelo menos MODBUS_QUADRO_MAX bytes.
 * @return Tamanho da resposta, ou 0 se não há o que responder (quadro de outro escravo, CRC errado,
 *         quadro curto ou difusão).
 */
size_t modbus_rtu_processa(const modbus_ops_t *ops, uint8_t escravo, const uint8_t *pedido, size_t tamanho,
                           uint8_t *resposta);

#endif /* MODBUS_RTU_H */
