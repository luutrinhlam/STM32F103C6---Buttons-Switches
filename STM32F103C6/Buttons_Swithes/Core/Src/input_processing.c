/*
 * input_processing.c
 *
 *  Created on: Oct 4, 2021
 *      Author: Admin
 */

#include "main.h"
#include "input_reading.h"
#include "timer.h"
#include "Led7Segment.h"

// we aim to work with more light, maybe...
#define N0_OF_LIGHTS 3

// number of button
#define N0_OF_BUTTON 4

// Cycle of decrease led 7 in mode 1
#define LIGHT_DURATION 1000

// for MODE 2,3,4
#define LED_BLINKING_FREQUENCY = 2000

// Initial period of light
#define INITIAL_RED_PERIOD 5
#define INITIAL_AMBER_PERIOD 2
#define INITIAL_GREEN_PERIOD 3

int is_button_pressed_and_released(int index);
void clear_all_light(void);
void prepare_for_modified_mode(int index);
void displayTrafficLight();
void updateLightPeriod(uint8_t light);
void prepare_for_DISPLAY();
void blinkingLight(uint8_t light);

// STATES OF FSM
enum Mode {
	DISPLAY, RED_DURATION_MODIFY, AMBER_DURATION_MODIFY, GREEN_DURATION_MODIFY
};

// STATES OF LIGHT
enum lightStateHorizontal {
	RED, AMBER, GREEN
};

// STATES OF BUTTON
enum ButtonState {
	PRESSED, RELEASED
};

// VARIABLE FOR TRAFFIC LIGHT STATE AND INITIALIZE
enum lightStateHorizontal lightStateHorizontal = RED;
enum lightStateHorizontal lightStateVertical = GREEN;

// VARIABLE FOR FSM STATE AND INITIALIZE TO MODE 1
enum Mode mode = DISPLAY;

// VARIABLE FOR TRAFFIC LIGHT STATE AND INITIALIZE TO REALEASED
enum ButtonState buttonState[N0_OF_BUTTON] = { RELEASED, RELEASED, RELEASED,
		RELEASED };

// output of FSM for button
static int buttonIsPressedAndReleased[N0_OF_BUTTON];

// light period of active
static uint8_t lightPeriod[N0_OF_LIGHTS] = { INITIAL_RED_PERIOD,
		INITIAL_AMBER_PERIOD, INITIAL_GREEN_PERIOD };

// buffer for displaying the changing value before confirming in RED_DURATION_MODIFY, AMBER_DURATION_MODIFY, GREEN_DURATION_MODIFY
int light_period_modify_buffer[N0_OF_LIGHTS] = { INITIAL_RED_PERIOD,
		INITIAL_AMBER_PERIOD, INITIAL_GREEN_PERIOD };

int RED_counter_horizontal = INITIAL_RED_PERIOD;
int AMBER_counter_horizontal = INITIAL_AMBER_PERIOD;
int GREEN_counter_horizontal = INITIAL_GREEN_PERIOD;

int RED_counter_vertical = INITIAL_RED_PERIOD;
int AMBER_counter_vertical = INITIAL_AMBER_PERIOD;
int GREEN_counter_vertical = INITIAL_GREEN_PERIOD;

void fsm_for_button_processing(void) {
	for (int index = 0; index < N0_OF_BUTTON; index++) {
		switch (buttonState[index]) {
		case RELEASED:
			if (is_button_pressed(index)) {
				buttonState[index] = PRESSED;
			}
			break;

		case PRESSED:
			if (!is_button_pressed(index)) {
				buttonState[index] = RELEASED;
				buttonIsPressedAndReleased[index] = 1;
			}
			break;
		}
	}
}

void fsm_for_mode_processing(void) {
	update_7seg_led();
	switch (mode) {
	case DISPLAY:
		if (is_button_pressed_and_released(0)) {
			mode = RED_DURATION_MODIFY;
			prepare_for_modified_mode(RED);
			setTimerTimeOut(10000);	// after 10s doing nothing, turn our machine to display normally
		} else {
			displayTrafficLight();
		}
		break;

	case RED_DURATION_MODIFY:
		if (is_button_pressed_and_released(0)) {
			mode = AMBER_DURATION_MODIFY;
			prepare_for_modified_mode(AMBER);
			setTimerTimeOut(10000);
		} else {
			blinkingLight(RED);
			updateLightPeriod(RED);
		}
		if (timerTimeOut_flag == 1) {
			mode = DISPLAY;
			prepare_for_DISPLAY();
		}
		break;

	case AMBER_DURATION_MODIFY:
		if (is_button_pressed_and_released(0)) {
			mode = GREEN_DURATION_MODIFY;
			prepare_for_modified_mode(GREEN);
			setTimerTimeOut(10000);
		} else {
			blinkingLight(AMBER);
			updateLightPeriod(AMBER);
		}
		if (timerTimeOut_flag == 1) {
			mode = DISPLAY;
			prepare_for_DISPLAY();
		}
		break;

	case GREEN_DURATION_MODIFY:
		if (is_button_pressed_and_released(0)) {
			mode = DISPLAY;
			prepare_for_DISPLAY();
			setTimerTimeOut(10000);
		} else {
			blinkingLight(GREEN);
			updateLightPeriod(GREEN);
		}
		if (timerTimeOut_flag == 1) {
			mode = DISPLAY;
			prepare_for_DISPLAY();
		}
		break;
	}
}

//check the flag if a single button has just been pressed and released
int is_button_pressed_and_released(int index) {
	if (index >= N0_OF_BUTTON)
		return 0;
	if (buttonIsPressedAndReleased[index]) {
		buttonIsPressedAndReleased[index] = 0;
		return 1;
	} else
		return 0;
}

