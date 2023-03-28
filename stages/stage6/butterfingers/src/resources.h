#ifndef _RESOURCES_H_
#define _RESOURCES_H_

#include "utils/circular_buffer.h"

#define DEFINE_CIRCULAR_BUFFER(name, size)\
    uint8_t __res_cbuf_##name##_buf[size] = { 0x00 };\
    circular_buf_t __res_cbuf_##name##_cbuf = {}

#define GET_CIRCULAR_BUFFER(name) (&__res_cbuf_##name##_cbuf)
#define INIT_CIRCULAR_BUFFER(name) circular_buf_init(&__res_cbuf_##name##_cbuf, __res_cbuf_##name##_buf, sizeof(__res_cbuf_##name##_buf))

#define DEFINE_TICKS_COUNTER(name)\
    uint32_t __res_ticks_##name##_counter = 0

#define GET_TICKS_COUNTER(name) (__res_ticks_##name##_counter)
#define INIT_TICKS_COUNTER(name)\
    do {\
        __res_ticks_##name##_counter = 0;\
    } while (0)
#define INCRESE_TICKS_COUNTER(name)\
    do {\
        __res_ticks_##name##_counter++;\
    } while (0)
#define RESET_TICKS_COUNTER(name)\
    do {\
        __res_ticks_##name##_counter = 0;\
    } while (0)

#endif /* _RESOURCES_H_ */
