/****************************************************************************
 Module
   TOT.c

 Revision
   1.0.1

 Description
   This is the state machine code to control the TOT servo and to issue events
	 to the other state machines related to TOT interactions

 Notes

 History
 When           Who     	
 -------------- ---     	
 11/27/19 10:51 Jack Han  
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "TOT.h"
#include "Servo_Actuator.h"
#include "PWM16Tiva.h"
#include "ShiftRegisterWrite.h"
#include "ES_Port.h"
#include "termio.h"
#include "EnablePA25_PB23_PD7_PF0.h"

// the headers to access the GPIO subsystem
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_pwm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"

// the headers to access the TivaWare Library
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"

/*----------------------------- Module Defines ----------------------------*/
#define TOT_HI BIT0HI
#define TOT_LO BIT0LO
#define SERVO_HI BIT1HI
#define SERVO_LO BIT1LO
#define BUTTON_HI BIT3HI
#define BUTTON_LO BIT3LO

#define NEXTGAMETIME 4500
#define AUDIO_TIME 140
#define IDLE_TIME 30000
#define WELCOME_TIME 1000
#define SERVO_RESET_TIME 500

#define TRAPDOOR_OPEN 180
#define TRAPDOOR_CLOSED 1
#define TIMING_SERVO_LOW 180

#define WELCOME_LO BIT7LO
#define WELCOME_HI BIT7HI
#define SUCCESS_LO BIT6LO
#define SUCCESS_HI BIT6HI
#define GOODBYE_LO BIT5LO
#define GOODBYE_HI BIT5HI

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
static void TOTInitialize( void);
static void ReleaseTOT( void);
static uint8_t GetTOTState( void);

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
     InitTOT

 Parameters
     uint8_t : the priority of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
		 Jack Han, 11/27/19, 10:55
