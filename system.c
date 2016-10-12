#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "system.h"
#include "gcode.h"
#include "config.h"
#include "motion.h"
#include "coolant.h"
#include "spindle.h"
#include "planner.h"
#include "debug.h"


static system_state System;

Modal* system_get_modal(){
	return &System.modal;
}

float* system_get_position(){
	return System.position;
}

void system_execute(Block *New_Block){
	//ORDER OF EXECUTION
	//[1. comments] NOT SUPPORTED

	//[2. set feedrate mode]
	System.modal.feed_rate = New_Block->modal.feed_rate;

	//[3.set feedrate]
	if(WORD_EXIST(New_Block->flags.value_flags, F)){ System.feed_rate = New_Block->value.f; }

	//[4. set spindle speed]
	if(System.spindle_speed != New_Block->value.s){
		if(System.modal.spindle != STOP_SPINDLE){ spindle_run(System.modal.spindle, New_Block->value.s); }
		System.spindle_speed = New_Block->value.s;
	}

	//[5. select tool]
	if(System.modal.tool_select != New_Block->value.t){
		System.tool = New_Block->value.t;
	}

	//[6. change tool] TODO: implement later! DO NOT FORGET!!!!

	//[7. spindle on or off]							TODO: fix this!!, spindle will never start
	if(System.modal.spindle != New_Block->modal.spindle){
		spindle_run(New_Block->modal.spindle, System.spindle_speed);
		System.modal.spindle = New_Block->modal.spindle;
	}


	//[8. coolant on or off]
	if(System.modal.coolant != New_Block->modal.coolant){
		coolant_run(New_Block->modal.coolant);
		System.modal.coolant = New_Block->modal.coolant;
	}

	//[9. enable disable overrides] NOT SUPPORTED

	//[10. dwell]
	if(New_Block->non_modal == DWELL){ dwell(New_Block->value.p); }

	//[11. set active plane]
	System.modal.plane_select = New_Block->modal.plane_select;
	switch(System.modal.plane_select){
	case XY_PLANE:
		New_Block->value.axis[0] = X_AXIS;
		New_Block->value.axis[1] = Y_AXIS;
		New_Block->value.axis[2] = Z_AXIS;
		break;
	case XZ_PLANE:
		New_Block->value.axis[0] = X_AXIS;
		New_Block->value.axis[1] = Z_AXIS;
		New_Block->value.axis[2] = X_AXIS;
		break;
	default:
		New_Block->value.axis[0] = Y_AXIS;
		New_Block->value.axis[1] = Z_AXIS;
		New_Block->value.axis[2] = X_AXIS;
		break;
	}

	//[12. set lenght units]
	System.modal.units = New_Block->modal.units;

	//[13. cutter radius compensation] NOT SUPPORTED

	//[14. cutter lenght compensation] NOT SUPPORTTED

	//[15. coordinate system selection] NOT SUPPORTED

	//[16. set path control] NOT SUPPORTED

	//[17. set distance mode]
	System.modal.distance = New_Block->modal.distance;
	if(System.modal.distance){
		New_Block->value.xyz[X] += System.position[X];
		New_Block->value.xyz[Y] += System.position[Y];
		New_Block->value.xyz[Z] += System.position[Z];
	}

	//[18. set retract mode] NOT SUPPORTED

	//[19. home, change coordinate data, set axis offsets] NOT SUPPORTED

	//[20. perform motion]
	if(!WORD_EXIST(New_Block->flags.value_flags, X)){ New_Block->value.xyz[X] = System.position[X]; }	//keep last coordinate if there is no axis input
	if(!WORD_EXIST(New_Block->flags.value_flags, Y)){ New_Block->value.xyz[Y] = System.position[Y]; }
	if(!WORD_EXIST(New_Block->flags.value_flags, Z)){ New_Block->value.xyz[Z] = System.position[Z]; }

	System.modal.motion = New_Block->modal.motion;
	if(System.modal.motion != CANCEL_MOTION_MODE){
		switch(System.modal.motion){
		case RAPID_POSITIONING:
			Queue_Linear_Motion(New_Block->value.xyz, -1.0, false);
			break;
		case LINEAR_INTERPOLATION:
			Queue_Linear_Motion(New_Block->value.xyz, System.feed_rate, System.modal.feed_rate);
			break;
		case CIRCULAR_INTERPOLATION_CW:
			motion_arc(System.position, New_Block->value.xyz, New_Block->value.ijk, New_Block->value.r, System.feed_rate,
					System.modal.feed_rate, New_Block->value.axis, true);
			break;
		case CIRCULAR_INTERPOLATION_CCW:
			motion_arc(System.position, New_Block->value.xyz, New_Block->value.ijk, New_Block->value.r, System.feed_rate,
					System.modal.feed_rate, New_Block->value.axis, false);
			break;
		}

		System.position[X] = New_Block->value.xyz[X];	//update system position
		System.position[Y] = New_Block->value.xyz[Y];
		System.position[Z] = New_Block->value.xyz[Z];
	}

	//[21. stop]
	System.modal.program_flow = New_Block->modal.program_flow;
	switch(System.modal.program_flow){
	case PROGRAM_END:
		//report that the program has ended (AKA file streaming)
		break;
	case PROGRAM_RUNNING:
		//continue with normal operation
		break;
	case OPTTIONAL_PROGRAM_STOP:
		//stop program here
		break;
	}

	#ifdef DEBUG
		DEBUG_SYSTEM(&System);
	#endif
}


void system_update_position(int8_t step, uint8_t axis){
	switch(axis){
	case X:
		System.rt_position[X] += (float)(step * MM_PER_STEP);
		break;
	case Y:
		System.rt_position[Y] += (float)(step * MM_PER_STEP);
		break;
	case Z:
		System.rt_position[Z] += (float)(step * MM_PER_STEP);
		break;
	}
}

float *system_get_position_rt(void){
	return System.rt_position;
}

float *system_get_postion(void){
	return System.position;
}

uint8_t system_all(){
	return System.status;
}

bool system_check_same_position(float *target_position){
	uint8_t idx;
	for(idx = 0; idx < N_AXIS; idx++){
		if(target_position[idx] != System.position[idx]){ return false;}
	}
	return true;
}

void system_set_status(uint8_t status, bool set){
	switch(status){
	case PARSING:
		if(set){ System.status |= BIT(PARSING); }
		else{ System.status &= ~BIT(PARSING); }
		break;
	case MOTION:
		if(set){ System.status |= BIT(MOTION); }
		else{ System.status &= ~BIT(MOTION); }
		break;
	case BUSY:
		if(set){ System.status |= BIT(BUSY); }
		else{ System.status &= ~BIT(BUSY); }
		break;
	case DATA:
		if(set){ System.status |= BIT(DATA); }
		else { System.status &= ~BIT(DATA);  }
	}
}

bool system_check_status(uint8_t status){
	switch(status){
	case PARSING:
		return System.status & BIT(PARSING);
	case MOTION:
		return System.status & BIT(MOTION);
	case BUSY:
		return System.status & BIT(BUSY);
	case DATA:
		return System.status & BIT(DATA);
	}
	return false;
}


void system_init(void){
	memset(&System, 0, sizeof(System));
}

