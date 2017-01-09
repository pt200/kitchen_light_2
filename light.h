#ifndef  __LIGHT_H__
#define  __LIGHT_H__


#include <avr/io.h>
#include <avr/wdt.h>
#include "def_types.h"

void light_init();
uint8 light_brightness_up();
uint8 light_brightness_down();
void light_on();
void light_off();


#endif
