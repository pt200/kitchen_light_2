#ifndef  __LIGHT_H__
#define  __LIGHT_H__


#include <avr/io.h>
#include <avr/wdt.h>
#include "def_types.h"


enum
{
	  LIGHT_OFF,
	  LIGHT_ON,
	  LIGHT_PWM
};

void light_init();
uint16_t get_light_sensor();
uint8_t get_ligth_state();
uint8_t light_brightness_up();
uint8_t light_brightness_down();
void light_on();
void light_off();


#endif
