/******************************************************************************
 *
 * Module: CTRL App
 *
 * File Name: App.c
 *
 * Description: source file for Application
 *
 * Author: Ahmed Hazem
 *
 ******************************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "UART.h"
#include "twi.h"
#include "timer.h"
#include "motor.h"
#include "buzzer.h"
#include "external_eeprom.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* Password-related constants */
#define PASSWORD_LEGTH		5
#define PASSWORD_MATCH			TRUE
#define PASSWORD_UNMATCH		FALSE
/* Communication-related constants */
#define HMI_READY_UART			0x00
#define CONTROL_READY_UART		0xFF
#define EEPROM_PASS_ADDRESS		0x0300
/* Door-related constants */
#define DOOR_OPEN_TIME 15
#define DOOR_HOLD_TIME 3
#define DOOR_CLOSE_TIME 15
/* Alarm-related constants */
#define ALARM_TIME				60

/*******************************************************************************
 *                               Global-Variables                              *
 *******************************************************************************/

uint8 volatile g_ticks;
uint8 Trials = 0;

/*******************************************************************************
 *                             Functions Prototypes                            *
 *******************************************************************************/

void create_password(void);
void open_door(void);
void change_password(void);
void activate_alarm_mode(void);
void Timer_CallBackFunction(void);
uint8 check_password(uint8 *pass1 , uint8 *pass2);
void save_password(uint8 *pass);
uint8 check_saved_password(uint8 *pass_entered);
void mainMenu();


/*******************************************************************************
 *                            Main Function                                    *
 *******************************************************************************/

int main(void)
{
	/* Initialize UART with 8bits mode, no parity bit, 1 stop bit and 9600 baud rate */
	UART_ConfigType UART_config = {Bits_8,
									DISABLED,
									ONE_BIT,
									9600};
	/* Initialize I2C with No prescalar mode and 400 kbps bit rate */
	TWI_ConfigType TWI_conf={
						TWI_SLAVE_ADDRESS,
						400,
						NO_PRESCALER
				};
	/* Initialize Timer1 with 0 initial value, 23437 compare value, prescalar of 1024 and CTC mode */
	Timer1_ConfigType TIMER1_config = {0,
									   23437,
									   F_CPU_1024,
										COMPARE_MODE};

	/* Initialize modules and enable global interrupts */
	Timer1_init(&TIMER1_config);
	Timer1_setCallBack(&Timer_CallBackFunction);
	UART_init(&UART_config);
	TWI_init(&TWI_conf);
	DC_Motor_init();
	Buzzer_init();
	SREG |= (1<<7);

	/* Create the password */
	create_password();
	/* Program Menu function */
	while(1)
	{
		mainMenu();
	}
}
/*
 * Function: create_password
 * ----------------------------------
 * The create_password() function prompts the user to enter a new password and then compares
 *  the entered passwords to ensure that they match. If the passwords match, the password is saved
 *  to the EEPROM.
 *
 * Parameters: None
 *
 * Returns: None
 */
void create_password(void)
{

	uint8 firstPassword[PASSWORD_LEGTH + 1];
	uint8 secondPassword[PASSWORD_LEGTH + 1];
	uint8 matchCheck;

	/* Wait for a "ready" signal from the HMI */
	UART_sendByte(CONTROL_READY_UART);
	UART_receiveString(firstPassword);

	/* Send a "ready" signal back to the HMI */
	UART_sendByte(CONTROL_READY_UART);

	/* Receive the second password from the HMI */
	UART_receiveString(secondPassword);

	/* Check if the passwords match */
	matchCheck = check_password(firstPassword, secondPassword);

	/* Wait for a "ready" signal from the HMI */
	while(UART_receiveByte() != HMI_READY_UART){}

	/* If the passwords match, save the password to EEPROM */
	if(matchCheck)
	{
		UART_sendByte(PASSWORD_MATCH);
		save_password(firstPassword);
	}
	/* If the passwords do not match, prompt the user to enter a new password */
	else
	{
		UART_sendByte(PASSWORD_UNMATCH);
		create_password();
	}
}
/*
 * Function: open_door
 * ----------------------------------
 * The open_door() function prompts the user to enter their password and checks if it matches
 *  the saved password. If the password is correct, the door is opened for a preset amount of time.
 *  If the password is incorrect, the function prompts the user to enter their password again.
 *
 * Parameters: None
 *
 * Returns: None
 */