//clear all traffic light - turn off all
void clear_all_light(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
}

// for mode 2, 3, 4
void prepare_for_modified_mode(int index) {
	clear_all_light();
	for (int i = 0; i < N0_OF_BUTTON; i++) {
		buttonIsPressedAndReleased[i] = 0;
	}
	light_period_modify_buffer[index] = lightPeriod[index];
	update_LED7_buffer_vertical(index + 2); // display the mode
	update_LED7_buffer_horizontal(light_period_modify_buffer[index]);
}

// display traffic light in DISPLAY Mode, using an FSM to control RED,AMBER,GREEN states by timer interrupt
void displayTrafficLight() {
	switch (lightStateHorizontal) {
	case RED:
		if (timerTrafficHorizontal_flag) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);

			update_LED7_buffer_horizontal(RED_counter_horizontal);
			RED_counter_horizontal--;

			if (RED_counter_horizontal < 0) {
				lightStateHorizontal = GREEN;
				RED_counter_horizontal = lightPeriod[RED];
			} else {
				setTimerTrafficHorizontal(LIGHT_DURATION);
			}
		}

		break;

	case GREEN:
		if (timerTrafficHorizontal_flag) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, RESET);

			update_LED7_buffer_horizontal(GREEN_counter_horizontal);
			GREEN_counter_horizontal--;

			if (GREEN_counter_horizontal < 0) {
				lightStateHorizontal = AMBER;
				GREEN_counter_horizontal = lightPeriod[GREEN];
			} else {
				setTimerTrafficHorizontal(LIGHT_DURATION);
			}
		}

		break;

	case AMBER:
		if (timerTrafficHorizontal_flag) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, RESET);

			update_LED7_buffer_horizontal(AMBER_counter_horizontal);
			AMBER_counter_horizontal--;

			if (AMBER_counter_horizontal < 0) {
				lightStateHorizontal = RED;
				AMBER_counter_horizontal = lightPeriod[AMBER];
			} else {
				setTimerTrafficHorizontal(LIGHT_DURATION);
			}
		}
		break;
	}

	switch (lightStateVertical) {
	case RED:
		if (timerTrafficVertical_flag) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, RESET);

			update_LED7_buffer_vertical(RED_counter_vertical);
			RED_counter_vertical--;

			if (RED_counter_vertical < 0) {
				lightStateVertical = GREEN;
				RED_counter_vertical = lightPeriod[RED];
			} else {
				setTimerTrafficVertical(LIGHT_DURATION);
			}
		}

		break;

	case GREEN:
		if (timerTrafficVertical_flag) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, RESET);

			update_LED7_buffer_vertical(GREEN_counter_vertical);
			GREEN_counter_vertical--;

			if (GREEN_counter_vertical < 0) {
				lightStateVertical = AMBER;
				GREEN_counter_vertical = lightPeriod[GREEN];
			} else {
				setTimerTrafficVertical(LIGHT_DURATION);
			}
		}
		break;

	case AMBER:
		if (timerTrafficVertical_flag) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, RESET);

			update_LED7_buffer_vertical(AMBER_counter_vertical);
			AMBER_counter_vertical--;

			if (AMBER_counter_vertical < 0) {
				lightStateVertical = RED;
				AMBER_counter_vertical = lightPeriod[AMBER];
			} else {
				setTimerTrafficVertical(LIGHT_DURATION);
			}
		}
		break;
	}
}

// Blinking 1 type of traffic light
// Use in 3 Duration Modify mode
void blinkingLight(uint8_t light) {
	if (timerBlink_flag == 1) {
		switch (light) {
		case RED:
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
			setTimerBlink(250);
			break;
		case GREEN:
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
			setTimerBlink(250);
			break;
		case AMBER:
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			setTimerBlink(250);
			break;
		default:
			break;
		}
	}
}

// Function of SET button to set the require duration for 1 traffic light
void updateLightPeriod(uint8_t light) {
	if (is_button_pressed_and_released(1)) {
		setTimerTimeOut(10000);
		if (light_period_modify_buffer[light] == 99)
			light_period_modify_buffer[light] = 1;
		else
			light_period_modify_buffer[light]++;
		update_LED7_buffer_horizontal(light_period_modify_buffer[light]);
	}

	if (is_button_pressed_and_released(2)) {
		setTimerTimeOut(10000);
		if (light_period_modify_buffer[light] == 1)
			light_period_modify_buffer[light] = 99;
		else
			light_period_modify_buffer[light]--;
		update_LED7_buffer_horizontal(light_period_modify_buffer[light]);

	}

	if (is_button_pressed_and_released(3)) {
		setTimerTimeOut(10000);
		lightPeriod[light] = light_period_modify_buffer[light];
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
	}
}

// preparing to transfer from Mode modify GREEN to mode display
void prepare_for_DISPLAY() {
	// handle different situation of modified duration, to get the consistency of 2 road

	lightStateHorizontal = RED;
	RED_counter_horizontal = lightPeriod[RED];
	AMBER_counter_horizontal = lightPeriod[AMBER];
	GREEN_counter_horizontal = lightPeriod[GREEN];

	lightStateVertical = GREEN;
	RED_counter_vertical = lightPeriod[RED];
	AMBER_counter_vertical = lightPeriod[AMBER];
	GREEN_counter_vertical = lightPeriod[GREEN];

	clear_all_light();


	timerTrafficHorizontal_flag = 1;
	timerTrafficVertical_flag = 1;

}
