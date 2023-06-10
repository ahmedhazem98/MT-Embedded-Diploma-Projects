/******************************************************************************
 *
 * Module: Ultrasonic Sensor
 *
 * File Name: ultrasonic.c
 *
 * Description: source file for the HC-SR04 Ultrasonic Sensor driver
 *
 * Author: Ahmed Hazem
 *
 ******************************************************************************/

#include "ultrasonic.h"
#include "gpio.h"
#include <avr/io.h>
#include <math.h>
#include <util/delay.h>

/*******************************************************************************
 *                           Global Variables                                  *
 ******************************************************************************/

static uint8 g_edgeCount = 0;     /* Number of edges detected by the ICU */
static uint16 g_highTime = 0;     /* High time between the two edges detected by the ICU */

/*******************************************************************************
 *                      Functions Definitions                                  *
 ******************************************************************************/

/**
 * Function: Ultrasonic_init
 *
 * Description: Initializes the ICU and sets up the trigger pin as an output pin.
 *
 * Inputs: None
 *
 * Returns: None
 */
void Ultrasonic_init(void)
{
    /* Configure the ICU with a clock prescaler of F_CPU/8 and a rising edge type */
	ICU_ConfigType config = {F_CPU_8, RAISING};
    SREG |= (1 << 7);
    ICU_init(&config);

    /* Set up the callback function to be executed whenever an edge is detected by the ICU */
    ICU_setCallBack(Ultrasonic_edgeProcessing);

    /* Set up the trigger pin as an output pin */
    GPIO_setupPinDirection(PORTB_ID, PIN5_ID, PIN_OUTPUT);
}

/**
 * Function: Ultrasonic_Trigger
 *
 * Description: Sends a trigger pulse to the HC-SR04 Ultrasonic Sensor.
 *
 * Inputs: None
 *
 * Returns: None
 */
void Ultrasonic_Trigger(void)
{
    /* Send a trigger pulse to the Ultrasonic Sensor by setting the trigger pin to logic high for 10 microseconds and then setting it to logic low */
    GPIO_writePin(PORTB_ID, PIN5_ID, LOGIC_HIGH);
    _delay_ms(0.001);
    GPIO_writePin(PORTB_ID, PIN5_ID, LOGIC_LOW);
}

/**
 * Function: Ultrasonic_readDistance
 *
 * Description: Triggers the Ultrasonic Sensor and waits for two edges to be detected by the ICU.
 * Calculates the distance based on the high time between these two edges and returns it.
 *
 * Inputs: None
 *
 * Returns: The distance in centimeters
 */
uint16 Ultrasonic_readDistance(void)
{
	Ultrasonic_Trigger();
	while(g_edgeCount != 2);
	g_edgeCount = 0;
	return ceil(g_highTime/58.8);
}

/**
 * Function: Ultrasonic_edgeProcessing
 *
 * Description: Called by the ICU whenever an edge is detected. Increments the g_edgeCount variable
 * and sets the edge type and input capture value of the ICU based on the value of g_edgeCount.
 *
 * Inputs: None
 *
 * Returns: None
 */
void Ultrasonic_edgeProcessing(void)
{
    /* Increment the edge count */
    g_edgeCount++;

    if (g_edgeCount == 1)
    {
        /* First rising edge detected, clear the time value and set the ICU to detect a falling edge */
    	ICU_clearTimerValue();
    	ICU_setEdgeDetectionType(FALLING);
    }
    else if (g_edgeCount == 2)
    {
        /* Second falling edge detected, get the input capture value and clear the time value. Set the ICU to detect a rising edge. */
        g_highTime = ICU_getInputCaptureValue();
        ICU_clearTimerValue();
        ICU_setEdgeDetectionType(RAISING);
    }

    /* Do nothing if more than two edges are detected by the ICU. */
}
