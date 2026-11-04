# ADR-0003 — Atualização de firmware, espaço de flash e proteção do código e das credenciais

> **Criado em:** 04/11/2026 · **Última modificação:** 04/11/2026 · **Versão:** 0.1.1

**Estado:** Decidido em 04/11/2026 pelo responsável técnico: opção 2 (imagem na flash externa e bootloader).

## Contexto

- **Requisito** (`documentacao/atualizacao_firmware.md`): verificar validade e compatibilidade do pacote, recuperar a versão anterior se a atualização falhar ou faltar energia, manter as saídas desligadas durante a atualização e informar o resultado ao sistema. O mecanismo técnico, a autenticação e o armazenamento ainda não foram definidos (itens FW-040 a FW-042).
- **Espaço de flash do STM32 (verificado na base do STM32CubeMX em 04/11/2026):** o STM32L412**C8** do OloggerMulti tem **64 KB de flash e 40 KB de RAM**; o STM32L412**CB** tem 128 KB e a mesma RAM. A imagem atual do OloggerMulti usa **25,8 KB de flash e 8,6 KB de RAM**, e ainda faltam Modbus, fila, agenda, sessão e o protocolo com o ESP32.
- **Flash externa:** 16 MiB (GD25Q128), em ambas as placas, ainda sem driver (FW-018).
- **Credenciais do ESP32** (pendências P-004 e P-006): armazenamento em NVS criptografada ou criptografia de flash, e secure boot a registrar antes da fase 7.

## Opções para a atualização do STM32

1. **Dois espaços de imagem na flash interna** (atual e nova), com troca por bandeira. No L412C8 de 64 KB, cada espaço cabe só se a imagem ficar em torno de metade da flash menos o bootloader; a imagem atual já usa 25,8 KB e vai crescer. **Provavelmente não cabe.**
2. **Imagem nova guardada na flash externa**, validada ali, e um bootloader que copia para a flash interna. Mantém também a imagem anterior na flash externa para recuperação. Exige reservar espaço para o bootloader na flash interna e um driver confiável da flash externa antes da atualização.
3. **Pilha pronta da ST** (X-CUBE-SBSFU), se tiver suporte aos dois MCUs. *Suporte ao STM32L4 e ao STM32L0 e uso de memória não foram verificados.*
4. **Trocar o L412C8 pelo L412CB (128 KB)** na próxima revisão da placa, mantendo a opção 1 viável por mais tempo. A base do CubeMX lista os dois com o mesmo encapsulamento UFQFPN48; **conferir no datasheet**.

## Opções para a proteção

- **STM32:** o L412 e o L071 não têm TrustZone. Há proteção de leitura (RDP) e de escrita da flash. A autenticação da imagem seria por assinatura verificada pelo bootloader. *A verificar nos manuais de referência.*
- **ESP32-C6:** o ESP-IDF tem secure boot e criptografia de flash. *Suporte e requisitos na versão v5.4.4 a verificar.* A credencial do broker é individual por equipamento, e a rotação remota é pretendida.

## Consequências

- A opção 1 deve ser descartada se a imagem passar de ~28 KB de código. O firmware já está em 25,8 KB, então a decisão não pode esperar a fase 9.
- O espaço de flash do L412C8 também limita os módulos que ainda vão entrar. Vale medir o tamanho por fase (item de backlog sugerido) antes de comprometer o layout.
- A opção 4 é uma decisão de hardware; foi anotada em `documentacao/melhorias_hardware.md`.
- O FW-040 a FW-042 e o FW-018 (driver da flash externa) dependem desta decisão.

## Decisão

**Decidido em 04/11/2026 pelo responsável técnico: opção 2.** A imagem nova fica na flash externa de 16 MiB, é validada ali (integridade e compatibilidade, FW-040), e um bootloader a copia para a flash interna. A imagem anterior continua guardada na flash externa para a recuperação (FW-041).

- **Não adotadas:** a opção 1 (dois espaços internos, não cabe), a 3 (X-CUBE-SBSFU, não verificada) e a 4 (trocar para o L412CB). A troca de chip **não** foi escolhida; fica só como contingência, em `documentacao/melhorias_hardware.md` (item 8), se o bootloader e a aplicação não couberem em 64 KB.
- **Consequências para o backlog:** o driver da flash externa (FW-018) passa a ser pré-requisito da atualização. Novo item **FW-062**: reservar o espaço do bootloader no layout da flash interna, mover o início da aplicação no linker e registrar a folga de flash e de RAM por fase. Os itens FW-040 a FW-042 seguem como estão, agora com este mecanismo.
- **Ainda em aberto:** o tamanho do bootloader, o formato do pacote e a assinatura (proteção da imagem), a proteção das credenciais do ESP32 (P-004 e P-006) e o secure boot do ESP32-C6. Entram nos itens FW-040 a FW-042 e na fase 7.

## Histórico

| Data | Versão | Alteração |
|---|---|---|
| 04/11/2026 | 0.1.1 | Decisão de 04/11/2026: imagem na flash externa com bootloader (opção 2); criado o FW-062 |
| 04/11/2026 | 0.1.0 | Criado como proposta, com o espaço de flash do STM32L412C8 verificado na base do CubeMX |
