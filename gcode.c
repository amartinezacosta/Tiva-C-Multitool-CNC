//*****************************************************************************
//
// cmdline.c - Functions to help with processing command lines.
//
// Copyright (c) 2007-2015 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.2.111 of the Tiva Utility Library.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup cmdline_api
//! @{
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "gcode.h"
#include "system.h"
#include "config.h"
#include "report.h"
#include "planner.h"
#include "debug.h"

//*****************************************************************************
//
//! Process a command line string into arguments and execute the command.
//!
//! \param pcCmdLine points to a string that contains a command line that was
//! obtained by an application by some means.
//!
//! This function will take the supplied command line string and break it up
//! into individual arguments.  The first argument is treated as a command and
//! is searched for in the command table.  If the command is found, then the
//! command function is called and all of the command line arguments are passed
//! in the normal argc, argv form.
//!
//! The command table is contained in an array named <tt>g_psCmdTable</tt>
//! containing <tt>tCmdLineEntry</tt> structures which must be provided by the
//! application.  The array must be terminated with an entry whose \b pcCmd
//! field contains a NULL pointer.
//!
//! \return Returns \b CMDLINE_BAD_CMD if the command is not found,
//! \b CMDLINE_TOO_MANY_ARGS if there are more arguments than can be parsed.
//! Otherwise it returns the code that was returned by the command function.
//
//*****************************************************************************
#include "dastlib.h"
#include "config.h"
#include "print.h"

Block New_Block;																						//TODO: maybe a pointer?
unsigned char token[MAX_TOKEN_SIZE];
//static int8_t Error_Gcode(Block *Check_Block, uint8_t value_flags, uint8_t command_flags);			TODO: Check for errors later when motion works
//static bool Same_Position(float *current, float *target);

int8_t Parse_Gcode(unsigned char *block){
	memset(token, 0, sizeof(token));									//initialize token buffer to 0
	memset(&New_Block, 0, sizeof(Block));								//Initialize Block struct variable to 0 TODO: test if it initializes to 0 automatically
	memcpy(&New_Block.modal, system_get_modal(), sizeof(Modal)); 		//copy last Modal states from the system
	uint8_t count = 0;													//count necessary to traverse the string (AKA block)

	uint16_t global_flags;												//bitwise compare of flags

	while(*block){														//traverse the token
		count = Token_get(block, GCODEDELIMITERS, token); 				//get next token (aka Gcode command)
		if(!count){ return NO_COMMANDS_FOUND; }							//if count == 0 no tokens found
		unsigned char letter = *token;					  				//Point to first letter of the token
		if(letter < 'A' || letter > 'Z'){ return NO_COMMANDS_FOUND; } 	//invalid first character, error out (I dont't think it will ever hit it but just in case)
		float value = ustrtof(token);									//Extract float from the token
		uint8_t command = trunc(value);									//truncate float for command parsing


		switch(letter){

		case 'G':

		switch(command){
		case 0:
			New_Block.modal.motion = RAPID_POSITIONING; global_flags = GROUP_1; break;
		case 1:
			New_Block.modal.motion = LINEAR_INTERPOLATION; global_flags = GROUP_1; break;
		case 2:
			New_Block.modal.motion = CIRCULAR_INTERPOLATION_CW; global_flags = GROUP_1; break;
		case 3:
			New_Block.modal.motion = CIRCULAR_INTERPOLATION_CCW; global_flags = GROUP_1; break;
		case 21:
			New_Block.modal.units = MILIMITERS; global_flags = GROUP_6; break;
		case 90:
			New_Block.modal.distance = ABSOLUTE_DISTANCE; global_flags = GROUP_3; break;
		case 91:
			New_Block.modal.distance = RELATIVE_DISTANCE; global_flags = GROUP_3; break;
		default:
			//ERROR: Command is not supported by this parser
			return COMMAND_NOT_SUPPORTED;
		}
		if(WORD_EXIST(New_Block.flags.command_flags, global_flags)){ return REPEATED_COMMAND; } //ERROR: Repeated Modal command
		New_Block.flags.command_flags |= BIT(global_flags);
		break;

		case 'M':

		switch(command){
		case 3:
			New_Block.modal.spindle = TURN_SPINDLE_CW; global_flags = GROUP_7; break;
		case 5:
			New_Block.modal.spindle = STOP_SPINDLE; global_flags = GROUP_7; break;
		case 2:
			New_Block.modal.program_flow = PROGRAM_END; global_flags = GROUP_4; break;
		}
		if(WORD_EXIST(New_Block.flags.command_flags, global_flags)){ return REPEATED_COMMAND; } //ERROR: Repeated Modal command
		New_Block.flags.command_flags |= BIT(global_flags);
		break;


		default:

		switch(letter){
		case 'X':
			global_flags = X;
			New_Block.value.xyz[0] = value;
			break;
		case 'Y':
			global_flags = Y;
			New_Block.value.xyz[1] = value;
			break;
		case 'Z':
			global_flags = Z;
			New_Block.value.xyz[2] = value;
			break;
		case 'F':
			global_flags = F;
			New_Block.value.f = value;
			break;
		case 'I':
			global_flags = I;
			New_Block.value.ijk[0] = value;
			break;
		case 'J':
			global_flags = J;
			New_Block.value.ijk[1] = value;
			break;
		case 'S':
			global_flags = S;
			New_Block.value.s = value;
			break;
		case '%': //file beginning optional just acknowledge
			return FILE_BEGIN;
		default:
				//ERROR: Letter value not supported by this parser
			return COMMAND_NOT_SUPPORTED;
		}
		if(WORD_EXIST(New_Block.flags.value_flags, global_flags)){ return REPEATED_COMMAND; } //ERROR: Repeated Value word
		if(BIT(global_flags) & (BIT(F) | BIT(P) | BIT(T) | BIT(S))){
			if(value < 0.0){ return NEGATIVE_VALUE_ERROR; }//ERROR: negative value for non negative word
		}
		New_Block.flags.value_flags |= BIT(global_flags);

		}

		block += count; //point to the next token
	}


	//#define DEBUG
	#ifdef DEBUG
		DEBUG_BLOCK(&New_Block);
	#endif

	//if(!Error_Gcode(&New_Block, value_flags, command_flags)){ return; } 	//parsing complete check block for errors, return if found any
	system_execute(&New_Block);												//Begin Execution of the Block, change the system accordingly (at this point I don't think is necessary to return anything
																			//no errors should happen after checking the block thoroughly)

	return STATUS_OK;

}


//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
