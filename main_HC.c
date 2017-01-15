#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include "sbit.h"

#include "light.h"

#include "def_types.h"
#include "config.h"


//#define HS_TX_PIN1_BIT   1
//#define HS_TX_PIN2_BIT   2
#define HS_TX_PIN1_DDR   SBIT( DDRA, 6)
#define HS_TX_PIN2_DDR   SBIT( DDRA, 7)
#define HS_TX_PIN1       SBIT( PORTA, 6)
#define HS_TX_PIN2       SBIT( PORTA, 7)
#define HS_SET_TX_PINS( p1, p2) /* 8 clk */ \
{ \
  	  HS_TX_PIN1 = 0; /* reset cap */ \
  	  HS_TX_PIN2 = 0; /* reset cap */ \
	  wait_us( 0.25); \
  	  HS_TX_PIN1 = p1; \
  	  HS_TX_PIN2 = p2; \
}
//#define HS_TX_PINS( p1, p2, add_timeout)

#define HS_ECHO_PIN  BIT4
#define HS_ECHO_AIN  2

#define HS_TX_FREQ   40000l


#define HS_ECHO_SKIP_SAMPLES  15
#define HS_ECHO_SENS_SAMPLES  45

#define HS_NO_ACK  0xFF

#define LIGHT_DELAY_OFF 100

#define ADCSRA_ADIF  SBIT( ADCSRA, 4) /*ADIF*/







// US sensor button
uint16 _btn;
#define isBTN()   ( _btn < ( 15 * HS_ECHO_SKIP_SAMPLES))


uint16 EEMEM p_amb_light_trig = 150 * 64;








void HS_init()
{
  PORTB &= ~HS_ECHO_PIN;

  HS_TX_PIN1 = 0;
  HS_TX_PIN2 = 0;
  HS_TX_PIN1_DDR = 1;
  HS_TX_PIN2_DDR = 1;
}

uint8 HS_SCAN()
{
  uint16 ref_level = 0;
  uint16 _adc;
  uint8 q;
  uint8 qc;

  ADMUX = 0x00; // ADC0( PA0) | Vref = VCC
  ADCSRB = 0x00; // Free mode
  ADCSRA = 0xE6; // start the conversion | ~9.3 ksps

  //Skip first sample
  ADCSRA_ADIF = 1;
  while( !ADCSRA_ADIF);
  ADCSRA_ADIF = 1;

  ref_level = 0;
  for( q = 0; q < 16; q++)
  {
    // ADSC is cleared when the conversion finishes
    while( !ADCSRA_ADIF);
    ADCSRA_ADIF = 1;

    ref_level += ADC;
  }
  ref_level >>= 4;
  ref_level += 15;

  //***************************************************************************
  // 8 SCAN PULSEs
  HS_SET_TX_PINS( 0, 0);
  wait_us( 10);
  for( uint8 q = 0; q < 8; q++)
  {
	  HS_SET_TX_PINS( 1, 0);
  	  wait_us2( 1000000.0 / HS_TX_FREQ / 2, 8);// -8 clk = port io
  	  HS_SET_TX_PINS( 0, 1);
  	  wait_us2( 1000000.0 / HS_TX_FREQ / 2, 8 + 3);// -8 + 3 clk = port io + for routine
  }
  // Send 1 clk anti phase || Active damping
  wait_us2( 1000000.0 / HS_TX_FREQ / 2, 8);// -8 clk = port io + for routine
  HS_SET_TX_PINS( 1, 0);
  wait_us2( 1000000.0 / HS_TX_FREQ / 2, 8);// -8 clk = port io
  HS_SET_TX_PINS( 0, 0);
  //***************************************************************************


  //Skip N samples + BTN logic
  _btn = 0;
  for( q = 0; q < HS_ECHO_SKIP_SAMPLES; q++)
  {
    // ADSC is cleared when the conversion finishes
    while( !ADCSRA_ADIF);
    ADCSRA_ADIF = 1;

    _adc = ADC;
    if( _adc > ref_level)
      _btn += ( _adc - ref_level);
  }

  // Sampling ECHO
  qc = 0;
  for( q = 0; q < HS_ECHO_SENS_SAMPLES; q++)
  {
    // ADSC is cleared when the conversion finishes
    while( !ADCSRA_ADIF);
    ADCSRA_ADIF = 1;
    
    if( ADC > ref_level)
      qc++;
  }
  return qc;
}



//main Func
int main()
{
  uint8 light_off_delay = 0;
  uint8 ll = 0;
  uint16 amb_light = 0;
  uint16_t amb_light_trig;

  wdt_reset();
  /* Start timed sequence */
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  /* Set new prescaler(time-out) value = 1024K cycles (~8 s) */
  WDTCSR = (1<<WDE) | 0x21;
  

  DIDR0 = 0xFF; // Disable all inputs


  light_init();
  HS_init();
  amb_light_trig = eeprom_read_word( &p_amb_light_trig);

  
  while( 1)
  {
    wdt_reset();

/*
HS_SCAN();
if( isBTN())
light_brightness_up();
else
light_brightness_down();
*/
    if( get_ligth_state() == LIGHT_OFF)
      amb_light = get_light_sensor();

    if( ( HS_SCAN() > 7) || isBTN()) // !!! CONST !!!
    {
        if( amb_light < amb_light_trig)
        {
          light_off_delay = LIGHT_DELAY_OFF;
          light_on();
        }
    }
    else
    {
      if( light_off_delay == 0)
        ll = light_brightness_down();
      else
        light_off_delay--;
    }

    if( isBTN())
    {
    	if( get_ligth_state() == LIGHT_OFF)
    	{
    		amb_light_trig = amb_light + 10 * 64;
//    		eeprom_write_word( &p_amb_light_trig, amb_light_trig);
    	}
    	else
        	if( get_ligth_state() == LIGHT_ON)
        	{
        		amb_light_trig = amb_light - 10 * 64;
//        		eeprom_write_word( &p_amb_light_trig, amb_light_trig);
        		light_off_delay = 0; // Force light off
                light_off();
        	}
    }

   
    
    if( light_off_delay == LIGHT_DELAY_OFF)
      wait_ms( 1000);
    else
    {
      if( ll > 0)
        wait_ms( 30);
      else
        wait_ms( 100);
    }
  }
  return 0;
}
