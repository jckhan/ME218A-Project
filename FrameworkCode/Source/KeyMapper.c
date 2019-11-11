/****************************************************************************
 Module
   TemplateService.c

 Revision
   1.0.1

 Description
   This is a template file for implementing a simple service under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "KeyMapper.h"
#include "EventCheckers.h"

// Include every state machine
#include "TOT.h"
#include "Servo.h"
#include "Motor.h"
#include "Spinner.h"


/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateService

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any
     other required initialization for this service
 Notes

 Author
     J. Edward Carryer, 01/16/12, 10:00
****************************************************************************/
bool InitKeyMapper(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
<<<<<<< HEAD
	
	
   in here you write your initialization code
	
	
=======
   in here you write your initialization code
>>>>>>> 83b5678eb423f25f722abee4939afdc1b757a271
   *******************************************/
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
     PostTemplateService

 Parameters
     EF_Event_t ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostKeyMapper(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTemplateService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes

 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event_t RunKeyMapper(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  /********************************************/
	
	
	if (ThisEvent.EventType == ES_NEW_KEY) {
		char newKey = ThisEvent.EventParam;
		ES_Event_t Event2Post;
		
		printf("\n\r");
		
		switch(newKey)
		{
			case 'C':
			{
				Event2Post.EventType = GAME_COMPLETED;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'T':
			{
				Event2Post.EventType = TOT_DETECTED;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			
			case 't':
			{
				Event2Post.EventType = TOT_REMOVED;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'r':
			{
				Event2Post.EventType = RESET;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'S':
			{
				Event2Post.EventType = SPINNER_START;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 's':
			{
				Event2Post.EventType = SPINNER_STOP;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'z':
			{
				Event2Post.EventType = START_POTATO;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'x':
			{
				Event2Post.EventType = END_POTATO;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'u':
			{
				Event2Post.EventType = PULSE_DETECTED;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'c':
			{
				Event2Post.EventType = PP_COMPLETED;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'B':
			{
				Event2Post.EventType = BLOWING_START;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'b':
			{
				Event2Post.EventType = BLOWING_STOP;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'm':
			{
				Event2Post.EventType = POS_MIDDLE;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'n':
			{
				Event2Post.EventType = POS_TOP;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'a':
			{
				Event2Post.EventType = FALLING_MIDDLE;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'A':
			{
				Event2Post.EventType = FALLING_TOP;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'i':
			{
				Event2Post.EventType = RISING_MIDDLE;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case 'I':
			{
				Event2Post.EventType = RISING_TOP;
				Event2Post.EventParam = 1;
				ES_PostAll(Event2Post);
				break;
			}
			case '1':
			{
				Event2Post.EventType = ES_TIMEOUT;
				Event2Post.EventParam = 0;
				PostTOT(Event2Post);
				break;
			}
			case '2':
			{
				Event2Post.EventType = ES_TIMEOUT;
				Event2Post.EventParam = 1;
				PostServo(Event2Post);
				break;
			}
			case '3':
			{
				Event2Post.EventType = ES_TIMEOUT;
				Event2Post.EventParam = 2;
				PostServo(Event2Post);
				break;
			}
			case '4':
			{
				Event2Post.EventType = ES_TIMEOUT;
				Event2Post.EventParam = 0;
				PostMotor(Event2Post);
				break;
			}
			case '5':
			{
				Event2Post.EventType = ES_TIMEOUT;
				Event2Post.EventParam = 0;
				PostSpinner(Event2Post);
				break;
			}
			
		}
		//printf("%c", newKey);
	}
	
	
   /*******************************************/
  return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

