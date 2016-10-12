#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "stepper.h"
#include "planner.h"
#include "config.h"
#include "system.h"
#include "dastlib.h"
#include "protocol.h"

static int32_t position[N_AXIS];
static Linear_Motion new;

void Queue_Linear_Motion(float *target, float feed_rate, uint8_t inverse_feedrate){
	//STEP 1: Initialize new motion structure
	memset(&new, 0, sizeof(Linear_Motion));

	new.direction_bits = 0;
	new.total_steps = 0;
	int32_t target_steps[N_AXIS];

	//STEP 2: Compute steps requiered to reach the desired position, compute max steps from the axis
	target_steps[X] = lround(target[X] * STEPS_PER_MM_X);
	target_steps[Y] = lround(target[Y] * STEPS_PER_MM_Y);
	target_steps[Z] = lround(target[Z] * STEPS_PER_MM_Z);

	if(target_steps[X] < position[X]){ new.direction_bits |= X_STEPS; } //set direction bits, bits mapped in config.h
	if(target_steps[Y] < position[Y]){ new.direction_bits |= Y_STEPS; }
	if(target_steps[Z] < position[Z]){ new.direction_bits |= Z_STEPS; }

	new.steps[X] = labs(target_steps[X]-position[X]);						//absolute amount of steps in each axis
	new.steps[Y] = labs(target_steps[Y]-position[Y]);
	new.steps[Z] = labs(target_steps[Z]-position[Z]);
	new.total_steps = MAX(new.steps[X], MAX(new.steps[Y], new.steps[Z]));	//total steps in this motion


	//STEP 3: Compute desired feedrate for this motion, steps/second
	uint32_t feed_steps;

	if(feed_rate <= 0){
		feed_rate = MAX_FEEDRATE;
		feed_steps = (feed_rate * STEPS_PER_MM) / 60;
		new.min_delay = SysCtlClockGet()/feed_steps;
	}else{
		feed_steps = (feed_rate * STEPS_PER_MM) / 60;
		new.min_delay = SysCtlClockGet()/feed_steps;
	}

	position[X] = target_steps[X];			//set the position of the planner equal to the target steps, done for arc segmentation positioning
	position[Y] = target_steps[Y];
	position[Z] = target_steps[Z];

	stepper_prepare(new);					//TODO: begin state machine????
}
