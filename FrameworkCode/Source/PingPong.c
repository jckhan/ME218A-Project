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
#include "ShiftRegisterWrite.h"

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
#define IR_TOP_HI BIT5HI
#define IR_TOP_LO BIT5LO
#define IR_MID_HI BIT4HI
#define IR_MID_LO BIT4LO

#define HOLD_TIME 3000

#define POP_LO BIT4LO
#define POP_HI BIT4HI

#define AUDIO_TIME 140
#define IDLE_TIME 30000
#define HACKY_TIME 300


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
static void PingPongInitialize( void);
static uint8_t GetIRMiddle( void);
static uint8_t GetIRTop( void);

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
     InitPingPong

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
     M.Swai, 11/11/19, 22:08
****************************************************************************/
bool InitPingPong(uint8_t Priority)
{
  MyPriority = Priority;
	
	// Initialize hardware ports
	PingPongInitialize();
	
  // Put us into the Initial PseudoState
  CurrentState = PPStandby;
	
	// Get the current state of the pins
	LastMiddleState = GetIRMiddle();
	LastTopState = GetIRTop();
	
	return true;
}


/****************************************************************************
 Function
     PostPingPong

 Parameters
     EF_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     M.Swai, 11/11/19, 22:31
****************************************************************************/
bool PostPingPong(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}


/****************************************************************************
 Function
    RunPingPong

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   
 Notes
   uses nested switch/case to implement the machine.
 Author
   M.swai, 11/11/19, 23:17
****************************************************************************/
ES_Event_t RunPingPong(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

	if (ThisEvent.EventType != ES_TIMEOUT) {
		ES_Timer_InitTimer(IDLE_TIMER, IDLE_TIME);
	}
	
  switch (CurrentState)
  {
		case PPStandby:
		{
			if (ThisEvent.EventType == SPINNER_START) {
				CurrentState = Neutral;
			}
			break;
		}
		
		case Neutral:
		{
			if (ThisEvent.EventType == SPINNER_STOP) {
				CurrentState = PPStandby;
			}
			else if (ThisEvent.EventType == FALLING_MIDDLE) {
				// Init hold timer
				ES_Timer_InitTimer(PINGPONG_TIMER, HOLD_TIME);
				
				StopAudio();
				AUDIO_SR_Write(POP_LO);

				ES_Timer_InitTimer(AUDIO_TIMER, AUDIO_TIME);
				CurrentState = Middle;
			}
			else if (ThisEvent.EventType == FALLING_TOP) {
				ES_Timer_InitTimer(HACKY_TIMER, HACKY_TIME);
				
				// Init hold timer
				ES_Timer_InitTimer(PINGPONG_TIMER, HOLD_TIME);
				
				StopAudio();
				AUDIO_SR_Write(POP_LO);
				
				ES_Timer_InitTimer(AUDIO_TIMER, AUDIO_TIME);
				CurrentState = Top;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				if(ThisEvent.EventParam == AUDIO_TIMER){
					StopAudio();
				}
			}
			break;
		}
		
		case Middle:
		{
			if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == PINGPONG_TIMER) {
				ES_Event_t Event2Post;
				Event2Post.EventType = POS_MIDDLE;
				PostGame(Event2Post);
				
				CurrentState = Middle;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == AUDIO_TIMER) {;
				StopAudio();
				
				CurrentState = Middle;
			}
			else if (ThisEvent.EventType == RISING_MIDDLE) {
				StopAudio();
				AUDIO_SR_Write(POP_LO);
				ES_Timer_InitTimer(AUDIO_TIMER, AUDIO_TIME);
				CurrentState = Neutral;
			}
			
			else if (ThisEvent.EventType == SPINNER_STOP) {
				CurrentState = PPStandby;
			}
			break;
		}
		
		case Top:
		{
			if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == PINGPONG_TIMER) {
				ES_Event_t Event2Post;
				Event2Post.EventType = POS_TOP;
				PostGame(Event2Post);
				
				CurrentState = Top;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == AUDIO_TIMER) {
				StopAudio();
				
				CurrentState = Top;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == HACKY_TIMER) {
				AUDIO_SR_Write(BIT4LO);
				ES_Timer_InitTimer(AUDIO_TIMER, AUDIO_TIME);
				CurrentState = Neutral;
			}
			
			else if (ThisEvent.EventType == FALLING_TOP) {
				ES_Timer_InitTimer(HACKY_TIMER, HACKY_TIME);
				CurrentState = Top;
			}

			else if (ThisEvent.EventType == SPINNER_STOP) {
				CurrentState = PPStandby;
			}
			break;
		}
		
		default:
			break;
  }
	
  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryPingPong

 Parameters
     None

 Returns
     PingPongState_t The current state of the PingPongSM

 Description
     returns the current state of the PingPongSM
 Notes

 Author
     M.Swai, 11/11/19, 22:29
****************************************************************************/
PingPongState_t QueryPingPong(void)
{
  return CurrentState;
}


// Event Checker for PingPong IRs
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
	  ES_Timer_InitTimer(AUDIO_TIMER, AUDIO_TIME);
      ReturnVal = true;
  }
	if (CurrentTopState != LastTopState) {
			if(CurrentTopState == 0) {
				ES_Event_t ThisEvent;
				ThisEvent.EventType = FALLING_TOP;
				PostPingPong(ThisEvent);
				ReturnVal = true;
			}
  }
	
  LastMiddleState = CurrentMiddleState;
	LastTopState = CurrentTopState;
  return ReturnVal;
	
}

/***************************************************************************
 private functions
 ***************************************************************************/

// Initialize the input lines to check for the middle and top IR LEDs
static void PingPongInitialize( void) {
  // set PB3 and PB4 to be used as digital I/O
  HWREG(GPIO_PORTC_BASE+GPIO_O_DEN) |= (IR_TOP_HI | IR_MID_HI) ;

  // set PB3 direction to input
  HWREG(GPIO_PORTC_BASE+GPIO_O_DIR) &= (IR_TOP_LO & IR_MID_LO);
}

// Get the pin status of the middle IR
static uint8_t GetIRMiddle( void) {
  uint8_t InputState;
  InputState  = HWREG(GPIO_PORTC_BASE + (GPIO_O_DATA + ALL_BITS));
  if (InputState & IR_MID_HI) {
    return 1;
  } else {
    return 0;
  }
}

// Get the pin status of the top IR
static uint8_t GetIRTop( void) {
  uint8_t InputState = 1;
	InputState  = HWREG(GPIO_PORTC_BASE + (GPIO_O_DATA + ALL_BITS));
  if (InputState & IR_TOP_HI) {
    return 1;
  } else {
    return 0;
  }
}





		
