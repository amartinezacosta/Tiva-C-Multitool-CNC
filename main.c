#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "protocol.h"
#include "motion.h"
#include "serial.h"
#include "stepper.h"
#include "utils.h"
#include "limits.h"
#include "system.h"
#include "spindle.h"

int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	uart_init();
	stepper_init();
	limits_init();
	spindle_init();
	system_init();

	while(1){
		protocol();
	}

}
