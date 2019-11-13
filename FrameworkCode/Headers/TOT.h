/****************************************************************************

  Header file for template Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef TOT_H
#define TOT_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
	NoTOT,
	YesTOT,
	Waiting4NextGame
}TOTState_t;

// Public Function Prototypes

bool InitTOT(uint8_t Priority);
bool PostTOT(ES_Event_t ThisEvent);
ES_Event_t RunTOT(ES_Event_t ThisEvent);
TOTState_t QueryTOT(void);

void TOTInitialize( void);
void ReleaseTOT( void);
uint8_t GetTOTState( void);
bool CheckTOTEvents( void);

#endif 

