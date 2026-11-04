# Desvios do MISRA C:2012 — firmware oLoggerIOT

> **Criado em:** 04/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.0

Registro dos desvios deliberados das regras da base MISRA C:2012 adotada em `documentacao/padrao_codigo_c.md`. Todo desvio no código leva um comentário `MISRA-DEV` no ponto onde ocorre e uma linha aqui, com a regra, o motivo e o risco. O projeto ainda **não tem ferramenta de verificação estática MISRA** (proposta em `documentacao/adr/ADR-0002-analise-estatica-misra.md`), então a lista abaixo vem da marcação no código e da revisão, não de um relatório de ferramenta.

| ID | Arquivo | Regra | Motivo | Risco e como é contido | Estado |
|---|---|---|---|---|---|
| DEV-001 | `lib/adc/src/adc.c`, em `adc_inicia` (comentário `MISRA-DEV`) | 11.5 (conversão de ponteiro para `void` em ponteiro para objeto), conforme marcado no código. A conversão passa de `uint16_t *` por `void *` para `uint32_t *`, então a regra 11.3 (conversão entre ponteiros para tipos de objeto diferentes) também pode se aplicar | A API do HAL (`HAL_ADC_Start_DMA`) recebe o buffer como `uint32_t *`, mesmo com o ADC e o DMA configurados para palavras de 16 bits | O buffer é `uint16_t` alinhado em 4 bytes (`__attribute__((aligned(4)))`) e o tamanho passado ao HAL conta palavras de 16 bits; o DMA escreve só 16 bits por posição. Medido em bancada com o ADC funcionando em 02/11/2026 | Registrado em 04/11/2026; a regra exata (11.3 ou 11.5) deve ser confirmada quando houver ferramenta |

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.0 | Criado com o desvio DEV-001, que já estava marcado no código, sem registro, desde 02/11/2026 |
