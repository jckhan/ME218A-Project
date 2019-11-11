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

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static SpinnerState_t CurrentState;

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

			}
			else if (ThisEvent.EventType == END_POTATO) {
				printf("END_POTATO in Waiting4Pulse\n\r");

			}
			break;
		}
		case MaybeSpinning:
		{
			if (ThisEvent.EventType == PULSE_DETECTED) {
				printf("PULSE_DETECTED in MaybeSpinning\n\r");

			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				printf("ES_TIMEOUT in MaybeSpinning\n\r");

			}
			else if (ThisEvent.EventType == END_POTATO) {
				printf("END_POTATO in MaybeSpinning\n\r");

			}
			break;
		}
		case Spinning:
		{
			if (ThisEvent.EventType == PULSE_DETECTED) {
				printf("PULSE_DETECTED in Spinning\n\r");

			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				printf("ES_TIMEOUT in Spinning\n\r");

			}
			else if (ThisEvent.EventType == END_POTATO) {
				printf("END_POTATO in Spinning\n\r");

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
	// Initialize a data line as the input for the TOT IR
	// Initialize the control line for the trapdoor servo
}
