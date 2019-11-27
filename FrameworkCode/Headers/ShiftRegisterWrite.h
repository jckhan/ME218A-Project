/****************************************************************************

  Header file for the shift register write service

 ****************************************************************************/

#ifndef ShiftRegisterWrite_H
#define ShiftRegisterWrite_H

// Public Function Prototypes
void SR_Init(void);
uint8_t SR_GetCurrentRegister(void);
void LED_SR_Write(uint8_t NewValue);
void AUDIO_SR_Write(uint8_t NewValue);

void StopLEDs(void);
void StopAudio(void);

#endif
