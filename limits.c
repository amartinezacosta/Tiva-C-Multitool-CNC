#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "limits.h"
#include "motion.h"
#include "utils.h"

void limits_init(void){
	//init limits gpio
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIODirModeSet(GPIO_PORTB_BASE, LIMIT_X | LIMIT_Y, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTB_BASE, LIMIT_X | LIMIT_Y, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
	GPIOIntTypeSet(GPIO_PORTB_BASE,LIMIT_X | LIMIT_Y,GPIO_FALLING_EDGE);

	//IntMasterEnable();
	IntEnable(INT_GPIOB);
	GPIOIntEnable(GPIO_PORTB_BASE, LIMIT_X | LIMIT_Y);

}

void Limits_ISR(void){
}


