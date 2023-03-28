#include <stdint.h>

#include "verifier.h"

#include "bsp.h"
#include "../utils/crc32c.h"
#include "../utils/crypto.h"

typedef struct _VERSATILE_PC_VERIFIER_REGS
{
    const uint32_t SEED;            /* UART Data Register, UARTDR, read only */
    const uint32_t READY;           /* Receive Status Register, Error Clear Register, UARTRSR/UARTECR */
    uint32_t HASH[8];               /* reserved, should not be modified */
} VERSATILE_PC_VERIFIER_REGS;

/* Shared VERIFIER register: */
static volatile VERSATILE_PC_VERIFIER_REGS* const pReg = (VERSATILE_PC_VERIFIER_REGS*) BSP_VERIFIER_BASE_ADDRESS;

int verifier_check_running_on_versatilepc(void)
{
    const uint32_t seed = pReg->SEED;
    uint8_t hash[SHA256_SIZE] = { 0x00 };
    
    int ret = sha256((const uint8_t*)&seed, sizeof(seed), hash, sizeof(hash));
    if (ret != 0)
    {
        return -1;
    }

    for (int i = 0; i < sizeof(pReg->HASH) / sizeof(pReg->HASH[0]); i++)
    {
        pReg->HASH[i] = *((const uint32_t*)hash + i);
    }

    const uint32_t crc = crc32c(0, (const uint8_t*)&seed, sizeof(seed));
    if (pReg->READY != crc)
    {
        return -1;
    }

    return 0;
}
