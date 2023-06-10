/*
 * StopWatch.c
 *
 *  Created on: Apr 12, 2023
 *      Author: Ahmed Hazem
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Global Variables
volatile unsigned char Runningflag = 0; // Flag to indicate if the stop watch is running
volatile unsigned char Pauseflag = 0; // Flag to indicate if the stop watch is paused
volatile unsigned char Resumeflag = 0; // Flag to indicate if the stop watch is resumed
volatile unsigned char secCount1 = 0; // Ones place of seconds
volatile unsigned char secCount2 = 0; // Tens place of seconds
volatile unsigned char minCount1 = 0; // Ones place of minutes
volatile unsigned char minCount2 = 0; // Tens place of minutes
volatile unsigned char hourCount1 = 0; // Ones place of hours
volatile unsigned char hourCount2 = 0; // Tens place of hours

// Initializes Timer1 in CTC mode for a resolution of 1 millisecond
void Timer1_Init_CTC_Mode(void)
{
	/*For a stop watch, we want a timer resolution of 1 millisecond (ms)or less, for
	 * resolution of 1ms we can use prescaler of 64, Timer Resolution = 1 / (1MHz * 64 * OCR1A) = 1ms
	  Then OCR1A = (1 / (1MHz * 64)) * 1000 = 15.625 */

	TCNT1 = 0;
	OCR1A = 15625;
	TIMSK |= (1<<OCIE1A);			  //Timer Interrupt Enable
	TCCR1A = (1<<FOC1A) | (1<<FOC1B); // Non PWM
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10); //Prescaler of 64, CTC


}

// Initializes INT0 for the Reset button
void INT0_Init(void)
{
	MCUCR |= (1<<ISC01);			//Generates Request at Falling Edge
	GICR |= (1<<INT0);				//Enable Module Interrupt (INT0)
}

// Initializes INT1 for the Pause button
void INT1_Init(void)
{
	MCUCR |= (1<<ISC10);			//Generates Request at Rising Edge
	MCUCR |= (1<<ISC11);			//Generates Request at Rising Edge
	GICR |= (1<<INT1);				//Enable Module Interrupt (INT1)

}

// Initializes INT2 for the Resume button
void INT2_Init(void)
{

	MCUCR &= ~(1<<ISC2);			//Generates Request at Falling Edge
	GICR |= (1<<INT2);				//Enable Module Interrupt (INT2)
}

// Displays the stop watch time on a seven-segment display
void displaySevenSegment()
{
	// Display ones place of seconds
	PORTA = (PORTA & 0xC0) | 0x01;
	PORTC = (PORTC & 0xF0) | (secCount1 & 0x0F);
	_delay_us(50);

	// Display tens place of seconds
	PORTA = (PORTA & 0xC0) | 0x02;
	PORTC = (PORTC & 0xF0) | (secCount2 & 0x0F);
	_delay_us(50);

	// Display ones place of minutes
	PORTA = (PORTA & 0xC0) | 0x04;
	PORTC = (PORTC & 0xF0) | (minCount1 & 0x0F);
	_delay_us(50);

	// Display tens place of minutes
	PORTA = (PORTA & 0xC0) | 0x08;
	PORTC = (PORTC & 0xF0) | (minCount2 & 0x0F);
	_delay_us(50);

	// Display ones place of hours
	PORTA = (PORTA & 0xC0) | 0x10;
	PORTC = (PORTC & 0xF0) | (hourCount1 & 0x0F);
	_delay_us(50);

	// Display tens place of hours
	PORTA = (PORTA & 0xC0) | 0x20;
	PORTC = (PORTC & 0xF0) | (hourCount2 & 0x0F);
	_delay_us(50);
}

// Timer1 ISR
ISR(TIMER1_COMPA_vect)
{
	cli();
	Runningflag = 1;
	sei();
}

// Reset Button ISR
ISR(INT0_vect)
{
	cli();
	TIMSK &= ~(1<<OCIE1A); //Disable Timer Interrupt
	secCount1 = 0;
	secCount2 = 0;
	minCount1 = 0;
	minCount2 = 0;
	hourCount1 = 0;
	hourCount2 = 0;
	sei();
}

// Pause Button ISR
ISR(INT1_vect)
{
	cli();
	TIMSK &= ~(1<<OCIE1A); //Disable Timer Interrupt
	Pauseflag = 1;
	sei();
}

//Resume Button ISR
ISR(INT2_vect)
{
	cli();
	TIMSK |= (1<<OCIE1A); //Enable Timer Interrupt
	Resumeflag = 1;
	sei();
}

int main(void)
{
	//PORTA
	DDRA |= 0x3F;					// Configure PA0 => PA5 as Output pins
	PORTA &= 0xC0;					// Clearing PA0 => PA5
	//PORTB
	DDRB  &= (~(1<<PB2));  			// Configure PB2 as input pin
	PORTB |= (1<<PB2);    		    // Enable the internal pull up resistor at PB2 pin
	//PORTC
	DDRC |= 0x0F;					// Configure PC0 => PC3 as Output pins
	PORTC &= 0xF0;					// Clearing PC0 => PC3
	//PORTD
	DDRD &= 0xF3;					// Configure PD2,PD3 as input pins
	PORTD |= (1<<PD2);				//Internal Pull-up resistor Enable

	SREG |= (1<<7);					//Global Interrupt Enable

	INT0_Init(); // Initialize Reset button
	INT1_Init(); // Initialize Pause button
	INT2_Init(); // Initialize Resume button
	Timer1_Init_CTC_Mode(); // Initialize Timer1 in CTC mode
	sei();

	while(1)
	{
		if(Runningflag)
		{
			secCount1++;
			if (secCount1 == 10)
			{
				secCount2++;
				secCount1 = 0;
			}
			if(secCount2 == 6)
			{
				secCount2 = 0;
				minCount1++;
			}
			if (minCount1 == 10)
			{
				minCount2++;
				minCount1 = 0;
			}
			if (minCount2 == 6)
			{
				minCount2 = 0;
				hourCount1++;
			}
			if (hourCount1 == 9)
			{
				hourCount2++;
				hourCount1 = 0;
			}

			Runningflag = 0;
		}
		else
		{
			displaySevenSegment();
		}

		while(Pauseflag == 1)
		{
			displaySevenSegment();

			if(Resumeflag == 1)
			{
				Resumeflag = 0;
				Pauseflag = 0;
			}
		}

	}

}

