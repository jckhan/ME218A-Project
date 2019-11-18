/****************************************************************************

  Header file for template Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef PingPong_H
#define PingPong_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  PPStandby,
	Neutral,
	Middle,
	Top
}PingPongState_t;

// Public Function Prototypes

bool InitPingPong(uint8_t Priority);
bool PostPingPong(ES_Event_t ThisEvent);
ES_Event_t RunPingPong(ES_Event_t ThisEvent);
PingPongState_t QueryPingPong(void);

bool CheckPingPongEvents( void);


#endif 