void open_door(void)
{
	uint8 password[PASSWORD_LEGTH + 1];
	uint8 matchCheck;
	/* Wait for a signal from the HMI that it is ready to receive data */
	while(UART_receiveByte() != HMI_READY_UART);
	UART_sendByte(CONTROL_READY_UART);
	/* Receive the user's password from the HMI */
	UART_receiveString(password);
	/* Check if the entered password matches the saved password */
	matchCheck = check_saved_password(password);
	/* Wait for a signal from the HMI that it is ready to receive data */
	while(UART_receiveByte() != HMI_READY_UART);
	UART_sendByte(CONTROL_READY_UART);
	/* If the passwords match, open the door */
	if(matchCheck)
	{
		UART_sendByte(PASSWORD_MATCH);
		Trials = 0;
		/* Open the door for 15 Secs */
		g_ticks = 0;
		DcMotor_Rotate(MOTOR_CW,50);
		while(g_ticks < DOOR_OPEN_TIME/3);	/* waiting for the door opening time */
		/* Hold the door for 3 Secs */
		g_ticks = 0;
		DcMotor_Rotate(MOTOR_STOP,0);
		while(g_ticks < DOOR_HOLD_TIME/3); /* waiting for the door hold time */
		/* Close the door for 15 Secs */
		g_ticks = 0;
		DcMotor_Rotate(MOTOR_ACW,50);
		while(g_ticks < DOOR_CLOSE_TIME/3); /* waiting for the door closing time */

		DcMotor_Rotate(MOTOR_STOP,0);
	}

	else
	{
		UART_sendByte(PASSWORD_UNMATCH);
		Trials++;

		if(Trials == 3)
		{
			activate_alarm_mode(); /* activating the alarm mode if the maximum number of trials has been reached */
		}

		else
		{
			open_door(); /* allowing the user to try again if the maximum number of trials has not been reached */
		}
	}

}
/*
 * Function: change_password
 * ------------------------
 * Prompts the user to enter their old password and checks if it matches the
 * stored password. If the old password is correct, prompts the user to enter a
 * new password and sends it to the control unit. If the old password is not
 * correct, displays an error message and allows the user to try again up to
 * three times before activating the alarm mode.
 *
 * Parameters: None
 *
 * Returns: None
 */
void change_password(void)
{
	uint8 password[PASSWORD_LEGTH + 1];
	uint8 matchCheck;
	while(UART_receiveByte() != HMI_READY_UART);
	UART_sendByte(CONTROL_READY_UART);
	UART_receiveString(password);
	matchCheck = check_saved_password(password);
	while(UART_receiveByte() != HMI_READY_UART);
	UART_sendByte(CONTROL_READY_UART);


	if(matchCheck)
	{
		UART_sendByte(PASSWORD_MATCH);
		Trials = 0;
		create_password();
	}

	else
	{

		UART_sendByte(PASSWORD_UNMATCH);
		Trials++;
		if(Trials == 3)
		{
			activate_alarm_mode();
		}

		else
		{
			change_password();
		}
	}

}
/*
 * Function: activate_alarm_mode
 * -----------------------------
 * Activates the alarm mode by displaying a message on the LCD screen and sending
 * a signal to the control unit to activate the alarm. After a specified delay,
 * sends a signal to the control unit to deactivate the alarm and resets the
 * incorrect password count.
 *
 * Parameters: None
 *
 * Returns: None
 */
void activate_alarm_mode(void)
{

	Buzzer_on();
	g_ticks = 0;
	while(g_ticks < ALARM_TIME);

}
/*
 * Function: timer_callback_function
 * ----------------------------------
 * A callback function that increments a global variable `g_ticks` by one every time
 * it is called. This function is used as a timer in the system.
 *
 * Parameters: None
 *
 * Returns: None
 */
void Timer_CallBackFunction(void)
{
	g_ticks++;
}
/*
 * Function: check_password
 * ------------------------
 * The user enters two passwords and the function checks if they are equal. if both are equal a Password created
 * message will be displayed and they will be then saved to the eeprom memory later. if they are equal it will
 * return true.
 *
 * Parameters: uint8*,uint8*
 *
 * Returns: uint8
 */
uint8 check_password(uint8 *firstPassword , uint8 *secondPassword)
{
	uint8 matchCheck = 1;
	uint8 i = 0;
	while(firstPassword[i] != '\0' || secondPassword[i] != '\0')
	{
		if(firstPassword[i] != secondPassword[i])
		{
			matchCheck = FALSE;
			break;
		}
		i++;
	}
	return matchCheck;
}
/*
 * Function: save_password
 * ----------------------------------
 * Function is responsible for saving the password taken as an input in the eeprom memory.
 *
 * Parameters: uint8*
 *
 * Returns: None
 */
void save_password(uint8 *pass)
{
	uint8 i;
	for(i = 0 ; i < PASSWORD_LEGTH ; i++)
	{
		EEPROM_writeByte(EEPROM_PASS_ADDRESS + i, *(pass + i));
		_delay_ms(20);
	}
}
/*
 * Function: check_saved_password
 * ------------------------
 *
 * The function compares between the entered password and the saved password in the memory and returns true
 * if they are equal.
 *
 * Parameters: uint8*
 *
 * Returns: uint8
 */
uint8 check_saved_password(uint8 *enteredPassword)
{
	uint8 i;
	uint8 savedPassword[PASSWORD_LEGTH];
	uint8 matchCheck = 1;

	for(i = 0 ; i < PASSWORD_LEGTH ; i++)
	{
		EEPROM_readByte((EEPROM_PASS_ADDRESS + i), &savedPassword[i]);
		_delay_ms(20);
	}

	for(i = 0 ; i < PASSWORD_LEGTH ; i++)
	{
		if(savedPassword[i] != enteredPassword[i])
		{
			matchCheck = 0;
			break;
		}
	}

	return matchCheck;
}
/*
 * Function: mainMenu
 * ----------------------------------
 * Function is responsible for displaying the main menu options to the user,
 *  getting the user's input, and then directing the program flow accordingly.
 *
 * Parameters: None
 *
 * Returns: None
 */
void mainMenu(){

	uint8 operation;
	/* wait until the HMI is ready */
	while(UART_receiveByte() != HMI_READY_UART);
	/* Send a "ready" signal back to the HMI */
	UART_sendByte(CONTROL_READY_UART);
	/* Getting the user input */
	operation = UART_receiveByte();
	if(operation == '+')
		{
			open_door();
		}
	else if(operation == '-')
		{
			change_password();
		}
}

