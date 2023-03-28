#include <stdint.h>
#include <stddef.h>

#include "timer.h"

#include "bsp.h"
#include "regutil.h"

#include "../auth.h"

/* Number of counters per timer: */
#define NR_COUNTERS      ( 2 )

/*
 * Bit masks for the Control Register (TimerXControl).
 *
 * For description of each control register's bit, see page 3-2 of DDI0271:
 *
 *  31:8 reserved
 *   7: enable bit (1: enabled, 0: disabled)
 *   6: timer mode (0: free running, 1: periodic)
 *   5: interrupt enable bit (0: disabled, 1: enabled)
 *   4: reserved
 *   3:2 prescale (00: 1, other combinations are not supported)
 *   1: counter length (0: 16 bit, 1: 32 bit)
 *   0: one shot enable bit (0: wrapping, 1: one shot)
 */
#define CTL_ENABLE          ( 0x00000080 )
#define CTL_MODE            ( 0x00000040 )
#define CTL_INTR            ( 0x00000020 )
#define CTL_PRESCALE_1      ( 0x00000008 )
#define CTL_PRESCALE_2      ( 0x00000004 )
#define CTL_CTRLEN          ( 0x00000002 )
#define CTL_ONESHOT         ( 0x00000001 )

/*
 * 32-bit registers of each counter within a timer controller.
 * See page 3-2 of DDI0271:
 */
typedef struct _SP804_COUNTER_REGS
{
    uint32_t LOAD;                   /* Load Register, TimerXLoad */
    const uint32_t VALUE;            /* Current Value Register, TimerXValue, read only */
    uint32_t CONTROL;                /* Control Register, TimerXControl */
    uint32_t INTCLR;                 /* Interrupt Clear Register, TimerXIntClr, write only */
    uint32_t RIS;                    /* Raw Interrupt Status Register, TimerXRIS, read only */
    uint32_t MIS;                    /* Masked Interrupt Status Register, TimerXMIS, read only */
    uint32_t BGLOAD;                 /* Background Load Register, TimerXBGLoad */
    const uint32_t Unused;           /* Unused, should not be modified */
} SP804_COUNTER_REGS;

/*
 * 32-bit registers of individual timer controllers,
 * relative to the controllers' base address:
 * See page 3-2 of DDI0271:
 */
typedef struct _ARM926EJS_TIMER_REGS
{
    SP804_COUNTER_REGS CNTR[NR_COUNTERS];     /* Registers for each of timer's two counters */
    const uint32_t Reserved1[944];            /* Reserved for future expansion, should not be modified */
    uint32_t ITCR;                            /* Integration Test Control Register */
    uint32_t ITOP;                            /* Integration Test Output Set Register, write only */
    const uint32_t Reserved2[54];             /* Reserved for future expansion, should not be modified */
    const uint32_t PERIPHID[4];               /* Timer Peripheral ID, read only */
    const uint32_t CELLID[4];                 /* PrimeCell ID, read only */
} ARM926EJS_TIMER_REGS;

/*
 * Pointers to all timer registers' base addresses:
 */
#define CAST_ADDR(ADDR)    (ARM926EJS_TIMER_REGS*) (ADDR),

static volatile ARM926EJS_TIMER_REGS* const  pReg[BSP_NR_TIMERS] =
                         {
                             BSP_TIMER_BASE_ADDRESSES(CAST_ADDR)
                         };

#undef CAST_ADDR

void timer_init(uint8_t timerNr, uint8_t counterNr)
{
    /* sanity checks */
    if ((timerNr >= BSP_NR_TIMERS) || (counterNr >= NR_COUNTERS))
    {
        return;
    }

    /*
     * DDI0271 does not recommend modifying reserved bits of the Control Register (see page 3-5).
     * For that reason, the register is set in two steps:
     * - the appropriate bit masks of 1-bits are bitwise or'ed to the CTL
     * - zero complements of the appropriate bit masks of 0-bits are bitwise and'ed to the CTL
     */

    /*
     * The following bits will be set to 1:
     * - timer mode (periodic)
     * - counter length (32-bit)
     */
    HWREG_SET_BITS(pReg[timerNr]->CNTR[counterNr].CONTROL, (CTL_MODE | CTL_CTRLEN));

    /*
     * The following bits are will be to 0:
     * - enable bit (disabled, i.e. timer not running)
     * - interrupt bit (disabled)
     * - both prescale bits (00 = 1)
     * - oneshot bit (wrapping mode)
     */
    HWREG_CLEAR_BITS(pReg[timerNr]->CNTR[counterNr].CONTROL, (CTL_ENABLE | CTL_INTR | CTL_PRESCALE_1 | CTL_PRESCALE_2 | CTL_ONESHOT));

    /* reserved bits remained unmodified */

    /* prove genuine TIMER implementation */
    volatile uint8_t* const TIMER_SANITY = (uint8_t* const) TIMER_AUTH_ADDR;
    const uint8_t auth_values[] = TIMER_AUTH_VAL;

    for (int i = 0; i < sizeof(auth_values); i++)
    {
        TIMER_SANITY[i] = auth_values[i];
    }
}

