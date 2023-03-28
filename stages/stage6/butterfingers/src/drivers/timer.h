#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>

/**
 * Initializes the specified timer's counter controller.
 * The following parameters are set:
 * - periodic mode (when the counter reaches 0, it is wrapped to the value of the Load Register)
 * - 32-bit counter length
 * - prescale = 1
 *
 * This function does not enable interrupt triggering and does not start the counter!
 *
 * Nothing is done if either 'timerNr' or 'counterNr' is invalid.
 *
 * @param timerNr - timer number (between 0 and 1)
 * @param counterNr - counter number of the selected timer (between 0 and 1)
 */
void timer_init(uint8_t timerNr, uint8_t counterNr);

/**
 * Starts the specified timer's counter.
 *
 * Nothing is done if either 'timerNr' or 'counterNr' is invalid.
 *
 * @param timerNr - timer number (between 0 and 1)
 * @param counterNr - counter number of the selected timer (between 0 and 1)
 */
void timer_start(uint8_t timerNr, uint8_t counterNr);

/**
 * Stops the specified timer's counter.
 *
 * Nothing is done if either 'timerNr' or 'counterNr' is invalid.
 *
 * @param timerNr - timer number (between 0 and 1)
 * @param counterNr - counter number of the selected timer (between 0 and 1)
 */
void timer_stop(uint8_t timerNr, uint8_t counterNr);

/**
 * Checks whether the specified timer's counter is enabled, i.e. running.
 *
 * If it is enabled, a nonzero value, typically 1, is returned,
 * otherwise a zero value is returned.
 *
 * If either 'timerNr' or 'counterNr' is invalid, a zero is returned
 * (as an invalid timer/counter cannot be enabled).
 *
 * @param timerNr - timer number (between 0 and 1)
 * @param counterNr - counter number of the selected timer (between 0 and 1)
 *
 * @return a zero value if the timer is disabled, a nonzero if it is enabled
 */
int8_t timer_isEnabled(uint8_t timerNr, uint8_t counterNr);

/**
 * Enables the timer's interrupt triggering (when the counter reaches 0).
 *
 * Nothing is done if either 'timerNr' or 'counterNr' is invalid.
 *
 * @param timerNr - timer number (between 0 and 1)
 * @param counterNr - counter number of the selected timer (between 0 and 1)
 */
void timer_enableInterrupt(uint8_t timerNr, uint8_t counterNr);

/**
 * Disables the timer's interrupt triggering (when the counter reaches 0).
 *
 * Nothing is done if either 'timerNr' or 'counterNr' is invalid.
 *
 * @param timerNr - timer number (between 0 and 1)
 * @param counterNr - counter number of the selected timer (between 0 and 1)
 */
void timer_disableInterrupt(uint8_t timerNr, uint8_t counterNr);

/**
 * Clears the interrupt output from the specified timer.
 *
 * Nothing is done if either 'timerNr' or 'counterNr' is invalid.
 *
 * @param timerNr - timer number (between 0 and 1)
 * @param counterNr - counter number of the selected timer (between 0 and 1)
 */
void timer_clearInterrupt(uint8_t timerNr, uint8_t counterNr);

/**
 * Sets the value of the specified counter's Load Register.
 *
 * When the timer runs in periodic mode and its counter reaches 0,
 * the counter is reloaded to this value.
 *
 * For more details, see page 3-4 of DDI0271.
 *
 * Nothing is done if either 'timerNr' or 'counterNr' is invalid.
 *
 * @param timerNr - timer number (between 0 and 1)
 * @param counterNr - counter number of the selected timer (between 0 and 1)
 * @param value - value to be loaded int the Load Register
 */
void timer_setLoad(uint8_t timerNr, uint8_t counterNr, uint32_t value);

/**
 * Returns the value of the specified counter's Value Register,
 * i.e. the value of the counter at the moment of reading.
 *
 * Zero is returned if either 'timerNr' or 'counterNr' is invalid.
 *
 * @param timerNr - timer number (between 0 and 1)
 * @param counterNr - counter number of the selected timer (between 0 and 1)
 *
 * @return value of the timer's counter at the moment of reading
 */
uint32_t timer_getValue(uint8_t timerNr, uint8_t counterNr);

/**
 * Address of the specified counter's Value Register. It might be suitable
 * for applications that poll this register frequently and wish to avoid
 * the overhead due to calling timer_getValue() each time.
 *
 * NULL is returned if either 'timerNr' or 'counterNr' is invalid.
 *
 * @note Contents at this address are read only and should not be modified.
 *
 * @param timerNr - timer number (between 0 and 1)
 * @param counterNr - counter number of the selected timer (between 0 and 1)
 *
 * @return read-only address of the timer's counter (i.e. the Value Register)
 */
const volatile uint32_t* timer_getValueAddr(uint8_t timerNr, uint8_t counterNr);

/**
 * @return number of counters per timer
 */
uint8_t timer_countersPerTimer(void);

#endif /* _TIMER_H_*/
