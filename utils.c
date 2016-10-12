#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"
#include "driverlib/uart.h"

#include "serial.h"
#include "motion.h"


void pause(float step_delay){
	ROM_SysCtlDelay(step_delay);
}

void ready(void){

}

void clear_terminal(void){
	UARTCharPut(UART0_BASE, 0x0C);
}