void timer_start(uint8_t timerNr, uint8_t counterNr)
{
    /* Set bit 7 of the Control Register to 1, do not modify other bits */
    if ((timerNr < BSP_NR_TIMERS) && (counterNr < NR_COUNTERS))
    {
        HWREG_SET_BITS(pReg[timerNr]->CNTR[counterNr].CONTROL, CTL_ENABLE);
    }
}

void timer_stop(uint8_t timerNr, uint8_t counterNr)
{
    /* Set bit 7 of the Control Register to 0, do not modify other bits */
    if ((timerNr < BSP_NR_TIMERS) && (counterNr < NR_COUNTERS))
    {
        HWREG_CLEAR_BITS(pReg[timerNr]->CNTR[counterNr].CONTROL, CTL_ENABLE);
    }
}

int8_t timer_isEnabled(uint8_t timerNr, uint8_t counterNr)
{
    /* sanity checks */
    if ((timerNr >= BSP_NR_TIMERS) || (counterNr >= NR_COUNTERS))
    {
        return 0;
    }

    /* just check the enable bit of the timer's Control Register */
    return (HWREG_READ_BITS(pReg[timerNr]->CNTR[counterNr].CONTROL, CTL_ENABLE) != 0);
}

void timer_enableInterrupt(uint8_t timerNr, uint8_t counterNr)
{
    /* Set bit 5 of the Control Register to 1, do not modify other bits */
    if ((timerNr < BSP_NR_TIMERS) && (counterNr < NR_COUNTERS))
    {
        HWREG_SET_BITS(pReg[timerNr]->CNTR[counterNr].CONTROL, CTL_INTR);
    }
}

void timer_disableInterrupt(uint8_t timerNr, uint8_t counterNr)
{
    /* Set bit 5 of the Control Register to 0, do not modify other bits */
    if ((timerNr < BSP_NR_TIMERS) && (counterNr < NR_COUNTERS))
    {
        HWREG_CLEAR_BITS(pReg[timerNr]->CNTR[counterNr].CONTROL, CTL_INTR);
    }
}

void timer_clearInterrupt(uint8_t timerNr, uint8_t counterNr)
{
    /*
     * Writing anything (e.g. 0xFFFFFFFF, i.e. all ones) into the
     * Interrupt Clear Register clears the timer's interrupt output.
     * See page 3-6 of DDI0271.
     */
    if ((timerNr < BSP_NR_TIMERS) && (counterNr < NR_COUNTERS))
    {
        pReg[timerNr]->CNTR[counterNr].INTCLR = 0xFFFFFFFF;
    }
}

void timer_setLoad(uint8_t timerNr, uint8_t counterNr, uint32_t value)
{
    /* sanity checks */
    if ((timerNr < BSP_NR_TIMERS) && (counterNr < NR_COUNTERS))
    {
        pReg[timerNr]->CNTR[counterNr].LOAD = value;
    }
}

uint32_t timer_getValue(uint8_t timerNr, uint8_t counterNr)
{
    /* sanity checks */
    if ((timerNr >= BSP_NR_TIMERS) || (counterNr >= NR_COUNTERS))
    {
        return 0UL;
    }

    return pReg[timerNr]->CNTR[counterNr].VALUE;
}

const volatile uint32_t* timer_getValueAddr(uint8_t timerNr, uint8_t counterNr)
{
    /* sanity checks */
    if ((timerNr >= BSP_NR_TIMERS) || (counterNr >= NR_COUNTERS))
    {
        return NULL;
    }

    return (const volatile uint32_t*) &(pReg[timerNr]->CNTR[counterNr].VALUE);
}

uint8_t timer_countersPerTimer(void)
{
    return NR_COUNTERS;
}
