#ifndef _REGUTIL_H_
#define _REGUTIL_H_

/**
 * A definition with a LSB set, it can be shifted left
 * to represent any single bit.
 */
#define MASK_ONE           ( 0x00000001 )

/**
 * All bits of "reg", whose equivalent "mask" bits equal 1,
 * are set to 1. All other "reg" bits remain unmodified.
 */
#define HWREG_SET_BITS(reg, mask)        reg |= (mask);

/**
 * All bits of "reg", whose equivalent 'mask' bits equal 1,
 * are cleared to 0. All other "reg" bits remain unmodified.
 */
#define HWREG_CLEAR_BITS(reg, mask)      reg &= ~(mask);

/**
 * First all "reg" bits, whose equivalent "mask" bits equal 1,
 * are cleared to 0.
 * Then all "reg" bits, whose equivalent "value" and "mask" bits
 * both equal 1, are set to 1.
 * All other "reg" bits remain unmodified.
 */
#define HWREG_SET_CLEAR_BITS(reg, value, mask) \
        reg &= ~( mask );   \
        reg |= ( (value) & (mask) );

/**
 * Returns status of all "reg" bits, whose equivalent "mask" bits
 * equal 1. Statuses of all other "reg" bits are returned as 0.
 */
#define HWREG_READ_BITS(reg, mask)       ( reg & (mask) )

/**
 * Returns a mask with a "bit"'th least significant bit set.
 */
#define HWREG_SINGLE_BIT_MASK(bit)       ( MASK_ONE << bit )

/**
 * The "bit"'th least significant bit of "reg" is set to 1.
 * All other "reg" bits remain unmodified.
 */
#define HWREG_SET_SINGLE_BIT(reg, bit)   HWREG_SET_BITS(reg, MASK_ONE<<(bit))

/**
 * The "bit"'th least significant bit of "reg" is cleared to 0.
 * All other "reg" bits remain unmodified.
 */
#define HWREG_CLEAR_SINGLE_BIT(reg, bit) HWREG_CLEAR_BITS(reg, MASK_ONE<<(bit))

/**
 * Returns status of the "REG"'s "bit"'th least significant bit. Statuses
 * of all other "reg"'s bits are returned as 0.
 */
#define HWREG_READ_SINGLE_BIT(reg, bit)  HWREG_READ_BITS(reg, MASK_ONE<<(bit))

#endif /* _REGUTIL_H_ */
