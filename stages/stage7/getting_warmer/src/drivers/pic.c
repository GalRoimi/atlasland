#include <stdint.h>
#include <stddef.h>

#include "pic.h"

#include "bsp.h"
#include "regutil.h"

/*
 * 32-bit registers of the Primary Interrupt Controller,
 * relative to the controller's base address:
 * See page 3-3 of DDI0181.
 *
 * Although not explicitly mentioned by DDI0181, there are gaps
 * among certain groups of registers. The gaps are filled by
 * Unused* "registers" and are treated as "should not be modified".
 */
typedef struct _ARM926EJS_PIC_REGS
{
    const uint32_t VICIRQSTATUS;      /* IRQ Status Register, read only */
    const uint32_t VICFIQSTATUS;      /* FIQ Status Register, read only */
    const uint32_t VICRAWINTR;        /* Raw Interrupt Status Register, read only */
    uint32_t VICINTSELECT;            /* Interrupt Select Register */
    uint32_t VICINTENABLE;            /* Interrupt Enable Register */
    uint32_t VICINTENCLEAR;           /* Interrupt Enable Clear Register */
    uint32_t VICSOFTINT;              /* Software Interrupt Register */
    uint32_t VICSOFTINTCLEAR;         /* Software Interrupt Clear Register */
    uint32_t VICPROTECTION;           /* Protection Enable Register */
    const uint32_t Unused1[3];        /* Unused, should not be modified*/
    uint32_t VICVECTADDR;             /* Vector Address Register */
    uint32_t VICDEFVECTADDR;          /* Default Vector Address Register */
    const uint32_t Unused2[50];       /* Unused, should not be modified */
    uint32_t VICVECTADDRn[16];        /* Vector Address Registers */
    const uint32_t Unused3[48];       /* Unused, should not be modified */
    uint32_t VICVECTCNTLn[16];        /* Vector Control Registers */
    const uint32_t Unused4[872];      /* Unused, should not be modified */
    const uint32_t VICPERIPHID[4];    /* Peripheral Identification Registers, read only */
    const uint32_t VICCELLID[4];      /* PrimeCell Identification Registers, read only */
} ARM926EJS_PIC_REGS;

#define UL0                    ( 0x00000000 )
#define ULFF                   ( 0xFFFFFFFF )
#define BM_IRQ_PART            ( 0x0000001F )
#define BM_VECT_ENABLE_BIT     ( 0x00000020 )

#define NR_VECTORS             ( 16 )

static volatile ARM926EJS_PIC_REGS* const pPicReg = (ARM926EJS_PIC_REGS*) (BSP_PIC_BASE_ADDRESS);

/*
 * A table with IRQs serviced by each VICVECTADDRn.
 * If a table's field is negative, its corresponding VICVECTADDRn presumably
 * does not serve any IRQ. In this case, the corresponding VICVECTCNTLn is
 * supposed to be set to 0 and its VICVECTADDRn should be set to __irq_dummyISR.
 */
typedef struct _isrVectRecord
{
    int8_t irq;                   /* IRQ handled by this record */
    pVectoredIsrPrototype isr;    /* address of the ISR */
    int8_t priority;              /* priority of this IRQ */
} isrVectRecord;

static isrVectRecord __irqVect[NR_VECTORS];

void irq_enableIrqMode(void)
{
    /*
     * To enable IRQ mode, bit 7 of the Program Status Register (CSPR)
     * must be cleared to 0. See pp. 2-15 to 2-17 of the DDI0222 for more details.
     * The CSPR can only be accessed using assembler.
     */
    __asm volatile("MRS r0, cpsr");        /* Read in the CPSR register. */
    __asm volatile("BIC r0, r0, #0x80");   /* Clear bit 8, (0x80) -- Causes IRQs to be enabled. */
    __asm volatile("MSR cpsr_c, r0");      /* Write it back to the CPSR register */
}

