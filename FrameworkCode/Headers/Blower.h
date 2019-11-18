/****************************************************************************

  Header file for template Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef Blower_H
#define Blower_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  BlowerStandby,
	NotBlowing,
	Blowing
}BlowerState_t;

// Public Function Prototypes

bool InitBlower(uint8_t Priority);
bool PostBlower(ES_Event_t ThisEvent);
ES_Event_t RunBlower(ES_Event_t ThisEvent);
BlowerState_t QueryBlower(void);

void BlowerInitialize( void);
bool CheckBlowerEvents( void);
static uint8_t GetMicState( void);
static void LED_on(uint8_t num);

#endif 

