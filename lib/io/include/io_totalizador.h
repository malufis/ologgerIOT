/**
 * @file io_totalizador.h
 * @brief Totalizador de pulsos por entrada, no estilo de um hodômetro.
 *
 * O total só soma e volta a zero ao passar de 32 bits, como um hodômetro. Quem o
 * lê calcula a diferença entre duas leituras, então uma transmissão perdida não
 * perde contagem. O total é guardado fora da RAM, em um armazém de palavras de 32
 * bits (na placa, os registradores de backup). Cada entrada usa duas cópias, cada
 * uma com o valor e o seu complemento; uma gravação interrompida deixa pelo menos
 * uma cópia válida. A lógica não acessa hardware e é testada no PC (tests/io).
 *
 * Não é seguro chamar io_totalizador_soma() ao mesmo tempo de dois contextos: quem
 * chama garante a exclusão (a interrupção externa fica desligada enquanto a tarefa
 * conta).
 *
 * @date criacao 03/11/2026
 * @date modificacao 03/11/2026
 * @version 0.1.0
 */

#ifndef IO_TOTALIZADOR_H
#define IO_TOTALIZADOR_H

#include <stdbool.h>
#include <stdint.h>

/** Quantidade de entradas com totalizador. */
#define IO_TOTALIZADOR_CANAIS         4U
/** Palavras do armazém usadas por entrada: duas cópias de valor e complemento. */
#define IO_TOTALIZADOR_PALAVRAS_CANAL 4U
/** Total de palavras do armazém usadas por todas as entradas. */
#define IO_TOTALIZADOR_PALAVRAS       (IO_TOTALIZADOR_CANAIS * IO_TOTALIZADOR_PALAVRAS_CANAL)

/** Código de retorno: sucesso. */
#define IO_TOTALIZADOR_OK             0
/** Código de retorno: parâmetro nulo ou canal fora de 1 a IO_TOTALIZADOR_CANAIS. */
#define IO_TOTALIZADOR_ERRO_PARAMETRO (-1)

/** Armazém de palavras de 32 bits que sobrevive a reset e ao sleep. */
typedef struct
{
  uint32_t (*le)(uint8_t palavra);                /**< Lê a palavra de índice dado. */
  void (*grava)(uint8_t palavra, uint32_t valor); /**< Grava a palavra de índice dado. */
} io_totalizador_armazem_t;

/**
 * @brief Carrega os totais do armazém e guarda a referência para as gravações seguintes.
 *
 * Uma entrada sem cópia válida começa em zero e é regravada. Entradas corrompidas ficam
 * marcadas, e quem chama pode consultar com io_totalizador_corrompido().
 *
 * @param armazem Armazém que deve continuar válido enquanto o módulo for usado.
 * @return IO_TOTALIZADOR_OK ou IO_TOTALIZADOR_ERRO_PARAMETRO.
 */
int32_t io_totalizador_inicia(const io_totalizador_armazem_t *armazem);

/**
 * @brief Soma pulsos ao total de uma entrada e grava no armazém.
 *
 * @param canal Entrada de 1 a IO_TOTALIZADOR_CANAIS.
 * @param quantidade Pulsos a somar.
 * @return IO_TOTALIZADOR_OK ou IO_TOTALIZADOR_ERRO_PARAMETRO.
 */
int32_t io_totalizador_soma(uint8_t canal, uint32_t quantidade);

/**
 * @brief Define o total de uma entrada, por exemplo para igualar à leitura do medidor.
 *
 * @param canal Entrada de 1 a IO_TOTALIZADOR_CANAIS.
 * @param valor Novo total.
 * @return IO_TOTALIZADOR_OK ou IO_TOTALIZADOR_ERRO_PARAMETRO.
 */
int32_t io_totalizador_define(uint8_t canal, uint32_t valor);

/**
 * @brief Lê o total de uma entrada.
 *
 * @param canal Entrada de 1 a IO_TOTALIZADOR_CANAIS.
 * @return Total acumulado; zero se o canal for inválido.
 */
uint32_t io_totalizador_le(uint8_t canal);

/**
 * @brief Informa se o total de uma entrada foi perdido por corrupção no armazém.
 *
 * @param canal Entrada de 1 a IO_TOTALIZADOR_CANAIS.
 * @return true se nenhuma cópia era válida na última carga e o total recomeçou em zero.
 *         Uma entrada nunca gravada (armazém zerado) também aparece como corrompida.
 */
bool io_totalizador_corrompido(uint8_t canal);

#endif /* IO_TOTALIZADOR_H */
