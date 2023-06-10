/******************************************************************************
 *
 * File Name: App.c
 *
 * Description: source file for the Application and main function
 *
 * Author: Ahmed Hazem
 *
 ******************************************************************************/
#include "lcd.h"
#include "icu.h"
#include "gpio.h"
#include "std_types.h"
#include "ultrasonic.h"
#include <avr/io.h>
#include <util/delay.h>

uint16 distance = 0;

int main(void) {
	LCD_init();
	Ultrasonic_init();
	LCD_displayString("Distance=    cm");

	while (1) {
		distance = Ultrasonic_readDistance();
		LCD_moveCursor(0, 10);
		if (distance >= 100) {
			LCD_intgerToString(distance);
		}

		else {
			LCD_intgerToString(distance);
			/* In case the digital value is two or one digits print space in the next digit place*/
			LCD_displayCharacter(' ');
		}
	}

}

