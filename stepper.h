/*
 * config.h
 *
 *  Created on: Apr 14, 2016
 *      Author: amart
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include "config.h"
#include "planner.h"

#define T1_FREQ (SysCtlClockGet())


#define ALPHA 			(.75)    			//
#define A_T_x100 		(ALPHA*T1_FREQ)    	// (ALPHA / T1_FREQ)*100
#define T1_FREQ_148 	(T1_FREQ*0.676) 	// divided by 100 and scaled by 0.676
#define A_SQ 			(ALPHA*2)         	// ALPHA*2
//#define A_x20000 		(ALPHA*20000)       // ALPHA*20000

#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3


void stepper_end(void);
void stepper_init(void);
void stepper_prepare(Linear_Motion New_Motion);

#endif /* CONFIG_H_ */
