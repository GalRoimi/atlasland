#include <stdint.h>
#include <memory.h>

#include "auth.h"
#include "resources.h"

#include "drivers/bsp.h"
#include "drivers/pic.h"
#include "drivers/uart.h"
#include "drivers/timer.h"
#include "drivers/verifier.h"

#include "utils/itoa.h"
#include "utils/crc32c.h"
#include "utils/crypto.h"
#include "utils/circular_buffer.h"

#include "message.gen.h"

#define IO_UART               ( 0 )
#define COM_UART              ( 1 )
#define TICK_TIMER            ( 0 )
#define TICK_TIMER_COUNTER    ( 0 )

#define TIMEOUT (3000)

#define HELLO_OPCODE (0xaa)
#define HMAC_SECRET "the last one..."
#define HMAC_SECRET_SIZE (strlen(HMAC_SECRET))

#define BANNER "                  ______\n\
               .-\"      \"-. \n\
              /            \\ \n\
  _          |              |          _ \n\
 ( \\         |,  .-.  .-.  ,|         / ) \n\
  > \"=._     | )(__/  \\__)( |     _.=\" < \n\
 (_/\"=._\"=._ |/     /\\     \\| _.=\"_.=\"\\_) \n\
        \"=._ (_     ^^     _)\"_.=\" \n\
            \"=\\__|IIIIII|__/=\" \n\
           _.=\"| \\IIIIII/ |\"=._ \n\
 _     _.=\"_.=\"\\          /\"=._\"=._     _ \n\
( \\_.=\"_.=\"     `--------`     \"=._\"=._/ ) \n\
 > _.=\"                            \"=._ < \n\
(_/                                    \\_)"

#define PRINT_MBEDTLS_ERR(error)\
    do { \
        char __temp_buf[32] = { 0x00 }; \
        print("# mbedtls_err: "); \
        print(my_itoa(-error, __temp_buf, 10)); \
        print("\r\n"); \
    } while (0) \

typedef enum
{
    state_ready = 0,
    state_hmac,
    state_finish
} state_t;

DEFINE_CIRCULAR_BUFFER(io, 1024);
DEFINE_TICKS_COUNTER(timer);

void init(void)
{
    irq_disableIrqMode();

    pic_init();

    /* init all counters of all available timers */
    const uint8_t ctrs = timer_countersPerTimer();

    for (uint8_t i = 0; i < BSP_NR_TIMERS; i++)
    {
        for (uint8_t j = 0; j < ctrs; j++)
        {
            timer_init(i, j);
        }
    }

    /* init all available UARTs */
    for (uint8_t i = 0; i < BSP_NR_UARTS; i++)
    {
        uart_init(i);
    }
}

static void uart_isr(void)
{
    char ch = 0;

    while (uart_readChar(COM_UART, &ch) == 0)
    {
        circular_buf_put(GET_CIRCULAR_BUFFER(io), ch);
    }
}

static void timer_isr(void)
{
    INCRESE_TICKS_COUNTER(timer);

    timer_clearInterrupt(TICK_TIMER, TICK_TIMER_COUNTER);
}

void setup_uart(void)
{
    const uint8_t uart_irqs[BSP_NR_UARTS] = BSP_UART_IRQS;
    const uint8_t irq = uart_irqs[COM_UART];

    uart_enableRx(COM_UART);
    uart_enableTx(COM_UART);

    uart_enableRxInterrupt(COM_UART);

    pic_registerIrq(irq, &uart_isr, 50);
    pic_enableInterrupt(irq);
}

void setup_timer(void)
{
    const uint8_t timer_irqs[BSP_NR_TIMERS] = BSP_TIMER_IRQS;
    const uint8_t irq = timer_irqs[TICK_TIMER];

    uint32_t compare_match = (CPU_CLOCK_HZ / TICK_RATE_HZ) * TICKS_PER_HUND;

    timer_setLoad(TICK_TIMER, TICK_TIMER_COUNTER, compare_match);
    timer_enableInterrupt(TICK_TIMER, TICK_TIMER_COUNTER);

    pic_registerIrq(irq, &timer_isr, PIC_MAX_PRIORITY);
    pic_enableInterrupt(irq);

    timer_start(TICK_TIMER, TICK_TIMER_COUNTER);
}

void print(const char* str)
{
    if (str != NULL)
    {
        uart_print(IO_UART, str);
    }
}

void print_num(uint32_t num, uint32_t base)
{
    char tmp[32] = { 0x00 };
    print(my_itoa(num, tmp, base));
}

int check_drivers_auth(void)
{
    volatile uint8_t* const PIC_SANITY = (uint8_t* const) PIC_AUTH_ADDR;
    volatile uint8_t* const UART_SANITY = (uint8_t* const) UART_AUTH_ADDR;
    volatile uint8_t* const TIMER_SANITY = (uint8_t* const) TIMER_AUTH_ADDR;

    const uint8_t pic_auth_values[] = PIC_AUTH_VAL;
    const uint8_t uart_auth_values[] = UART_AUTH_VAL;
    const uint8_t timer_auth_values[] = TIMER_AUTH_VAL;

    if (memcmp((const uint8_t*)PIC_SANITY, pic_auth_values, sizeof(pic_auth_values)) != 0)
    {
        return -1;
    }

    if (memcmp((const uint8_t*)UART_SANITY, uart_auth_values, sizeof(uart_auth_values)) != 0)
    {
        return -2;
    }

    if (memcmp((const uint8_t*)TIMER_SANITY, timer_auth_values, sizeof(timer_auth_values)) != 0)
    {
        return -3;
    }

    return 0;
}

