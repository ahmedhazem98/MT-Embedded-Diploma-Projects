 /******************************************************************************
 *
 * Module: ADC
 *
 * File Name: adc.c
 *
 * Description: Source file for the AVR ADC driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#include "avr/io.h"
#include "adc.h"
#include "common_macros.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void ADC_init(const ADC_ConfigType * configPtr)
{
    /* Set the reference voltage and adjust the result left or right */
	ADMUX = (configPtr->ref_volt << REFS0) | (configPtr->adjustResult << ADLAR);
    /* Enable the ADC and set the prescaler value */
	ADCSRA = (1<<ADEN)  | (configPtr->prescaler<<ADPS0);

}

uint16 ADC_readChannel(uint8 channel)
{
    /* Configure the channel */
	ADMUX = (ADMUX & 0xE0) | (channel & 0x07);
    /* Start the conversion */
	ADCSRA |= (1<<ADSC);
    /* Wait for the conversion to complete */
	while(BIT_IS_CLEAR(ADCSRA,ADIF));
	/* Clearing the Interrupt Flag */
	ADCSRA |= (1<<ADIF);
    /* Return the result */
	return ADC;
}
