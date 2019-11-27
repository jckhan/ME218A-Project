/****************************************************************************
 Module
   ShiftRegisterWrite.c

 Revision
   1.0.1

 Description
   This module acts as the low level interface to a write only shift register.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 10/11/15 19:55 jec     first pass
 
****************************************************************************/
// the common headers for C99 types 
#include <stdint.h>
#include <stdbool.h>

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
#include "ES_Port.h"

#include "BITDEFS.H"

// readability defines
#define L_SCLK_HI BIT3HI
#define L_SCLK_LO BIT3LO
#define A_SCLK_HI BIT6HI
#define A_SCLK_LO BIT6LO

#define L_RCLK_LO BIT4LO
#define L_RCLK_HI BIT4HI
#define A_RCLK_LO BIT7LO
#define A_RCLK_HI BIT7HI

#define GET_MSB_IN_LSB(x) ((x & 0x80)>>7)

// an image of the last 8 bits written to the shift register
static bool CheckHiLo(uint8_t data);

// Create your own function header comment
void SR_Init(void){

	HWREG(GPIO_PORTA_BASE+GPIO_O_DEN) |= (BIT2HI | BIT3HI | BIT4HI);
	HWREG(GPIO_PORTA_BASE+GPIO_O_DIR) |= (BIT2HI | BIT3HI | BIT4HI);
	HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT2LO & BIT3LO);
	HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT4HI);
  // peripheral to be ready and setting the direction
  // of PB0, PB1 & PB2 to output
  
  // start with the data & sclk lines low and the RCLK line high
	HWREG(GPIO_PORTA_BASE+GPIO_O_DEN) |= (BIT5HI | BIT6HI | BIT7HI);
	HWREG(GPIO_PORTA_BASE+GPIO_O_DIR) |= (BIT5HI | BIT6HI | BIT7HI);
	HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT6LO);
	HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT5HI | BIT7HI);
}

// Checker to determine if the data written in is a BITXHI value or a BITXLO value
bool CheckHiLo(uint8_t data){
	// Sample 3 different bits; if two bits are hi, then the value is lo, and vice versa
	if((!(BIT1HI & data)) && (!(BIT3HI & data))){
		return true;
	} else if(((BIT1HI & data)) && (!(BIT3HI & data))){
		if((!(BIT5HI & data))){
			return true;
		} else {
			return false;
		}
	} else if((!(BIT1HI & data)) && ((BIT3HI & data))){
		if((!(BIT5HI & data))){
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}

}

// Shift register write function for LEDs
void LED_SR_Write(uint8_t NewValue){
	static uint8_t LocalRegisterImage=0;

  if(CheckHiLo(NewValue)){
		LocalRegisterImage |= (NewValue);
	} else {
		LocalRegisterImage &= (NewValue);
	}
	uint8_t loopValue = LocalRegisterImage;
	
	// lower the register clock
	HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (L_RCLK_LO);
	
	// shift out the data while pulsing the serial clock  
	for(int i=0; i < 8; i++){
		// Isolate the MSB of NewValue, put it into the LSB position and output to port
		if(GET_MSB_IN_LSB(loopValue)){
			HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT2HI);
			loopValue = loopValue << 1;
		} else {
			HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT2LO);
			loopValue = loopValue << 1;
		}
		// raise SCLK
		HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (L_SCLK_HI);
		// lower SCLK
		HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (L_SCLK_LO);
	}

	// raise the register clock to latch the new data
  HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (L_RCLK_HI);
}

// Shift register write for audio
void AUDIO_SR_Write(uint8_t NewValue){
	static uint8_t LocalRegisterImage=0;
	
  if(CheckHiLo(NewValue)){
		LocalRegisterImage |= (NewValue);
	} else {
		LocalRegisterImage &= (NewValue);
	}
	uint8_t loopValue = LocalRegisterImage;
	
	// lower the register clock
	HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (A_RCLK_LO);
	
	for(int i=0; i < 8; i++){
		if(GET_MSB_IN_LSB(loopValue)){
			HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT5HI);
			loopValue = loopValue << 1;
		} else {
			HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT5LO);
			loopValue = loopValue << 1;
		}
		HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (A_SCLK_HI);
		HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (A_SCLK_LO);
	}
	
	// raise the register clock
  HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (A_RCLK_HI);
}

// Turns off all LEDs
void StopLEDs(void) {
	LED_SR_Write(BIT0LO);		// Blower 1
	LED_SR_Write(BIT1LO);		// Blower 2
	LED_SR_Write(BIT2LO);		// Blower 3
	LED_SR_Write(BIT3LO);		// Blower 4
	LED_SR_Write(BIT4LO); 	// Pingpong middle
	LED_SR_Write(BIT5LO);		// Pingpong top
}

// Stops playing all audio
void StopAudio(void) {
	AUDIO_SR_Write(BIT3HI);		// Level up
	AUDIO_SR_Write(BIT4HI);		// Pop
	AUDIO_SR_Write(BIT5HI);		// Goodbye
	AUDIO_SR_Write(BIT6HI);		// Success
	AUDIO_SR_Write(BIT7HI);		// Welcome
}
