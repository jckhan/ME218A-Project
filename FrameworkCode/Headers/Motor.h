/****************************************************************************

  Header file for template Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef Motor_H
#define Motor_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h"
#include "Fan.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  MotorOff, 
	MotorOn,
}MotorState_t;

// Public Function Prototypes

bool InitMotor(uint8_t Priority);
bool PostMotor(ES_Event_t ThisEvent);
ES_Event_t RunMotor(ES_Event_t ThisEvent);
MotorState_t QueryMotor(void);

#endif 

