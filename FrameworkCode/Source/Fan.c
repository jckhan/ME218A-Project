//#define TEST

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


//#define RESTRICT_POT 2183 //max output value for pot
#define BASE_PULSE_WIDTH 900 //max output value for pot
#define MAX_POT_OUTPUT 2183 //max output value for pot
#define PULSE_WIDTH_RANGE 300 //max output value for pot
#define POT_CHANNEL 1 //max output value for pot
#define FAN_CHANNEL 2 //max output value for pot
#define FAN_GROUP 1 //max output value for pot
#define PERIOD 1000

void Fan(uint8_t i)
{
	if (i == 1)
	{
		//initialize analog to digital conversion for potentiometer
		ADC_MultiInit(POT_CHANNEL);//0-3.3V, PE0

		//array to store output from pot at pin PE0; Values between 0 and 4095 corresponding to low to high voltage output values
		uint32_t Pot_ConversionResults[1];

		//Initialize PWM module on specific bits, Use PB4 for fan motor
		PWM_TIVA_Init(3); 

		// variable to hold value written to the Pot_ConversionResults array when read pot output
		uint32_t pot_voltage;

		//variable to map to pot voltage output
		//	uint32_t duty_cycle;

		uint32_t pulse_width;
		//storing and assigning period value
		uint32_t period = PERIOD;

		ADC_MultiRead(Pot_ConversionResults);

		//STORE POT OUTPUT "VOLTAGE VALUE" IN VARIABLE FROM ARRAY
		pot_voltage = Pot_ConversionResults[0];

		PWM_TIVA_SetPeriod(period, FAN_GROUP);

		//pulse_width = abs((0.4*(pot_voltage)*(1000-1))/4095);
		pulse_width = abs( BASE_PULSE_WIDTH - (double)pot_voltage*(double)((double)(PULSE_WIDTH_RANGE)/(double)MAX_POT_OUTPUT) );
		//printf("PW: %d\r\n", pulse_width);

		PWM_TIVA_SetPulseWidth( pulse_width,FAN_CHANNEL);
	}

	else if(i == 0)
		{
			PWM_TIVA_SetPeriod(0, FAN_GROUP);
			PWM_TIVA_SetDuty(0,FAN_CHANNEL);
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
