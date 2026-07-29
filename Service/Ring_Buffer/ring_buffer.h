#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "../STD_Types.h"

#define RING_BUFFER_SIZE 32

typedef struct {
    u8 buffer[RING_BUFFER_SIZE];
    u8 head;
    u8 tail;
} RingBuffer_t;

void RingBuffer_Init(RingBuffer_t *buf);
STD_ReturnType RingBuffer_Put(RingBuffer_t *buf, u8 data);
STD_ReturnType RingBuffer_Get(RingBuffer_t *buf, u8 *data);

#endif /* RING_BUFFER_H_ */