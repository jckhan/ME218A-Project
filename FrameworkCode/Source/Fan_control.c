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
void Fan_pot_control(void);

uint32_t Pot_ConversionResults[1];
//Initialize PWM module on specific bits
 //Jad already initialized this, Use PC4 for fan motor


uint32_t pot_voltage;// TO READ THE VOLTAGE AT THE POT OUTPUT AND MAP IT TO THE 0-4093 RANGE

uint32_t pulse_width;//TO STORE PULSE WIDTH

uint32_t duty_cycle;

//void Fan_pot_control(void)
//{
////	while(kbhit()!=1)
//ADC_MultiInit(1);//0-3.3V, PE0
////Array to store POT input

//		
////	PWM_TIVA_SetDuty(duty_cycle[98],7);
////	printf("while start\r\n");

////use kbhit() in while loop to test
//	//READ VOLTAGE AT POT OUTPUT
//	ADC_MultiRead(Pot_ConversionResults);
//	
//	//STORE POT OUTPUT "VOLTAGE VALUE" IN VARIABLE FROM ARRAY
//	pot_voltage = Pot_ConversionResults[0];
////SET FREQUENCY
//	PWM_TIVA_SetPeriod( 63000, 1);
//	
//	//MAPPING PULSE WIDTH TO POT OUTPUT VOLTAGE
//	pulse_width = abs((pot_voltage)*(63000/4093));
//	
//	//ASSIGNING PULSE WIDTH VALUE TO OUTPUT TO GATE OF MOSFET AND CONTROL FAN SPEED
//	PWM_TIVA_SetPulseWidth( pulse_width,2);
//}

void Fan_control(uint8_t duty_cycle)
{
	//initialize adc for pot
	ADC_MultiInit(1);//0-3.3V, PE0
	
	//initialize pwm for fan at 
	PWM_TIVA_Init(3); //Jad already initialized this, Use PC4 for fan motor
		

	//READ VOLTAGE AT POT OUTPUT
	ADC_MultiRead(Pot_ConversionResults);
	
	//STORE POT OUTPUT "VOLTAGE VALUE" IN VARIABLE FROM ARRAY
	pot_voltage = Pot_ConversionResults[0];
	
	//SET FREQUENCY: max was found to be optimal
	PWM_TIVA_SetPeriod( 63000, 1);
	
	//MAPPING PULSE WIDTH TO POT OUTPUT VOLTAGE
	//pulse_width = abs((pot_voltage)*(63000/4093));
	
	duty_cycle = abs((pot_voltage)*(100/4093));
	
	//ASSIGNING PULSE WIDTH VALUE TO OUTPUT TO GATE OF MOSFET AND CONTROL FAN SPEED
	//PWM_TIVA_SetPulseWidth( pulse_width,2);
	
	//ASSIGNING PULSE WIDTH VALUE TO OUTPUT TO GATE OF MOSFET AND CONTROL FAN SPEED
	PWM_TIVA_SetDuty( duty_cycle, 2);
	
	
//	//ASSIGNING PULSE WIDTH VALUE TO OUTPUT TO GATE OF MOSFET AND CONTROL FAN SPEED
//	PWM_TIVA_SetPulseWidth( pulse_width,2);
}
