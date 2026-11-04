/**
 * @file io_frequencia.h
 * @brief Tipo das saídas no firmware e o gancho que impede o sleep com saída em frequência.
 *
 * Guarda os tipos das saídas (tipo_saida.h) numa única instância, protegida por seção crítica,
 * e oferece as duas pontas da exclusão entre a saída em frequência e o sleep (FW-061):
 *
 * - io_frequencia_define_tipo(): o comando do mapa Modbus (40166 a 40180) que escolhe o tipo;
 *   quem chama informa se o sleep está ligado (energia_habilitada()).
 * - io_frequencia_sleep_permitido(): o gancho `sleep_permitido` de energia.h, que recusa ligar o
 *   sleep enquanto houver saída em frequência.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef IO_FREQUENCIA_H
#define IO_FREQUENCIA_H

#include <stdbool.h>
#include <stdint.h>

#include "tipo_saida.h"

/**
 * @brief Define o tipo de uma saída, respeitando a exclusão com o sleep.
 *
 * @param canal Saída de 1 a TIPO_SAIDA_QTD (só 2, 3 e 4 aceitam frequência).
 * @param tipo TIPO_SAIDA_DISCRETO ou TIPO_SAIDA_FREQUENCIA.
 * @param sleep_ativo true se o modo sleep está ligado.
 * @return TIPO_SAIDA_OK ou o código de erro de tipo_saida.h.
 */
int32_t io_frequencia_define_tipo(uint8_t canal, uint16_t tipo, bool sleep_ativo);

/**
 * @brief Lê o tipo de uma saída.
 *
 * @param canal Saída de 1 a TIPO_SAIDA_QTD.
 * @param tipo Recebe o tipo.
 * @return TIPO_SAIDA_OK ou TIPO_SAIDA_ERRO_PARAMETRO.
 */
int32_t io_frequencia_tipo(uint8_t canal, uint16_t *tipo);

/**
 * @brief Gancho `sleep_permitido` de energia.h.
 *
 * @return false se alguma saída está em frequência (o sleep não pode ligar).
 */
bool io_frequencia_sleep_permitido(void);

#endif /* IO_FREQUENCIA_H */
