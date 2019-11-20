#define FAN_TEST

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
#include "ADMulti.h"
#include "Fan_test.h"


#define RESTRICT_POT 0 //max output value for pot
#define BASE_PULSE_WIDTH 30000 //max output value for pot
#define MAX_POT_OUTPUT 4095 //max output value for pot
#define PULSE_WIDTH_RANGE 10000 //max output value for pot
#define POT_CHANNEL 1 //max output value for pot
#define FAN_CHANNEL 2 //max output value for pot
#define FAN_GROUP 1 //max output value for pot
#define period 65535
//#define pot_baseline 4095 //calibration constant to reduce the pot_valtage value because its too high on its own
void Fan_test(uint8_t I)
{
	//initialize analog to digital conversion for potentiometer
//	ADC_MultiInit(POT_CHANNEL);//0-3.3V, PE0

	//array to store output from pot at pin PE0; Values between 0 and 4095 corresponding to low to high voltage output values
//	uint32_t Pot_ConversionResults[1];
	
	//Initialize PWM module on specific bits, Use PB4 for fan motor
	PWM_TIVA_Init(3); 

	// variable to hold value written to the Pot_ConversionResults array when read pot output
//	uint32_t pot_voltage;

	//variable to map to pot voltage output
	uint32_t duty_cycle;
			
	uint32_t pulse_width;
	//storing and assigning period value
//	uint32_t period = 1000; // PERIOD
			
PWM_TIVA_SetPeriod(period, FAN_GROUP);
	
 uint8_t i;
   
for(i = 1;i<100;i++)
	{
				//mapping duty cycle to pot output voltage
		duty_cycle = i;
			
//		pulse_width = abs(BASE_PULSE_WIDTH+ pot_voltage*(double)((double)(PULSE_WIDTH_RANGE)/(double)MAX_POT_OUTPUT));
			PWM_TIVA_SetDuty(duty_cycle,FAN_CHANNEL);
			printf("duty_cycle = %u\r\n",duty_cycle);
		
	while(kbhit()!=1)
////use kbhit() in while loop to test
	{

	}
	getchar(); 
//PWM_TIVA_SetDuty(0,FAN_CHANNEL);

	}
}


//TEST HARNESS
#ifdef FAN_TEST
#include "termio.h"

int main(void)
{

		
		TERMIO_Init();
			puts("\r\n In test harness for Module\r\n");
	
	Fan_test(1);

}
#endif