void irq_disableIrqMode(void)
{
    /*
     * To disable IRQ mode, bit 7 of the Program Status Register (CSPR)
     * must be set t1 0. See pp. 2-15 to 2-17 of the DDI0222 for more details.
     * The CSPR can only be accessed using assembler.
     */
    __asm volatile("MRS r0, cpsr");       /* Read in the CPSR register. */
    __asm volatile("ORR r0, r0, #0xC0");  /* Disable IRQ and FIQ exceptions. */
    __asm volatile("MSR cpsr_c, r0");     /* Write it back to the CPSR register. */
}

/*
 * A dummy ISR routine for servicing vectored IRQs.
 *
 * It is supposed to be set as a default address of all vectored IRQ requests. If an "unconfigured"
 * IRQ is triggered, it is still better to be serviced by this dummy function instead of
 * being directed to an arbitrary address with possibly dangerous effects.
 */
static void __irq_dummyISR(void)
{
}

/*
 * Default handler of vectored IRQs. Typically the address of this function should be
 * set as a default value to pPicReg->VICDEFVECTADDR. It handles IRQs whose ISRs are note
 * entered into vectored registers. It is very similar to non vectored handling of IRQs.
 */
static void __defaultVectorIsr(void)
{
}

/*
 * IRQ handler routine, called directly from the IRQ vector, implemented in exception.c
 * Prototype of this function is not public and should not be exposed in a .h file. Instead,
 * its prototype must be declared as 'extern' where required (typically in exception.c only).
 *
 * NOTE:
 * There is no check that provided addresses are correct! It is up to developers
 * that valid ISR addresses are assigned before the IRQ mode is enabled!
 *
 * It supports two modes of IRQ handling, vectored and nonvectored mode. They are implemented
 * for testing purposes only, in a real world application, only one mode should be selected
 * and implemented.
 */
void __attribute__((interrupt("irq"))) irq_handler()
{
    /*
     * Vectored implementation, a.k.a. "Vectored interrupt flow sequence", described
     * on page 2-9 of DDI0181.
     */

    /*
     * Reads the Vector Address Register with the ISR address of the currently active interrupt.
     * Reading this register also indicates to the priority hardware that the interrupt
     * is being serviced.
     */
    pVectoredIsrPrototype isrAddr = (pVectoredIsrPrototype) pPicReg->VICVECTADDR;

    /* Execute the routine at the vector address */
    (*isrAddr)();

    /*
     * Writes an arbitrary value to the Vector Address Register. This indicates to the
     * priority hardware that the interrupt has been serviced.
     */
    pPicReg->VICVECTADDR = ULFF;
}

void pic_init(void)
{
    /* All interrupt request lines generate IRQ interrupts: */
    pPicReg->VICINTSELECT = UL0;

    /* Disable all interrupt request lines: */
    pPicReg->VICINTENCLEAR = ULFF;

    /* Clear all software generated interrupts: */
    pPicReg->VICSOFTINTCLEAR = ULFF;

    /* Reset the default vector address: */
    pPicReg->VICDEFVECTADDR = (uint32_t) &__defaultVectorIsr;

    /* clear all vectored ISR addresses: */
    for (uint8_t i = 0; i < NR_VECTORS; i++)
    {
        /* clear its entry in the table */
        __irqVect[i].irq = -1;                 /* no IRQ assigned */
        __irqVect[i].isr = &__irq_dummyISR;    /* dummy ISR routine */
        __irqVect[i].priority = -1;            /* lowest priority */

        /* clear its control register */
        pPicReg->VICVECTCNTLn[i] = UL0;
        /* and clear its ISR address to a dummy function */
        pPicReg->VICVECTADDRn[i] = (uint32_t) &__irqVect[i].isr;
    }
}

void pic_enableInterrupt(uint8_t irq)
{
    if (irq < NR_VECTORS)
    {
        /* See description of VICINTENABLE, page 3-7 of DDI0181: */
        HWREG_SET_SINGLE_BIT(pPicReg->VICINTENABLE, irq);

        /* Only the bit for the requested interrupt source is modified. */
    }
}

