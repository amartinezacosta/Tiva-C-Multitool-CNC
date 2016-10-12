/*
 * execution.h
 *
 *  Created on: Apr 14, 2016
 *      Author: amart
 */

#ifndef MOTION_H_
#define MOTION_H_

void motion_line(float *target, float feed_rate, uint8_t invert_feedrate);
void motion_arc(float *position, float *target, float *offset, float radius, float feed_rate,
				uint8_t invert_feedrate,uint8_t *axis, uint8_t is_clockwise);
void dwell(float p);

#endif /* MOTION_H_ */
