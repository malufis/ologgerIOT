/**
 * @file teste.h
 * @brief Macros mínimas para testes de unidade no PC.
 *
 * Cada teste é uma função void. VERIFICA registra a falha e segue, para um
 * teste mostrar todos os problemas de uma vez. EXECUTA roda um teste e conta.
 *
 * @date criacao 02/11/2026
 * @date modificacao 02/11/2026
 * @version 0.1.0
 */

#ifndef TESTE_H
#define TESTE_H

#include <math.h>
#include <stdio.h>

static int teste_total_verificacoes;
static int teste_total_falhas;

/** Verifica uma condição e registra a falha com arquivo e linha. */
#define VERIFICA(cond)                                                                    \
  do                                                                                      \
  {                                                                                       \
    teste_total_verificacoes++;                                                           \
    if (!(cond))                                                                          \
    {                                                                                     \
      teste_total_falhas++;                                                               \
      printf("  FALHA %s:%d: %s\n", __FILE__, __LINE__, #cond);                           \
    }                                                                                     \
  } while (0)

/** Verifica que dois números diferem no máximo pela tolerância. */
#define VERIFICA_PROXIMO(obtido, esperado, tolerancia)                                    \
  do                                                                                      \
  {                                                                                       \
    teste_total_verificacoes++;                                                           \
    if (fabs((double)(obtido) - (double)(esperado)) > (double)(tolerancia))               \
    {                                                                                     \
      teste_total_falhas++;                                                               \
      printf("  FALHA %s:%d: %s = %f, esperado %f (tolerancia %f)\n", __FILE__, __LINE__, \
             #obtido, (double)(obtido), (double)(esperado), (double)(tolerancia));        \
    }                                                                                     \
  } while (0)

/** Executa um teste e imprime o nome. */
#define EXECUTA(nome)                                                                     \
  do                                                                                      \
  {                                                                                       \
    printf("%s\n", #nome);                                                                \
    nome();                                                                               \
  } while (0)

/** Resultado final do programa de teste: 0 se nenhuma verificação falhou. */
#define RESULTADO()                                                                       \
  (printf("%d verificacoes, %d falhas\n", teste_total_verificacoes, teste_total_falhas), \
   (teste_total_falhas == 0) ? 0 : 1)

#endif /* TESTE_H */
