 /******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.h
 *
 * Description: Header file for Timer driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

typedef enum{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024,EXTERNAL_ON_FALLING,EXTERNAL_ON_RISING
}Timer1_Prescaler;

typedef enum{
		NORMAL_MODE,PWM_MODE,COMPARE_MODE,FAST_PWM_MODE
}Timer1_Mode;

typedef struct {
 uint16 initial_value;
 uint16 compare_value; // it will be used in compare mode only.
 Timer1_Prescaler prescaler;
 Timer1_Mode mode;
} Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function responsible for Initializing Timer1.
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);
/*
 * Description :
 * Function responsible to disable Timer1.
 */
void Timer1_deInit(void);
/*
 * Description :
 *  Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void));

#endif /* TIMER_H_ */
