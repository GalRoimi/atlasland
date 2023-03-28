#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "circular_buffer.h"

static inline size_t advance_headtail_value(size_t value, size_t max)
{
    if (++value == max)
    {
        value = 0;
    }

    return value;
}

static inline void advance_head_pointer(cbuf_handle_t handle)
{
    if (circular_buf_full(handle))
    {
        handle->tail = advance_headtail_value(handle->tail, handle->max);
    }

    handle->head = advance_headtail_value(handle->head, handle->max);
    handle->full = (handle->head == handle->tail);
}

void circular_buf_init(cbuf_handle_t handle, uint8_t* buffer, size_t size)
{
    handle->buffer = buffer;
    handle->max = size;

    circular_buf_reset(handle);
}

void circular_buf_reset(cbuf_handle_t handle)
{
    handle->head = 0;
    handle->tail = 0;
    handle->full = false;
}

inline size_t circular_buf_size(cbuf_handle_t handle)
{
    size_t size = handle->max;

    if (!circular_buf_full(handle))
    {
        if (handle->head >= handle->tail)
        {
            size = (handle->head - handle->tail);
        }
        else
        {
            size = (handle->max + handle->head - handle->tail);
        }
    }

    return size;
}

inline size_t circular_buf_capacity(cbuf_handle_t handle)
{
    return handle->max;
}

inline void circular_buf_put(cbuf_handle_t handle, uint8_t data)
{
    handle->buffer[handle->head] = data;

    advance_head_pointer(handle);
}

inline int circular_buf_try_put(cbuf_handle_t handle, uint8_t data)
{
    int r = -1;

    if (!circular_buf_full(handle))
    {
        handle->buffer[handle->head] = data;
        advance_head_pointer(handle);
        r = 0;
    }

    return r;
}

int circular_buf_get(cbuf_handle_t handle, uint8_t* data)
{
    int r = -1;

    if (!circular_buf_empty(handle))
    {
        *data = handle->buffer[handle->tail];
        handle->tail = advance_headtail_value(handle->tail, handle->max);
        handle->full = false;

        r = 0;
    }

    return r;
}

inline bool circular_buf_empty(cbuf_handle_t handle)
{
    return (!circular_buf_full(handle) && (handle->head == handle->tail));
}

inline bool circular_buf_full(cbuf_handle_t handle)
{
    return handle->full;
}

int circular_buf_peek(cbuf_handle_t handle, uint8_t* data, size_t look_ahead_counter)
{
    int r = -1;
    size_t pos;

    // We can't look beyond the current buffer size
    if (circular_buf_empty(handle) || look_ahead_counter > circular_buf_size(handle))
    {
        return r;
    }

    pos = handle->tail;
    for (size_t i = 0; i < look_ahead_counter; i++)
    {
        data[i] = handle->buffer[pos];
        pos = advance_headtail_value(pos, handle->max);
    }

    return 0;
}
