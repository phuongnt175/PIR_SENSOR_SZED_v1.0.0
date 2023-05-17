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
}PirActtionHandler_e;

typedef void(*pirControl)(PirActtionHandler_e pirEvent);

void pirInit(pirControl pirHandler);
void pirEnable(boolean boEnable);

#endif /* SOURCE_MID_PIR_PIR_H_ */
