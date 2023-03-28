#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "message.gen.h"

#define FLASH_ADDR    (0x34000000)
#define CHUNK1_OFFSET (0x000000dd)
#define CHUNK2_OFFSET (0x000012ab)
#define CHUNK3_OFFSET (0x000ba199)
#define CHUNK4_OFFSET (0x008cc57b)

volatile unsigned int* const UART0DR = (unsigned int*)0x101f1000;

void print_uart0(const char* str)
{
    while(*str != '\0') {
        *UART0DR = (unsigned int)(*str);
        str++;
    }
}
 
// Function to swap two numbers
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
 
// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}
 
char* my_itoa(unsigned int value, char* buffer, unsigned int base)
{
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }
 
    // consider the absolute value of the number
    unsigned int n = value;
 
    int i = 0;
    while (n)
    {
        int r = n % base;
 
        if (r >= 10) {
            buffer[i++] = 97 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
 
        n = n / base;
    }
 
    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }
 
    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }
 
    buffer[i] = '\0'; // null terminate string
 
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

int main(void)
{
    uint32_t chunk1 = *(uint32_t*)(FLASH_ADDR + CHUNK1_OFFSET); // 0x3294adee
    uint32_t chunk2 = *(uint32_t*)(FLASH_ADDR + CHUNK2_OFFSET); // 0xaabbccdd
    uint32_t chunk3 = *(uint32_t*)(FLASH_ADDR + CHUNK3_OFFSET); // 0x13141819
    uint32_t chunk4 = *(uint32_t*)(FLASH_ADDR + CHUNK4_OFFSET); // 0xff93532d

    uint32_t secret = (((chunk1 * 3) % 112) + 3) << 24; // 0x5d000000
    secret += (((chunk2 / 8) - 153) % 72) << 16; // 0x5d120000
    secret += (((chunk3 / 49328) % 642) + 101) << 8; // 0x5d12a900
    secret += ((((chunk4 + 365392) / 3) % 432) - 35); // 0x5d12a99c

    char buffer[20] = { 0x00 };
    print_uart0(PLAIN_MESSAGE);
    print_uart0(my_itoa(secret, buffer, 16));
    print_uart0("\r\n");

    return 0;
}
