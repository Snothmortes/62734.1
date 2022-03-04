#include <math.h>
#include <string.h>
#include "i2c.hpp"
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "ssd1306.hpp"
#include "data.hpp"
#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }
#define _vccstate 1

uint8_t _i2c_address = 0x78;

void ssd1306_command(uint8_t c) {
	uint8_t control = 0x00;
	i2c_start(_i2c_address);
	i2c_write(control);
	i2c_write(c);
	i2c_stop();
}

void ssd1306_data(uint8_t c) {
	i2c_start(_i2c_address);
	i2c_write(_i2c_address);
	i2c_write(0X40);
	i2c_write(c);
	i2c_stop();
}

void setColAddress() {
	ssd1306_command(SSD1306_COLUMNADDR);
	ssd1306_command(0);
	ssd1306_command(SSD1306_LCDWIDTH-1);
}

void setPageAddress() {
	ssd1306_command(SSD1306_PAGEADDR);
	ssd1306_command(0);
	ssd1306_command((SSD1306_LCDHEIGHT/8)-1);
}

void initializeDisplay() {
	ssd1306_command(SSD1306_DISPLAYOFF);
	ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);
	ssd1306_command(0x80);
	ssd1306_command(SSD1306_SETMULTIPLEX);
	ssd1306_command(0x3F);
	ssd1306_command(SSD1306_SETDISPLAYOFFSET);
	ssd1306_command(0x0);
	ssd1306_command(SSD1306_SETSTARTLINE);
	ssd1306_command(SSD1306_CHARGEPUMP);
	ssd1306_command(0x14);
	ssd1306_command(SSD1306_MEMORYMODE);
	ssd1306_command(0x00);
	ssd1306_command(SSD1306_SEGREMAP | 0x1);
	ssd1306_command(SSD1306_COMSCANDEC);
	ssd1306_command(SSD1306_SETCOMPINS);
	ssd1306_command(0x12);
	ssd1306_command(SSD1306_SETCONTRAST);
	ssd1306_command(0xCF);
	ssd1306_command(SSD1306_SETPRECHARGE);
	ssd1306_command(0xF1);
	ssd1306_command(SSD1306_SETVCOMDETECT);
	ssd1306_command(0x40);
	ssd1306_command(SSD1306_DISPLAYALLON_RESUME);
	ssd1306_command(SSD1306_NORMALDISPLAY);
	ssd1306_command(SSD1306_DISPLAYON);
}

void resetDisplay(void) {
	displayOff();
	clearDisplay();
	displayOn();
}
void displayOn(void) {
	ssd1306_command(0xaf);
}

void displayOff(void) {
	ssd1306_command(0xae);
}

void clearDisplay(void) {
	unsigned char i,k;
	for(k=0;k<8;k++)
	{
		setXY(k,0);
		{
			for(i=0;i<128;i++) {
				sendChar(0);
			}
		}
	}
}

void printBigTime(char *string) {
	int Y=0;
	int lon = strlen(string);
	if(lon == 3) {
		Y = 0;
		} 
		else if (lon == 2) {
		Y = 3;
		} 
		else if (lon == 1) {
		Y = 6;
	}

	int X = 4;
	while(*string) {
		printBigNumber(*string, X, Y);

		Y+=3;
		X=4;
		setXY(X,Y);
		string++;
	}
}

void printBigNumber(char string, int X, int Y) {
	setXY(X,Y);
	int salto=0;
	for(int i=0;i<96;i++)
	{
		if(string == ' ') {
			sendChar(0);
		} else
		sendChar(pgm_read_byte(bigNumbers[string-0x30]+i));

		if(salto == 23) {
			salto = 0;
			X++;
			setXY(X,Y);
			} else {
			salto++;
		}
	}
}

void sendChar(unsigned char data) {
	i2c_start(_i2c_address);
	i2c_write(0x40);
	i2c_write(data);
	i2c_stop();
}

void sendCharXY(unsigned char data, int X, int Y) {
	setXY(X, Y);
	i2c_start(_i2c_address);
	i2c_write(0x40);

	for(int i=0;i<8;i++)
	i2c_write(pgm_read_byte(myFont[data-0x20]+i));

	i2c_stop();
}

void setXY(unsigned char row,unsigned char col) {
	ssd1306_command(0xb0 + row);
	ssd1306_command(0x00 + (8 * col & 0x0f));
	ssd1306_command(0x10 + ((8 * col >> 4) & 0x0f));
}

void sendStr(char *string) {
	unsigned char i=0;
	while(*string) {
		for(i=0;i<8;i++) {
			sendChar(pgm_read_byte(myFont[*string - 0x20] + i));
		}
		string++;
	}
}

void sendStrXY( char *string, int X, int Y) {
	setXY(X,Y);
	unsigned char i=0;
	while(*string) {
		if (*string == '\n') {
			setXY(X + 1,0);
			string++;
		}
		for(i=0; i<8; i++) {
			sendChar(pgm_read_byte(myFont[*string-0x20]+i));
		}
		string++;
	}
}
void ssd1306_setPos(uint8_t x, uint8_t y) {
	ssd1306_command(0xb0 + y);
	ssd1306_command(((x & 0xf0) >> 4) | 0x10);
}
void printFonts(){
	clearDisplay();

	uint8_t data=32;
	for(int k=0;k<6;k++) {
		setXY(k,0);

		for (int j=0;j<16;j++) {

			for(int i=0;i<8;i++) {
				sendChar(pgm_read_byte(myFont[(data+j)-0x20]+i));
			}
		}
		data=data+16;
	}
}

void ssd1306_drawBmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[]) {
	uint16_t j = 0;
	uint8_t y;
	if (y1 % 8 == 0) y = y1 / 8;
	else y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		ssd1306_setPos(x0,y);

		for (uint8_t x = x0; x < x1; x++) {
			ssd1306_data(pgm_read_byte(&bitmap[j++]));
		}

	}
}

void invertDisplay(uint8_t i) {
	if (i) {
		ssd1306_command(SSD1306_INVERTDISPLAY);
		} else {
		ssd1306_command(SSD1306_NORMALDISPLAY);
	}
}

void startScrollRight(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X00);
	ssd1306_command(0XFF);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void startScrollLeft(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X00);
	ssd1306_command(0XFF);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void startScrollDiagRight(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	ssd1306_command(0X00);
	ssd1306_command(SSD1306_LCDHEIGHT);
	ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void startScrollDiagLeft(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	ssd1306_command(0X00);
	ssd1306_command(SSD1306_LCDHEIGHT);
	ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void stopScroll(void) {	ssd1306_command(SSD1306_DEACTIVATE_SCROLL); }

void dim(bool dim) {
	uint8_t contrast;

	if (dim) {
		contrast = 0;
		} else {
		if (_vccstate == SSD1306_EXTERNALVCC) {
			contrast = 0x9F;
			} else {
			contrast = 0xCF;
		}
	}
	
	ssd1306_command(SSD1306_SETCONTRAST);
	ssd1306_command(contrast);
}