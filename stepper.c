#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#include "config.h"
#include "utils.h"
#include "stepper.h"
#include "planner.h"
#include "system.h"
#include "debug.h"

static Linear_Motion Motion;									//TODO: Maybe just point to the next Linear_Motion?

void stepper_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);												//Steps port initialization
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, STEPS_MASK);											//Steps port initialization

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);												//Direction port initialization
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, DIRECTION_MASK);										//Direction port initialization

	GPIOPadConfigSet(GPIO_PORTF_BASE, STEPS_MASK, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD); 	//8mA drive with slew rate control, Push-pull, Both steps and direction ports
	GPIOPadConfigSet(GPIO_PORTE_BASE, DIRECTION_MASK, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);	//8mA drive with slew rate control, Push-pull, Both steps and direction ports

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);												//Timer 0 initialization, Full-width periodic timer
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	IntEnable(INT_TIMER0A);																		//Enable Interrupts for Timer 0
	//IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);															//Enbale Timer0
}

void Stepper_ISR(void){
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);							//clear interrupt
	GPIOPinWrite(GPIO_PORTE_BASE, DIRECTION_MASK, Motion.direction_bits);	//set steppers direction first
	GPIOPinWrite(GPIO_PORTF_BASE, STEPS_MASK, Motion.step_outbits);			//pulse steppers according to the bresenham line displacement algorithm
	Motion.step_outbits = 0;												//reset stepper pulse bits

	TimerLoadSet(TIMER0_BASE, TIMER_A, Motion.min_delay);					//load next timer TODO: not an ideal implementation of delays, Fix this


	 //Perform Bresenham Line algorithm displacement
	if(Motion.total_steps != Motion.step_count){
		Motion.counter[X] += Motion.steps[X];
		if (Motion.counter[X] > Motion.total_steps) {
			Motion.step_outbits |= X_STEPS;
			Motion.counter[X] -= Motion.total_steps;
			if (Motion.direction_bits & X_DIRECTION_BIT) { system_update_position(-1, X); }
			else { system_update_position(1, X); }
		}

		Motion.counter[Y] += Motion.steps[Y];
		if (Motion.counter[Y] > Motion.total_steps) {
			Motion.step_outbits |= Y_STEPS;
			Motion.counter[Y] -= Motion.total_steps;
			if (Motion.direction_bits & Y_DIRECTION_BIT) { system_update_position(-1, Y); }
			else { system_update_position(1, Y); }
		}

		Motion.counter[Z] += Motion.steps[Z];
		if (Motion.counter[Z] > Motion.total_steps) {
			Motion.step_outbits |= Z_STEPS;
			Motion.counter[Z] -= Motion.total_steps;
			if (Motion.direction_bits & Z_DIRECTION_BIT) { system_update_position(-1, Z); }
			else { system_update_position(1, Z); }
		 }

		  GPIOPinWrite(GPIO_PORTF_BASE, STEPS_MASK, 0);
		  Motion.step_count++;

	}else if(Motion.total_steps == Motion.step_count){
		system_set_status(BUSY, false);
		GPIOPinWrite(GPIO_PORTF_BASE, STEPS_MASK, 0);
		TimerDisable(TIMER0_BASE, TIMER_A);
	}

	#ifdef DEBUG
		DEBUG_RT;
	#endif
}

void stepper_prepare(Linear_Motion New_Motion){
	//system_set_status(MOTION, false); //acknowledge motion
	while(system_check_status(BUSY)); 														//wait until execution of last motion is done

	Motion = New_Motion;

	Motion.counter[X] = Motion.counter[Y] = Motion.counter[Z] = (Motion.total_steps / 2);	//initialize counters for bresenham line displacement
	Motion.step_count = 0;

	system_set_status(BUSY, true);
	//set counter load to pulse after 1 clock cycles, enable interrupts for timerA
	TimerLoadSet(TIMER0_BASE, TIMER_A, 1);
	TimerEnable(TIMER0_BASE, TIMER_A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}
