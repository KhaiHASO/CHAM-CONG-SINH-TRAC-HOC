#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
void joystick_init(ADC_HandleTypeDef* hadc);
uint8_t get_joystick_position();
char* get_joystick_position_string(void);

#endif /* __JOYSTICK_H */
