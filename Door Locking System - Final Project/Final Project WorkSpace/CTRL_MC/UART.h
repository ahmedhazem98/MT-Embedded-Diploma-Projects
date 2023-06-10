 /******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.h
 *
 * Description: Header file for Timer driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/*Data Bits possible values*/
typedef enum
{
	Bits_5, Bits_6, Bits_7, Bits_8
}UART_BitData;


/*Parity bit possible states*/
typedef enum
{
	DISABLED, EVEN_PARITY = 2, ODD_PARITY
}UART_Parity;


/*Number of stop bits in UART frame*/
typedef enum
{
	ONE_BIT, TWO_BITS
}UART_StopBit;


/*BaudRate standard values:
 * 10, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000, 256000 bps.
 */
typedef uint32 UART_BaudRate;


/*Configuration Structure*/
typedef struct{
 UART_BitData bit_data;
 UART_Parity parity;
 UART_StopBit stop_bit;
 UART_BaudRate baud_rate;
}UART_ConfigType;



/*******************************************************************************
 *                           Function Proto-types                              *
 *******************************************************************************/



/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType *Config_Ptr);



/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);



/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_receiveByte(void);



/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);



/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str);

#endif /* UART_H_ */
