/**
 * @file modbus_mapa.h
 * @brief Mapa Modbus do OloggerMulti: liga os registradores do mapa ao firmware.
 *
 * Implementa as operações do escravo Modbus (modbus_rtu.h) conforme `documentacao/mapa_modbus.md`.
 * Os endereços de protocolo começam em 0; o número do mapa é o endereço mais 1 somado à base da tabela.
 *
 * | Tabela | Números | Conteúdo |
 * |---|---|---|
 * | Bobinas (função 01, 05, 15) | 00001 a 00004 | Estado e comando manual das quatro saídas |
 * | Entradas discretas (02) | 10001 a 10004, 10016 a 10019, 10031 | Entradas digitais, falha do sensor analógico n, hora válida |
 * | Registradores de entrada (04) | 30001 a 30008, 30031 a 30038, 30091 a 30098, 30121 a 30126 | Valor de engenharia (float), corrente em mA (float), frequência atual da saída (float), data e hora local |
 * | Registradores de retenção (03, 06, 16) | 40001 a 40413 | Configuração (config.h), mais 40286 a 40289 (modo manual ou automático das saídas) |
 *
 * - **Retenção:** a configuração toda é lida e gravada por config_tarefa_le() e config_tarefa_escreve(),
 *   com escrita atômica. O modo manual ou automático (40286 a 40289, 0 = automático, 1 = manual) é estado,
 *   e não configuração: não é gravado na flash. Um pedido não pode misturar esses registradores com os
 *   outros (exceção 02).
 * - **Bobina:** escrever liga ou desliga a saída em modo manual (io_saidas_comando_manual()).
 * - **Float:** dois registradores, palavra alta primeiro (ABCD). Entrada analógica ainda sem leitura
 *   responde zero.
 * - **Data e hora (30121 a 30126):** ano, mês, dia, hora, minuto e segundo locais (com o fuso da
 *   configuração); tudo zero se o relógio não tem hora válida.
 *
 * O totalizador de pulsos e as medições de tensão e temperatura ainda não têm endereço no mapa e ficam
 * de fora. Sem teste em hardware.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef MODBUS_MAPA_H
#define MODBUS_MAPA_H

#include "modbus_rtu.h"

/**
 * @brief Operações do escravo ligadas ao firmware.
 *
 * @return Ponteiro para uma tabela constante, válida durante toda a execução.
 */
const modbus_ops_t *modbus_mapa_ops(void);

#endif /* MODBUS_MAPA_H */
