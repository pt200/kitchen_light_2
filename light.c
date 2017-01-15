#include <avr/io.h>


#include "sbit.h"

#include "light.h"

#include "def_types.h"
#include "config.h"


#define ADCSRA_ADIF  SBIT( ADCSRA, 4) /*ADIF*/

int16 light_brightness = 0;


void light_init()
{
  LIGHT_PIN = 0;
  LIGHT_PIN_DDR = 1;

  TCCR1A = 0;
  light_brightness = 0;
}

uint16_t get_light_sensor()
{
  uint8_t q;
  uint16_t ret;

  ADMUX = 0x01; // ADC1( PA1) | Vref = VCC
  ADCSRB = 0x00; // Free mode
  ADCSRA = 0xE6; // start the conversion | ~9.3 ksps

  //Skip first sample
  ADCSRA_ADIF = 1;
  while( !ADCSRA_ADIF);
  ADCSRA_ADIF = 1;

  ret = 0;
  for( q = 0; q < 64; q++)
  {
    // ADSC is cleared when the conversion finishes
    while( !ADCSRA_ADIF);
    ADCSRA_ADIF = 1;

    ret += ADC;
  }
  return ret;
}

uint8_t get_ligth_state()
{
	if( light_brightness <= 0)
		return LIGHT_OFF;
	if( light_brightness >= 255)
		return LIGHT_ON;
	return LIGHT_PWM;
}

void light_on()
{
  while( light_brightness_up() < 255)
  {
    wdt_reset();
    wait_ms( 30);
  }
}

void light_off()
{
  while( light_brightness_down() > 0)
  {
    wdt_reset();
    wait_ms( 30);
  }
}

uint8_t light_brightness_up()
{
  light_brightness += ( light_brightness >> 2) + 1;

  if( light_brightness >= 255)
  {
    light_brightness = 255;
    LIGHT_PIN = 1;
    TCCR1A = 0;
    return 255;
  }else
  {
    LIGHT_PIN = 0;
    OCR1BL = light_brightness; // SET PWM
    TCCR1B = 0x0B;  // Fast PWM 8 bit + Prescaller :64
    TCCR1A = 0x21;  // Fast PWM 8 bit + OCRB:NORM
    return OCR1BL;
  }
}

uint8_t light_brightness_down()
{
  light_brightness -= ( light_brightness >> 4) + 1;

  if( light_brightness <= 0)
  {
    light_brightness = 0;
    LIGHT_PIN = 0;
    TCCR1A = 0;
    return 0;
  }else
  {
    LIGHT_PIN = 0;
    OCR1BL = light_brightness; // SET PWM
    TCCR1B = 0x0B;  // Fast PWM 8 bit + Prescaller :64
    TCCR1A = 0x21;  // Fast PWM 8 bit + OCRB:NORM
    return OCR1BL;
  }
}
