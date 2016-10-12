#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

#include "serial.h"
#include "config.h"
#include "dastlib.h"
#include "report.h"

RingBuffer SerialRingBufferRX;						//RX rinfbuffer object
static unsigned char RX_Buffer[RX_BUFFER_SIZE];		//RX Buffer

RingBuffer SerialRingBufferTX;						//TX ringbuffer object
static unsigned char TX_Buffer[TX_BUFFER_SIZE];		//TX BUffer


void uart_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
		(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8); //set FIFO level to cause an interrupt at 1/8 full FIFO, both TX and RX

	IntMasterEnable();
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	Serial_Buffer_init();
}

void Serial_Buffer_init(void){
	RingBuffer_init(&SerialRingBufferRX, RX_BUFFER_SIZE, RX_Buffer);
	RingBuffer_init(&SerialRingBufferTX, TX_BUFFER_SIZE, TX_Buffer);
}


unsigned char serial_read(){
	unsigned char c = 0;
	if(RingBuffer_Read(&SerialRingBufferRX, &c)){
		return c;
	}else { return 0; }
}


uint8_t serial_gets(unsigned char *Buffer, uint8_t BufferSize){
	uint8_t count = 0;
	unsigned char c;

	while(1){
		while((c = serial_read()) != 0){
			if((c == '\r') || (c == '\n') || (c == 0x1b)){
				Buffer[count] = 0;
				return count;
			}
			if(count < BufferSize){
				Buffer[count] = c;
				count++;
			}else{
				//Buffer overflow!! TODO: handle it here
			}
		}
	}
}

void serial_write(unsigned char data) {
	unsigned char c;

	RingBuffer_Write(&SerialRingBufferTX, data);
	RingBuffer_Read(&SerialRingBufferTX, &c);


	UARTCharPut(UART0_BASE, data);
	UARTIntEnable(UART0_BASE, UART_INT_TX);
}


void Serial_ISR(){
	uint32_t ui32Ints;
	int32_t char32;
	int8_t data;

	ui32Ints = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, ui32Ints);

	// Are we being interrupted because the TX FIFO has space available?
	if(ui32Ints & UART_INT_TX){
		// Send a byte from the buffer
		unsigned char data = 0;
		if(RingBuffer_Read(&SerialRingBufferTX, &data)){
			UARTCharPut(UART0_BASE, data);
		}else{
			UARTIntDisable(UART0_BASE, UART_INT_TX);
		}
	}

	//Are we being interrupted due to a received character?
	if(ui32Ints & (UART_INT_RX | UART_INT_RT)){
		while(UARTCharsAvail(UART0_BASE)){
			char32 = UARTCharGetNonBlocking(UART0_BASE);
			data = (unsigned char)(char32 & 0xFF);
			RingBuffer_Write(&SerialRingBufferRX, data);
		}
	}
}
