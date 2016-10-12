/*
 * serial.h
 *
 *  Created on: Apr 14, 2016
 *      Author: amart
 */

#ifndef SERIAL_H_
#define SERIAL_H_
#include <stdint.h>


void uart_init(void);
void Serial_Buffer_init(void);
unsigned char serial_read(void);
void serial_write(uint8_t data);
uint8_t serial_gets(unsigned char *Buffer, uint8_t BufferSize);

#endif /* SERIAL_H_ */
