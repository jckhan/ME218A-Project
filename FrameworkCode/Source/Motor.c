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

#include "Motor.h"
#include "PWM16Tiva.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static MotorState_t CurrentState;

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
bool InitMotor(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
	
	MotorInitialize();
	
  // put us into the Initial PseudoState
  CurrentState = MotorOff;
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
bool PostMotor(ES_Event_t ThisEvent)
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
ES_Event_t RunMotor(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
		case MotorOff:
		{
			if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
      {
        CurrentState = MotorOff;
				break;
      }
			else if (ThisEvent.EventType == SPINNER_START) {
				printf("SPINNER_START in MotorOff\n\r");
				
				// Replace this with the queried state of Game_SM
				GameState_t GameState = PingPong_NotCompleted; //  <== REPLACE THIS!!!
				
				if (GameState == PingPong_NotCompleted) {
					// Init PWM at 0
					printf("Initializing PWM at 0...\n\r");
					// Init timer 50ms
					printf("Starting timer (50ms)...\n\r");
				}
				
				CurrentState = MotorOn;
			}
			break;
		}
		
		case MotorOn:
		{
			if (ThisEvent.EventType == SPINNER_STOP) {
				printf("SPINNER_STOP in MotorOn\n\r");
				
				// Stop PWM
				printf("Stopping PWM...\n\r");
				
				CurrentState = MotorOff;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				printf("ES_TIMEOUT in MotorOn\n\r");
				
				GetInputSignal();
				ConvertSignal();
				
				// Init timer 50ms
				printf("Starting timer (50ms)...\n\r");
				
				CurrentState = MotorOn;
			}
			else if (ThisEvent.EventType == GAME_COMPLETED) {
				printf("GAME_COMPLETED in MotorOn\n\r");
				
				// Stop PWM
				printf("Stopping PWM...\n\r");
				
				CurrentState = MotorOff;
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
MotorState_t QueryMotor(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void MotorInitialize( void) {
	
	// Initialize the analog input line
	
	// Initialize the output line for the motor
	
}

void GetInputSignal( void) {
	
	printf("Getting the input signal...\n\r");
	// Read the current analog input
	
}

void ConvertSignal( void) {
	
	printf("Converting the analog signal...\n\r");
	// Convert the analog signal into a digital output signal to the motor
	
}
