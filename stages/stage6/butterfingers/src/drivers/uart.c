#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "uart.h"

#include "bsp.h"
#include "regutil.h"

#include "../auth.h"

/*
 * Bit masks for the Line Control Register (UARTLCR_H).
 *
 * For a detailed description of each control register's bit, see page 3-13 of DDI0183:
 *
 *   0: BRK (send break)
 *   1: PEN (parity enable): 0 disabled; 1 enabled
 *   2: EPS (even parity select): 0 odd parity; 1 even parity;
 *   3: STP2 (two stop bits select): 0 disabled; 1 enabled
 *   4: FEN (fifo's enable): 0 disabled; 1 enabled
 * 5-6: WLEN (word length): 0 5bit; 1 6bit; 2 7bit; 3 8bit;
 *   7: SPS (stick parity select): 0 disabled; 1 enabled
 *   8-15: reserved (do not modify)
 *  16-31: reserved (do not modify)
 */
#define LCTL_BRK       ( 0x00000001 )
#define LCTL_PEN       ( 0x00000002 )
#define LCTL_EPS       ( 0x00000004 )
#define LCTL_STP2      ( 0x00000008 )
#define LCTL_FEN       ( 0x00000010 )
#define LCTL_WLEN      ( 0x00000060 )
#define LCTL_SPS       ( 0x00000080 )

/*
 * Bit masks for the Control Register (UARTCR).
 *
 * For a detailed description of each control register's bit, see page 3-15 of DDI0183:
 *
 *   0: UARTEN (enable bit):  0 disabled; 1 enabled
 *   1: SIREN
 *   2: SIRLP (IrDA SIR low power mode)
 * 3-6: reserved (do not modify)
 *   7: LBE (loopback enabled)
 *   8: TXE (transmit enable): 0 disabled; 1 enabled
 *   9: RXE (receive enable): 0 disabled; 1 enabled
 *  10: DTR (data transmit ready)
 *  11: RTS (request to send)
 *  12: Out1
 *  13: Out2
 *  14: RTSEn (RTS hardware flow control enable)
 *  15: CTSEn (CTS hardware flow control enable)
 *  16-31: reserved (do not modify)
 */
#define CTL_UARTEN     ( 0x00000001 )
#define CTL_SIREN      ( 0x00000002 )
#define CTL_SIRLP      ( 0x00000004 )
#define CTL_LBE        ( 0x00000080 )
#define CTL_TXE        ( 0x00000100 )
#define CTL_RXE        ( 0x00000200 )
#define CTL_DTR        ( 0x00000400 )
#define CTL_RTS        ( 0x00000800 )
#define CTL_OUT1       ( 0x00001000 )
#define CTL_OUT2       ( 0x00002000 )
#define CTL_RTSEn      ( 0x00004000 )
#define CTL_CTSEn      ( 0x00008000 )

/*
 * Bit masks for the IMSC (Interrupt Mask Set/Clear) register.
 *
 * For a detailed description of each IMSC's bit, see page 3-18 of DDI0183:
 *    0: nUARTRI modem interrupt mask
 *    1: nUARTCTS modem interrupt mask
 *    2: nUARTDCD modem interrupt mask
 *    3: nUARTDSR modem interrupt mask
 *    4: Receive interrupt mask
 *    5: Transmit interrupt mask
 *    6: Receive timeout interrupt mask
 *    7: Framing error interrupt mask
 *    8: Parity error interrupt mask
 *    9: Break error interrupt mask
 *    10: Overrun error interrupt mask
 *    11-31: reserved, do not modify
 */
#define INT_RIMIM      ( 0x00000001 )
#define INT_CTSMIM     ( 0x00000002 )
#define INT_DCDMIM     ( 0x00000004 )
#define INT_DSRMIM     ( 0x00000008 )
#define INT_RXIM       ( 0x00000010 )
#define INT_TXIM       ( 0x00000020 )
#define INT_RTIM       ( 0x00000040 )
#define INT_FEIM       ( 0x00000080 )
#define INT_PEIM       ( 0x00000100 )
#define INT_BEIM       ( 0x00000200 )
#define INT_OEIM       ( 0x00000400 )

