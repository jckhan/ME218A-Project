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
#include "Servo_Actuator.h"
#include "ES_Port.h"
#include "termio.h"
#include "EnablePA25_PB23_PD7_PF0.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_pwm.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "inc/hw_sysctl.h"
#include "PWM16Tiva.h"
#include "Game.h"
#include "ShiftRegisterWrite.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static GameState_t CurrentState;

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
bool InitGame(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
	
	GameInitialize();
	
  // put us into the Initial PseudoState
  CurrentState = GameStandby;
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
bool PostGame(ES_Event_t ThisEvent)
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
ES_Event_t RunGame(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

	if (ThisEvent.EventType != ES_TIMEOUT) {
		ES_Timer_InitTimer(11, 30000);
	}
	
  switch (CurrentState)
  {
		case GameStandby:
		{
			if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
      {
        CurrentState = GameStandby;
				break;
      }
			else if (ThisEvent.EventType == SPINNER_START) {
				printf("SPINNER_START in GameStandby\n\r");
				
				// Turn on middle LEDs
				LEDMiddle(1);
				AUDIO_SR_Write(BIT6HI);
				AUDIO_SR_Write(BIT3HI);
				AUDIO_SR_Write(BIT5HI);
				AUDIO_SR_Write(BIT4HI);
				AUDIO_SR_Write(BIT7HI);
				CurrentState = Level1;
			}
			break;
		}
		
		case Level1:
		{
			if (ThisEvent.EventType == POS_MIDDLE) {
				printf("POS_MIDDLE in Level1\n\r");
				
				// Turn off middle LEDs
				LEDMiddle(0);
				// Turn on top LEDs
				LEDTop(1);
				AUDIO_SR_Write(BIT5HI);
				AUDIO_SR_Write(BIT4HI);
				AUDIO_SR_Write(BIT7HI);
				AUDIO_SR_Write(BIT6HI);
				AUDIO_SR_Write(BIT3LO);
				ES_Timer_InitTimer(10,140); //Audio Timer
				CurrentState = Level2;
			}
			else if(ThisEvent.EventType == ES_TIMEOUT){
				if(ThisEvent.EventParam == 10){
					AUDIO_SR_Write(BIT6HI);
					AUDIO_SR_Write(BIT3HI);
					AUDIO_SR_Write(BIT5HI);
					AUDIO_SR_Write(BIT4HI);
					AUDIO_SR_Write(BIT7HI);
				}
			}
			else if (ThisEvent.EventType == END_POTATO) {
				printf("END_POTATO in Level1\n\r");
				
				// Turn off all LEDs
				LEDMiddle(0);
				
				CurrentState = GameStandby;
			}
			break;
		}
		case Level2:
		{
			if (ThisEvent.EventType == POS_TOP) {
				printf("POS_TOP in Level2\n\r");
				
				// Turn off top LEDs
				LEDTop(0);
				// Turn on middle LEDs
				LEDMiddle(1);
				AUDIO_SR_Write(BIT5HI);
				AUDIO_SR_Write(BIT4HI);
				AUDIO_SR_Write(BIT7HI);
				AUDIO_SR_Write(BIT6HI);
				AUDIO_SR_Write(BIT3LO);
				ES_Timer_InitTimer(10,140);
				CurrentState = Level3;
			}
			else if(ThisEvent.EventType == ES_TIMEOUT){
				if(ThisEvent.EventParam == 10){
					AUDIO_SR_Write(BIT6HI);
					AUDIO_SR_Write(BIT3HI);
					AUDIO_SR_Write(BIT5HI);
					AUDIO_SR_Write(BIT3HI);
					AUDIO_SR_Write(BIT7HI);
				}
			}
			else if (ThisEvent.EventType == END_POTATO) {
				printf("END_POTATO in Level2\n\r");
				
				// Turn off all LEDs
				LEDTop(0);
				
				CurrentState = GameStandby;
			}
			break;
		}
		case Level3:
		{
			if (ThisEvent.EventType == POS_MIDDLE) {
				printf("POS_MIDDLE in Level3\n\r");
				
				// Turn off middle LEDs
				LEDMiddle(0);
				// Turn on success LEDs
				LEDSuccess(1);
				
				// Init 3s timer
				printf("Starting timer (3s)...\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = PP_COMPLETED;
				ES_PostAll(Event2Post);
				AUDIO_SR_Write(BIT6LO);
				printf("Turning on success sound\n\r");
				
				ES_Timer_InitTimer(10,140);
				CurrentState = PingPong_Completed;
			}
			else if(ThisEvent.EventType == ES_TIMEOUT){
				if(ThisEvent.EventParam == 10){
					AUDIO_SR_Write(BIT6HI);
					AUDIO_SR_Write(BIT3HI);
					AUDIO_SR_Write(BIT5HI);
					AUDIO_SR_Write(BIT3HI);
					AUDIO_SR_Write(BIT7HI);
				}
			}
			else if (ThisEvent.EventType == END_POTATO) {
				printf("END_POTATO in Level3\n\r");
				
				// Turn off all LEDs
				LEDMiddle(0);
				
				CurrentState = GameStandby;
			}
			break;
		}
		case PingPong_Completed:
		{
			if (ThisEvent.EventType == ES_TIMEOUT) {
				printf("ES_TIMEOUT in PingPong_Completed\n\r");
					if(ThisEvent.EventParam == 10){
						AUDIO_SR_Write(BIT6HI);
						AUDIO_SR_Write(BIT3HI);
						AUDIO_SR_Write(BIT5HI);
						AUDIO_SR_Write(BIT3HI);
						AUDIO_SR_Write(BIT7HI);
						printf("Wrote success sound high\n\r");
				}
					else if(ThisEvent.EventParam == 8){
				// Turn off success LEDs
				LEDSuccess(0);
				
				CurrentState = PingPong_Completed;
					}
			}
			else if (ThisEvent.EventType == END_POTATO) {
				printf("END_POTATO in PingPong_Completed\n\r");
				
				// Turn off success LEDs
				LEDSuccess(0);
				AUDIO_SR_Write(BIT6HI);
				AUDIO_SR_Write(BIT3HI);
				AUDIO_SR_Write(BIT5HI);
				AUDIO_SR_Write(BIT4HI);
				AUDIO_SR_Write(BIT7HI);
				CurrentState = GameStandby;
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
GameState_t QueryGame(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void GameInitialize( void) {
	//Initialize pins necessary for this module (LED Outputs, Mic Input)
//	HWREG(GPIO_PORTA_BASE+GPIO_O_DEN) |= (BIT2HI | BIT3HI | BIT4HI); //Digital Enable PA2 PA3 and PA4 for Serial Data, Shift Clock, and Register clock respectively OUPUTS
//	HWREG(GPIO_PORTA_BASE+GPIO_O_DIR) |= (BIT2HI | BIT3HI | BIT4HI); //Set as output
//	HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT2LO & BIT3LO); //Set Data and shift clock low
//	HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT4HI);	//Set Reg clock high
//	
//	HWREG(GPIO_PORTB_BASE+GPIO_O_DEN) |= (BIT2HI); //Digital Enable PB2 for Mic Input
//	HWREG(GPIO_PORTB_BASE+GPIO_O_DIR) |= (BIT2LO); //set as input
}

void LEDMiddle(uint8_t Setting) {
	if (Setting == 0) {
		LED_SR_Write(BIT4LO);
		printf("Turning off middle LEDs...\n\r");
	}
	else if (Setting == 1){
		LED_SR_Write(BIT4HI);
		printf("Turning on middle LEDs...\n\r");
	}
}

void LEDTop(uint8_t Setting) {
	if (Setting == 0) {
		// Turn off top LEDs
		LED_SR_Write(BIT5LO);
		printf("Turning off top LEDs...\n\r");
	}
	else if (Setting == 1){
		LED_SR_Write(BIT5HI);
		// Turn on top LEDs
		printf("Turning on top LEDs...\n\r");
	}
}

void LEDSuccess(uint8_t Setting) {
	if (Setting == 0) {
		LED_SR_Write(BIT4LO);
		LED_SR_Write(BIT5LO);
		printf("Turning off success LEDs...\n\r");
	}
	else if (Setting == 1){
		// Turn on success LEDs
		LED_SR_Write(BIT4HI);
		LED_SR_Write(BIT5HI);
		printf("Turning on success LEDs...\n\r");
	}
}
		
