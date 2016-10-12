#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "print.h"
#include "system.h"
#include "report.h"


void report_status_message(int8_t status_code){
	switch(status_code){
	case STATUS_OK:
		print_string("ok");
		break;
	case FILE_BEGIN:
		print_string("ok");
		break;
	default:
		print_string("error");
		printInteger(status_code);
		break;
	}
	//print_string("\n\r");
}

void report_position(void){
	float *System_position;

	System_position = system_get_position();
	print_string("X:");
	printFloat(System_position[0], 4);
	print_string("\n\r");

	print_string("Y:");
	printFloat(System_position[1], 4);
	print_string("\n\r");

	print_string("Z:");
	printFloat(System_position[2], 4);
	print_string("\n\r");
}

