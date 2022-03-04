#pragma once

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

#include "debounce.hpp"
#include "i2c.hpp"
#include "ssd1306.hpp"
#include <stdio.h>

#define BUTTON_MASK (1 << PE4)
#define BUTTON_PIN PINE
#define BUTTON_PORT PORTE

#define LED_MASK (1 << PJ1)
#define LED_PORT PORTJ
#define LED_DDR DDRJ

void init(void);