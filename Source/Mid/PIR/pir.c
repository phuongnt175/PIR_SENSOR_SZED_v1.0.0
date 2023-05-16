/*
 * pir.c
 *
 *  Created on: Apr 3, 2023
 *      Author: admin1
 */

#include "app/framework/include/af.h"
#include "pir.h"
#include "gpiointerrupt.h"
#include "stddef.h"
#include "em_cmu.h"
#include "em_gpio.h"


pirControl PirCallbackFunc = NULL;
pirActtionHandler_e pirState;
EmberEventControl pirDetectEventControl;
bool status = false;

void PIR_INTSignalHandle(uint8_t pin);
bool isMotionSignal(void);

/**
 * @func		PIR_Init
 * @brief		PIR init
 * @param		pirControl
 * @retval		None
 */
void PIR_Init(pirControl PirHandler)
{
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIOINT_Init();
	GPIO_PinModeSet(PIR_PORT, PIR_PIN, gpioModeInput, 0); // Register callbacks before setting up and enabling pin interrupt
	GPIOINT_CallbackRegister(PIR_PIN, PIR_INTSignalHandle);
	PIR_Enable(true);
	PirCallbackFunc = PirHandler;
}

/**
 * @func		PIR_Enable
 * @brief		PIR Enable
 * @param		boolean
 * @retval		None
 */
void PIR_Enable(boolean enable)
{
	if(enable){
		GPIO_ExtIntConfig(PIR_PORT, PIR_PIN, PIR_PIN,
						true, false, true);
	}else {
		GPIO_ExtIntConfig(PIR_PORT, PIR_PIN, PIR_PIN,
							false, false, false);
	}
}

/**
 * @func		PIR_INTSignalHandle
 * @brief		PIR Int handle
 * @param		uint8_t
 * @retval		None
 */
void PIR_INTSignalHandle(uint8_t pin)
{
	emberAfCorePrintln("PIR_INTSignalHandle");
	status = true;
	if(pin != PIR_PIN){
		return;
	}

	if(isMotionSignal()){
		pirState = PIR_STATE_DEBOUNCE;
		PIR_Enable(false);
		emberEventControlSetInactive(pirDetectEventControl);
		emberEventControlSetDelayMS(pirDetectEventControl, 200);
	}
}

/**
 * @func		pirDetectEventHandler
 * @brief		PIR detect event handler
 * @param		None
 * @retval		None
 */
void pirDetectEventHandler(void)
{
	emberEventControlSetInactive(pirDetectEventControl);

	switch(pirState){

		case PIR_STATE_DEBOUNCE:
			if(isMotionSignal()){
				emberAfCorePrintln("PIR_DeTECT_MOTION");
				pirState = PIR_STATE_WAIT_5S;
				if(PirCallbackFunc != NULL){
					PirCallbackFunc(PIR_MOTION);
				}
				emberEventControlSetDelayMS(pirDetectEventControl,5000);
			}else {
				PIR_Enable(true);
			}
			break;

		case PIR_STATE_WAIT_5S:
			{
				status = false;
				pirState = PIR_STATE_WAIT_30S;
				PIR_Enable(true);
				emberEventControlSetDelayMS(pirDetectEventControl, 30000);
			}
			break;

		case PIR_STATE_WAIT_30S:
			{
				if(PirCallbackFunc != NULL){
					emberAfCorePrintln("PIR_DETECT_UNMOTION");
					PirCallbackFunc(PIR_UNMOTION);
				}
			}
			break;

		default:
			break;
	}
}

/**
 * @func		isMotionSignal
 * @brief		MotionSignal
 * @param		None
 * @retval		isMotion
 */
bool isMotionSignal(void)
{
	bool isMotion;

	if(status == true){
		isMotion = true;
	}else {
		isMotion = false;
	}

	return isMotion;
}
