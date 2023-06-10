/******************************************************************************
 *
 * Module: DC MOTOR
 *
 * File Name: motor.h
 *
 * Description: Header file for DC Motor driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#ifndef MOTOR_H_
#define MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* Motor HW Ports and Pins Ids */
#define MOTOR_PORT_ID               PORTB_ID
#define MOTOR_E_PIN_ID              PIN2_ID
#define MOTOR_FIRST_PIN_ID          PIN0_ID
#define MOTOR_SECOND_PIN_ID         PIN1_ID

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum
{
	MOTOR_STOP,MOTOR_CW,MOTOR_ACW
}DCMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialize the Motor.
 */
void DC_Motor_init(void);
/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or
 * stop the motor based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the
 * required speed value
 */
void DcMotor_Rotate(DCMotor_State state,uint8 speed);

#endif /* MOTOR_H_ */
