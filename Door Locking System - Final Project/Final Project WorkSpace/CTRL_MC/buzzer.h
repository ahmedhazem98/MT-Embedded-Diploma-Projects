 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for Buzzer driver
 *
 * Author: Ahmed Hazem
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT		PORTC_ID
#define BUZZER_PIN		PIN5_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function responsible for Initializing the buzzer pin output.
 */
void Buzzer_init();
/*
 * Description :
 * Function responsible for Turning ON the buzzer.
 */
void Buzzer_on();
/*
 * Description :
 * Function responsible for Turning OFF the buzzer.
 */
void Buzzer_off();


#endif /* BUZZER_H_ */
