#include "main.hpp"

int main(void) {
	init();

	uint16_t count = 0;
	
	while (true) {
		uint8_t buttonDown = debounce();

		if (buttonDown) {
			buttonDown = 0;
			LED_PORT ^= LED_MASK;
			char str[15] = "";
			sprintf(str, "Count: %u", ++count);
			sendStrXY(str, 0, 0);
		}
		_delay_ms(10);
	}
	return 0;
}

void init(void) {
	i2c_init();
 	initializeDisplay();
 	printFonts();
 	clearDisplay();

	LED_DDR |= LED_MASK;
	BUTTON_PORT |= BUTTON_MASK;
}