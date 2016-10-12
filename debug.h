/*
 * debug.h
 *
 *  Created on: Aug 9, 2016
 *      Author: amart
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "print.h"
#include "gcode.h"

#define DEBUG_COMMAND(L, C) print_string(&L); print_string("\n\r"); printFloat(C, 4); 							print_string("\n\r");

#define DEBUG_BLOCK(B)		print_string("Non Modal: "); print_uint8((B)->non_modal); 							print_string("\n\r"); \
							print_string("Modal Motion: "); print_uint8((B)->modal.motion); 					print_string("\n\r"); \
							print_string("Modal Feed Rate: "); print_uint8((B)->modal.feed_rate); 				print_string("\n\r"); \
							print_string("Modal Units: "); print_uint8((B)->modal.units); 						print_string("\n\r"); \
							print_string("Modal Distance: "); print_uint8((B)->modal.distance); 				print_string("\n\r"); \
							print_string("Modal Plane Select: "); print_uint8((B)->modal.plane_select); 		print_string("\n\r"); \
							print_string("Modal Tool Lenght: "); print_uint8((B)->modal.tool_length); 			print_string("\n\r"); \
							print_string("Modal Coordinate Select: "); print_uint8((B)->modal.coord_select); 	print_string("\n\r"); \
							print_string("Modal Program Flow: "); print_uint8((B)->modal.program_flow); 		print_string("\n\r"); \
							print_string("Modal Coolant: "); print_uint8((B)->modal.coolant); 					print_string("\n\r"); \
							print_string("Modal Spindle: "); print_uint8((B)->modal.spindle); 					print_string("\n\r"); \
							print_string("Modal Tool Select: "); print_uint8((B)->modal.tool_select); 			print_string("\n\r"); \
							print_string("X: "); printFloat((B)->value.xyz[0], 4); 								print_string("\n\r"); \
							print_string("Y: "); printFloat((B)->value.xyz[1], 4); 								print_string("\n\r"); \
							print_string("Z: "); printFloat((B)->value.xyz[2], 4); 								print_string("\n\r"); \
							print_string("I: "); printFloat((B)->value.ijk[0], 4); 								print_string("\n\r"); \
							print_string("J: "); printFloat((B)->value.ijk[1], 4); 								print_string("\n\r"); \
							print_string("K: "); printFloat((B)->value.ijk[2], 4); 								print_string("\n\r"); \
							print_string("F: "); printFloat((B)->value.f, 4); 									print_string("\n\r"); \
							print_string("S: "); printFloat((B)->value.s, 4); 									print_string("\n\r"); \
							print_string("P: "); printFloat((B)->value.p, 4); 									print_string("\n\r"); \
												 print_string("block end");										print_string("\n\r"); \

#define DEBUG_SYSTEM(B);	print_string("System Modal Motion: "); print_uint8((B)->modal.motion); 						print_string("\n\r"); \
							print_string("System Modal Feed Rate: "); print_uint8((B)->modal.feed_rate); 				print_string("\n\r"); \
							print_string("System Modal Units: "); print_uint8((B)->modal.units); 						print_string("\n\r"); \
							print_string("System Modal Distance: "); print_uint8((B)->modal.distance); 					print_string("\n\r"); \
							print_string("System Modal Plane Select: "); print_uint8((B)->modal.plane_select); 			print_string("\n\r"); \
							print_string("System Modal Tool Lenght: "); print_uint8((B)->modal.tool_length); 			print_string("\n\r"); \
							print_string("System Modal Coordinate Select: "); print_uint8((B)->modal.coord_select); 	print_string("\n\r"); \
							print_string("System Modal Program Flow: "); print_uint8((B)->modal.program_flow); 			print_string("\n\r"); \
							print_string("System Modal Coolant: "); print_uint8((B)->modal.coolant); 					print_string("\n\r"); \
							print_string("System Modal Spindle: "); print_uint8((B)->modal.spindle); 					print_string("\n\r"); \
							print_string("System Modal Tool Select: "); print_uint8((B)->modal.tool_select); 			print_string("\n\r"); \
							print_string("System X: "); printFloat((B)->position[0], 4); 								print_string("\n\r"); \
							print_string("System Y: "); printFloat((B)->position[1], 4);  								print_string("\n\r"); \
							print_string("System Z: "); printFloat((B)->position[2], 4);  								print_string("\n\r"); \
							print_string("Spindle Speed: "); printFloat((B)->spindle_speed, 4); 						print_string("\n\r"); \
							print_string("Feed Rate: "); printFloat((B)->feed_rate, 4); 								print_string("\n\r"); \
							print_string("Tool: "); print_uint8((B)->tool); 											print_string("\n\r"); \
							print_string("System end");																	print_string("\n\r"); \


#define DEBUG_RT			float *rt = system_get_position_rt();															  \
							print_string("RT X: "); printFloat(rt[0], 4); 								print_string("\n\r"); \
							print_string("RT Y: "); printFloat(rt[1], 4);  								print_string("\n\r"); \
							print_string("RT Z: "); printFloat(rt[2], 4);								print_string("\n\r"); \
																										print_string("\n\r"); \

#define DEBUG_GCODE_SECTION			Parse_Gcode("G00 Z5.000000");															\
									Parse_Gcode("G00 X13.436991 Y72.014866");												\
									Parse_Gcode("G01 Z-0.125000 F100.0");													\
									Parse_Gcode("G02 X13.349722 Y79.108401 Z-0.125000 I95.993963 J4.728290 F400.000000");	\
									Parse_Gcode("G03 X13.455957 Y86.203775 Z-0.125000 I-367.920472 J9.057148");				\
									Parse_Gcode("G02 X18.414413 Y85.837244 Z-0.125000 I1.879700 J-8.293727");				\
									Parse_Gcode("G02 X20.687901 Y81.312804 Z-0.125000 I-1.367275 J-3.520466");				\
									Parse_Gcode("G02 X17.508552 Y71.591544 Z-0.125000 I-3.786936 J-0.625158");				\
									Parse_Gcode("G03 X15.459666 Y71.534169 Z-0.125000 I-0.733614 J-10.414333");				\
									Parse_Gcode("G02 X13.436991 Y72.014866 Z-0.125000 I-0.372106 J2.930105");				\
									Parse_Gcode("G01 X13.436991 Y72.014866 Z-0.125000");									\
									Parse_Gcode("G00 Z5.000000");															\

#endif /* DEBUG_H_ */
