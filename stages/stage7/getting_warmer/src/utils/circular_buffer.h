#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct circular_buf_t
{
	uint8_t* buffer;
	size_t head;
	size_t tail;
	size_t max; // of the buffer
	bool full;
} circular_buf_t;

typedef circular_buf_t* cbuf_handle_t;

/**
 * Initializes a circular buffer with the specified buffer and size.
 *
 * @param handle - The handle to the circular buffer to initialize.
 * @param buffer - The buffer to use for the circular buffer.
 * @param size - The size of the buffer.
 */
void circular_buf_init(cbuf_handle_t handle, uint8_t* buffer, size_t size);

/**
 * Resets the circular buffer to its original state, with no data stored.
 *
 * @param handle - The handle to the circular buffer to reset.
 */
void circular_buf_reset(cbuf_handle_t handle);

/**
 * Adds a new element to the end of the circular buffer. If the buffer is full, 
 * the oldest data in the buffer will be overwritten.
 *
 * @param handle - The handle to the circular buffer to add the element to.
 * @param data - The element to add to the circular buffer.
 */
void circular_buf_put(cbuf_handle_t handle, uint8_t data);

/**
 * Adds a new element to the end of the circular buffer if the buffer is not full.
 * If the buffer is full, this function returns -1.
 *
 * @param handle - The handle to the circular buffer to add the element to.
 * @param data - The element to add to the circular buffer.
 * @return 0 if successful, -1 if the buffer is full.
 */
int circular_buf_try_put(cbuf_handle_t handle, uint8_t data);

/**
 * Retrieves the next element from the circular buffer. If the buffer is empty,
 * this function returns -1.
 *
 * @param handle - The handle to the circular buffer to retrieve the element from.
 * @param data - A pointer to where the retrieved element will be stored.
 * @return 0 if successful, -1 if the buffer is empty.
 */
int circular_buf_get(cbuf_handle_t handle, uint8_t* data);

/**
 * Checks if the circular buffer is currently empty.
 *
 * @param handle - The handle to the circular buffer to check.
 * @return true if the circular buffer is empty, false otherwise.
 */
bool circular_buf_empty(cbuf_handle_t handle);

/**
 * Checks if the circular buffer is currently full.
 *
 * @param handle - The handle to the circular buffer to check.
 * @return true if the circular buffer is full, false otherwise.
 */
bool circular_buf_full(cbuf_handle_t handle);

/**
 * Returns the maximum capacity of the circular buffer.
 *
 * @param handle - The handle to the circular buffer to check.
 * @return the maximum capacity of the circular buffer.
 */
size_t circular_buf_capacity(cbuf_handle_t handle);

/**
 * Returns the current number of elements stored in the circular buffer.
 *
 * @param handle - The handle to the circular buffer to check.
 * @return the number of elements stored in the circular buffer.
 */
size_t circular_buf_size(cbuf_handle_t handle);

/**
 * Returns the element at a specified index in the circular buffer without
 * removing the data. If the data is not available, this function returns -1.
 *
 * @param handle - The handle to the circular buffer to peek into.
 * @param data - A pointer to where the peeked element will be stored.
 * @param look_ahead_counter - The number of elements to look ahead from the current head position.
 * @return 0 if successful, -1 if data is not available.
 */
int circular_buf_peek(cbuf_handle_t handle, uint8_t* data, size_t look_ahead_counter);

#endif /* _CIRCULAR_BUFFER_H_ */