void pic_disableInterrupt(uint8_t irq)
{
    if (irq < NR_VECTORS)
    {
        /*
         * VICINTENCLEAR is a write only register and any attempt of reading it
         * will result in a crash. For that reason, operators as |=, &=, etc.
         * are not permitted and only direct setting of it (using operator =)
         * is possible. This is not a problem anyway as only 1-bits disable their
         * corresponding IRQs while 0-bits do not affect their corresponding
         * interrupt lines.
         *
         * For more details, see description of VICINTENCLEAR on page 3-7 of DDI0181.
         */
        pPicReg->VICINTENCLEAR = HWREG_SINGLE_BIT_MASK(irq);
    }
}

void pic_disableAllInterrupts(void)
{
    /*
     * See description of VICINTENCLEAR, page 3-7 of DDI0181.
     * All 32 bits of this register are set to 1.
     */
    pPicReg->VICINTENCLEAR = ULFF;
}

int8_t pic_isInterruptEnabled(uint8_t irq)
{
    /* See description of VICINTENCLEAR, page 3-7 of DDI0181: */
    return ((irq < NR_VECTORS) && (HWREG_READ_SINGLE_BIT(pPicReg->VICINTENABLE, irq) != 0));
}

int8_t pic_getInterruptType(uint8_t irq)
{
    /*
     * See description of VICINTSELECT, page 3-7 of DDI0181.
     *
     * If the corresponding bit is set to 1, the interrupt's type is FIQ,
     * otherwise it is IRQ.
     */
    return ((irq < NR_VECTORS) && (HWREG_READ_SINGLE_BIT(pPicReg->VICINTSELECT, irq) == 0));
}

void pic_setInterruptType(uint8_t irq, int8_t toIrq)
{
    if (irq < NR_VECTORS)
    {
        /*
         * Only the corresponding bit must be modified, all other bits must remain unmodified.
         * For that reason, appropriate bitwise operators are applied.
         *
         * The interrupt's type is set via VICINTSELECT. See description
         * of the register at page 3-7 of DDI0181.
         */
        if (toIrq != 0)
        {
            /* Set the corresponding bit to 0 by bitwise and'ing bitmask's zero complement */
            HWREG_CLEAR_SINGLE_BIT(pPicReg->VICINTSELECT, irq);
        }
        else
        {
            /* Set the corresponding bit to 1 by bitwise or'ing the bitmask */
            HWREG_SET_SINGLE_BIT(pPicReg->VICINTSELECT, irq);
        }
    }
}

