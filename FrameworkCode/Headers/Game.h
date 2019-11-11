/****************************************************************************

  Header file for template Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef Game_H
#define Game_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  GameStandby,
	Level1,
	Level2,
	Level3,
	PingPong_Completed
}GameState_t;

// Public Function Prototypes

bool InitGame(uint8_t Priority);
bool PostGame(ES_Event_t ThisEvent);
ES_Event_t RunGame(ES_Event_t ThisEvent);
GameState_t QueryGame(void);

void GameInitialize( void);
void LEDMiddle(uint8_t Setting);
void LEDTop(uint8_t Setting);
void LEDSuccess(uint8_t Setting);

#endif 

