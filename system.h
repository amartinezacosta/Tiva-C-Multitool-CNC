#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <stdint.h>
#include <stdbool.h>
#include "gcode.h"


//hidden between this states, there is a finite state machine FIND IT!!!!!!
/*typedef enum{
	READY,
	BUSY
}STATE;

typedef enum{
	QUEUE,
	DEQUEUE,
	MOTION
}

typedef struct {
  Modal modal;
  float spindle_speed;          // RPM
  float feed_rate;              // Millimeters/min
  uint8_t tool;                 // Tracks tool number.
  float position[N_AXIS];       // Where the interpreter considers the tool to be at this point in the code
  float rt_position[N_AXIS];	//Real time postioning of the tool
  float coord_system[N_AXIS];   // Current work coordinate system (G54+). Stores offset from absolute machine
                                // position in mm. Loaded from EEPROM when called.
  float coord_offset[N_AXIS];   // Retains the G92 coordinate offset (work coordinates) relative to
                                // machine zero in mm. Non-persistent. Cleared upon reset and boot.
  float tool_length_offset;     // Tracks tool length offset value when enabled.
  STATE state;
  EVENT event;
}system_state;


*/
#define PARSING	0
#define MOTION	1
#define BUSY	2
#define DATA	3


typedef struct {
  Modal modal;
  float spindle_speed;          // RPM
  float feed_rate;              // Millimeters/min
  uint8_t tool;                 // Tracks tool number.
  float position[N_AXIS];       // Where the interpreter considers the tool to be at this point in the code
  float rt_position[N_AXIS];	//Real time postioning of the tool
  float coord_system[N_AXIS];   // Current work coordinate system (G54+). Stores offset from absolute machine
                                // position in mm. Loaded from EEPROM when called.
  float coord_offset[N_AXIS];   // Retains the G92 coordinate offset (work coordinates) relative to
                                // machine zero in mm. Non-persistent. Cleared upon reset and boot.
  float tool_length_offset;     // Tracks tool length offset value when enabled.
  uint16_t status;
}system_state;


Modal* system_get_modal(void);
void system_update_position(int8_t step, uint8_t axis);
float* system_get_position(void);
float *system_get_position_rt(void);
void system_execute(Block *New_Block);

void system_init(void);
bool system_check_same_position(float *taget_position);
void system_set_status(uint8_t status, bool set);
bool system_check_status(uint8_t status);
uint8_t system_all(void);


#endif /* SYSTEM_H_ */
