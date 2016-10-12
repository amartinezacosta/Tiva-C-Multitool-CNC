#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"


#include "debug.h"
#include "protocol.h"
#include "serial.h"
#include "gcode.h"
#include "motion.h"
#include "config.h"
#include "system.h"
#include "report.h"
#include "stepper.h"
#include "planner.h"
#include "print.h"
#include "dastlib.h"

static unsigned char Block_Buffer[MAX_LINE_SIZE];

void protocol(void){
	//Planner_init();
	//Protocol_ISR_Begin();
	memset(Block_Buffer, 0x00, sizeof(Block_Buffer));
	while(1){
		if(serial_gets(Block_Buffer, MAX_LINE_SIZE)){			//stuck in a while loop, will not return until a \r\n is recieved in the RX buffer
			if(Block_Buffer[0] == '~'){
				report_position();
			}else{
				report_status_message(Parse_Gcode(Block_Buffer));							//Parse, execute and report the recieved block
				//protocol_start_motion(); 			 				//start the motion sequence
				//print_string(Block_Buffer);
			}
		}else{
			//we are just receiving \n\r, do nothing or display a new line maybe?
			//while(1)
		}

	}
}

void protocol_start_motion(){ system_set_status(MOTION, true); }

void Protocol_ISR_Begin(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
	IntEnable(INT_TIMER1A);

	TimerEnable(TIMER1_BASE, TIMER_BOTH);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	TimerLoadSet(TIMER1_BASE, TIMER_BOTH, SysCtlClockGet()*2);	//TODO: Protocol ISR call time(seconds) = SysCtlClockGet() * seconds, define it in config.h
}

void Protocol_ISR(){
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	uint8_t status = system_all();

	if(BIT_TRUE(status, BIT(MOTION))){
		//stepper_prepare();
	}
}

