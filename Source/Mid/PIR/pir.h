/*
 * pir.h
 *
 *  Created on: Apr 3, 2023
 *      Author: admin1
 */

#ifndef SOURCE_MID_PIR_PIR_H_
#define SOURCE_MID_PIR_PIR_H_

#define PIR_PORT			gpioPortC
#define PIR_PIN				(4U)

typedef enum
{
	PIR_UNMOTION,
	PIR_MOTION,
	PIR_STATE_DEBOUNCE,
	PIR_STATE_WAIT_5S,
	PIR_STATE_WAIT_30S
}pirActtionHandler_e;

typedef void(*pirControl)(pirActtionHandler_e pirEvent);

void PIR_Init(pirControl PirHandler);
void PIR_Enable(boolean enable);

#endif /* SOURCE_MID_PIR_PIR_H_ */
