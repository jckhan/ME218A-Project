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
#include "PingPong.h"
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
static PingPongState_t CurrentState;
static uint8_t LastMiddleState;
static uint8_t LastTopState;

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
bool InitPingPong(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
	
	PingPongInitialize();
	
  // put us into the Initial PseudoState
  CurrentState = PPStandby;
	
	LastMiddleState = GetIRMiddle();
	LastTopState = GetIRTop();
	
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
bool PostPingPong(ES_Event_t ThisEvent)
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
ES_Event_t RunPingPong(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
		case PPStandby:
		{
			if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
      {
        CurrentState = PPStandby;
				break;
      }
			else if (ThisEvent.EventType == SPINNER_START) {
				printf("SPINNER_START in PPStandby\n\r");
				
				CurrentState = Neutral;
			}
			break;
		}
		
		case Neutral:
		{
			if (ThisEvent.EventType == SPINNER_STOP) {
				printf("SPINNER_STOP in Neutral\n\r");
				
				CurrentState = PPStandby;
			}
			else if (ThisEvent.EventType == FALLING_MIDDLE) {
				printf("FALLING_MIDDLE in Neutral\n\r");
				
				// Init 3s timer
				printf("Starting timer (3s)...\n\r");
				
				CurrentState = Middle;
			}
			else if (ThisEvent.EventType == FALLING_TOP) {
				printf("FALLING_TOP in Neutral\n\r");
				
				// Init 3s timer
				printf("Starting timer (3s)...\n\r");
				
				CurrentState = Top;
			}
			break;
		}
		case Middle:
		{
			if (ThisEvent.EventType == ES_TIMEOUT) {
				printf("ES_TIMEOUT in Middle\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = POS_MIDDLE;
				PostGame(Event2Post);
				
				CurrentState = Middle;
			}
			else if (ThisEvent.EventType == RISING_MIDDLE) {
				printf("RISING_MIDDLE in Middle\n\r");
				
				CurrentState = Neutral;
			}
			else if (ThisEvent.EventType == SPINNER_STOP) {
				printf("SPINNER_STOP in Middle\n\r");
				
				CurrentState = PPStandby;
			}
			break;
		}
		case Top:
		{
			if (ThisEvent.EventType == ES_TIMEOUT) {
				printf("ES_TIMEOUT in Top\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = POS_TOP;
				PostGame(Event2Post);
				
				CurrentState = Top;
			}
			else if (ThisEvent.EventType == RISING_TOP) {
				printf("RISING_Top in Top\n\r");
				
				CurrentState = Neutral;
			}
			else if (ThisEvent.EventType == SPINNER_STOP) {
				printf("SPINNER_STOP in Top\n\r");
				
				CurrentState = PPStandby;
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
PingPongState_t QueryPingPong(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void PingPongInitialize( void) {
	// Initialize the input lines to check for the middle and top IR LEDs
}

static uint8_t GetIRMiddle( void) {
  uint8_t InputState = 1;
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

static uint8_t GetIRTop( void) {
  uint8_t InputState = 1;

	return InputState;
}

bool CheckPingPongEvents( void) {
	
	bool ReturnVal = false;
	// Sample the middle IR line
	uint8_t CurrentMiddleState = GetIRMiddle();
	// Sample the top IR line
	uint8_t CurrentTopState = GetIRTop();
	
	if (CurrentMiddleState != LastMiddleState) {
      if (CurrentMiddleState) {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = RISING_MIDDLE;
        PostPingPong(ThisEvent);
      }
      else {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = FALLING_MIDDLE;
        PostPingPong(ThisEvent);
      }

      ReturnVal = true;
  }
	
	if (CurrentTopState != LastTopState) {
      if (CurrentTopState) {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = RISING_TOP;
        PostPingPong(ThisEvent);
      }
      else {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = FALLING_TOP;
        PostPingPong(ThisEvent);
      }

      ReturnVal = true;
  }
	
  LastMiddleState = CurrentMiddleState;
	LastTopState = CurrentTopState;
  return ReturnVal;
	
}





		
