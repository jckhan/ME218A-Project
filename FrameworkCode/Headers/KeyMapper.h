/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServTemplate_H
#define ServTemplate_H

#include "ES_Types.h"
#include "ES_Events.h"

// Public Function Prototypes

bool InitKeyMapper(uint8_t Priority);
bool PostKeyMapper(ES_Event_t ThisEvent);
ES_Event_t RunKeyMapper(ES_Event_t ThisEvent);

#endif /* ServTemplate_H */

