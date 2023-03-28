#ifndef _ITOA_H_
#define _ITOA_H_

#include <stdint.h>

/*
 * Convert a uint32_t value to a string representation in the given base.
 *
 * @param value   The value to convert.
 * @param buffer  A buffer to store the string representation.
 * @param base    The base to use for the conversion (e.g., 10 for decimal).
 *
 * @return        A pointer to the string representation.
 */
char* my_itoa(uint32_t value, char* buffer, uint32_t base);

#endif /* _ITOA_H_ */
