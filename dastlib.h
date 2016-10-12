#ifndef DASTLIB_H_
#define DASTLIB_H_

#include "planner.h"
//PARSING SECTION------------------------------------------------

uint8_t Token_get(unsigned char *string, const unsigned char *delimiters, unsigned char *token);
float ustrtof(unsigned char *nptr);

//QUEUE SECTION--------------------------------------------------
typedef struct{
	uint16_t front;
	int16_t rear;
	uint16_t ItemCount;
	uint16_t Max_Items;
	Linear_Motion *Buffer;
}Queue;


void Queue_init(Queue *queue, Linear_Motion *motion_buffer, uint16_t max_items);
Linear_Motion Queue_peek(Queue *queue);
bool Queue_Empty(Queue *queue);
bool Queue_Full(Queue *queue);
uint16_t Queue_size(Queue *queue);
bool Queue_insert(Queue *queue, Linear_Motion motion);
bool Queue_extract(Queue *queue, Linear_Motion *motion);

//RINGBUFFER SECTION----------------------------------------------------------
typedef struct{
	uint16_t lenght;
	volatile uint16_t start;
	volatile uint16_t end;
	unsigned char *buffer;
}RingBuffer;

void RingBuffer_init(RingBuffer *object, uint16_t lenght, unsigned char *buffer);
bool RingBuffer_Read(RingBuffer *object, unsigned char *target);
bool RingBuffer_Write(RingBuffer *object,  unsigned char data);
bool Ringbuffer_empty(RingBuffer *object);
bool Ringbufer_full(RingBuffer *object);

#endif
