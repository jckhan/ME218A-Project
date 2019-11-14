/****************************************************************************

  Header file for template Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef Servo_H
#define Servo_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  ServoStandby, 
	ServoRunning,
}ServoState_t;

// Public Function Prototypes

bool InitServo(uint8_t Priority);
bool PostServo(ES_Event_t ThisEvent);
ES_Event_t RunServo(ES_Event_t ThisEvent);
ServoState_t QueryServo(void);

#endif 

