/**
 * @file test_modbus_rtu.c
 * @brief Testes de unidade do escravo Modbus RTU.
 *
 * @date criacao 04/11/2026
 * @date modificacao 04/11/2026
 * @version 0.1.0
 */

#include <string.h>

#include "modbus_rtu.h"
#include "teste.h"

#define ESCRAVO 5U

/* Estado da aplicação simulada. */
typedef struct
{
  uint16_t retencao[16];
  uint16_t entrada[16];
  bool bobinas[16];
  bool discretas[16];
  uint32_t escritas_retencao;
  uint32_t escritas_bobinas;
  uint16_t ultimo_endereco;
} app_t;

static int32_t sim_le_bobinas(void *c, uint16_t e, uint16_t q, bool *v)
{
  app_t *a = (app_t *)c;
  uint16_t i;

  if ((e + q) > 16U)
  {
    return MODBUS_EXC_ENDERECO;
  }
  for (i = 0U; i < q; i++)
  {
    v[i] = a->bobinas[e + i];
  }
  return MODBUS_OK;
}

static int32_t sim_escreve_bobinas(void *c, uint16_t e, uint16_t q, const bool *v)
{
  app_t *a = (app_t *)c;
  uint16_t i;

  if ((e + q) > 16U)
  {
    return MODBUS_EXC_ENDERECO;
  }
  for (i = 0U; i < q; i++)
  {
    a->bobinas[e + i] = v[i];
  }
  a->escritas_bobinas++;
  return MODBUS_OK;
}

static int32_t sim_le_discretas(void *c, uint16_t e, uint16_t q, bool *v)
{
  app_t *a = (app_t *)c;
  uint16_t i;

  if ((e + q) > 16U)
  {
    return MODBUS_EXC_ENDERECO;
  }
  for (i = 0U; i < q; i++)
  {
    v[i] = a->discretas[e + i];
  }
  return MODBUS_OK;
}

static int32_t sim_le_entrada(void *c, uint16_t e, uint16_t q, uint16_t *r)
{
  app_t *a = (app_t *)c;

  if ((e + q) > 16U)
  {
    return MODBUS_EXC_ENDERECO;
  }
  (void)memcpy(r, &a->entrada[e], (size_t)q * 2U);
  return MODBUS_OK;
}

static int32_t sim_le_retencao(void *c, uint16_t e, uint16_t q, uint16_t *r)
{
  app_t *a = (app_t *)c;

  if ((e + q) > 16U)
  {
    return MODBUS_EXC_ENDERECO;
  }
  (void)memcpy(r, &a->retencao[e], (size_t)q * 2U);
  return MODBUS_OK;
}

static int32_t sim_escreve_retencao(void *c, uint16_t e, uint16_t q, const uint16_t *r)
{
  app_t *a = (app_t *)c;
  uint16_t i;

  if ((e + q) > 16U)
  {
    return MODBUS_EXC_ENDERECO;
  }
  for (i = 0U; i < q; i++)
  {
    if (r[i] == 0xDEADU)
    {
      return MODBUS_EXC_VALOR; /* Valor que a aplicação recusa. */
    }
  }
  (void)memcpy(&a->retencao[e], r, (size_t)q * 2U);
  a->escritas_retencao++;
  a->ultimo_endereco = e;
  return MODBUS_OK;
}

static modbus_ops_t ops_de(app_t *a)
{
  modbus_ops_t o = {a, sim_le_bobinas, sim_escreve_bobinas, sim_le_discretas, sim_le_entrada,
                    sim_le_retencao, sim_escreve_retencao};

  return o;
}

/* Acrescenta o CRC a um quadro e devolve o novo tamanho. */
static size_t com_crc(uint8_t *q, size_t n)
{
  uint16_t crc = modbus_crc16(q, n);

  q[n] = (uint8_t)(crc & 0xFFU);
  q[n + 1U] = (uint8_t)(crc >> 8U);
  return n + 2U;
}

static void crc_dos_vetores_conhecidos(void)
{
  const uint8_t a[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A};
  const uint8_t b[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01};

  VERIFICA(modbus_crc16(a, sizeof(a)) == 0xCDC5U); /* Quadro 01 03 00 00 00 0A C5 CD. */
  VERIFICA(modbus_crc16(b, sizeof(b)) == 0x0A84U); /* Quadro 01 03 00 00 00 01 84 0A. */
}

