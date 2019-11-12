/****************************************************************************

  Header file for template Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef Spinner_H
#define Spinner_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  Waiting4TOT,
	Waiting4Pulse,
	MaybeSpinning,
	Spinning
}SpinnerState_t;

// Public Function Prototypes

bool InitSpinner(uint8_t Priority);
bool PostSpinner(ES_Event_t ThisEvent);
ES_Event_t RunSpinner(ES_Event_t ThisEvent);
SpinnerState_t QuerySpinner(void);

void SpinnerInitialize( void);
uint8_t GetSpinnerState( void);
bool CheckSpinnerEvents( void);

#endif 

