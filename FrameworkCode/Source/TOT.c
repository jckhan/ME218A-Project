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
#include "Servo.h"

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
#include "ShiftRegisterWrite.h"

/*----------------------------- Module Defines ----------------------------*/
#define NEXTGAMETIME 4500
#define IDLE_TIME 30000
#define WELCOME_TIME 1000
#define TIMING_SERVO_LOW 180
/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static TOTState_t CurrentState;
static uint8_t LastTOTState;
static uint8_t WelcomeIncrement;

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
	
	TOTInitialize();
	
  // put us into the Initial PseudoState
  CurrentState = NoTOT;
	
	LastTOTState = GetTOTState();

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
	
	if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == 11) {
		ES_Event_t Event2Post;
		Event2Post.EventType = RESET;
		ES_PostAll(Event2Post);
	} else {
		ES_Timer_InitTimer(11, IDLE_TIME);
	}

  switch (CurrentState)
  {
		case NoTOT:
						//printf("current state: notot\n\r");

		{
			if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
      {
				WelcomeIncrement = 0;
        CurrentState = NoTOT;
				ES_Timer_InitTimer(12, WELCOME_TIME);
				//break;
      }
			else if (ThisEvent.EventType == TOT_DETECTED) {
				printf("TOT_DETECTED in NoTOT\n\r");
				
				WelcomeIncrement = 0;
				LED_SR_Write(BIT0LO);		// Blower 1
				LED_SR_Write(BIT1LO);		// Blower 2
				LED_SR_Write(BIT2LO);		// Blower 3
				LED_SR_Write(BIT3LO);		// Blower 4
				LED_SR_Write(BIT4LO); 	// Pingpong middle
				LED_SR_Write(BIT5LO);		// Pingpong top
							
//				ResetServo();
				ES_Event_t Event2Post;
				Event2Post.EventType = START_POTATO;
				ES_PostAll(Event2Post);
				AUDIO_SR_Write((uint8_t)BIT7LO);
				ES_Timer_InitTimer(10, 140);
				ES_Timer_InitTimer(11, IDLE_TIME);
				
				printf("POTATO started...\n\r");
				
		
				// Game timer is started by the servo
				
				CurrentState = YesTOT;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == 12) {
				uint8_t Remainder = WelcomeIncrement % 9;
				
				switch (Remainder) {
					case 0:
					{
						LED_SR_Write(BIT0LO);		// Blower 1
						LED_SR_Write(BIT1LO);		// Blower 2
						LED_SR_Write(BIT2LO);		// Blower 3
						LED_SR_Write(BIT3LO);		// Blower 4
						LED_SR_Write(BIT4LO); 	// Pingpong middle
						LED_SR_Write(BIT5LO);		// Pingpong top
						break;
					}
					case 1:
					{
						LED_SR_Write(BIT4HI);
						break;
					}
					case 2:
					{
						LED_SR_Write(BIT5HI);
						break;
					}
					case 3:
					{
						LED_SR_Write(BIT0HI);
						break;
					}
					case 4:
					{
						LED_SR_Write(BIT1HI);
						break;
					}
					case 5:
					{
						LED_SR_Write(BIT2HI);
						break;
					}
					case 6:
					{
						LED_SR_Write(BIT3HI);
						break;
					}
					case 7:
					{
						LED_SR_Write(BIT0LO);		// Blower 1
						LED_SR_Write(BIT1LO);		// Blower 2
						LED_SR_Write(BIT2LO);		// Blower 3
						LED_SR_Write(BIT3LO);		// Blower 4
						LED_SR_Write(BIT4LO); 	// Pingpong middle
						LED_SR_Write(BIT5LO);		// Pingpong top
						break;
					}
					case 8:
					{
						LED_SR_Write(BIT0HI);		// Blower 1
						LED_SR_Write(BIT1HI);		// Blower 2
						LED_SR_Write(BIT2HI);		// Blower 3
						LED_SR_Write(BIT3HI);		// Blower 4
						LED_SR_Write(BIT4HI); 	// Pingpong middle
						LED_SR_Write(BIT5HI);		// Pingpong top
						break;
					}
				}
				
				WelcomeIncrement++;
				ES_Timer_InitTimer(12, WELCOME_TIME);
			}
			else if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == 14) {
				printf("THIS IS THE TIMING RESET TIMER !!!!!!!!!!!!! \n\r");
				ServoPWM(TIMING_SERVO_LOW, 0, 1);
			}
			break;
		}
		
		case YesTOT:
						//printf("current state: yestot\n\r");

		{
			if (ThisEvent.EventType == TOT_REMOVED) {
				printf("TOT_REMOVED in YesTOT\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
				
				printf("POTATO ended...\n\r");
				//init timer for next game
				ES_Timer_InitTimer(1, 1000);
//				ReleaseTOT();
				CurrentState = Waiting4NextGame;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				
				if(ThisEvent.EventParam == 10){
					printf("AUDIO TIMEOUT in YesTOT\n\r");
					AUDIO_SR_Write(BIT3HI);
					AUDIO_SR_Write(BIT4HI);
					AUDIO_SR_Write(BIT5HI);
					AUDIO_SR_Write(BIT6HI);
					AUDIO_SR_Write(BIT7HI);
				}
				else if(ThisEvent.EventParam == 1){
					printf("Timer 1 TIMEOUT in YesTOT\n\r");
					
					ES_Event_t Event2Post;
					Event2Post.EventType = END_POTATO;
					ES_PostAll(Event2Post);
				
					printf("POTATO ended...\n\r");
				
				// Open trapdoor to release TOT
					ReleaseTOT();
//					AUDIO_SR_Write(BIT5LO);
//					ES_Timer_InitTimer(10, 140);
					ES_Timer_InitTimer(1, NEXTGAMETIME);
					CurrentState = Waiting4NextGame;
				}
				}
			else if (ThisEvent.EventType == GAME_COMPLETED) {
				printf("GAME_COMPLETED in YesTOT\n\r");
				
				printf("RESET in YesTOT\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
				
				// Open trapdoor to release TOT
				ReleaseTOT();
				ES_Timer_InitTimer(1, NEXTGAMETIME);
				CurrentState = Waiting4NextGame;
			}
			else if (ThisEvent.EventType == RESET) {
				printf("RESET in YesTOT\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
				
				// Open trapdoor to release TOT
				ReleaseTOT();
				ES_Timer_InitTimer(1, NEXTGAMETIME);
				CurrentState = Waiting4NextGame;
			}
			break;
		}
		case Waiting4NextGame:
			//printf("current state: waiting4nextgame\n\r");
		{
			if(ThisEvent.EventType == ES_TIMEOUT){
				if(ThisEvent.EventParam == 10){
					AUDIO_SR_Write(BIT5HI);
					AUDIO_SR_Write(BIT6HI);
					
					AUDIO_SR_Write(BIT5LO);	// Play welcome to Mars
					ES_Timer_InitTimer(10, 140);
				}
				else if (ThisEvent.EventParam == 1){
					ServoPWM(1,0,0);
					AUDIO_SR_Write(BIT3HI);
					AUDIO_SR_Write(BIT4HI);
					AUDIO_SR_Write(BIT5HI);
					AUDIO_SR_Write(BIT6HI);
					AUDIO_SR_Write(BIT7HI);
					LED_SR_Write(BIT0LO);		// Blower 1
					LED_SR_Write(BIT1LO);		// Blower 2
					LED_SR_Write(BIT2LO);		// Blower 3
					LED_SR_Write(BIT3LO);		// Blower 4
					LED_SR_Write(BIT4LO); 	// Pingpong middle
					LED_SR_Write(BIT5LO);		// Pingpong top
					
					WelcomeIncrement = 0;
					ES_Timer_InitTimer(12, WELCOME_TIME);
					
					uint16_t CurrentPosition = TIMING_SERVO_LOW;
					//			ServoPWM(CurrentPosition,0,1);
					ES_Timer_InitTimer(14, 500);
					
					CurrentState = NoTOT;
				}
			}
			else if (ThisEvent.EventType == TOT_DETECTED) {
				printf("TOT_DETECTED in Waiting4NextGame\n\r");
				
				ServoPWM(1,0,0);
					AUDIO_SR_Write(BIT3HI);
					AUDIO_SR_Write(BIT4HI);
					AUDIO_SR_Write(BIT5HI);
					AUDIO_SR_Write(BIT6HI);
					AUDIO_SR_Write(BIT7HI);
					LED_SR_Write(BIT0LO);		// Blower 1
					LED_SR_Write(BIT1LO);		// Blower 2
					LED_SR_Write(BIT2LO);		// Blower 3
					LED_SR_Write(BIT3LO);		// Blower 4
					LED_SR_Write(BIT4LO); 	// Pingpong middle
					LED_SR_Write(BIT5LO);		// Pingpong top
					
				ES_Timer_InitTimer(12, WELCOME_TIME);
				
				WelcomeIncrement = 0;
				LED_SR_Write(BIT0LO);		// Blower 1
				LED_SR_Write(BIT1LO);		// Blower 2
				LED_SR_Write(BIT2LO);		// Blower 3
				LED_SR_Write(BIT3LO);		// Blower 4
				LED_SR_Write(BIT4LO); 	// Pingpong middle
				LED_SR_Write(BIT5LO);		// Pingpong top
							
//				ResetServo();
				ES_Event_t Event2Post;
				Event2Post.EventType = START_POTATO;
				ES_PostAll(Event2Post);
				AUDIO_SR_Write((uint8_t)BIT7LO);
				ES_Timer_InitTimer(10, 140);
				ES_Timer_InitTimer(11, IDLE_TIME);
				
				printf("POTATO started...\n\r");
				
		
				// Game timer is started by the servo
				
				CurrentState = YesTOT;
			break;
		}
			if (ThisEvent.EventType == TOT_REMOVED) {
				printf("TOT_REMOVED in Waiting4NextGame\n\r");
				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
				
				printf("POTATO ended...\n\r");
				//init timer for next game
				ES_Timer_InitTimer(1, 1000);
//				ReleaseTOT();
				CurrentState = Waiting4NextGame;
			}
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
	HWREG(SYSCTL_RCGCGPIO) |= BIT1HI; //Enable port B
	while ((HWREG(SYSCTL_PRGPIO) & BIT1HI) != BIT1HI){
	}		
	HWREG(GPIO_PORTB_BASE+GPIO_O_DEN) |= (BIT0HI | BIT1HI | BIT3HI); //Digital Enable pins 0 through 1
	HWREG(GPIO_PORTB_BASE+GPIO_O_DIR) &= (BIT0LO & BIT1LO & BIT3LO); //Set pins 0 and 1, and 3 to inputs
	HWREG(GPIO_PORTB_BASE+GPIO_O_PUR) |= BIT3HI;// set PB3 internal pull-up
	
	
	// Initialize the control line for the trapdoor servo
	ServoPinInit(2); //Need 2 servos, channel 0 will be TOT system, channel 1 will be Timer System [BOTH ARE GROUP 0]
	ServoPWM(1,0,0); //This is PB6
	
	//Set Audio ports high
	SR_Init();
	AUDIO_SR_Write(BIT3HI);
	AUDIO_SR_Write(BIT4HI);
	AUDIO_SR_Write(BIT5HI);
	AUDIO_SR_Write(BIT6HI);
	AUDIO_SR_Write(BIT7HI);
}

void ReleaseTOT( void) {
	ServoPWM(TIMING_SERVO_LOW,0,0);
	printf("Opening trapdoor to release TOT...\n\r");
	// Return trapdoor to its default position
}

uint8_t GetTOTState( void) {
  uint8_t InputState  = HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS));
  if (InputState & BIT0HI) {
    return 1;
  } else {
    return 0;
  }
}

uint8_t GetResetState( void) {
  uint8_t InputState  = HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS));
  if (InputState & BIT3HI) {
    return 1;
  } else {
    return 0;
  }
}

bool CheckTOTEvents( void){
  
  bool ReturnVal = false;
  uint8_t CurrentTOTState = GetTOTState();
	uint8_t CurrentResetState = GetResetState();
	
  if (CurrentTOTState != LastTOTState) {
      if (CurrentTOTState) {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = TOT_DETECTED;
        ES_PostAll(ThisEvent);
        ReturnVal = true;
      }
      else {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = TOT_REMOVED;
        ES_PostAll(ThisEvent);
        ReturnVal = true;
      }

      ReturnVal = true;
  }
	
	if (CurrentResetState == 0) {
			ES_Event_t ThisEvent;
			ThisEvent.EventType = RESET;
			ES_PostAll(ThisEvent);
      ReturnVal = true;
  }
	
  LastTOTState = CurrentTOTState;
  return ReturnVal;
}
