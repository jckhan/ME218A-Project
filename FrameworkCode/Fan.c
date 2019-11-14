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
//	while(kbhit()!=1)
ADC_MultiInit(1);//0-3.3V, PE0
//Array to store POT input


uint32_t Pot_ConversionResults[1];
//Initialize PWM module on specific bits

	bool i;

i = PWM_TIVA_Init(7); //Jad already initialized this, Use PB4 for fan motor

//if we set to goup 1 will it change to freq for both PB4 and PB5?
//Not if you dont Initialize PB5 as PWM
//Need to experimentally find right frequency


//for spinner start until game end continuously read the below 
//(I guess the change in output voltage of the POT would be an event? 
//No, the tiva would just be cintinuously reading this throughout the game, 
//it wouldnt be an event, it would just be a while loop)


uint32_t pot_voltage;

//I can only have max 100 possible duty cycle values, 
//need to saegment 4093 into 100 tranches and let that correspond to different duty cycle values


uint32_t pulse_width;

//	Duty_Cycle = abs((pot_voltage)*(100/4093));
		

		
//	PWM_TIVA_SetDuty(duty_cycle[98],7);
	printf("while start\r\n");

while(kbhit()!=1)
//use kbhit() in while loop to test
{
	ADC_MultiRead(Pot_ConversionResults);
	
	pot_voltage = Pot_ConversionResults[0];
	printf("Pot Voltage = %u\r\n",pot_voltage);
	

	PWM_TIVA_SetPeriod( 63000, 3);
	pulse_width = abs((pot_voltage)*(63000/4093));
	
	PWM_TIVA_SetPulseWidth( pulse_width,6);
	
	
		
	printf("pulse_width = %u\r\n",pulse_width);
		
//	PWM_TIVA_SetDuty(Duty_Cycle,7);
}

}
#ifdef TEST
#include "termio.h"

int main(void)
{

		
		TERMIO_Init();
			puts("\r\n In test harness for Module\r\n");
	Fan();

}
#endif