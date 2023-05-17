 /* File name: main.c
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
#include "protocol/zigbee/stack/include/binding-table.h"
#include "Source/Mid/Led/led.h"
#include "Source/App/Network/network.h"
#include "Source/App/Send/send.h"
#include "Source/Mid/PIR/pir.h"
#include "main.h"

/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

bool g_boNetworkReady = false;
SystemState_e g_systemState = POWER_ON_STATE;
/* Event *********************************************************************/
EmberEventControl mainStateEventControl;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

void mainNetworkEventHandler(uint8_t byNetworkResult);
void mainPIREventHandler(uint8_t byPirAction);

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/
/*
 * * @brief Main Init
 */
void emberAfMainInitCallback(void)
{
	emberAfCorePrintln("Main Init");
	ledInit();
	networkInit(mainNetworkEventHandler);
	pirInit(mainPIREventHandler);
	emberEventControlSetActive(mainStateEventControl);
}


/****************************EVENT HANDLER MIDDER********************************************************************/
/*
 * @func	mainPIREventHandler
 * @brief	Pir event handler
 * @param	pirAction
 * @retval	None
 */
void mainPIREventHandler(uint8_t byPirAction)
{
	static boolean boSendFlag = true;
	switch (byPirAction) {

		case PIR_MOTION:
			if(boSendFlag){
				boSendFlag = false;
				sendPIRStateReport(2,PIR_MOTION);
			}
			toggleLed(LED1,ledPink,1,150,150);
			break;

		case PIR_UNMOTION:
			boSendFlag = true;
			toggleLed(LED1,ledRed,1,150,150);
			emberAfCorePrintln("PIR_UNMOTION");
			sendPIRStateReport(2,PIR_UNMOTION);
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
	switch (g_systemState) {

		case POWER_ON_STATE:
			nwkStatusCurrent = emberAfNetworkState();
			if(nwkStatusCurrent == EMBER_NO_NETWORK){
				toggleLed(LED1,ledRed,3,200,200);
				networkFindAndJoin();
			}
			g_systemState = IDLE_STATE;
			break;

		case REPORT_STATE:
			g_systemState = IDLE_STATE;
			sendReportInfoHc();
			break;

		case IDLE_STATE:
			break;

		case REBOOT_STATE:
			g_systemState = IDLE_STATE;
			EmberNetworkStatus networkStatus = emberAfNetworkState();
			if (networkStatus != EMBER_NO_NETWORK) {
				sendZigDevRequest();
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
 * @func	mainNetworkEventHandler
 * @brief	Main network event handler
 * @param	uint8_t
 * @retval	None
 */
void mainNetworkEventHandler(uint8_t byNetworkResult)
{
	emberAfCorePrintln("Network Event Handle");
	switch (byNetworkResult) {

		case NETWORK_HAS_PARENT:
			emberAfCorePrintln("Network has parent");
			toggleLed(LED1, ledPink, 3, 300, 300);
			g_boNetworkReady = true;
			g_systemState = REPORT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;

		case NETWORK_JOIN_FAIL:
			g_systemState = IDLE_STATE;
			emberAfCorePrintln("Network Join Fail");
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;

		case NETWORK_JOIN_SUCCESS:
			emberAfCorePrintln("Network Join Success");
			toggleLed(LED1, ledPink, 3, 300, 300);
			g_boNetworkReady = true;
			g_systemState = REPORT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;

		case NETWORK_LOST_PARENT:
			emberAfCorePrintln("Network lost parent");
			toggleLed(LED1,ledPink,3,300,300);
			g_systemState = IDLE_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;

		case NETWORK_OUT_NETWORK:
			if(g_boNetworkReady){
				emberAfCorePrintln("Network Out network");
				toggleLed(LED1, ledPink, 3, 300, 300);
				g_systemState = REBOOT_STATE;
				emberEventControlSetDelayMS(mainStateEventControl, 3000);
			}
			break;

		default:
			break;
	}
}
/******************************************************************************/

