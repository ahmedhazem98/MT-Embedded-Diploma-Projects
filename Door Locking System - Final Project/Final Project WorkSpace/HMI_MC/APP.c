/******************************************************************************
 *
 * Module: HMI App
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
#include "keypad.h"
#include "lcd.h"
#include "timer.h"

/******************************************************************************
 *                           Definitions and Variables
 ******************************************************************************/

/* Password-related constants */
#define PASSWORD_LENGTH 5
#define PASSWORD_MATCH 1
#define PASSWORD_UNMATCH 0

/* Communication-related constants */
#define HMI_READY_UART 0x00
#define CONTROL_READY_UART 0xFF

/* Door-related constants */
#define DOOR_OPEN_TIME 15
#define DOOR_HOLD_TIME 3
#define DOOR_CLOSE_TIME 15

/* Alarm-related constants */
#define ALARM_TIME 60

/* Global variables */
uint8_t volatile g_ticks; // global variable for timer
uint8_t Trials = 0; // global variable for incorrect password count

/******************************************************************************
 *                           Function Prototypes
 ******************************************************************************/

void create_password(void); // function to create a password
void open_door(void); // function to open the door
void change_password(void); // function to change the password
void activate_alarm_mode(void); // function to activate the alarm mode
void timer_callback_function(void); // callback function for timer
void mainMenu();

/******************************************************************************
 *                           Main Function
 ******************************************************************************/

int main(void) {
	/* Initialize UART with 8bits mode, no parity bit, 1 stop bit and 9600 baud rate */
	UART_ConfigType uart_config = { Bits_8,
									DISABLED,
									ONE_BIT,
									9600 };
	/* Initialize Timer1 with 0 initial value, 23437 compare value, prescalar of 1024 and CTC mode */
	Timer1_ConfigType g_timer1_config = { 0,
										  23437,
										  F_CPU_1024,
										  COMPARE_MODE };
	UART_init(&uart_config);
	Timer1_init(&g_timer1_config);
	Timer1_setCallBack(&timer_callback_function);
	LCD_init();
	SREG |= (1 << 7);

	/* Creating a password */
	LCD_clearScreen();
	create_password();

	while (1) {
		mainMenu();
	}
}

/******************************************************************************
 *                           Function Definitions
 ******************************************************************************/

/*
 * Function: create_password
 * -------------------------
 * Prompts the user to enter a new password twice and checks if the two entries
 * match. If the entries match, sends the new password to the control unit and
 * displays a message indicating that the password has been created. If the
 * entries do not match, displays an error message and allows the user to try
 * again.
 *
 * Parameters: None
 *
 * Returns: None
 */
void create_password(void) {
	uint8 firstPassword[PASSWORD_LENGTH + 2];
	uint8 secondPassword[PASSWORD_LENGTH + 2];
	uint8 matchCheck;
	uint8 i = 0;
	uint8 keyPressed;
	/* Prompting the user to enter a new password */
	LCD_clearScreen();
	LCD_displayString("Enter Password:");
	LCD_moveCursor(1, 0);

	for (i = 0; i < PASSWORD_LENGTH; i++) {
		/* Getting the user input */
		keyPressed = KEYPAD_getPressedKey();
		/* Displaying an asterisk to mask the password */
		if (keyPressed >= 0 && keyPressed <= 9) {
			LCD_displayString("*");
			firstPassword[i] = keyPressed;
			_delay_ms(500);
		}
	}
	firstPassword[PASSWORD_LENGTH] = '#';
	firstPassword[PASSWORD_LENGTH + 1] = '\0';
	while (KEYPAD_getPressedKey() != '=');

	LCD_clearScreen();
	_delay_ms(500);
	/* Prompting the user to re-enter the new password */
	LCD_displayString("Re-Enter Pass:");
	LCD_moveCursor(1, 0);
	/* Getting the user input */
	for (i = 0; i < PASSWORD_LENGTH;) {
		keyPressed = KEYPAD_getPressedKey();
		if (keyPressed >= 0 && keyPressed <= 9) {
			/* Displaying an asterisk to mask the password */
			LCD_displayString("*");
			secondPassword[i] = keyPressed;
			_delay_ms(500);
			i++;
		}
	}
	secondPassword[PASSWORD_LENGTH] = '#';
	secondPassword[PASSWORD_LENGTH + 1] = '\0';
	while (KEYPAD_getPressedKey() != '=');

	while (UART_receiveByte() != CONTROL_READY_UART);/* waiting for a ready signal from the control unit */
	UART_sendString(firstPassword);
	while (UART_receiveByte() != CONTROL_READY_UART);/* waiting for a ready signal from the control unit */
	UART_sendString(secondPassword);
	LCD_clearScreen();
	_delay_ms(1000);

	UART_sendByte(HMI_READY_UART);	/* sending a ready signal to the control unit */

	matchCheck = UART_receiveByte();

	if (matchCheck) {
		/* Displaying a message to indicate that the password has been created */
		LCD_clearScreen();
		LCD_displayString("Pass Created");
		_delay_ms(1000);

	}

	else {
		/* Displaying an error message and prompting the user to enter the password again */
		LCD_clearScreen();
		LCD_displayString("Not matched");
		_delay_ms(1000);
		create_password();
	}

}

/*
 * Function: open_door
 * -------------------
 * Prompts the user to enter a password and checks if it matches the stored
 * password. If the password is correct, opens the door. If the password is
 * not correct, displays an error message and allows the user to try again
 * up to three times before activating the alarm mode.
 *
 * Parameters: None
 *
 * Returns: None
 */

