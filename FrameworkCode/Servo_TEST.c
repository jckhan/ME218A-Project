#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "inc/hw_sysctl.h"

#include "ES_Configure.h"
#include "ES_Framework.h"
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

#include "PWM16Tiva.h"

#define TowerProLimLow 1250 //Lower Limit of pulse in ticks (1 tick = 0.8 microseconds)
#define TowerProLimHigh 2500 //Upper Limit of pulse in ticks (1 tick = 0.8 microseconds)
#define clrScrn() printf("\x1b[2J")

int main(void){
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
      | SYSCTL_XTAL_16MHZ);
  TERMIO_Init();
  clrScrn();
	
	printf("running main\r\n");
	HWREG(SYSCTL_RCGCGPIO) |= BIT1HI;
	while((HWREG(SYSCTL_PRGPIO) & BIT1HI) != BIT1HI){
	}
	HWREG(GPIO_PORTB_BASE +GPIO_O_DEN) |=  BIT6HI;
	HWREG(GPIO_PORTB_BASE +GPIO_O_DIR) |=  BIT6HI;
  // Set the clock to run at 40MhZ using the PLL and 16MHz external crystal
	printf("Testing Servo \r\n");
	uint8_t HowMany = 1; // How many PWM channels we need
	PWM_TIVA_Init(HowMany);
	while(!kbhit()){
	}
	printf("keyboard hit \r\n");
	uint16_t NewPW = 2450; //Set width of pulse in ticks (1 tick = 0.8 microseconds)
	uint8_t channel = 0;
	uint16_t reqPeriod = 25000; //Set width of PWM Period in ticks
	uint8_t group = 0;
	PWM_TIVA_SetPeriod( reqPeriod, group);
	PWM_TIVA_SetPulseWidth( NewPW, channel);
	return 0;
}

