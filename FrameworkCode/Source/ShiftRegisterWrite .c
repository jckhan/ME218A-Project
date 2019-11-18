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
#define DATA GPIO_PIN_0

#define SCLK GPIO_PIN_1
#define SCLK_HI BIT1HI
#define SCLK_LO BIT1LO

#define RCLK GPIO_PIN_2
#define RCLK_LO BIT2LO
#define RCLK_HI BIT2HI

#define GET_MSB_IN_LSB(x) ((x & 0x80)>>7)

// an image of the last 8 bits written to the shift register
bool CheckHiLo(uint8_t data);

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

// Create your own function header comment
//uint8_t SR_GetCurrentRegister(void){
//  return LocalRegisterImage;
//}

bool CheckHiLo(uint8_t data){
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

// Create your own function header comment
	

void LED_SR_Write(uint8_t NewValue){
	static uint8_t LocalRegisterImage=0;
	//printf("First Register Image %d\n\r", LocalRegisterImage);
  if(CheckHiLo(NewValue)){
		LocalRegisterImage |= (NewValue);
	} else {
		LocalRegisterImage &= (NewValue);
	}
//	printf("Second Register Image %d\n\r", LocalRegisterImage);
	uint8_t loopValue = LocalRegisterImage;
// lower the register clock
	HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT4LO);
	for(int i=0; i < 8; i++){
		if(GET_MSB_IN_LSB(loopValue)){
			HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT2HI);
			loopValue = loopValue << 1;
	} else {
			HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT2LO);
			loopValue = loopValue << 1;
		}
		HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT3HI);
		HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT3LO);
}
	
// shift out the data while pulsing the serial clock  
// Isolate the MSB of NewValue, put it into the LSB position and output to port
// raise SCLK
// finish looping through bits in NewValue
// raise the register clock to latch the new data
  HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT4HI);
//printf("writing %d\n\r", LocalRegisterImage);
}

void AUDIO_SR_Write(uint8_t NewValue){
	static uint8_t LocalRegisterImage=0;
  if(CheckHiLo(NewValue)){
		LocalRegisterImage |= (NewValue);
	} else {
		LocalRegisterImage &= (NewValue);
	}
	uint8_t loopValue = LocalRegisterImage;
// lower the register clock
	HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT7LO);
	for(int i=0; i < 8; i++){
		if(GET_MSB_IN_LSB(loopValue)){
			HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT5HI);
			loopValue = loopValue << 1;
	} else {
			HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT5LO);
			loopValue = loopValue << 1;
		}
		HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT6HI);
		HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) &= (BIT6LO);
}
	
// shift out the data while pulsing the serial clock  
// Isolate the MSB of NewValue, put it into the LSB position and output to port
// raise SCLK
// finish looping through bits in NewValue
// raise the register clock to latch the new data
  HWREG(GPIO_PORTA_BASE + (GPIO_O_DATA + ALL_BITS)) |= (BIT7HI);
//printf("written %d\n\r",LocalRegisterImage);
}

