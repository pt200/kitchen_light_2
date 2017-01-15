#ifndef  __CONFIG_H__
#define  __CONFIG_H__



#define CLK_US ( F_CPU / 1000000.0)


//__builtin_avr_delay_cycles
//__delay_cycles

#define wait_us( q)  __builtin_avr_delay_cycles( ( uint32_t)( ( q) * CLK_US + 0.50))
#define wait_us2( q, diff)  __builtin_avr_delay_cycles( ( uint32_t)( ( q) * CLK_US + 0.50) - diff)
#define wait_ms( q)  __builtin_avr_delay_cycles( ( uint32_t)( ( q) * 1000.0 * CLK_US))

////////////////////////////////////////////////////////////////////////////////
//#define LED_PIN  BIT0
#define LIGHT_PIN_DDR  SBIT( DDRA, 5)
#define LIGHT_PIN      SBIT( PORTA, 5)


#endif
