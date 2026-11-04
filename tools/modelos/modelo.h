/**
 * @file modelo.h
 * @brief Descrição curta do módulo, em uma frase.
 *
 * Descrição mais longa, se necessária: responsabilidade do módulo e o que
 * ele não faz. Cada módulo tem uma responsabilidade (ver
 * documentacao/padrao_codigo_c.md).
 *
 * @date criacao DD/MM/AAAA
 * @date modificacao DD/MM/AAAA
 * @version 0.1.0
 */

#ifndef MODELO_H
#define MODELO_H

#include <stdint.h>

/** Tempo máximo de espera da operação, em milissegundos. */
#define MODELO_TIMEOUT_MS 100U

/**
 * @brief Resumo do que a função faz, em uma frase.
 *
 * @param[in]  entrada  Descrição do parâmetro de entrada, com unidade.
 * @param[out] saida    Descrição do parâmetro de saída.
 * @return     0 em sucesso, código negativo em erro.
 * @note       Observação útil ao chamador, quando houver.
 */
int32_t modelo_executa(uint32_t entrada, uint32_t *saida);

#endif /* MODELO_H */
