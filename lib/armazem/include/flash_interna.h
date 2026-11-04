/**
 * @file flash_interna.h
 * @brief Operações do armazém sobre a flash interna do STM32L412 (as duas últimas páginas).
 *
 * Reserva as **duas últimas páginas de 2 KB** da flash interna (0x0800F000 e 0x0800F800) para o
 * armazém de configuração (armazem.h). O script de ligação (`STM32L412C8_FLASH.ld`) tira essas
 * duas páginas da área do programa, então a imagem nunca as ocupa. Elas ficam no fim da flash,
 * longe do início onde o bootloader do ADR-0003 irá ficar, e a atualização da imagem não as toca
 * (a posição definitiva sai do FW-062).
 *
 * Apagar e gravar a flash para o processador enquanto o código roda dela (cerca de 22 ms por
 * página): as interrupções esperam. Por isso só se grava com pouca frequência (a cada comando do
 * CCO, depois de um intervalo sem novas mudanças). Sem teste em hardware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef FLASH_INTERNA_H
#define FLASH_INTERNA_H

#include "armazem.h"

/** Endereço do primeiro espaço do armazém. */
#define FLASH_INTERNA_ESPACO_0   0x0800F000UL
/** Endereço do segundo espaço do armazém. */
#define FLASH_INTERNA_ESPACO_1   0x0800F800UL
/** Tamanho de cada espaço, igual ao da página da flash. */
#define FLASH_INTERNA_TAMANHO    2048U

/**
 * @brief Devolve as operações do armazém sobre as duas últimas páginas da flash interna.
 *
 * @return Ponteiro para uma tabela constante, válida durante toda a execução.
 */
const armazem_ops_t *flash_interna_ops(void);

#endif /* FLASH_INTERNA_H */
