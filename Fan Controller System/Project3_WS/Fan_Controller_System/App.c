/******************************************************************************
 *
 * File Name: App.c
 *
 * Description: Application source File
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#include "gpio.h"
#include "adc.h"
#include "lcd.h"
#include "lm35_sensor.h"
#include "motor.h"
#include "pwm.h"

int main(void) {

	uint8 temp;
	/* Initializing configuration for ADC */
	ADC_ConfigType adcConfig;
	adcConfig.prescaler = ADC_PRESCALER_8;
	adcConfig.ref_volt = ADC_REF_2P56V;
	adcConfig.adjustResult = 0;
	/* initialize LCD driver */
	LCD_init();
	/* initialize ADC driver */
	ADC_init(&adcConfig);
	/* initialize MOTOR driver */
	DC_Motor_init();
	/* Display the State of the Fan */
	LCD_displayString("FAN IS OFF");
	/* Display this string "Temp =   C" only once on LCD at the Second row */
	LCD_moveCursor(1,0);
	LCD_displayString("Temp =    C");

	while(1)
	{
    	temp = LM35_getTemperature();
    	/* If temperature is less than 30 C ==> Turn OFF the Fan */
		if(temp < 30)
		{
			DcMotor_Rotate(MOTOR_STOP,0);
			LCD_moveCursor(0,0);
			LCD_displayString("FAN IS OFF");
			LCD_moveCursor(1,7);
			LCD_intgerToString(temp);

		}
		/* If temperature is more than 30 C ==> Turn ON the Fan with speed = 25% */
		else if(temp >= 30 && temp < 60)
		{
			DcMotor_Rotate(MOTOR_CW,25);
			LCD_moveCursor(0,0);
			LCD_displayString("FAN IS ON ");
			LCD_moveCursor(1,7);
			LCD_intgerToString(temp);
			LCD_displayCharacter(' ');
		}
		/* If temperature is more than 60 C ==> Turn ON the Fan with speed = 50% */
		else if (temp >= 60 && temp < 90)
		{
			DcMotor_Rotate(MOTOR_CW,50);
			LCD_moveCursor(0,0);
			LCD_displayString("FAN IS ON ");
			LCD_moveCursor(1,7);
			LCD_intgerToString(temp);
			LCD_displayCharacter(' ');
		}
		/* If temperature is more than 90 C ==> Turn ON the Fan with speed = 75% */
		else if (temp >= 90 && temp < 120)
		{
			DcMotor_Rotate(MOTOR_CW,75);
			LCD_moveCursor(0,0);
			LCD_displayString("FAN IS ON ");
			LCD_moveCursor(1,7);
			if (temp >= 100)
				LCD_intgerToString(temp);
			else{
				LCD_intgerToString(temp);
				LCD_displayCharacter(' ');
				}

		}
		/* If temperature is more than 120 C ==> Turn ON the Fan with speed = 100% */
		else if(temp >= 120)
		{
			DcMotor_Rotate(MOTOR_CW,100);
			LCD_moveCursor(0,0);
			LCD_displayString("FAN IS ON ");
			LCD_moveCursor(1,7);
			LCD_intgerToString(temp);

		}


	}

}
