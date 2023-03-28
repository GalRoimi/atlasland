#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

/**
 * Initializes a UART controller.
 * It is enabled for transmission (Tx) only, receive must be enabled separately.
 * By default all IRQ sources are disabled (masked out).
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 */
void uart_init(uint8_t nr);

/**
 * Outputs a character to the specified UART.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 * @param ch - character to be sent to the UART
 */
void uart_printChar(uint8_t nr, char ch);

/**
 * Outputs a string to the specified UART.
 *
 * "<NULL>" is transmitted if 'str' is equal to NULL.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 * @param str - string to be sent to the UART, must be '\0' terminated.
 */
void uart_print(uint8_t nr, const char* str);

/**
 * Enables the specified UART controller.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 */
void uart_enableUart(uint8_t nr);

/**
 * Disables the specified UART controller.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 */
void uart_disableUart(uint8_t nr);

/**
 * Enables specified UART's transmit (Tx) section.
 * UART's general enable status (UARTEN) remains unmodified.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 */
void uart_enableTx(uint8_t nr);

/**
 * Disables specified UART's transmit (Tx) section.
 * UART's general enable status (UARTEN) remains unmodified.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 */
void uart_disableTx(uint8_t nr);

/**
 * Enables specified UART's transmit (Rx) section.
 * UART's general enable status (UARTEN) remains unmodified.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 */
void uart_enableRx(uint8_t nr);

/**
 * Disables specified UART's transmit (Rx) section.
 * UART's general enable status (UARTEN) remains unmodified.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 */
void uart_disableRx(uint8_t nr);

/**
 * Enables the interrupt triggering by the specified UART when a character is received.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 */
void uart_enableRxInterrupt(uint8_t nr);

/**
 * Disables the interrupt triggering by the specified UART when a character is received.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 */
void uart_disableRxInterrupt(uint8_t nr);

/**
 * Clears receive interrupt at the specified UART.
 *
 * Nothing is done if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 */
void uart_clearRxInterrupt(uint8_t nr);

/**
 * Reads a character that was received by the specified UART.
 * The function may block until a character appears in the UART's receive buffer.
 * It is recommended that the function is called, when the caller is sure that a
 * character has actually been received, e.g. by notification via an interrupt.
 *
 * A zero is returned immediately if 'nr' is invalid (equal or greater than 3).
 *
 * @param nr - number of the UART (between 0 and 2)
 *
 * @return character received at the UART
 */
int uart_readChar(uint8_t nr, char* ch);

#endif /* _UART_H_ */
