#define TEST

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
#include "Fan.h"


void Fan(void);

void Fan(void)
{
	//initialize analog to digital conversion for potentiometer
	ADC_MultiInit(1);//0-3.3V, PE0

	//array to store output from pot at pin PE0; Values between 0 and 4095 corresponding to low to high voltage output values
	uint32_t Pot_ConversionResults[1];
	
	//Initialize PWM module on specific bits, Use PB4 for fan motor
	PWM_TIVA_Init(3); 

	// variable to hold value written to the Pot_ConversionResults array when read pot output
	uint32_t pot_voltage;

	//variable to map to pot voltage output
	uint32_t duty_cycle;
		
	//storing and assigning period value
	uint32_t period = 1000;

while(kbhit()!=1)
//use kbhit() in while loop to test
{
		//READ VOLTAGE AT POT OUTPUT
		ADC_MultiRead(Pot_ConversionResults);
	
		//STORE POT OUTPUT "VOLTAGE VALUE" IN VARIABLE FROM ARRAY
		pot_voltage = Pot_ConversionResults[0];
	
		//SET FREQUENCY
		PWM_TIVA_SetPeriod(period, 1);
	
		//mapping duty cycle to pot output voltage
		duty_cycle = abs((pot_voltage*(100-1))/4095);
	
		//outputting duty cycle value to tiva
		PWM_TIVA_SetDuty(duty_cycle,2);
}

}

//TEST HARNESS
#ifdef TEST
#include "termio.h"

int main(void)
{

		
		TERMIO_Init();
			puts("\r\n In test harness for Module\r\n");
	Fan();

}
#endif