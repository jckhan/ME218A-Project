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
#include "TOT.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static TOTState_t CurrentState;

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
bool InitTOT(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  // put us into the Initial PseudoState
  CurrentState = NoTOT;
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
bool PostTOT(ES_Event_t ThisEvent)
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
ES_Event_t RunTOT(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
		case NoTOT:
		{
			if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
      {
				
        TOTInitialize();
        CurrentState = NoTOT;
				break;
      }
			else if (ThisEvent.EventType == TOT_DETECTED) {
				printf("TOT_DETECTED in NoTOT\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = START_POTATO;
				ES_PostAll(Event2Post);
				
				//Start game timer (60s)
				
				CurrentState = YesTOT;
			}
			break;
		}
		
		case YesTOT:
		{
			if (ThisEvent.EventType == TOT_REMOVED) {
				printf("TOT_REMOVED in YesTOT\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
				
				// Open trapdoor to release TOT
				
				CurrentState = NoTOT;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				printf("TIMEOUT in YesTOT\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
				
				// Open trapdoor to release TOT
				
				CurrentState = NoTOT;
			}
			else if (ThisEvent.EventType == GAME_COMPLETED) {
				printf("GAME_COMPLETED in YesTOT\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
				
				// Open trapdoor to release TOT
				
				CurrentState = NoTOT;
			}
			else if (ThisEvent.EventType == RESET) {
				printf("RESET in YesTOT\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
				
				// Open trapdoor to release TOT
				
				CurrentState = NoTOT;
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
TOTState_t QueryTOT(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void TOTInitialize( void) {
	// Initialize a data line as the input for the TOT IR
	
}
