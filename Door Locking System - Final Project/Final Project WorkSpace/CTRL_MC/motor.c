 /******************************************************************************
 *
 * Module: DC MOTOR
 *
 * File Name: motor.c
 *
 * Description: Source file for DC Motor driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#include "motor.h"
#include "gpio.h"
#include "std_types.h"
#include "pwm.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void DC_Motor_init(void)
{
    // Sets the direction of the motor control pins as output pins
    GPIO_setupPinDirection(MOTOR_PORT_ID, MOTOR_E_PIN_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(MOTOR_PORT_ID, MOTOR_FIRST_PIN_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(MOTOR_PORT_ID, MOTOR_SECOND_PIN_ID, PIN_OUTPUT);

    GPIO_writePin(MOTOR_PORT_ID, MOTOR_E_PIN_ID, LOGIC_HIGH);
}

void DcMotor_Rotate(DCMotor_State state, uint8 speed)
{
	switch(state)
    {
        case MOTOR_STOP:
            // Stops the motor by setting both control pins low
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_FIRST_PIN_ID, LOGIC_LOW);
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_SECOND_PIN_ID, LOGIC_LOW);
            break;
        case MOTOR_CW:
            // Rotates the motor in the clockwise direction
            // by setting the first control pin low and the second control pin high
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_FIRST_PIN_ID, LOGIC_LOW);
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_SECOND_PIN_ID, LOGIC_HIGH);
            break;
        case MOTOR_ACW:
            // Rotates the motor in the anti-clockwise direction
            // by setting the first control pin high and the second control pin low
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_FIRST_PIN_ID, LOGIC_HIGH);
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_SECOND_PIN_ID, LOGIC_LOW);
            break;
    }

	PWM_Timer0_Start(speed);
}
