//#define FAN_TEST

#include <stdint.h>
#include <stdio.h>
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
#include "ADMulti.h"
#include "Fan_test.h"


#define RESTRICT_POT 0 //max output value for pot
#define BASE_PULSE_WIDTH 30000 //max output value for pot
#define MAX_POT_OUTPUT 4095 //max output value for pot
#define PULSE_WIDTH_RANGE 10000 //max output value for pot
#define POT_CHANNEL 1 //max output value for pot
#define FAN_CHANNEL 2 //max output value for pot
#define FAN_GROUP 1 //max output value for pot
#define period 1000
#define PULSE_BASE 250
//#define pot_baseline 4095 //calibration constant to reduce the pot_valtage value because its too high on its own
void Fan_test(uint8_t I)
{
	
	PWM_TIVA_Init(3); 

//	uint32_t duty_cycle;
			
	uint32_t pulse_width;

			
PWM_TIVA_SetPeriod(period, FAN_GROUP);
	
 uint32_t i;
   
for(i = 0;i<10000;i = i+10)
	{
				
//		duty_cycle = i;
			
//		pulse_width = abs(BASE_PULSE_WIDTH+ pot_voltage*(double)((double)(PULSE_WIDTH_RANGE)/(double)MAX_POT_OUTPUT));
//			PWM_TIVA_SetDuty(duty_cycle,FAN_CHANNEL);
//			printf("duty_cycle = %u\r\n",duty_cycle);
		
		int pulse_base;
		printf("Enter pulse base: ");
		scanf("%d", &pulse_base);
		
		pulse_width = pulse_base; //+i;
		PWM_TIVA_SetPulseWidth( pulse_width,FAN_CHANNEL);
		printf("pulse_width = %u\r\n",pulse_width);
		
	while(kbhit()!=1)

	{

	}
	getchar(); 


	}
}

#ifdef FAN_TEST
#include "termio.h"

int main(void)
{

	
		TERMIO_Init();
			puts("\r\n In test harness for Module\r\n");
	
	Fan_test(1);

}
#endif