#pragma once

#define F_CPU 16000000UL

#include "main.hpp"

#define SCL_CLK 100000
#define BITRATE = (TWSR)((F_CPU/SCL_CLK) - 16)/(2*pow(4, (TWSR & ((1 << TWPS0) | (1 << TWPS1)))))

void i2c_init();
void i2c_stop();

uint8_t i2c_start(char writeAddress);
uint8_t i2c_repeatedStart(char readAddress);
uint8_t i2c_write(char data);

char i2c_readAck();
char i2c_readNack();