/*
 * planner.h
 *
 *  Created on: May 7, 2016
 *      Author: amart
 */

#ifndef PLANNER_H_
#define PLANNER_H_

#include "config.h"

#define RAMP_IDLE		0
#define RAMP_UP			1
#define RAMP_MAX		2
#define RAMP_DOWN		3
#define RAMP_LAST		4

#define MAX(a, b)						( ((a) > (b)) ? (a) : (b) )

typedef struct{
	volatile uint8_t direction_bits;
	volatile uint8_t step_outbits;
	volatile uint32_t steps[N_AXIS];
	volatile uint32_t counter[N_AXIS];
	volatile uint32_t total_steps;
	uint32_t new_step_delay;
	uint32_t last_accel_delay;
	uint32_t step_count;
	uint32_t rest;
	uint8_t run_state;
	uint32_t step_delay;
	uint32_t decel_start;
	uint32_t decel_val;
	uint32_t min_delay;
	int32_t accel_count;
}Linear_Motion;

void Queue_Linear_Motion(float *target, float feed_rate, uint8_t inverse_feedrate);
Linear_Motion DeQueue_Linear_Motion(void);
void Planner_init(void);

#endif /* PLANNER_H_ */
