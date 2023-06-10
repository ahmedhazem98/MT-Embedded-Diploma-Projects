/******************************************************************************
 *
 * Module: PWM
 *
 * File Name: pwm.h
 *
 * Description: Header file for PWM driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#ifndef PWM_H_
#define PWM_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define PWM_OC0_PORT_ID     PORTB_ID
#define PWM_OC0_PIN_ID      PIN3_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for Setting the Motor speed.
 */
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* PWM_H_ */
