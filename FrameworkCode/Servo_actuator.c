#include <stdint.h>
#include <stdbool.h>

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

#define TowerProLimLow 650 //Lower Limit of pulse in ticks (1 tick = 0.8 microseconds)
#define TowerProLimHigh 3000 //Upper Limit of pulse in ticks (1 tick = 0.8 microseconds)
#define clrScrn() printf("\x1b[2J")
void ServoPinInit(uint8_t HowMany){ //Takes input how many PWM channels we need (Each channel activates two pins [called 'groups'], this is a hardware limitation)
	PWM_TIVA_Init(HowMany); //Max 16 channels, which is 8 groups, and so only 8 unique PWM PERIODS, but can have 16 unique pulse widths
} 

void ServoPWM(uint16_t des_angle, uint8_t group, uint8_t channel){ //Desired angle is between 0 and 180 // Channel and group both start from 0
	uint16_t reqPeriod = 25000;
	uint16_t PulseWidth = TowerProLimLow + (((double)(des_angle)/180)*(TowerProLimHigh - TowerProLimLow));
	PWM_TIVA_SetPeriod( reqPeriod, group);
	PWM_TIVA_SetPulseWidth( PulseWidth, channel);
}


