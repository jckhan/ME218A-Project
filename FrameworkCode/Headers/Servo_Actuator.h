#ifndef _SERVO_ACTUATOR_H
#define _SERVO_ACTUATOR_H

#include <stdint.h>
#include <stdbool.h>

void ServoPinInit(uint8_t HowMany); //Initialize Pin as Servo Pin
void ServoPWM(uint16_t des_angle, uint8_t group, uint8_t channel); // Set position

#endif
