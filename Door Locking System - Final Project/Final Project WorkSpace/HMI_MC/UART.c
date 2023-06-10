 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: UART.c
 *
 * Description: Source file for UART driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#include "UART.h"
#include <avr/io.h>
#include "common_macros.h"



/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType *Config_Ptr)
{
	uint16 ubrr_value = 0;

	/*Enable UART Double speed mode*/
	UCSRA = (1<<U2X);

	/*
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * TXEN  = 1 Transmitter Enable
	 * UCSZ2 = 0 For (5,6,7,8) bit data mode
	 * RXB8 & TXB8 not used for (5,6,7,8) bit data mode
	 */
	UCSRB = (1<<RXEN) | (1<<TXEN);


	/*
	 * URSEL 	= 1 Accessing UCSRC Register
	 * UMSEL 	= 0 Asynchronous mode select
	 * UPM1:0 	= Parity data-type in Configuration structure
	 * USBS		= Stop bits data-type in Configuration structure
	 * UCSZ1:0	= Data bits data-type in Configuration structure
	 */
	UCSRC = (1<<URSEL) | ((Config_Ptr->parity)<<UPM0) | ((Config_Ptr->stop_bit)<<USBS) | ((Config_Ptr->bit_data)<<UCSZ0);

	/* BaudRate = (F_CPU) / 8(UBRR + 1)		"UART Double speed mode"
	 * UBRR = ((F_CPU) / (BaudRate * 8)) - 1
	 */
	ubrr_value = (uint16)(F_CPU / ((Config_Ptr->baud_rate) * 8UL) - 1);


	UBRRH = (ubrr_value>>8);		/*UBRR-High = ubrr_value shifted right by 8 bits*/
	UBRRL = ubrr_value;				/*UBRR-Low = first 8 bits of ubrr_value*/
}



/*
 * Description :
 * Function responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data)
{
	/*Wait until the buffer is empty and ready to take new data frame*/
	while(BIT_IS_CLEAR(UCSRA, UDRE));

	UDR = data;
}



/*
 * Description :
 * Function responsible for receive byte from another UART device.
 */
uint8 UART_receiveByte(void)
{
	while(BIT_IS_CLEAR(UCSRA, RXC));

	return UDR;
}


/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	/* Send the whole string */
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
}


/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = UART_receiveByte();

	/* Receive the whole string until the '#' */
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_receiveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}