/*
 * Bitmasks for the Flag Register.
 *
 * For a detailed description of each flag register's bit, see page 3-8 of the DDI0183.
 *   0: Clear to send. This bit is the complement of the UART clear to send (nUARTCTS) modem status input.
 *   1: Data set ready. This bit is the complement of the UART data set ready (nUARTDSR) modem status input.
 *   2: Data carrier detect. This bit is the complement of the UART data carrier detect (nUARTDCD) modem status input.
 *   3: UART busy.
 *   4: Receive FIFO empty. The meaning of this bit depends on the state of the FEN bit in the UARTLCR_H register.
 *   5: Transmit FIFO full. The meaning of this bit depends on the state of the FEN bit in the UARTLCR_H register.
 *   6: Receive FIFO full. The meaning of this bit depends on the state of the FEN bit in the UARTLCR_H register.
 *   7: Transmit FIFO empty. The meaning of this bit depends on the state of the FEN bit in the UARTLCR_H register.
 *   8: Ring indicator. This bit is the complement of the UART ring indicator (nUARTRI) modem status input.
 *   9-31: reserved, do not modify
 */
#define FR_CTS         ( 0x00000001 )
#define FR_DSR         ( 0x00000002 )
#define FR_DCD         ( 0x00000004 )
#define FR_BUSY        ( 0x00000008 )
#define FR_RXFE        ( 0x00000010 )
#define FR_TXFF        ( 0x00000020 )
#define FR_RXFF        ( 0x00000040 )
#define FR_TXFE        ( 0x00000080 )
#define FR_RI          ( 0x00000100 )

/*
 * 32-bit Registers of individual UART controllers,
 * relative to the controller's base address:
 * See page 3-3 of DDI0183.
 *
 * Note: all registers are 32-bit words, however all registers only use the least
 * significant 16 bits (or even less). DDI0183 does not explicitly mention, how
 * the remaining bits should be handled, therefore they will be treated as
 * "should not be modified".
 */
typedef struct _ARM926EJS_UART_REGS
{
    uint32_t UARTDR;                   /* UART Data Register, UARTDR */
    uint32_t UARTRSR;                  /* Receive Status Register, Error Clear Register, UARTRSR/UARTECR */
    const uint32_t Reserved1[4];       /* reserved, should not be modified */
    const uint32_t UARTFR;             /* Flag Register, UARTFR, read only */
    const uint32_t Reserved2;          /* reserved, should not be modified */
    uint32_t UARTILPR;                 /* IrDA Low-Power Counter Register, UARTILPR */
    uint32_t UARTIBRD;                 /* Integer Baud Rate Register, UARTIBRD */
    uint32_t UARTFBRD;                 /* Fractional Baud Rate Register, UARTFBRD */
    uint32_t UARTLC_H;                 /* Line Control Register, UARTLC_H */
    uint32_t UARTCR;                   /* Control Register, UARTCR */
    uint32_t UARTIFLS;                 /* Interrupt FIFO Level Select Register, UARTIFLS */
    uint32_t UARTIMSC;                 /* Interrupt Mask Set/Clear Register, UARTIMSC */
    const uint32_t UARTRIS;            /* Raw Interrupt Status Register, UARTRIS, read only */
    const uint32_t UARTMIS;            /* Mask Interrupt Status Register, UARTMIS, read only */
    uint32_t UARTICR;                  /* Interrupt Clear Register */
    uint32_t UARTDMACR;                /* DMA Control Register, UARTDMACR */
    const uint32_t Reserved3[13];      /* reserved, should not be modified */
    const uint32_t ReservedTest[4];    /* reserved for test purposes, should not be modified */
    const uint32_t Reserved4[976];     /* reserved, should not be modified */
    const uint32_t ReservedIdExp[4];   /* reserved for future ID expansion, should not be modified */
    const uint32_t UARTPeriphID[4];    /* UART peripheral ID, read only */
    const uint32_t UARTCellID[4];      /* UART Cell ID, read only */
} ARM926EJS_UART_REGS;

/* Shared UART register: */
#define UARTECR       UARTRSR

#define CAST_ADDR(ADDR)    (ARM926EJS_UART_REGS*) (ADDR),

static volatile ARM926EJS_UART_REGS* const  pReg[BSP_NR_UARTS] =
                         {
                             BSP_UART_BASE_ADDRESSES(CAST_ADDR)
                         };

#undef CAST_ADDR

