/**
 * @file io_backup.h
 * @brief Armazém de palavras nos registradores de backup do STM32L412 (TAMP).
 *
 * Os registradores de backup sobrevivem a reset e ao Stop 2, e à falta de energia
 * enquanto houver bateria no pino VBAT. Servem de armazém do totalizador de pulsos.
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#ifndef IO_BACKUP_H
#define IO_BACKUP_H

#include <stdint.h>

/** Quantidade de palavras de backup disponíveis neste módulo. */
#define IO_BACKUP_PALAVRAS 16U

/**
 * @brief Liga o acesso ao domínio de backup e o clock do TAMP.
 */
void io_backup_inicia(void);

/**
 * @brief Lê uma palavra de backup.
 *
 * @param palavra Índice de 0 a IO_BACKUP_PALAVRAS - 1.
 * @return Valor da palavra; zero se o índice for inválido.
 */
uint32_t io_backup_le(uint8_t palavra);

/**
 * @brief Grava uma palavra de backup.
 *
 * @param palavra Índice de 0 a IO_BACKUP_PALAVRAS - 1; índice inválido é ignorado.
 * @param valor Valor a gravar.
 */
void io_backup_grava(uint8_t palavra, uint32_t valor);

#endif /* IO_BACKUP_H */
