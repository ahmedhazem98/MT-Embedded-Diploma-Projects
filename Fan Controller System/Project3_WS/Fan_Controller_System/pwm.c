/******************************************************************************
 *
 * Module: PWM
 *
 * File Name: pwm.c
 *
 * Description: Source file for PWM driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#include "pwm.h"
#include "avr/io.h"
#include "gpio.h"
#include <math.h>

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void PWM_Timer0_Start(uint8 duty_cycle) {

	/* Set Timer Initial Value to 0 */
	TCNT0 = 0;
	/* Set Compare Value */
    uint16 compareValue = (duty_cycle * 255) / 100;
	OCR0 = round(compareValue);
	/* Set Pwm Pin Direction as output */
    GPIO_setupPinDirection(PWM_OC0_PORT_ID, PWM_OC0_PIN_ID, PIN_OUTPUT);
	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS01);
}