ssize_t recv(uint8_t* buffer, size_t len, size_t timeout)
{
    /* sanity checks */
    if ((buffer == NULL) || (len == 0))
    {
        return -1;
    }

    uint32_t s_ticks = GET_TICKS_COUNTER(timer);

    size_t i = 0;
    while (i < len)
    {
        if (circular_buf_get(GET_CIRCULAR_BUFFER(io), &(buffer[i])) == 0)
        {
            i++;
            s_ticks = GET_TICKS_COUNTER(timer);
        }
        else if ((GET_TICKS_COUNTER(timer) - s_ticks) >= (timeout / TICKS_PER_HUND))
        {
            break;
        }
    }

    return i;
}

ssize_t send(const uint8_t* buffer, size_t len)
{
    if (buffer == NULL || (len == 0))
    {
        return -1;
    }

    for (int i = 0; i < len; i++)
    {
        uart_printChar(COM_UART, buffer[i]);
    }

    return len;
}

uint32_t little_to_big_endian(uint32_t value)
{
    return ((value & 0xff) << 24) | ((value & 0xff00) << 8) |
           ((value & 0xff0000) >> 8) | ((value & 0xff000000) >> 24);
}

ssize_t handle_ready_state(state_t* state, uint32_t* nonce)
{
    /* sanity checks */
    if ((state == NULL) || (nonce == NULL))
    {
        return -1;
    }

    print("> Waiting for `HELLO` opcode ... ");

    uint8_t opcode = 0;
    if (recv(&opcode, sizeof(opcode), TIMEOUT) == sizeof(opcode))
    {
        if (opcode == HELLO_OPCODE)
        {
            print("[RECEIVED]\r\n");

            int ret = rand((uint8_t*)nonce, sizeof(*nonce));
            if (ret != 0)
            {
                PRINT_MBEDTLS_ERR(ret);
                return -1;
            }

            char buff[32] = { 0x00 };
            my_itoa(*nonce, buff, 16);

            print("> Sending NONCE (0x");
            print(buff);
            print(") ");

            for (int i = strlen(buff); i < 8; i++)
            {
                print(".");
            }

            print("... [SENT]\r\n");

            uint32_t nonce_be = little_to_big_endian(*nonce);

            if (send((uint8_t*)&nonce_be, sizeof(nonce_be)) != sizeof(nonce_be))
            {
                return -1;
            }

            *state = state_hmac;
        }
        else
        {
            print("[RECEIVED INVALID OPCODE (0x");
            print_num(opcode, 16);
            print(")]\r\n");
        }
    }
    else
    {
        print("[TIMEOUT]\r\n");
    }

    return 0;
}

ssize_t handle_hmac_state(state_t* state, uint32_t* nonce)
{
    /* sanity checks */
    if ((state == NULL) || (nonce == NULL))
    {
        return -1;
    }

    print("> Waiting for HMAC256 .......... ");

    uint8_t hmac[HMAC256_SIZE] = { 0x00 };
    int ret = hmac256((const uint8_t*)HMAC_SECRET, HMAC_SECRET_SIZE, (const uint8_t*)nonce, sizeof(*nonce), hmac, sizeof(hmac));
    if (ret != 0)
    {
        PRINT_MBEDTLS_ERR(ret);
        return -1;
    }

    uint8_t received_hmac[HMAC256_SIZE] = { 0x00 };
    if (recv(received_hmac, sizeof(received_hmac), TIMEOUT) != sizeof(received_hmac))
    {
        print("[TIMEOUT]\r\n");
        return 0;
    }

    if (memcmp(hmac, received_hmac, sizeof(hmac)) == 0)
    {
        print("[RECEIVED]\r\n");
        *state = state_finish;
    }
    else
    {
        print("[RECEIVED INVALID HMAC]\r\n");
        *state = state_ready;
    }

    return 0;
}

ssize_t handle_finish_state(state_t* state)
{
    /* sanity checks */
    if (state == NULL)
    {
        return -1;
    }

    print("your secret is: \"");
    print(PLAIN_MESSAGE);
    print("\".\r\n");

    *state = state_ready;

    return 0;
}

ssize_t handle_state(state_t* state, uint32_t* nonce)
{
    /* sanity checks */
    if ((state == NULL) || (nonce == NULL))
    {
        return -1;
    }

    switch (*state)
    {
        case (state_ready):
        {
            return handle_ready_state(state, nonce);
        }
        case (state_hmac):
        {
            return handle_hmac_state(state, nonce);
        }
        case (state_finish):
        {
            return handle_finish_state(state);
        }
        default:
        {
            return -1;
        }
    }

    return -1;
}

int main(void)
{
    INIT_CIRCULAR_BUFFER(io);
    INIT_TICKS_COUNTER(timer);

    setup_uart();
    setup_timer();

    irq_enableIrqMode();

    if (check_drivers_auth() != 0)
    {
        return -1;
    }

    if (verifier_check_running_on_versatilepc() != 0)
    {
        return -1;
    }

    print(BANNER);
    print("\r\n           = pushing through =            \r\n\r\n");

    print("# HMAC key: \"");
    print(HMAC_SECRET);
    print("\"\r\n\r\n");

    state_t state = state_ready;
    uint32_t nonce = 0;

    while (1)
    {
        ssize_t ret = handle_state(&state, &nonce);
        if (ret != 0)
        {
            print("### PANIC (");
            print_num(-ret, 10);
            print(")!! ###");

            while (1) {}; /* infinity loop */
        }
    }

    return 0;
}
