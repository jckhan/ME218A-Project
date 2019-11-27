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
#define MIC_HI BIT2HI
#define MIC_LO BIT2LO

#define BLOWING_DURATION 200
#define TWO_SEC 1500
#define NORMAL_SUCCESS 140
#define FINAL_SUCCESS 140

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static BlowerState_t CurrentState;
static uint8_t LastMicState;

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
	
	LED_on(0);
	
  // put us into the Initial PseudoState
  CurrentState = BlowerStandby;
	
	LastMicState = GetMicState();
	
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

	static uint8_t increment = 0;
	
	if (ThisEvent.EventType != ES_TIMEOUT) {
		ES_Timer_InitTimer(11, 30000);
	}
	
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
					increment = 0;
					
					// Turn off all LEDs
					
					CurrentState = NotBlowing;
				}				
			}
//			else if (ThisEvent.EventType == ES_TIMEOUT) {
//				static uint8_t increment = 0;
//				if(ThisEvent.EventParam == 10){
//					AUDIO_SR_Write(BIT6HI);
//					AUDIO_SR_Write(BIT4HI);
//					AUDIO_SR_Write(BIT5HI);
//					AUDIO_SR_Write(BIT3HI);
//					AUDIO_SR_Write(BIT7HI);
//				}
//			}
			break;
		}
		
		case NotBlowing:
		{
			//Switch off LEDs
			
			if (ThisEvent.EventType == SPINNER_STOP) {
				printf("SPINNER_STOP in NotBlowing\n\r");
				increment = 0;
				LED_on(0);
				CurrentState = BlowerStandby;
			}
			else if (ThisEvent.EventType == BLOWING_START) {
				printf("BLOWER_START in NotBlowing\n\r");
				//Init 200ms timer
				//printf("Starting timer (200 ms)...\n\r");
				ES_Timer_InitTimer(BLOWING_TIMER, BLOWING_DURATION);
				
				// Init 2s timer
				printf("Starting timer (2s)...\n\r");
				ES_Timer_InitTimer(BLOWING_LED_TIMER, TWO_SEC);
				
				CurrentState = Blowing;
			}
			break;
		}
		case Blowing:
		{
			if (ThisEvent.EventType == SPINNER_STOP) {
				printf("SPINNER_STOP in Blowing\n\r");
				
				// Switch off LEDs
				LED_on(0);
				increment = 0;
				CurrentState = BlowerStandby;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				
				if(ThisEvent.EventParam == 10){
					AUDIO_SR_Write(BIT4HI);
					AUDIO_SR_Write(BIT6HI);
					AUDIO_SR_Write(BIT5HI);
					AUDIO_SR_Write(BIT3HI);
					AUDIO_SR_Write(BIT7HI);
				}
				else if (ThisEvent.EventParam == BLOWING_LED_TIMER) {
					printf("BLOWING_LED_TIMEOUT in Blowing\n\r");
					
					// Increment LED on
					printf("Incrementing LEDs...\n\r");
					increment++;
					AUDIO_SR_Write(BIT3HI);
					AUDIO_SR_Write(BIT5HI);
					AUDIO_SR_Write(BIT6HI);
					AUDIO_SR_Write(BIT7HI);
					if (increment != 4) {
						AUDIO_SR_Write(BIT4LO);
					}
					ES_Timer_InitTimer(10,NORMAL_SUCCESS);
					printf("%d\n\r", increment);
					
					LED_on(increment);
					if(increment == 4){
						AUDIO_SR_Write(BIT4HI);
						AUDIO_SR_Write(BIT5HI);
						AUDIO_SR_Write(BIT3HI);
						AUDIO_SR_Write(BIT7HI);
						AUDIO_SR_Write(BIT6LO);
						
						printf("Playing final success\n\r");

						ES_Timer_InitTimer(10,FINAL_SUCCESS);
						
						ES_Event_t ThisEvent;
						ThisEvent.EventType = GAME_COMPLETED;
						ES_PostAll(ThisEvent);
						
						CurrentState = BlowerStandby;
					}

					// Restart 2s LED timer
					printf("Starting timer (2s)...\n\r");
					ES_Timer_InitTimer(BLOWING_LED_TIMER, TWO_SEC);
					
				} else if (ThisEvent.EventParam == BLOWING_TIMER) {
					printf("BLOWING_TIMEOUT in Blowing\n\r");
					// Reset and switch off LEDs
					increment = 0;
					LED_on(0);
					
					CurrentState = NotBlowing;
				}
					
			} else if (ThisEvent.EventType == BLOWING_START) {
				//printf("BLOWING_START in Blowing\n\r");
				
				// Restart blowing timer
				//printf("Restart 200 ms timer...\n\r");
				ES_Timer_InitTimer(BLOWING_TIMER, BLOWING_DURATION);			
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
// Initialize the input line for the microphone
void BlowerInitialize( void) {
	// set up port B by enabling the peripheral clock,
	SR_Init();
  // set PB42 to be used as digital I/O
  HWREG(GPIO_PORTB_BASE+GPIO_O_DEN) |= MIC_HI;

  // set PB2 direction to input
  HWREG(GPIO_PORTB_BASE+GPIO_O_DIR) &= MIC_LO;

}

// Control the leds by passing in the number of LED to swtich on
// 0 turns all LEDs off
static void LED_on(uint8_t num) {
	switch (num) {
		case 0:
			//printf("Writing LED ZERO\n\r");
			LED_SR_Write(BIT0LO);
			LED_SR_Write(BIT1LO);
			LED_SR_Write(BIT2LO);
			LED_SR_Write(BIT3LO);
			break;
		case 1:
			LED_SR_Write(BIT0HI);
			break;
		case 2:
			LED_SR_Write(BIT1HI);
			break;
		case 3:
			LED_SR_Write(BIT2HI);
			break;
		case 4:
			LED_SR_Write(BIT3HI);
			break;
		default:
			break;
	}
}

// Gets the current state of the mic pin
static uint8_t GetMicState( void) {
  uint8_t InputState = 0;
  InputState  = HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS));
  if (InputState & MIC_HI) {
    return 1;
  } else {
    return 0;
  }
}

// Blower event checker
// Returns true if there are falling and rising edges
bool CheckBlowerEvents( void) {
	
	bool ReturnVal = false;

	uint8_t CurrentMicState = GetMicState();
	
	if (CurrentMicState != LastMicState) {
			ES_Event_t ThisEvent;
			ThisEvent.EventType = BLOWING_START;
			PostBlower(ThisEvent);
      ReturnVal = true;
  }
	
  LastMicState = CurrentMicState;
  return ReturnVal;
	
}





		