int8_t pic_registerIrq(uint8_t irq, pVectoredIsrPrototype addr, uint8_t priority)
{
    const uint8_t prior = priority & PIC_MAX_PRIORITY;
    int8_t irqPos = -1;
    int8_t prPos = -1;

    /* sanity checks */
    if ((irq >= NR_VECTORS) || (addr == NULL))
    {
        return -1;
    }

    /*
     * The priority table is traversed and two values are obtained:
     * - irqPos: index of the existing 'irq' or the first "empty" line
     * - prPos: index of the first entry whose priority is not larger or equal than 'prior'
     * The entry will be inserted into prPos, prior to that, all entries between 'irqPos and 'prPos'
     * will be moved one line up or down.
     */
    for (int8_t i = 0; i < NR_VECTORS; i++)
    {
        if ((irqPos < 0) && (__irqVect[i].irq < 0 || __irqVect[i].irq == irq))
        {
            irqPos = i;
        }

        if ((prPos < 0) && (__irqVect[i].priority < 0 || __irqVect[i].priority < prior))
        {
            prPos = i;
        }
    }

    /* if prPos is less than irqPos, move all intermediate entries one line down */
    if (irqPos > prPos)
    {
        for (int8_t i = irqPos; i > prPos; i--)
        {
            __irqVect[i] = __irqVect[i-1];

            if ( __irqVect[i].irq >= 0 )
            {
                pPicReg->VICVECTCNTLn[i] = __irqVect[i].irq | BM_VECT_ENABLE_BIT;
                pPicReg->VICVECTADDRn[i] = (uint32_t) __irqVect[i].isr;
            }
            else
            {
                /* if i^th line is "empty", clear the appropriate vector registers */
                pPicReg->VICVECTCNTLn[i] = UL0;
                pPicReg->VICVECTADDRn[i] = (uint32_t) &__irq_dummyISR;
            }
        }
    }

    /* if prPos is greater than irqPos, move all intermediate entries one line up... */
    if (prPos > irqPos)
    {
        /* however this does not include the entry at prPos, whose priority is less than prior!!! */
        prPos--;

        for (int8_t i = irqPos; i < prPos; i++)
        {
            __irqVect[i] = __irqVect[i+1];

            if ( __irqVect[i].irq >= 0 )
            {
                pPicReg->VICVECTCNTLn[i] = __irqVect[i].irq | BM_VECT_ENABLE_BIT;
                pPicReg->VICVECTADDRn[i] = (uint32_t) __irqVect[i].isr;
            }
            else
            {
                /* if i^th line is "empty", clear the appropriate vector registers */
                pPicReg->VICVECTCNTLn[i] = UL0;
                pPicReg->VICVECTADDRn[i] = (uint32_t) &__irq_dummyISR;
            }
        }
    }

    /* finally fill the entry at 'prPos' with the input values */
    __irqVect[prPos].irq = irq;
    __irqVect[prPos].isr = addr;
    __irqVect[prPos].priority = prior;

    pPicReg->VICVECTCNTLn[prPos] = irq | BM_VECT_ENABLE_BIT;
    pPicReg->VICVECTADDRn[prPos] = (uint32_t) addr;

    return prPos;
}

void pic_unregisterIrq(uint8_t irq)
{
    /* sanity check */
    if (irq >= NR_VECTORS)
    {
        return;
    }

    /* Find the 'irq' in the priority table: */
    uint8_t pos = 0;

    for (pos = 0; pos < NR_VECTORS; pos++)
    {
        if (__irqVect[pos].irq == irq)
        {
            break;
        }
    }

    /* Nothing to do if IRQ has not been found: */
    if (pos >= NR_VECTORS)
    {
        return;
    }

    /*
     * Shift all entries past 'pos' (including invalid ones) one line up.
     * This will override the entry at 'pos'.
     */
    for ( ; pos < NR_VECTORS - 1; pos++)
    {
        __irqVect[pos] = __irqVect[pos + 1];

        if (__irqVect[pos].irq >= 0)
        {
            pPicReg->VICVECTCNTLn[pos] = __irqVect[pos].irq | BM_VECT_ENABLE_BIT;
            pPicReg->VICVECTADDRn[pos] = (uint32_t) __irqVect[pos].isr;
        }
        else
        {
            /* if pos^th line is "empty", clear the appropriate vector registers */
            pPicReg->VICVECTCNTLn[pos] = UL0;
            pPicReg->VICVECTADDRn[pos] = (uint32_t) &__irq_dummyISR;
        }
    }

    /* And "clear" the last entry to "default" values (see also pic_init()): */
    __irqVect[NR_VECTORS - 1].irq = -1;               /* no IRQ assigned */
    __irqVect[NR_VECTORS - 1].isr = &__irq_dummyISR;  /* dummy ISR routine */
    __irqVect[NR_VECTORS - 1].priority = -1;          /* lowest priority */
}

void pic_unregisterAllIrqs(void)
{
    /* Clear all entries in the priority table */
    for (uint8_t i = 0; i < NR_VECTORS; i++)
    {
        __irqVect[i].irq = -1;
        __irqVect[i].isr = &__irq_dummyISR;
        __irqVect[i].priority = -1;

        pPicReg->VICVECTCNTLn[i] = UL0;
        pPicReg->VICVECTADDRn[i] = (uint32_t) &__irqVect[i].isr;
    }
}