static void le_retencao_devolve_os_registradores(void)
{
  app_t a = {{0x1234U, 0xABCDU, 7U}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x03, 0x00, 0x00, 0x00, 0x03};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t n = com_crc(req, 6U);
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, n, resp);

  VERIFICA(r == 11U);
  VERIFICA(resp[0] == ESCRAVO && resp[1] == 0x03 && resp[2] == 6U);
  VERIFICA(resp[3] == 0x12 && resp[4] == 0x34 && resp[5] == 0xAB && resp[6] == 0xCD && resp[8] == 7U);
  VERIFICA(modbus_crc16(resp, r) == 0U);
}

static void le_entrada_pela_funcao_04(void)
{
  app_t a = {{0}, {0x0001U, 0x4248U}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x04, 0x00, 0x01, 0x00, 0x01};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp);

  VERIFICA(r == 7U && resp[1] == 0x04 && resp[3] == 0x42 && resp[4] == 0x48);
}

static void le_bobinas_empacotando_os_bits(void)
{
  app_t a = {{0}, {0}, {true, false, true, true, false, false, false, false, true, true}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x01, 0x00, 0x00, 0x00, 0x0A};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp);

  VERIFICA(r == 7U && resp[2] == 2U);
  VERIFICA(resp[3] == 0x0DU); /* Bobinas 0, 2 e 3 ligadas: 0b00001101. */
  VERIFICA(resp[4] == 0x03U); /* Bobinas 8 e 9 ligadas. */
}

static void le_discretas_pela_funcao_02(void)
{
  app_t a = {{0}, {0}, {0}, {false, true, true}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x02, 0x00, 0x00, 0x00, 0x03};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp);

  VERIFICA(r == 6U && resp[1] == 0x02 && resp[3] == 0x06U);
}

static void escreve_um_registrador_e_ecoa_o_pedido(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x06, 0x00, 0x03, 0x12, 0x34};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t n = com_crc(req, 6U);
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, n, resp);

  VERIFICA(r == 8U && memcmp(resp, req, 8U) == 0);
  VERIFICA(a.retencao[3] == 0x1234U);
}

static void escreve_varios_registradores(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[16] = {ESCRAVO, 0x10, 0x00, 0x02, 0x00, 0x02, 0x04, 0x00, 0x0A, 0x01, 0x02};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 11U), resp);

  VERIFICA(r == 8U && resp[1] == 0x10 && resp[5] == 0x02);
  VERIFICA(a.retencao[2] == 0x000AU && a.retencao[3] == 0x0102U && a.escritas_retencao == 1U);
}

static void escreve_bobina_liga_e_desliga(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x05, 0x00, 0x01, 0xFF, 0x00};
  uint8_t resp[MODBUS_QUADRO_MAX];

  VERIFICA(modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp) == 8U);
  VERIFICA(a.bobinas[1] == true);
  req[4] = 0x00;
  VERIFICA(modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp) == 8U);
  VERIFICA(a.bobinas[1] == false);
}

static void bobina_com_valor_estranho_e_recusada(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x05, 0x00, 0x01, 0x12, 0x34};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp);

  VERIFICA(r == 5U && resp[1] == 0x85 && resp[2] == MODBUS_EXC_VALOR);
  VERIFICA(a.escritas_bobinas == 0U);
}

static void escreve_varias_bobinas(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[16] = {ESCRAVO, 0x0F, 0x00, 0x00, 0x00, 0x0A, 0x02, 0x0D, 0x02};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 9U), resp);

  VERIFICA(r == 8U && resp[1] == 0x0F);
  VERIFICA(a.bobinas[0] && !a.bobinas[1] && a.bobinas[2] && a.bobinas[3] && a.bobinas[9]);
}

static void endereco_ilegal_vira_excecao_02(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x03, 0x00, 0x20, 0x00, 0x01};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp);

  VERIFICA(r == 5U && resp[1] == 0x83 && resp[2] == MODBUS_EXC_ENDERECO && modbus_crc16(resp, r) == 0U);
}

static void quantidade_invalida_vira_excecao_03(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x03, 0x00, 0x00, 0x00, 0x00};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp);

  VERIFICA(r == 5U && resp[2] == MODBUS_EXC_VALOR);
  req[5] = 126U; /* Acima de 125. */
  r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp);
  VERIFICA(r == 5U && resp[2] == MODBUS_EXC_VALOR);
}

static void valor_recusado_pela_aplicacao_vira_excecao_03(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x06, 0x00, 0x00, 0xDE, 0xAD};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp);

  VERIFICA(r == 5U && resp[1] == 0x86 && resp[2] == MODBUS_EXC_VALOR && a.escritas_retencao == 0U);
}

