/****************************************************************************
 Module
   Servo.c

 Revision
   1.0.1

 Description
   This is the state machine to control the TOT servo and the system timer 
	 servo under the Gen2 Events and Services Framework.

 Notes

 History
 When           Who     
 -------------- ---     
 11/27/19 12:12 Jack Han      
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "Servo.h"
#include "Servo_actuator.h"

/*----------------------------- Module Defines ----------------------------*/
#define SERVO_LOW 180
#define SERVO_HIGH 120

#define TOTAL_TIME 65000
#define SHORT_TIME 1000
#define RESET_TIME 500

#define INCREMENTS 65

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
static void ResetServo(void);
static void IncrementServo(void);
static void ServoInitialize(void);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static ServoState_t CurrentState;
static uint16_t ServoIncrement;
static uint16_t IncrementsLeft = INCREMENTS;
static uint16_t CurrentPosition = SERVO_LOW;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitServo

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
 Jack Han, 11/27/19, 12:13
****************************************************************************/
bool InitServo(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
	
	ServoInitialize();
		
  // put us into the initial state
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
     PostServo

 Parameters
     EF_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     Jack Han, 11/27/19, 12:13
****************************************************************************/
bool PostServo(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunServo

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   Jack Han, 11/27/19, 12:13
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
				ResetServo();
				CurrentPosition = SERVO_LOW;
				// Init short timer 
				ES_Timer_InitTimer(SERVO_TIMER, SHORT_TIME);
				
				// Init system timer for the POTATO
				ES_Timer_InitTimer(TOT_TIMER, TOTAL_TIME);
				
				CurrentState = ServoRunning;
			}
			break;
		}
		
		case ServoRunning:
		{
			if (ThisEvent.EventType == TOT_REMOVED) {

				CurrentPosition = SERVO_LOW;	// Next time the timing servo moves, it will reset 
				IncrementsLeft = INCREMENTS;  // Reset the number of increments
				
				CurrentState = ServoStandby;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
					// Increment servo
					IncrementServo();
					
					// Init short timer 
					ES_Timer_InitTimer(SERVO_TIMER, SHORT_TIME);
					
					CurrentState = ServoRunning;		
			}
			else if (ThisEvent.EventType == RESET) {				
				// Return servo to original position
				ResetServo();
				CurrentPosition = SERVO_LOW;
				IncrementsLeft = INCREMENTS;
				
				CurrentState = ServoStandby;
			}
			else if (ThisEvent.EventType == END_POTATO) {
				// Return servo to original position
				ResetServo();
				CurrentPosition = SERVO_LOW;
				IncrementsLeft = INCREMENTS;
				
				CurrentState = ServoStandby;
			}
			break;
		}
		default:
      ;
  } // end switch on Current State
  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryServo

 Parameters
     None

 Returns
     TemplateState_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
     Jack Han, 11/27/19, 12:13
****************************************************************************/
ServoState_t QueryServo(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/

// Initialization sequence for the servo pins
void ServoInitialize( void) 
{
	// Set the timer servo to its default position
	ServoPWM(SERVO_LOW,0,1); //This is PB6
}

// Move the servo by the predefined increment
void IncrementServo( void) {
	ServoIncrement = (CurrentPosition - SERVO_HIGH) / IncrementsLeft;
	IncrementsLeft -= 1;
	CurrentPosition -= ServoIncrement;
	if (CurrentPosition <= 180 && IncrementsLeft <= INCREMENTS) {  // When IncrementsLeft drops below 0, it will wrap around to a high positive integer
		ServoPWM(CurrentPosition,0,1);
	}
}

void ResetServo( void) {
	// NOTE: the servo reset function is writte in this way because the timer servo and trapdoor servo conflict with each other if both try to move
	// at the same time. We implemented a workaround by having the servo actually reset (in the NoTOT state in TOT.c) after a waiting time.
	// The timer for that waiting time is initialized in the Waiting4NextGame state in TOT.c
	
	CurrentPosition = SERVO_LOW;	// Reset the servo position so it will goes to that position the next time it moves
	ES_Timer_StopTimer(SERVO_TIMER);
}
