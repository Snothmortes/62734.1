#include "debounce.hpp"

bool debounce(){
	static uint8_t count = 0;
	static uint8_t buttonState = 0;
	uint8_t currentState = (~BUTTON_PIN & BUTTON_MASK) != 0;

	if (currentState != buttonState) {
		count++;
		if (count >= 4) {
			buttonState = currentState;
			if (currentState != 0) return 1;
			count = 0;
		}
		return 0;
	}
	count = 0;
	return 0;
}