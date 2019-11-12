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
#include "Blower.h"
#include "Game.h"

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

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static BlowerState_t CurrentState;
static uint8_t LastBlowerState;

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
bool InitBlower(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
	
	BlowerInitialize();
	
  // put us into the Initial PseudoState
  CurrentState = BlowerStandby;
	
	LastBlowerState = GetCurrentState();
	
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
bool PostBlower(ES_Event_t ThisEvent)
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
ES_Event_t RunBlower(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
		case BlowerStandby:
		{
			if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
      {
        CurrentState = BlowerStandby;
				break;
      }
			else if (ThisEvent.EventType == PP_COMPLETED) {
				printf("PP_COMPLETED in BlowerStandby\n\r");
				
				CurrentState = NotBlowing;
			}
			else if (ThisEvent.EventType == SPINNER_START) {
				GameState_t GameState = QueryGame();
				if (GameState == PingPong_Completed) {
					printf("SPINNER_START in BlowerStandby\n\r");
					CurrentState = NotBlowing;
				}				
			}
			break;
		}
		
		case NotBlowing:
		{
			if (ThisEvent.EventType == SPINNER_STOP) {
				printf("SPINNER_STOP in NotBlowing\n\r");
				
				CurrentState = BlowerStandby;
			}
			else if (ThisEvent.EventType == BLOWING_START) {
				printf("BLOWER_START in NotBlowing\n\r");
				
				// Init 8s timer
				printf("Starting timer (8s)...\n\r");
				
				// Init 2s timer
				printf("Starting timer (2s)...\n\r");
				
				CurrentState = Blowing;
			}
			break;
		}
		case Blowing:
		{
			if (ThisEvent.EventType == ES_TIMEOUT) {
				printf("ES_TIMEOUT in Blowing\n\r");
				if (ThisEvent.EventParam == 1) {
					
					// Increment LEDs
					printf("Incrementing LEDs...\n\r");
					
					// Init 2s timer
					printf("Starting timer (2s)...\n\r");
					
					CurrentState = Blowing;
				} else if (ThisEvent.EventParam == 2) {
					
					ES_Event_t Event2Post;
					Event2Post.EventType = GAME_COMPLETED;
					ES_PostAll(Event2Post);
					
					CurrentState = BlowerStandby;
				}
			}
			else if (ThisEvent.EventType == BLOWING_STOP) {
				printf("BLOWING_STOP in Blowing\n\r");
				
				// Turn off all LEDs
				printf("Turning off all LEDs...\n\r");
				
				CurrentState = NotBlowing;
			}
			else if (ThisEvent.EventType == SPINNER_STOP) {
				printf("SPINNER_STOP in Blowing\n\r");
				
				CurrentState = BlowerStandby;
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
BlowerState_t QueryBlower(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void BlowerInitialize( void) {
	// Initialize the input line for the microphone
}

static uint8_t GetCurrentState( void) {
  uint8_t InputState = 0;
	/*
  InputState  = HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS));
  if (InputState & BIT7HI) {
    return 1;
  } else {
    return 0;
  }
	*/
	return InputState;
}

bool CheckBlowerEvents( void) {
	
	bool ReturnVal = false;

	uint8_t CurrentBlowerState = GetCurrentState();
	
	if (CurrentBlowerState != LastBlowerState) {
      if (CurrentBlowerState) {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = BLOWING_START;
        PostBlower(ThisEvent);
      }
      else {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = BLOWING_STOP;
        PostBlower(ThisEvent);
      }

      ReturnVal = true;
  }
	
  LastBlowerState = CurrentBlowerState;
  return ReturnVal;
	
}





		
