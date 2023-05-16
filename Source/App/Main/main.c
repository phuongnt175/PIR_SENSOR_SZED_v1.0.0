/*
 * main.c
 *
 *  Created on: Mar 30, 2023
 *      Author: admin1
 */

#include "app/framework/include/af.h"
#include "Source/Mid/Led/led.h"
#include "protocol/zigbee/stack/include/binding-table.h"
#include "Source/App/Network/network.h"
#include "Source/App/Send/send.h"
#include "Source/Mid/PIR/pir.h"
#include "main.h"


bool boNetworkReady = false;
systemState_e systemState = POWER_ON_STATE;

void Main_networkEventHandler(uint8_t networkResult);
void Main_PIREventHandler(uint8_t pirAction);
/* Event **************************************************************/
EmberEventControl mainStateEventControl;
/*
 * * @brief Main Init
 */
void emberAfMainInitCallback(void)
{
	emberAfCorePrintln("Main Init");
	ledInit();
	Network_Init(Main_networkEventHandler);
	PIR_Init(Main_PIREventHandler);
	emberEventControlSetActive(mainStateEventControl);
}


/****************************EVENT HANDLER MIDDER********************************************************************/
/*
 * @func	Main_PIREventHandler
 * @brief	Pir event handler
 * @param	uint8_t
 * @retval	None
 */
void Main_PIREventHandler(uint8_t pirAction)
{
	static boolean sendFlag = true;
	switch (pirAction) {

		case PIR_MOTION:
			if(sendFlag){
				sendFlag = false;
				SEND_PIRStateReport(2,PIR_MOTION);
			}
			toggleLed(LED1,ledPink,1,150,150);
			break;

		case PIR_UNMOTION:
			sendFlag = true;
			toggleLed(LED1,ledRed,1,150,150);
			emberAfCorePrintln("PIR_UNMOTION");
			SEND_PIRStateReport(2,PIR_UNMOTION);
			break;

		default:
			break;
	}
}

/*
 * @func	mainStateEventHandler
 * @brief	Main state event handler
 * @param	None
 * @retval	None
 */
void mainStateEventHandler(void)
{
	emberEventControlSetInactive(mainStateEventControl);
	EmberNetworkStatus nwkStatusCurrent;
	switch (systemState) {

		case POWER_ON_STATE:
			nwkStatusCurrent = emberAfNetworkState();
			if(nwkStatusCurrent == EMBER_NO_NETWORK){
				toggleLed(LED1,ledRed,3,200,200);
				NETWORK_FindAndJoin();
			}
			systemState = IDLE_STATE;
			break;

		case REPORT_STATE:
			systemState = IDLE_STATE;
			SEND_ReportInfoHc();
			break;

		case IDLE_STATE:
			break;

		case REBOOT_STATE:
			systemState = IDLE_STATE;
			EmberNetworkStatus networkStatus = emberAfNetworkState();
			if (networkStatus != EMBER_NO_NETWORK) {
				SendZigDevRequest();
				emberClearBindingTable();
				emberLeaveNetwork();
			} else {
				halReboot();
			}
			break;

		default:
			break;
	}

}

/*
 * @func	Main_networkEventHandler
 * @brief	Main network event handler
 * @param	uint8_t
 * @retval	None
 */
void Main_networkEventHandler(uint8_t networkResult)
{
	emberAfCorePrintln("Network Event Handle");
	switch (networkResult) {

		case NETWORK_HAS_PARENT:
			emberAfCorePrintln("Network has parent");
			toggleLed(LED1,ledPink,3,300,300);
			boNetworkReady = true;
			systemState = REPORT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;

		case NETWORK_JOIN_FAIL:
			systemState = IDLE_STATE;
			emberAfCorePrintln("Network Join Fail");
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;

		case NETWORK_JOIN_SUCCESS:
			emberAfCorePrintln("Network Join Success");
			toggleLed(LED1,ledPink,3,300,300);
			boNetworkReady =true;
			systemState = REPORT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;

		case NETWORK_LOST_PARENT:
			emberAfCorePrintln("Network lost parent");
			toggleLed(LED1,ledPink,3,300,300);
			systemState = IDLE_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;

		case NETWORK_OUT_NETWORK:
			if(boNetworkReady){
				emberAfCorePrintln("Network Out network");
				toggleLed(LED1,ledPink,3,300,300);
				systemState = REBOOT_STATE;
				emberEventControlSetDelayMS(mainStateEventControl, 3000);
			}
			break;

		default:
			break;
	}
}
