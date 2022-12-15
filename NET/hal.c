#include "hal.h"
#include "uart.h"
#include "header.h"


#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void init_adc()
{
    ADMUX = _BV(REFS0); // AVCC reference and select ADC=0
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1); // Set prescaler to 64
    ADCSRA |= _BV(ADEN); // Enable ADC
}

al_data_t read_adc()
{
    uint16_t adc_val;
    al_data_t data;
    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
    adc_val = ADC;
    data.buf[0] = (uint8_t)(adc_val >> 8);
    data.buf[1] = (uint8_t)adc_val;
    data.len = 2;
    return data;
}

void init_pwm()
{ 
    DDRD |= _BV(PD5); // enable output driver for OC1A
    TCCR1A |= _BV(COM1A1);
    TCCR1A |= _BV(WGM12) | _BV(WGM11) | _BV(WGM10); // toggle OC1A on match
    TCCR1B |= _BV(CS11);
}


void set_pwm(al_data_t value)
{
    uint16_t pwm_val = (value.buf[0] << 8) | value.buf[1];
    OCR1A = pwm_val;
#if(DEBUG)
    char buf[10];
    put_str("PWM Value ");
    put_str(itoa(pwm_val, buf, 10));
    put_str("\r\n");
#endif
}


void ext_interrupt_setup(int num)
{
    switch (num)
    {
    case 0: 
        EIMSK |= _BV(INT0); // Enable INT0 interrupt mask
        EICRA |= _BV(ISC01) | _BV(ISC00); // enable the interrupt on falling edge
        break;
    case 1:
        EIMSK |= _BV(INT1); // Enable INT1 interrupt mask
        EICRA |= _BV(ISC11) | _BV(ISC10); // enable the interrupt on falling edge
        break;
    case 2:
        EIMSK |= _BV(INT2); // Enable INT2 interrupt mask
        EICRA |= _BV(ISC21) | _BV(ISC20); // enable the interrupt on falling edge
        break;
    }
}


void timeout_setup()
{
    TCCR2A |= _BV(COM2A1) | _BV(COM2A0);
    TCCR2B |= _BV(FOC2A) | _BV(CS22) | _BV(CS21) | _BV(CS20);
    OCR2A = 255; // timeout = 1s
}

void device_setup()
{
    init_uart0();    //init uart
    sei(); // enable interrupt

    /* For interrupt */
    INT_DDR = 0x00;
    INT_PORT |= _BV(INT0_BIT) | _BV(INT1_BIT); // set up pull-up resistors
    ext_interrupt_setup(0);
    ext_interrupt_setup(1);

    init_adc();
    init_pwm();
    timeout_setup();
}