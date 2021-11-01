/*
 * Led7Segment.h
 *
 *  Created on: Nov 1, 2021
 *      Author: Admin
 */

#ifndef INC_LED7SEGMENT_H_
#define INC_LED7SEGMENT_H_

void update_7seg_led();
void update_LED7_buffer_vertical(int num);
void update_LED7_buffer_horizontal(int num);
void display7SEG(int num);

extern int LED7_buffer[];
extern int LED7_index;

#endif /* INC_LED7SEGMENT_H_ */
