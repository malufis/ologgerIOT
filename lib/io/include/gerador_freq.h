/**
 * @file gerador_freq.h
 * @brief Gerador de onda quadrada de 50% de duty por acumulador de fase (uma saída em frequência).
 *
 * Lógica pura, sem acesso a hardware, testada no PC (tests/io). Cada saída em frequência tem o
 * seu gerador. Um timer chama gerador_freq_tick() a uma taxa fixa (GERADOR_FREQ_TICK_HZ) e usa o
 * nível devolvido para escrever o pino. Como cada saída tem o seu acumulador de 32 bits, as
 * frequências das saídas são **independentes**, ainda que o timer seja um só (os canais de timer
 * dos pinos PB13 a PB15 dividem o período entre si).
 *
 * - **Frequência:** o passo do acumulador vale `f / GERADOR_FREQ_TICK_HZ` de volta por tick. A
 *   frequência média é exata dentro de 2 mHz (resolução do passo em ponto flutuante); cada borda pode atrasar até um tick.
 * - **Duty cycle:** 50% (a fase abaixo da metade da volta fica em nível alto).
 * - **0 Hz:** a saída fica em nível baixo (transistor desligado).
 * - **Limite:** de 0 a GERADOR_FREQ_MAX_HZ, a faixa de projeto de 20 kHz. Com 100 kHz de tick, um
 *   período de 20 kHz tem só cinco ticks: a frequência média continua exata, mas o duty fica em
 *   60% (ou 40%) em vez de 50% e a borda treme até 20% do período. Quanto menor a frequência,
 *   mais perto de 50%: a 1 kHz o erro do duty é de 0,1%. Para duty de 50% no topo da faixa, a taxa
 *   do tick teria de subir para 200 kHz, ao custo de dobrar a carga da interrupção.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#ifndef GERADOR_FREQ_H
#define GERADOR_FREQ_H

#include <stdbool.h>
#include <stdint.h>

/** Taxa em que o timer chama gerador_freq_tick(), em Hz. */
#define GERADOR_FREQ_TICK_HZ 100000U
/** Frequência máxima aceita, em Hz (faixa de projeto). */
#define GERADOR_FREQ_MAX_HZ  20000.0F

/** Código de retorno: sucesso. */
#define GERADOR_FREQ_OK              0
/** Código de retorno: parâmetro nulo. */
#define GERADOR_FREQ_ERRO_PARAMETRO  (-1)
/** Código de retorno: frequência não numérica, negativa ou acima de GERADOR_FREQ_MAX_HZ. */
#define GERADOR_FREQ_ERRO_FREQUENCIA (-2)

/** Estado de um gerador. */
typedef struct
{
  uint32_t fase;  /**< Acumulador de fase: a volta inteira são 2^32. */
  uint32_t passo; /**< Avanço da fase por tick; zero com a saída parada. */
} gerador_freq_t;

/**
 * @brief Para o gerador: nível baixo e fase zerada.
 *
 * @param gerador Gerador a limpar.
 * @return GERADOR_FREQ_OK, ou GERADOR_FREQ_ERRO_PARAMETRO se @p gerador for nulo.
 */
int32_t gerador_freq_limpa(gerador_freq_t *gerador);

/**
 * @brief Define a frequência do gerador.
 *
 * Mudar de uma frequência para outra mantém a fase, sem salto no sinal. Passar de 0 Hz a uma
 * frequência começa da fase zero. Zero Hz para o gerador e leva a saída ao nível baixo.
 *
 * @param gerador Gerador.
 * @param hz Frequência em Hz, de 0 a GERADOR_FREQ_MAX_HZ.
 * @return GERADOR_FREQ_OK, ou o código de erro (o gerador fica como estava).
 */
int32_t gerador_freq_define(gerador_freq_t *gerador, float hz);

/**
 * @brief Avança um tick e devolve o nível da saída.
 *
 * Curta e sem laços: feita para rodar na interrupção do timer.
 *
 * @param gerador Gerador.
 * @return true para nível alto (transistor ligado), false para nível baixo.
 */
bool gerador_freq_tick(gerador_freq_t *gerador);

#endif /* GERADOR_FREQ_H */
