/*
 * input_processing.c
 *
 *  Created on: Oct 4, 2021
 *      Author: Admin
 */

#include "main.h"
#include "input_reading.h"
#include "timer.h"

// we aim to work with more light, maybe...
#define N0_OF_LIGHTS 3

// Number of led 7 segment
#define N0_OF_LED_7_SEGMENT 4

// Total frequency for display {N0_OF_LED_7_SEGMENT} led 7 segment
#define FREQUENCY_OF_4_LED7 200

// Cycle of decrease led 7 in mode 1
#define LIGHT_DURATION 1000

// for MODE 2,3,4
#define LED_BLINKING_FREQUENCY = 2000

// Initial period ò light
#define INITIAL_RED_PERIOD 5
#define INITIAL_AMBER_PERIOD 2
#define INITIAL_GREEN_PERIOD 3

void displayTrafficLight();
void updateLightPeriod(uint8_t light);
void update_7seg_led();
void update_LED7_buffer_vertical(int num);
void update_LED7_buffer_horizontal(int num);
void resetAll_MODE1();
void blinkingLight(uint8_t light);

// STATES OF FSM
enum Mode {
	MODE_1, MODE_2, MODE_3, MODE_4
};

// lights
enum LightState {
	RED, AMBER, GREEN
};
// VARIABLE FOR TRAFFIC LIGHT STATE AND INITIALIZE TO RED
enum LightState lightState = RED;

// VARIABLE FOR FSM STATE AND INITIALIZE TO MODE 1
enum Mode mode = MODE_1;

// light period of active
static uint8_t lightPeriod[N0_OF_LIGHTS] = { INITIAL_RED_PERIOD,
INITIAL_AMBER_PERIOD, INITIAL_GREEN_PERIOD };

// buffer for displaying the changing value before confirming in MODE_2, MODE_3, MODE_4
int light_period_modify_buffer[N0_OF_LIGHTS] = { INITIAL_RED_PERIOD,
INITIAL_AMBER_PERIOD, INITIAL_GREEN_PERIOD };

int RED_counter = INITIAL_RED_PERIOD;
int AMBER_counter = INITIAL_AMBER_PERIOD;
int GREEN_counter = INITIAL_GREEN_PERIOD;
int LED7_buffer[] = { 0, 0, 0, 0 };

void fsm_for_input_processing(void) {
	update_7seg_led();
	switch (mode) {
	case MODE_1:
		if (is_button_pressed(0)) {
			mode = MODE_2;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
			light_period_modify_buffer[RED] = lightPeriod[RED];
			update_LED7_buffer_vertical(2);
			update_LED7_buffer_horizontal(light_period_modify_buffer[RED]);
		} else {
			displayTrafficLight();
		}
		break;

	case MODE_2:
		if (is_button_pressed(0)) {
			mode = MODE_3;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
			light_period_modify_buffer[AMBER] = lightPeriod[AMBER];
			update_LED7_buffer_vertical(3);
			update_LED7_buffer_horizontal(light_period_modify_buffer[AMBER]);
		} else {
			blinkingLight(RED);
			updateLightPeriod(RED);
		}
		break;

	case MODE_3:
		if (is_button_pressed(0)) {
			mode = MODE_4;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
			light_period_modify_buffer[GREEN] = lightPeriod[GREEN];
			update_LED7_buffer_vertical(4);
			update_LED7_buffer_horizontal(light_period_modify_buffer[GREEN]);
		} else {
			blinkingLight(AMBER);
			updateLightPeriod(AMBER);
		}
		break;

	case MODE_4:
		if (is_button_pressed(0)) {
			mode = MODE_1;
			resetAll_MODE1();
		} else {
			blinkingLight(GREEN);
			updateLightPeriod(GREEN);
		}
		break;
	}
}

uint8_t led7_number[10] = { 0x40, //0
		0xf9, //1
		0x24, //2
		0x30, //3
		0x99, //4
		0x12, //5
		0x82, //6
		0xF8, //7
		0x80, //8
		0x90 //9
		};
void display7SEG(int num) {
	uint8_t code = led7_number[num];
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, ((code >> 0) & 0x01));
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, ((code >> 1) & 0x01));
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, ((code >> 2) & 0x01));
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, ((code >> 3) & 0x01));
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, ((code >> 4) & 0x01));
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, ((code >> 5) & 0x01));
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, ((code >> 6) & 0x01));
}
// index for display 4 led7 by timner
int LED7_index = N0_OF_LED_7_SEGMENT;
void update_7seg_led() {
	if (timerLED7_flag) {
		// disable all enable pin of led7seg
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, SET);

		if (LED7_index == N0_OF_LED_7_SEGMENT)
			LED7_index = 0;

		// display the value;
		display7SEG(LED7_buffer[LED7_index]);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7 << LED7_index, RESET);

		LED7_index++;
		setTimerLED7(FREQUENCY_OF_4_LED7 / 4);
	}

}

void update_LED7_buffer_vertical(int num) {
	LED7_buffer[0] = num / 10;
	LED7_buffer[1] = num % 10;
}
void update_LED7_buffer_horizontal(int num) {
	LED7_buffer[2] = num / 10;
	LED7_buffer[3] = num % 10;
}

void displayTrafficLight() {
	switch (lightState) {
	case RED:
		if (timerTrafficHorizontal_flag) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);

			update_LED7_buffer_horizontal(RED_counter);
			RED_counter--;

			if (RED_counter < 0) {
				lightState = GREEN;
				RED_counter = lightPeriod[RED];
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

			update_LED7_buffer_horizontal(GREEN_counter);
			GREEN_counter--;

			if (GREEN_counter < 0) {
				lightState = AMBER;
				GREEN_counter = lightPeriod[GREEN];
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

			update_LED7_buffer_horizontal(AMBER_counter);
			AMBER_counter--;

			if (AMBER_counter < 0) {
				lightState = RED;
				AMBER_counter = lightPeriod[AMBER];
			} else {
				setTimerTrafficHorizontal(LIGHT_DURATION);
			}
		}

		break;

	}
}
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
void updateLightPeriod(uint8_t light) {
	if (is_button_pressed(1)) {
		if (light_period_modify_buffer[light] == 99)
			light_period_modify_buffer[light] = 1;
		else
			light_period_modify_buffer[light]++;
		update_LED7_buffer_horizontal(light_period_modify_buffer[light]);
	}

	if (is_button_pressed(2)) {
		if (light_period_modify_buffer[light] == 1)
			light_period_modify_buffer[light] = 99;
		else
			light_period_modify_buffer[light]--;
		update_LED7_buffer_horizontal(light_period_modify_buffer[light]);

	}

	if (is_button_pressed(3)) {
		lightPeriod[light] = light_period_modify_buffer[light];
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
	}
}

void resetAll_MODE1() {
	lightState = RED;
	RED_counter = lightPeriod[RED];
	AMBER_counter = lightPeriod[AMBER];
	GREEN_counter = lightPeriod[GREEN];

	timerTrafficHorizontal_flag = 1;

}
