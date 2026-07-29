#include "ring_buffer.h"

void RingBuffer_Init(RingBuffer_t *buf) {
    if (buf != NULL) {
        buf->head = 0;
        buf->tail = 0;
    }
}

STD_ReturnType RingBuffer_Put(RingBuffer_t *buf, u8 data) {
    if (buf == NULL) return E_NOK;
    u8 next = (buf->head + 1) % RING_BUFFER_SIZE;
    if (next == buf->tail) {
        return E_NOK; // Buffer Full
    }
    buf->buffer[buf->head] = data;
    buf->head = next;
    return E_OK;
}

STD_ReturnType RingBuffer_Get(RingBuffer_t *buf, u8 *data) {
    if (buf == NULL || data == NULL) return E_NOK;
    if (buf->head == buf->tail) {
        return E_NOK; // Buffer Empty
    }
    *data = buf->buffer[buf->tail];
    buf->tail = (buf->tail + 1) % RING_BUFFER_SIZE;
    return E_OK;
}