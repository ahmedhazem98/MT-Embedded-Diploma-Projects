/******************************************************************************
 *
 * Module: ADC
 *
 * File Name: adc.h
 *
 * Description: Header file for the AVR ADC driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#ifndef ADC_H_
#define ADC_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define ADC_MAXIMUM_VALUE    1023

#define ADC_ADJUST_RIGHT    0x00
#define ADC_ADJUST_LEFT     0x01

#define ADC_CHANNEL_0       0x00
#define ADC_CHANNEL_1       0x01
#define ADC_CHANNEL_2       0x02
#define ADC_CHANNEL_3       0x03
#define ADC_CHANNEL_4       0x04
#define ADC_CHANNEL_5       0x05
#define ADC_CHANNEL_6       0x06
#define ADC_CHANNEL_7       0x07

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	ADC_PRESCALER_2,ADC_PRESCALER_4 = 2,ADC_PRESCALER_8,ADC_PRESCALER_16,ADC_PRESCALER_32,ADC_PRESCALER_64,ADC_PRESCALER_128
}ADC_Prescaler;

typedef enum
{
	ADC_REF_OFF,ADC_REF_5V,ADC_REF_RESERVED,ADC_REF_2P56V
}ADC_RefrenceVoltage;

typedef struct {
    uint8 ref_volt;           // Reference voltage selection
    uint8 adjustResult;        // Result adjustment (left or right)
    uint8 prescaler;  		  // ADC clock prescaler value
} ADC_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialize the ADC driver.
 */
void ADC_init(const ADC_ConfigType * configPtr);
/*
 * Description :
 * Function responsible for read analog data from a certain ADC channel
 * and convert it to digital using the ADC driver.
 */
uint16 ADC_readChannel(uint8 channel);

#endif /* ADC_H_ */
