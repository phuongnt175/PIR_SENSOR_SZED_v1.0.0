 /* File name: pir.c
 *
 * Description:
 *
 *
 * Last Changed By:  $Author: $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $May 17, 2023
 *
 * Code sample:
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "app/framework/include/af.h"
#include "gpiointerrupt.h"
#include "stddef.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "pir.h"

/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

pirControl pirCallbackFunc = NULL;
PirActtionHandler_e pirState;
EmberEventControl pirDetectEventControl;
bool g_status = false;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

void pirINTSignalHandle(uint8_t byPin);
bool isMotionSignal(void);

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/
/**
 * @func		PIR_Init
 * @brief		PIR init
 * @param		pirControl
 * @retval		None
 */
void pirInit(pirControl pirHandler)
{
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIOINT_Init();
	GPIO_PinModeSet(PIR_PORT, PIR_PIN, gpioModeInput, 0); // Register callbacks before setting up and enabling pin interrupt
	GPIOINT_CallbackRegister(PIR_PIN, pirINTSignalHandle);
	pirEnable(true);
	pirCallbackFunc = pirHandler;
}

/**
 * @func		pirEnable
 * @brief		PIR Enable
 * @param		boolean
 * @retval		None
 */
void pirEnable(boolean boEnable)
{
	if(boEnable){
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
void pirINTSignalHandle(uint8_t byPin)
{
	emberAfCorePrintln("PIR_INTSignalHandle");
	g_status = true;
	if(byPin != PIR_PIN){
		return;
	}

	if(isMotionSignal()){
		pirState = PIR_STATE_DEBOUNCE;
		pirEnable(false);
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
				if(pirCallbackFunc != NULL){
					pirCallbackFunc(PIR_MOTION);
				}
				emberEventControlSetDelayMS(pirDetectEventControl,5000);
			}else {
				pirEnable(true);
			}
			break;

		case PIR_STATE_WAIT_5S:
			{
				g_status = false;
				pirState = PIR_STATE_WAIT_30S;
				pirEnable(true);
				emberEventControlSetDelayMS(pirDetectEventControl, 30000);
			}
			break;

		case PIR_STATE_WAIT_30S:
			{
				if(pirCallbackFunc != NULL){
					emberAfCorePrintln("PIR_DETECT_UNMOTION");
					pirCallbackFunc(PIR_UNMOTION);
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

	if(g_status == true){
		isMotion = true;
	}else {
		isMotion = false;
	}

	return isMotion;
}
