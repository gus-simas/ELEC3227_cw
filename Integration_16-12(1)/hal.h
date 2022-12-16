#pragma once

#include "header.h"
#include <stdio.h>
#include <stdint.h>

#define INT_DDR DDRD
#define INT_PORT PORTD
#define INT_PIN PIND
#define INT0_BIT 2
#define INT1_BIT 3

void init_adc();
al_data_t read_adc();
void init_pwm();
void set_pwm(const al_data_t value);
void ext_interrupt_setup(int num);
void timeout_setup();
void device_setup();
void enable_timeout();
void disable_timeout();
bool read_timer_status();