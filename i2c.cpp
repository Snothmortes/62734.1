#include <avr/io.h>

#include "i2c.hpp"

void i2c_init() {
	DDRA|=(1<<DDA0);
	PORTA|=(1<<PA0);
	TWBR=18;
	TWSR&=0xFC;
	TWCR=0x05;
}

uint8_t i2c_start(char write_address) {
	uint8_t status;
	TWCR=(1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
	while(!(TWCR&(1<<TWINT)));
	status=TWSR&0xF8;
	if(status!=0x08)
	return 0;
	TWDR=write_address;
	TWCR=(1<<TWEN)|(1<<TWINT);
	while(!(TWCR&(1<<TWINT)));
	status=TWSR&0xF8;
	if(status==0x18)
	return 1;
	if(status==0x20) {
		
		return 2;
	}
	return 3;
}

uint8_t i2c_repeatedStart(char read_address) {
	uint8_t status;
	TWCR=(1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
	while(!(TWCR&(1<<TWINT)));
	status=TWSR&0xF8;
	if(status!=0x10)
	return 0;
	TWDR=read_address;
	TWCR=(1<<TWEN)|(1<<TWINT);
	while(!(TWCR&(1<<TWINT)));
	status=TWSR&0xF8;
	if(status==0x40)
	return 1;
	if(status==0x20)
	return 2;
	else
	return 3;
}

uint8_t i2c_write(char data) {
	uint8_t status;
	TWDR=data;
	TWCR=(1<<TWEN)|(1<<TWINT);
	while(!(TWCR&(1<<TWINT)));
	status=TWSR&0xF8;
	if(status==0x28)
	return 0;
	if(status==0x30)
	return 1;
	else
	return 2;
}

char i2c_readAck() {
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while(!(TWCR&(1<<TWINT)));
	return TWDR;
}

char i2c_readNack() {
	TWCR=(1<<TWEN)|(1<<TWINT);
	while(!(TWCR&(1<<TWINT)));
	return TWDR;
}

void i2c_stop() {
	TWCR=(1<<TWSTO)|(1<<TWINT)|(1<<TWEN);
	while(TWCR&(1<<TWSTO));
}