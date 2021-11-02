/*
 * timer.h
 *
 *  Created on: Oct 4, 2021
 *      Author: Admin
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

void setTimerTimeOut(int duration);
void setTimerBlink(int duration);
void setTimerTrafficHorizontal(int duration);
void setTimerTrafficVertical(int duration);
void setTimerLED7(int duration);
void timer_run();

extern int timerTimeOut_flag;
extern int timerTrafficHorizontal_flag;
extern int timerTrafficVertical_flag;
extern int timerBlink_flag;
extern int timerLED7_flag;


#endif /* INC_TIMER_H_ */
