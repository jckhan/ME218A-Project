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
#include "Servo.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static ServoState_t CurrentState;

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
bool InitServo(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
	
	ServoInitialize();
	
  // put us into the Initial PseudoState
  CurrentState = ServoStandby;
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
bool PostServo(ES_Event_t ThisEvent)
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
ES_Event_t RunServo(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
		case ServoStandby:
		{
			if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
      {
        CurrentState = ServoStandby;
				break;
      }
			else if (ThisEvent.EventType == TOT_DETECTED) {
				printf("TOT_DETECTED in ServoStandby\n\r");
				
				// Init short timer 50ms
				printf("Starting timer (50ms)...\n\r");
				// Init long timer 60s
				printf("Starting timer (60s)...\n\r");
				// Start the servo motor
				printf("Starting servo motor...\n\r");
				
				CurrentState = ServoRunning;
			}
			break;
		}
		
		case ServoRunning:
		{
			if (ThisEvent.EventType == TOT_REMOVED) {
				printf("TOT_REMOVED in ServoRunning\n\r");
				
				// Return servo to original position
				printf("Returning servo to original position...\n\r");
				
				CurrentState = ServoStandby;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				if (ThisEvent.EventParam == 1) {
					printf("TIMEOUT_SHORT in ServoRunning\n\r");
					
					// Increment servo
					printf("Incrementing servo...\n\r");
					// Init short timer 50ms
					printf("Starting timer (50ms)...\n\r");
					
					CurrentState = ServoRunning;
				}
				else if (ThisEvent.EventParam == 2) {
					printf("TIMEOUT_LONG in ServoRunning\n\r");
					
					ES_Event_t Event2Post;
					Event2Post.EventType = GAME_COMPLETED;
					ES_PostAll(Event2Post);
					
					// Return servo to original position
					printf("Returning servo to original position...\n\r");
					
					CurrentState = ServoStandby;
				}				
			}
			else if (ThisEvent.EventType == RESET) {
				printf("RESET in ServoRunning\n\r");
				
				// Return servo to original position
				printf("Returning servo to original position...\n\r");
				
				CurrentState = ServoStandby;
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
ServoState_t QueryServo(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void ServoInitialize( void) {
	
	// Initialize the output line for the servo
	
}