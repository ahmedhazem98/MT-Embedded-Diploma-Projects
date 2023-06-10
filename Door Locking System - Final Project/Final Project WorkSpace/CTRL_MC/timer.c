 /******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.c
 *
 * Description: Source file for Timer driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#include "timer.h"
#include <avr/interrupt.h>
#include <avr/io.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/* Global variable to store the address of the callback function */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                      	Functions Definitions                              *
 *******************************************************************************/
/* Timer 1 Compare Mode Interrupt ISR */
ISR(TIMER1_COMPA_vect)
{
	/*Increment the ticks of the program*/
	if(g_callBackPtr != NULL_PTR){
		(*g_callBackPtr)();
	}
}


/* Timer 1 Normal Mode Interrupt ISR */
ISR(TIMER1_OVF_vect)
{

	/*Increment the ticks of the program*/
	(*g_callBackPtr)();

}
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	TCCR1A = 0; // Normal mode
	TCCR1B = 0; // Stop the timer
	TCNT1 = Config_Ptr->initial_value;	/* Set timer1 initial value */
	OCR1A = Config_Ptr->compare_value;	/* Set timer1 compare value */
	TIMSK |= (1<<OCIE1A); /* Enable Timer1 Compare A Interrupt */
	TCCR1B |= ((TCCR1B & 0xF8) | (Config_Ptr->prescaler & 0x07));

	if(Config_Ptr->mode == NORMAL_MODE)
	{
		/* Configure timer1 control registers TCCR1A & TCCR1B
		 * 1. Normal Mode (Mode Number 0)
		*/
		TCCR1B &= ~(1 << WGM13) & ~(1 << WGM12);
	}
	else if (Config_Ptr->mode == PWM_MODE)
	{
		/* Configure timer1 control registers TCCR1A & TCCR1B
		 * 1. PWM Mode WGM10=1 (Mode Number 1)
		*/
		TCCR1A |= (1 << WGM10);
        TCCR1B &= ~(1 << WGM13) & ~(1 << WGM12);
	}
	else if(Config_Ptr->mode == COMPARE_MODE)
	{
		/* Configure timer1 control register TCCR1B
		 * 1. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
		*/
        TCCR1B |= (1 << WGM12);
	}
	else if (Config_Ptr->mode == FAST_PWM_MODE)
	{
		/* Configure timer1 control registers TCCR1A & TCCR1B
		 * 1. Fast PWM 8-bit Mode WGM10=1 WGM12=1 (Mode Number 5)
		*/
		TCCR1A |= (1 << WGM10);
        TCCR1B |= (1 <<WGM12);
	}


}

void Timer1_deInit(void)
{
    /* Stop timer1 and clear its registers */
	TCCR1A=0;
	TCCR1B=0;
    TCNT1 = 0;
	TIMSK &= ~(1 << OCIE1A);
}

void Timer1_setCallBack(void(*a_ptr)(void))
{
	/* Assign the address of the callback function to the global variable */
	g_callBackPtr = a_ptr;
}