****************************************************************************/
bool InitTOT(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
	
	TOTInitialize();
	
  // put us into the default state
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
     PostTOT

 Parameters
     EF_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
		 Jack Han, 11/27/19, 10:55
****************************************************************************/
bool PostTOT(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTOT

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
	 Jack Han, 11/27/19, 10:55
****************************************************************************/
ES_Event_t RunTOT(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	// First check if there has been no activity for the idle time
	if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == IDLE_TIMER) {
		ES_Event_t Event2Post;
		Event2Post.EventType = RESET;
		ES_PostAll(Event2Post);
	} else {
		ES_Timer_InitTimer(IDLE_TIMER, IDLE_TIME);
	}

  switch (CurrentState)
  {
		case NoTOT:
		{
			if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
      {
				WelcomeIncrement = 0;
				ES_Timer_InitTimer(WELCOME_TIMER, WELCOME_TIME);
        CurrentState = NoTOT;
      }
			else if (ThisEvent.EventType == TOT_DETECTED) {
				WelcomeIncrement = 0;
				StopLEDs();
				
				ES_Event_t Event2Post;
				Event2Post.EventType = START_POTATO;
				ES_PostAll(Event2Post);
				
				// Play the welcome message
				AUDIO_SR_Write(WELCOME_LO);
				ES_Timer_InitTimer(AUDIO_TIMER, AUDIO_TIME);
				ES_Timer_InitTimer(IDLE_TIMER, IDLE_TIME);

				CurrentState = YesTOT;
			}
			// Welcome Mode sequence
			else if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == 12) {
				uint8_t Remainder = WelcomeIncrement % 9;
				
				switch (Remainder) {
					case 0:
					{
						StopLEDs();
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
						StopLEDs();
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
				ES_Timer_InitTimer(WELCOME_TIMER, WELCOME_TIME);
			}
			else if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == SERVO_RESET_TIMER){
				// Reset the position of the system timer servo
				ServoPWM(TIMING_SERVO_LOW,0,1);
			}
			break;
		}
		
		case YesTOT:
		{
			if (ThisEvent.EventType == TOT_REMOVED) {		
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
				
				// Init timer for next game
				ES_Timer_InitTimer(TOT_TIMER, 1000);
				CurrentState = Waiting4NextGame;
			}
			else if (ThisEvent.EventType == ES_TIMEOUT) {
				if(ThisEvent.EventParam == AUDIO_TIMER){
					// Turn off all audio
					StopAudio();
				}
				else if(ThisEvent.EventParam == TOT_TIMER){
					ES_Event_t Event2Post;
					Event2Post.EventType = END_POTATO;
					ES_PostAll(Event2Post);
				
				  // Open trapdoor to release TOT
					ReleaseTOT();
					ES_Timer_InitTimer(TOT_TIMER, NEXTGAMETIME);
					CurrentState = Waiting4NextGame;
				}
			}
			else if (ThisEvent.EventType == GAME_COMPLETED) {				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
								
				// Open trapdoor to release TOT
				ReleaseTOT();
				
				ES_Timer_InitTimer(TOT_TIMER, NEXTGAMETIME);
				CurrentState = Waiting4NextGame;
			}
			else if (ThisEvent.EventType == RESET) {				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);
				
				// Open trapdoor to release TOT
				ReleaseTOT();
				ES_Timer_InitTimer(TOT_TIMER, NEXTGAMETIME);
				CurrentState = Waiting4NextGame;
			}
			break;
		}
		case Waiting4NextGame:
		{
			if(ThisEvent.EventType == ES_TIMEOUT){
				if(ThisEvent.EventParam == AUDIO_TIMER){
					AUDIO_SR_Write(GOODBYE_HI);
					AUDIO_SR_Write(SUCCESS_HI);
					
					AUDIO_SR_Write(GOODBYE_LO);
					ES_Timer_InitTimer(AUDIO_TIMER, AUDIO_TIME);
					
					CurrentState = Waiting4NextGame;
				}
				else if (ThisEvent.EventParam == TOT_TIMER){
					ServoPWM(TRAPDOOR_CLOSED,0,0);
					StopAudio();
					StopLEDs();
					
					WelcomeIncrement = 0;
					ES_Timer_InitTimer(WELCOME_TIMER, WELCOME_TIME);
					
					ES_Timer_InitTimer(SERVO_RESET_TIMER, SERVO_RESET_TIME);
					
					CurrentState = NoTOT;
				}
			}
			else if (ThisEvent.EventType == TOT_DETECTED) {				
				ServoPWM(TRAPDOOR_CLOSED,0,0);
				StopAudio();
				StopLEDs();
					
				ES_Timer_InitTimer(WELCOME_TIMER, WELCOME_TIME);
				
				WelcomeIncrement = 0;
							
				ES_Event_t Event2Post;
				Event2Post.EventType = START_POTATO;
				ES_PostAll(Event2Post);
				
				AUDIO_SR_Write(WELCOME_LO);
				ES_Timer_InitTimer(AUDIO_TIMER, AUDIO_TIME);
				ES_Timer_InitTimer(IDLE_TIMER, IDLE_TIME);				
		
				CurrentState = YesTOT;
			}
			else if (ThisEvent.EventType == TOT_REMOVED) {				
				ES_Event_t Event2Post;
				Event2Post.EventType = END_POTATO;
				ES_PostAll(Event2Post);

				//init timer for next game
				ES_Timer_InitTimer(TOT_TIMER, 1000);

				CurrentState = Waiting4NextGame;
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
		 Jack Han, 11/27/19, 10:55
****************************************************************************/
TOTState_t QueryTOT(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/

//Initialization sequence for the TOT state machine
void TOTInitialize( void) {
	// Initialize a data line as the input for the coin sensor
	HWREG(GPIO_PORTB_BASE+GPIO_O_DEN) |= (TOT_HI | SERVO_HI | BUTTON_HI); //Digital Enable pins 0 through 1
	HWREG(GPIO_PORTB_BASE+GPIO_O_DIR) &= (TOT_LO & SERVO_LO & BUTTON_LO); //Set pins 0 and 1 to inputs
	HWREG(GPIO_PORTB_BASE+GPIO_O_PUR) |= BUTTON_HI;// set PB3 internal pull-up

	
	// Initialize the control line for the trapdoor servo
	ServoPinInit(2); //Need 2 servos, channel 0 will be TOT system, channel 1 will be Timer System [BOTH ARE GROUP 0]
	ServoPWM(TRAPDOOR_CLOSED,0,0); //This is PB6
	
	//Set Audio ports high
	SR_Init();
	StopAudio();
}

//Sets the trapdoor servo to the open position
void ReleaseTOT( void) {
	ServoPWM(TRAPDOOR_OPEN,0,0);
}


//Checks the state of the coin sensor and returns 1 if a TOT is present, 0 if
//the TOT is absent
uint8_t GetTOTState( void) {
  uint8_t InputState  = HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS));
  if (InputState & TOT_HI) {
    return 1;
  } else {
    return 0;
  }
}

// Checks the state of the reset button
uint8_t GetResetState( void) {
  uint8_t InputState  = HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS));
  if (InputState & BUTTON_HI) {
    return 1;
  } else {
    return 0;
  }
}

// Event checker for the coin sensor; return true if the state has changed
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
	
	// Button event checker
	if (CurrentResetState == 0) {
			ES_Event_t ThisEvent;
			ThisEvent.EventType = RESET;
			ES_PostAll(ThisEvent);
      ReturnVal = true;
  }
	
  LastTOTState = CurrentTOTState;
  return ReturnVal;
}