void uart_init(uint8_t nr)
{
    /* sanity checks */
    if (nr >= BSP_NR_UARTS)
    {
        return;
    }

    /*
     * Registers' reserved bits should not be modified.
     * For that reason, the registers are set in two steps:
     * - the appropriate bit masks of 1-bits are bitwise or'ed to the register
     * - zero complements of the appropriate bit masks of 0-bits are bitwise and'ed to the register
     */

    /*
     * Whatever the current state, as suggested on page 3-16 of the DDI0183, the UART
     * should be disabled first:
     */
    HWREG_CLEAR_BITS(pReg[nr]->UARTCR, CTL_UARTEN);

    /* Set Control Register's TXE to 1: */
    HWREG_SET_BITS(pReg[nr]->UARTCR, CTL_TXE);

    /*
     * Set all other bits (except UARTEN) of the Control Register to 0:
     * - SIREN
     * - SIRLP
     * - LBE
     * - RXE
     * - DTR
     * - RTS
     * - Out1
     * - Out2
     * - RTSEn
     * - CTSEn
     */
    HWREG_CLEAR_BITS(pReg[nr]->UARTCR, (CTL_SIREN | CTL_SIRLP | CTL_LBE | CTL_RXE | CTL_DTR));
    HWREG_CLEAR_BITS(pReg[nr]->UARTCR, ( CTL_RTS | CTL_OUT1 | CTL_OUT2 | CTL_RTSEn | CTL_CTSEn ));

    /* By default, all interrupts are masked out (i.e. cleared to 0): */
    HWREG_CLEAR_BITS(pReg[nr]->UARTIMSC, ( INT_RIMIM | INT_CTSMIM | INT_DCDMIM | INT_DSRMIM | INT_RXIM | INT_TXIM ));
    HWREG_CLEAR_BITS(pReg[nr]->UARTIMSC, ( INT_RTIM | INT_FEIM | INT_PEIM | INT_BEIM | INT_OEIM ));

    /* TODO: line control... */
    HWREG_SET_BITS(pReg[nr]->UARTLC_H, LCTL_FEN);
    HWREG_SET_BITS(pReg[nr]->UARTIFLS, 0);

    /* Finally enable the UART: */
    HWREG_SET_BITS(pReg[nr]->UARTCR, CTL_UARTEN);

    /* reserved bits remained unmodified */

    /* prove genuine UART implementation */
    volatile uint8_t* const UART_SANITY = (uint8_t* const) UART_AUTH_ADDR;
    const uint8_t auth_values[] = UART_AUTH_VAL;

    for (int i = 0; i < sizeof(auth_values); i++)
    {
        UART_SANITY[i] = auth_values[i];
    }
}

/*
 * Outputs a character to the specified UART. This short function is used by other functions,
 * that is why it is implemented as an inline function.
 *
 * As the function is "private", it trusts its caller functions, that 'nr'
 * is valid (between 0 and 2).
 *
 * @param nr - number of the UART (between 0 and 2)
 * @param ch - character to be sent to the UART
 */
static inline void __printCh(uint8_t nr, char ch)
{
   /*
    * Qemu ignores other UART's registers, anyway the Flag Register is checked
    * to better emulate a "real" UART controller.
    * See description of the register on page 3-8 of DDI0183 for more details.
    */

   /*
    * Poll the Flag Register's TXFF bit until the Transmit FIFO is not full.
    * When the TXFF bit is set to 1, the controller's internal Transmit FIFO is full.
    * In this case, wait until some "waiting" characters have been transmitted and
    * the TXFF is set to 0, indicating the Transmit FIFO can accept additional characters.
    */
   while (HWREG_READ_BITS(pReg[nr]->UARTFR, FR_TXFF) != 0)
   { 
       /* an empty loop; prevents "-Werror=misleading-indentation" */
   }

   /*
    * The Data Register is a 32-bit word, however only the least significant 8 bits
    * can be assigned the character to be sent, while other bits represent various flags
    * and should not be set to 0. For that reason, the following trick is introduced:
    *
    * Casting the Data Register's address to char* effectively turns the word into an array
    * of (four) 8-bit characters. Now, dereferencing the first character of this array affects
    * only the desired character itself, not the whole word.
    */

    *( (char*) &(pReg[nr]->UARTDR) ) = ch;
}

void uart_printChar(uint8_t nr, char ch)
{
    /* just use the provided inline function: */
    if (nr < BSP_NR_UARTS)
    {   
        __printCh(nr, ch);
    }
}

