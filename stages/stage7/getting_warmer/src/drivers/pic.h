#ifndef _PIC_H_
#define _PIC_H_

#include <stdint.h>

#define PIC_MAX_PRIORITY     ( 127 )

/**
 * Required prototype for vectored ISR servicing routines
 */
typedef void (*pVectoredIsrPrototype)(void);

/**
 * Enable CPU's IRQ mode that handles IRQ interrupt requests.
 */
void irq_enableIrqMode(void);

/**
 * Disable CPU's IRQ and FIQ mode that handle IRQ interrupt requests.
 */
void irq_disableIrqMode(void);

/**
 * Initializes the primary interrupt controller to default settings.
 *
 * All interrupt request lines are set to generate IRQ interrupts and all
 * interrupt request lines are disabled by default. Additionally, all vector
 * and other registers are cleared.
 */
void pic_init(void);

/**
 * Enable the the interrupt request line on the PIC for the specified interrupt number.
 *
 * Nothing is done if 'irq' is invalid, i.e. equal or greater than 32.
 *
 * @param irq - interrupt number (must be smaller than 32)
 */
void pic_enableInterrupt(uint8_t irq);

/**
 * Disable the the interrupt request line on the PIC for the specified interrupt number.
 *
 * Nothing is done if 'irq' is invalid, i.e. equal or greater than 32.
 *
 * @param irq - interrupt number (must be smaller than 32)
 */
void pic_disableInterrupt(uint8_t irq);

/**
 * Disable all interrupt request lines of the PIC.
 */
void pic_disableAllInterrupts(void);

/**
 * Checks whether the interrupt request line for the requested interrupt is enabled.
 *
 * 0 is returned if 'irq' is invalid, i.e. equal or greater than 32.
 *
 * @param irq - interrupt number (must be smaller than 32)
 *
 * @return 0 if disabled, a nonzero value (typically 1) if the interrupt request line is enabled
 */
int8_t pic_isInterruptEnabled(uint8_t irq);

/**
 * What type (IRQ or FIQ) is the requested interrupt of?
 *
 * 0 is returned if 'irq' is invalid, i.e. equal or greater than 32.
 *
 * @param irq - interrupt number (must be smaller than 32)
 *
 * @return 0 if irq's type is FIQ, a nonzero value (typically 1) if the irq's type is IRQ
 */
int8_t pic_getInterruptType(uint8_t irq);

/**
 * Set the requested interrupt to the desired type (IRQ or FIQ).
 *
 * Nothing is done if 'irq' is invalid, i.e. equal or greater than 32.
 *
 * @param irq - interrupt number (must be smaller than 32)
 * @param toIrq - if 0, set the interrupt's type to FIQ, otherwise set it to IRQ
 */
void pic_setInterruptType(uint8_t irq, int8_t toIrq);

/**
 * Registers a vector interrupt ISR for the requested interrupt request line.
 * The vectored interrupt is enabled by default.
 *
 * Nothing is done and -1 is returned if either 'irq' is invalid (must be less than 32)
 * or ISR's address is NULL.
 *
 * Entries are internally sorted in descending order by priority.
 * Entries with the same priority are additionally sorted by the time of registration
 * (entries registered earlier are ranked higher).
 * If 'irq' has already been registered, its internal entry will be overridden with
 * new values and resorted by priority.
 * The first 16 entries, sorted by priority, are automatically entered into appropriate vector
 * registers of the primary interrupt controller.
 *
 * @note IRQ handling should be completely disabled prior to calling this function!
 *
 * @param irq - interrupt number (must be smaller than 32)
 * @param addr - address of the ISR that services the interrupt 'irq'
 * @param priority - priority of handling this IRQ (higher value means higher priority), the actual priority
 *                   will be silently truncated to 127 if this value is exceeded.
 *
 * @return position of the IRQ handling entry within an internal table, a negative value if registration was unsuccessful
 */
int8_t pic_registerIrq(uint8_t irq, pVectoredIsrPrototype addr, uint8_t priority);

/**
 * Unregisters a vector interrupt ISR for the requested interrupt request line.
 *
 * Nothing is done if 'irq' is invalid, i.e. equal or greater than 32 or
 * no vector for the 'irq' exists.
 *
 * @note IRQ handling should be completely disabled prior to calling this function!
 *
 * @param irq - interrupt number (must be smaller than 32)
 */
void pic_unregisterIrq(uint8_t irq);

/**
 * Unregisters all vector interrupts.
 */
void pic_unregisterAllIrqs(void);

#endif /* _PIC_H_ */
