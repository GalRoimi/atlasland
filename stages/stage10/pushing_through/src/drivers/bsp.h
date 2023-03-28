#ifndef _BSP_H_
#define _BSP_H_

#define TICKS_PER_SEC               (1000)
#define TICKS_PER_HUND              (100)

#define CPU_CLOCK_HZ                ( 1000000 )
#define TICK_RATE_HZ                ( 1000 )

/* Base address of the Primary Interrupt Controller (see page 4-44 of the DUI0225D): */
#define BSP_PIC_BASE_ADDRESS        ( 0x10140000 )

/* Base address of the Secondary Interrupt Controller (see page 4-44 of the DUI0225D): */
#define BSP_SIC_BASE_ADDRESS        ( 0x10003000 )

/*
 * Base addresses and IRQs of all 3 UARTs
 * (see page 4-68 of the DUI0225D):
 */
#define BSP_NR_UARTS        ( 3 )

#define BSP_UART_BASE_ADDRESSES(CAST) \
    CAST( (0x101F1000) ) \
    CAST( (0x101F2000) ) \
    CAST( (0x101F3000) )

#define BSP_UART_IRQS       { ( 12 ), ( 13 ), ( 14 ) }

/*
 * Base address and IRQs of both timer controllers
 * (see pp. 4-21  and 4-67 of DUI0225D):
 */

#define BSP_NR_TIMERS       ( 2 )

#define BSP_TIMER_BASE_ADDRESSES(CAST) \
    CAST( (0x101E2000) ) \
    CAST( (0x101E3000) )

#define BSP_TIMER_IRQS      { ( 4 ), ( 5 ) }

/*
 * Base address and IRQ of the built-in real time clock (RTC) controller
 * (see page 4-60 of the DUI0225D):
 */
#define BSP_RTC_BASE_ADDRESS        ( 0x101E8000 )

#define BSP_RTC_IRQ                 ( 10 )

/*
 * Base address and IRQ of the built-in watchdog controller
 * (see page 4-72 of the DUI0225D):
 */
#define BSP_WATCHDOG_BASE_ADDRESS   ( 0x101E1000 )

#define BSP_WATCHDOG_IRQ            ( 0 )

/*
 * IRQ, reserved for software generated interrupts.
 * See pp.4-46 to 4-48 of the DUI0225D.
 */

#define BSP_SOFTWARE_IRQ            ( 1 )

/*
 * Base address of the built-in verifier controller
 */
#define BSP_VERIFIER_BASE_ADDRESS   ( 0x101f6000 )

#endif /* _BSP_H_ */
