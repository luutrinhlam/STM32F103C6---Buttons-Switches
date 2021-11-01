/*
 * timer.h
 *
 *  Created on: Oct 4, 2021
 *      Author: Admin
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void setTimerBlink(int duration);
void setTimerTrafficHorizontal(int duration);
void setTimerTrafficVertical(int duration);
void setTimerLED7(int duration);
void timer_run();


extern int timerTrafficHorizontal_flag;
extern int timerTrafficVertical_flag;
extern int timerBlink_flag;
extern int timerLED7_flag;


#endif /* INC_TIMER_H_ */
