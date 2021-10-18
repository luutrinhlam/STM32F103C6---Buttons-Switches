/*
 * input_reading.c
 *
 *  Created on: Oct 4, 2021
 *      Author: Admin
 */
# include "main.h"

// we aim to work with more than one buttons
# define N0_OF_BUTTONS 4

// timer interrupt duration is 10ms , so to pass 1 second ,
// we need to jump to the interrupt service routine 100 time
# define DURATION_FOR_AUTO_INCREASING 100
# define BUTTON_IS_PRESSED GPIO_PIN_RESET
# define BUTTON_IS_RELEASED GPIO_PIN_SET

// the buffer that the final result is stored after debouncing
static GPIO_PinState buttonBuffer[N0_OF_BUTTONS] = {1,1,1,1};

// we define two buffers for debouncing
static GPIO_PinState debounceButtonBuffer1[N0_OF_BUTTONS] = {1,1,1,1};
static GPIO_PinState debounceButtonBuffer2[N0_OF_BUTTONS] = {1,1,1,1};

// we define a flag for a button pressed and release
static uint8_t flagForButtonPressAndRelease[N0_OF_BUTTONS] = {0,0,0,0};

void button_reading(void) {
	for (int i = 0; i < N0_OF_BUTTONS; i++) {
		debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
		debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8 << i);
		if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i]) {
			if (debounceButtonBuffer1[i] == BUTTON_IS_RELEASED
					&& buttonBuffer[i] == BUTTON_IS_PRESSED)
				flagForButtonPressAndRelease[i] = 1;
			buttonBuffer[i] = debounceButtonBuffer1[i];
		}
	}
}

//Checking a button is pressed or not
unsigned char is_button_pressed(uint8_t index) {
	if (index >= N0_OF_BUTTONS)
		return 0;
	if (flagForButtonPressAndRelease[index]) {
		flagForButtonPressAndRelease[index] = 0; //solved
		return 1;
	} else
		return 0;
}

