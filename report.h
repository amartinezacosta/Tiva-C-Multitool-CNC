/*
 * report.h
 *
 *  Created on: May 16, 2016
 *      Author: amart
 */

#ifndef REPORT_H_
#define REPORT_H_

//CNC status flags
#define STATUS_OK 						 0
#define FILE_BEGIN						 2

//Parser error flags
#define NO_COMMANDS_FOUND 				-1
#define TOO_MANY_COMMANDS 				-2
#define COMMAND_NOT_SUPPORTED 			-3
#define WRONG_INPUT 					-4
#define REPEATED_COMMAND				-5
#define NOT_SUPPORTED_COMMAND			-6
#define NEGATIVE_VALUE_ERROR			-7

//Motion error flags
#define UNDEFINED_FEED_RATE				-8
#define ERROR_AXIS_OMITTED				-9
#define ERROR_WRONG_ARC_INPUT			-10
#define ERROR_ARC_IJK 					-11
#define ERROR_INVALID_TARGET			-12
#define ERROR_ARC_TARGET				-13
#define ERROR_AXIS_EXIST				-14
#define ERROR_EXECUTION					-15
#define AXIS_COMMAND_NONE				-16

void report_status_message(int8_t status_code);
void report_position(void);

#endif /* REPORT_H_ */
