/*
 * pwm.h
 *
 *  Created on: 2023Äê11ÔÂ6ÈÕ
 *      Author: 77249
 */

#ifndef USER_PWM_H_
#define USER_PWM_H_

#include "debug.h"

/* PWM Output Mode Definition */
#define PWM_MODE1   0
#define PWM_MODE2   1

/* PWM Output Mode Selection */
//#define PWM_MODE PWM_MODE1
#define PWM_MODE PWM_MODE2

void TIM8_PWMOut_Init( u16 arr, u16 psc, u16 ccp );

#endif /* USER_PWM_H_ */