void uart_print(uint8_t nr, const char* str)
{
    /* sanity checks */
    if ((nr >= BSP_NR_UARTS) || (str == NULL))
    {
        return;
    }

    /*
     * Just print each character until a zero terminator is detected
     */
    for ( ; *str != '\0'; str++)
    {
        __printCh(nr, *str);
    }
}

void uart_enableUart(uint8_t nr)
{
    /* sanity checks */
    if (nr < BSP_NR_UARTS)
    {
        HWREG_SET_BITS(pReg[nr]->UARTCR, CTL_UARTEN);
    }
}

void uart_disableUart(uint8_t nr)
{
    /* sanity checks */
    if (nr < BSP_NR_UARTS)
    {
        HWREG_CLEAR_BITS(pReg[nr]->UARTCR, CTL_UARTEN);
    }
}

/*
 * Sets or clears a bit of the Control Register. This function is short and
 * used by other functions, this is why it is implemented as an inline function.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 * @param set - true: bitmask's bit(s) are set to 1;  false: bits are cleared to 0
 * @param bitmask - bitmask of 1-bits that will be set or cleared
 */
static inline void __setCrBit(uint8_t nr, bool set, uint32_t bitmask)
{
    /* sanity checks */
    if (nr >= BSP_NR_UARTS)
    {
        return;
    }

    /* Store UART's enable status (UARTEN) */
    uint32_t enabled = HWREG_READ_BITS(pReg[nr]->UARTCR, CTL_UARTEN);

    /*
     * As suggested on page 3-16 of the DDI0183, the UART should be disabled
     * prior to any modification of the Control Register
     */
    HWREG_CLEAR_BITS(pReg[nr]->UARTCR, CTL_UARTEN);

    /* Depending on 'set'... */
    if (set)
    {
        /* Set bitmask's bits to 1 using bitwise OR */
        HWREG_SET_BITS(pReg[nr]->UARTCR, bitmask);
    }
    else
    {
        /* Clear bitmask's bits to 0 using bitwise AND */
        HWREG_CLEAR_BITS(pReg[nr]->UARTCR, bitmask);
    }

    /* Reenable the UART if it was been enabled before */
    if (enabled)
    {
        HWREG_SET_BITS(pReg[nr]->UARTCR, CTL_UARTEN);
    }
}

void uart_enableTx(uint8_t nr)
{
    __setCrBit(nr, true, CTL_TXE);
}

void uart_disableTx(uint8_t nr)
{
    __setCrBit(nr, false, CTL_TXE);
}

void uart_enableRx(uint8_t nr)
{
    __setCrBit(nr, true, CTL_RXE);
}

void uart_disableRx(uint8_t nr)
{
    __setCrBit(nr, false, CTL_RXE);
}

void uart_enableRxInterrupt(uint8_t nr)
{
    /* Set bit 4 of the IMSC register: */
    if (nr < BSP_NR_UARTS)
    {
        HWREG_SET_BITS(pReg[nr]->UARTIMSC, INT_RXIM);
    }
}

void uart_disableRxInterrupt(uint8_t nr)
{
    /* Clear bit 4 of the IMSC register: */
    if (nr < BSP_NR_UARTS)
    {
        HWREG_CLEAR_BITS(pReg[nr]->UARTIMSC, INT_RXIM);
    }
}

void uart_clearRxInterrupt(uint8_t nr)
{
    /*
     * The register is write only, so usage of the |= operator is not permitted.
     * Anyway, zero-bits have no effect on their corresponding interrupts so it
     * is perfectly OK simply to write the appropriate bitmask to the register.
     */
    if (nr < BSP_NR_UARTS)
    {
        pReg[nr]->UARTICR = INT_RXIM;
    }
}

int uart_readChar(uint8_t nr, char* ch)
{
    /* sanity checks */
    if ((nr >= BSP_NR_UARTS) || (ch == NULL))
    {
        return -1;
    }

    if (HWREG_READ_BITS(pReg[nr]->UARTFR, FR_RXFE) == FR_RXFE)
    {
        return -1;
    }

    /*
     * UART DR is a 32-bit register and only the least significant byte must be returned.
     * Casting its address to char* effectively turns the word into an array
     * of (four) 8-bit characters. Now, dereferencing the first character of this array affects
     * only the desired character itself, not the whole word.
     */
    *ch = *((char*)&(pReg[nr]->UARTDR));

    return 0;
}