static void funcao_desconhecida_vira_excecao_01(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x2B, 0x00, 0x00, 0x00, 0x00};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp);

  VERIFICA(r == 5U && resp[1] == 0xABU && resp[2] == MODBUS_EXC_FUNCAO);
}

static void operacao_ausente_vira_excecao_01(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x03, 0x00, 0x00, 0x00, 0x01};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r;

  o.le_retencao = NULL;
  r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp);
  VERIFICA(r == 5U && resp[2] == MODBUS_EXC_FUNCAO);
}

static void quadro_com_crc_errado_e_descartado(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x03, 0x00, 0x00, 0x00, 0x01};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t n = com_crc(req, 6U);

  req[3] ^= 0x01U;
  VERIFICA(modbus_rtu_processa(&o, ESCRAVO, req, n, resp) == 0U);
}

static void quadro_de_outro_escravo_e_descartado(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO + 1U, 0x03, 0x00, 0x00, 0x00, 0x01};
  uint8_t resp[MODBUS_QUADRO_MAX];

  VERIFICA(modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp) == 0U);
}

static void quadro_curto_ou_nulo_e_descartado(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {ESCRAVO, 0x03, 0x00};
  uint8_t resp[MODBUS_QUADRO_MAX];

  VERIFICA(modbus_rtu_processa(&o, ESCRAVO, req, 3U, resp) == 0U);
  VERIFICA(modbus_rtu_processa(NULL, ESCRAVO, req, 8U, resp) == 0U);
  VERIFICA(modbus_rtu_processa(&o, ESCRAVO, NULL, 8U, resp) == 0U);
  VERIFICA(modbus_rtu_processa(&o, ESCRAVO, req, MODBUS_QUADRO_MAX + 1U, resp) == 0U);
}

static void difusao_executa_a_escrita_e_nao_responde(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {0x00, 0x06, 0x00, 0x04, 0x00, 0x2A};
  uint8_t resp[MODBUS_QUADRO_MAX];

  VERIFICA(modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp) == 0U);
  VERIFICA(a.retencao[4] == 0x2AU);
}

static void difusao_de_leitura_e_ignorada(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[8] = {0x00, 0x03, 0x00, 0x00, 0x00, 0x01};
  uint8_t resp[MODBUS_QUADRO_MAX];

  VERIFICA(modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 6U), resp) == 0U);
}

static void escrita_com_contagem_de_bytes_errada_e_recusada(void)
{
  app_t a = {{0}, {0}, {0}, {0}, 0U, 0U, 0U};
  modbus_ops_t o = ops_de(&a);
  uint8_t req[16] = {ESCRAVO, 0x10, 0x00, 0x02, 0x00, 0x02, 0x06, 0x00, 0x0A, 0x01, 0x02};
  uint8_t resp[MODBUS_QUADRO_MAX];
  size_t r = modbus_rtu_processa(&o, ESCRAVO, req, com_crc(req, 11U), resp);

  VERIFICA(r == 5U && resp[2] == MODBUS_EXC_VALOR && a.escritas_retencao == 0U);
}

int main(void)
{
  EXECUTA(crc_dos_vetores_conhecidos);
  EXECUTA(le_retencao_devolve_os_registradores);
  EXECUTA(le_entrada_pela_funcao_04);
  EXECUTA(le_bobinas_empacotando_os_bits);
  EXECUTA(le_discretas_pela_funcao_02);
  EXECUTA(escreve_um_registrador_e_ecoa_o_pedido);
  EXECUTA(escreve_varios_registradores);
  EXECUTA(escreve_bobina_liga_e_desliga);
  EXECUTA(bobina_com_valor_estranho_e_recusada);
  EXECUTA(escreve_varias_bobinas);
  EXECUTA(endereco_ilegal_vira_excecao_02);
  EXECUTA(quantidade_invalida_vira_excecao_03);
  EXECUTA(valor_recusado_pela_aplicacao_vira_excecao_03);
  EXECUTA(funcao_desconhecida_vira_excecao_01);
  EXECUTA(operacao_ausente_vira_excecao_01);
  EXECUTA(quadro_com_crc_errado_e_descartado);
  EXECUTA(quadro_de_outro_escravo_e_descartado);
  EXECUTA(quadro_curto_ou_nulo_e_descartado);
  EXECUTA(difusao_executa_a_escrita_e_nao_responde);
  EXECUTA(difusao_de_leitura_e_ignorada);
  EXECUTA(escrita_com_contagem_de_bytes_errada_e_recusada);
  return RESULTADO();
}
