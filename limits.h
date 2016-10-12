/*
 * limits.h
 *
 *  Created on: Apr 24, 2016
 *      Author: amart
 */

#ifndef LIMITS_H_
#define LIMITS_H_

#define LIMIT_X		GPIO_PIN_0
#define LIMIT_Y		GPIO_PIN_1

void limits_init(void);
void int_limits(void);

#endif /* LIMITS_H_ */