void open_door(void) {
	uint8 password[PASSWORD_LENGTH + 2];
	uint8 matchCheck;
	uint8 i;
	uint8 keyPressed;

	/* Prompting the user to enter a new password */
	LCD_clearScreen();
	LCD_displayString("Enter Password:");
	LCD_moveCursor(1, 0);
	for (i = 0; i < PASSWORD_LENGTH;) {
		keyPressed = KEYPAD_getPressedKey();
		if (keyPressed >= 0 && keyPressed <= 9) {
			/* Displaying an asterisk to mask the password */
			LCD_displayString("*");
			password[i] = keyPressed;
			_delay_ms(500);
			i++;
		}
	}
	password[PASSWORD_LENGTH] = '#';
	password[PASSWORD_LENGTH + 1] = '\0';
	while (KEYPAD_getPressedKey() != '=');

	UART_sendByte(HMI_READY_UART);	/* sending a ready signal to the control unit */
	while (UART_receiveByte() != CONTROL_READY_UART);/* waiting for a ready signal from the control unit */

	UART_sendString(password); /* sending the password to the control unit */

	UART_sendByte(HMI_READY_UART);	/* sending a ready signal to the control unit */
	while (UART_receiveByte() != CONTROL_READY_UART);/* waiting for a ready signal from the control unit */

	matchCheck = UART_receiveByte(); /* receiving the result of the password match check from the control unit */

		if (matchCheck) {
			Trials = 0;
			/* Open the door for 15 Secs */
			g_ticks = 0;
			LCD_clearScreen();
			LCD_displayString("Door Opening");
			while (g_ticks < DOOR_OPEN_TIME/3);	/* waiting for the door opening time */
			/* Hold the door for 3 Secs */
			g_ticks = 0;
			LCD_clearScreen();
			LCD_displayString("Door Open");
			while (g_ticks < DOOR_HOLD_TIME/3);	/* waiting for the door hold time */
			/* Close the door for 15 Secs */
			g_ticks = 0;
			LCD_clearScreen();
			LCD_displayString("Door Closing");
			while (g_ticks < DOOR_CLOSE_TIME/3); /* waiting for the door closing time */
		}

		else {
			LCD_clearScreen();
			LCD_displayString("Pass Incorrect");

			_delay_ms(1000);

			Trials++;

			if (Trials == 3)
				activate_alarm_mode(); /* activating the alarm mode if the maximum number of trials has been reached */

			else
				open_door(); /* allowing the user to try again if the maximum number of trials has not been reached */
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
void change_password(void) {

	uint8 oldPassword[PASSWORD_LENGTH + 2];
	uint8 matchCheck;
	uint8 i;
	uint8 keyPressed;

	LCD_clearScreen();
	LCD_displayString("Enter Old Pass"); /* prompting the user to enter the old password */
	LCD_moveCursor(1, 0);

	for (i = 0; i < PASSWORD_LENGTH;) {
		keyPressed = KEYPAD_getPressedKey(); /* getting the key pressed by the user */
		if (keyPressed >= 0 && keyPressed <= 9) {
			LCD_displayString("*"); /* displaying an asterisk to mask the password */
			oldPassword[i] = keyPressed;
			_delay_ms(500);
			i++;
		}
	}

	oldPassword[PASSWORD_LENGTH] = '#';
	oldPassword[PASSWORD_LENGTH + 1] = '\0';
	while (KEYPAD_getPressedKey() != '=');

	UART_sendByte(HMI_READY_UART); /* sending a ready signal to the control unit */
	while (UART_receiveByte() != CONTROL_READY_UART); /* waiting for a ready signal from the control unit */

	UART_sendString(oldPassword); /* sending the old password to the control unit */

	UART_sendByte(HMI_READY_UART); /* sending a ready signal to the control unit */
	while (UART_receiveByte() != CONTROL_READY_UART); /* waiting for a ready signal from the control unit */

	matchCheck = UART_receiveByte(); /* receiving the result of the password match check from the control unit */

	if (matchCheck) {
		LCD_clearScreen();
		LCD_displayString("Pass Correct");
		_delay_ms(1000);

		create_password(); /* prompting the user to create a new password */
	}

	else {
		LCD_clearScreen();
		LCD_displayString("Pass Incorrect");
		_delay_ms(1000);

		Trials++; /* incrementing the number of password change trials */

		if (Trials == 3)
			activate_alarm_mode(); /* activating the alarm mode if the maximum number of trials has been reached */

		else
			change_password(); /* allowing the user to try again if the maximum number of trials has not been reached */
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
void activate_alarm_mode(void) {
	/* Displaying a message to indicate that the alarm has been activated */
	LCD_clearScreen();
	LCD_displayString("ALARM ACTIVATED!");
	_delay_ms(ALARM_TIME);

	/* Sending a ready signal to the control unit */
	UART_sendByte(HMI_READY_UART);
	while (UART_receiveByte() != CONTROL_READY_UART);

	/* Sending a signal to the control unit to deactivate the alarm */
	UART_sendByte(0xAA);

	/* Resetting the incorrect password count */
	Trials = 0;
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
void timer_callback_function(void) {
	g_ticks++;
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
	uint8_t key_pressed;

			/* Displaying options for the user */
			LCD_clearScreen();
			LCD_displayString("+ : Open Door");
			LCD_displayStringRowColumn(1, 0, "- : Change Pass");

			/* Getting the user input */
			key_pressed = KEYPAD_getPressedKey();

			/* Sending a ready signal to the control unit */
			UART_sendByte(HMI_READY_UART);
			while (UART_receiveByte() != CONTROL_READY_UART)
				;

			/* Sending the user input to the control unit */
			UART_sendByte(key_pressed);

			/* Handling the user input */
			if (key_pressed == '+') {
				LCD_clearScreen();
				open_door();
			} else if (key_pressed == '-') {
				LCD_clearScreen();
				change_password();
			}
}
