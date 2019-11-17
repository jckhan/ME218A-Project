/****************************************************************************
 Module
   TemplateFSM.c

 Revision
   1.0.1

 Description
   This is a template file for implementing flat state machines under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/15/12 11:12 jec      revisions for Gen2 framework
 11/07/11 11:26 jec      made the queue static
 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "Spinner.h"

#include "BITDEFS.H"

// the headers to access the GPIO subsystem
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"

// the headers to access the TivaWare Library
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "PWM16Tiva.h"
#include "ADMulti.h"
#include "Fan.h"

/*----------------------------- Module Defines ----------------------------*/
#define MAYBESPINNING_TIME 	100
#define SPINNING_TIME				200
#define SPINNERHI						BIT1HI
#define SPINNERLO						BIT1LO

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static SpinnerState_t CurrentState;
static uint8_t LastSpinnerState;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateFSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 18:55
****************************************************************************/
bool InitSpinner(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
	
	SpinnerInitialize();
	
  // put us into the Initial PseudoState
  CurrentState = Waiting4TOT;
	
	LastSpinnerState = GetSpinnerState();
	
  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  if (ES_PostToService(MyPriority, ThisEvent) == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/****************************************************************************
 Function
     PostTemplateFSM

 Parameters
     EF_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostSpinner(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTemplateFSM

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event_t RunSpinner(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
		case Waiting4TOT:
		{
			if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
      {
        CurrentState = Waiting4TOT;
				break;
      }
			else if (ThisEvent.EventType == START_POTATO) {
				printf("START_POTATO in Waiting4TOT\n\r");
				
				CurrentState = Waiting4Pulse;
			}
			break;
		}
		
		case Waiting4Pulse:
		{
			if (ThisEvent.EventType == PULSE_DETECTED) {
				printf("PULSE_DETECTED in Waiting4Pulse\n\r");
				
				// Init 200ms timer
				printf("Starting timer (200ms)...\n\r");
				ES_Timer_InitTimer(5, MAYBESPINNING_TIME);
				
				CurrentState = MaybeSpinning;
			}
			else if (ThisEvent.EventType == END_POTATO) {
				printf("END_POTATO in Waiting4Pulse\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = SPINNER_STOP;
				ES_PostAll(Event2Post);
				
				CurrentState = Waiting4TOT;
			}
			break;
		}
		case MaybeSpinning:
		{
			if (ThisEvent.EventType == PULSE_DETECTED) {
				printf("PULSE_DETECTED in MaybeSpinning\n\r");
				
				// Init 100ms timer
				printf("Starting timer (100ms)...\n\r");
				ES_Timer_InitTimer(5, SPINNING_TIME);
				
				ES_Event_t Event2Post;
				Event2Post.EventType = SPINNER_START;
				ES_PostAll(Event2Post);
				
				CurrentState = Spinning;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				printf("ES_TIMEOUT in MaybeSpinning\n\r");
				
				CurrentState = Waiting4Pulse;
			}
			else if (ThisEvent.EventType == END_POTATO) {
				printf("END_POTATO in MaybeSpinning\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = SPINNER_STOP;
				ES_PostAll(Event2Post);
				
				CurrentState = Waiting4TOT;
			}
			break;
		}
		case Spinning:
		{
			if (ThisEvent.EventType == PULSE_DETECTED) {
				printf("PULSE_DETECTED in Spinning\n\r");
				
				// Init the 200ms timer
				ES_Timer_InitTimer(5, SPINNING_TIME);
			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				printf("ES_TIMEOUT in Spinning\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = SPINNER_STOP;
				ES_PostAll(Event2Post);
				
				CurrentState = Waiting4Pulse;
			}
			else if (ThisEvent.EventType == END_POTATO) {
				printf("END_POTATO in Spinning\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = SPINNER_STOP;
				ES_PostAll(Event2Post);
				
				CurrentState = Waiting4TOT;
			}
			break;
		}
		default:
      ;
  }                                   // end switch on Current State
  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryTemplateSM

 Parameters
     None

 Returns
     TemplateState_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:21
****************************************************************************/
SpinnerState_t QuerySpinner(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void SpinnerInitialize( void) {
	// Initialize the input data line for the Hall Effect sensor
  HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R1;
	while ((HWREG(SYSCTL_PRGPIO) & SYSCTL_PRGPIO_R1) != SYSCTL_PRGPIO_R1) {
	}

	HWREG(GPIO_PORTB_BASE + GPIO_O_DEN) |= SPINNERHI;

	HWREG(GPIO_PORTB_BASE + GPIO_O_DIR) &= SPINNERLO;

  //printf("Spinner initialized\n\r");
}

uint8_t GetSpinnerState( void) {
	uint8_t InputState;
	
  InputState  = HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS));
  if (InputState & SPINNERHI) {
    return 1;
  } else {
    return 0;
  }
}

bool CheckSpinnerEvents( void) {
	
	bool ReturnVal = false;

	uint8_t CurrentSpinnerState = GetSpinnerState();
  //printf("Spinner state: %d\n\r", CurrentSpinnerState);
	
	if (CurrentSpinnerState != LastSpinnerState) {
      //printf("Last spinner state: %d\n\r", LastSpinnerState);
      //printf("Current spinner state: %d\n\r", CurrentSpinnerState);
			ES_Event_t ThisEvent;
			ThisEvent.EventType = PULSE_DETECTED;
			PostSpinner(ThisEvent);
      
      ReturnVal = true;
  }
	
  LastSpinnerState = CurrentSpinnerState;

  return ReturnVal;
	
}
