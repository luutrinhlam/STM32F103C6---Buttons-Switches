/*
 * Led7Segment.c
 *
 *  Created on: Nov 1, 2021
 *      Author: Admin
 */
#include "timer.h"
#include "main.h"


// Number of led 7 segment
#define N0_OF_LED_7_SEGMENT 4

// Total frequency for display {N0_OF_LED_7_SEGMENT} led 7 segment
#define FREQUENCY_OF_4_LED7 200




int LED7_buffer[] = { 0, 0, 0, 0 };
int led7_number[10] = { 0x40, //0
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
// index for display 4 led7 by timer
int LED7_index = N0_OF_LED_7_SEGMENT;

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


void update_LED7_buffer_vertical(int num) {
	LED7_buffer[0] = num / 10;
	LED7_buffer[1] = num % 10;
}
void update_LED7_buffer_horizontal(int num) {
	LED7_buffer[2] = num / 10;
	LED7_buffer[3] = num % 10;
}

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
