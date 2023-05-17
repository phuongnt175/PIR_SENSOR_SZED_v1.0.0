 /* File name: network.c
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
#include "network.h"

/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

EmberEventControl joinNetworkEventControl;
uint32_t g_timeFindAndJoin = 0;
networkEventHandler g_pNetworkEventHandle = NULL;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/
/*
 * @function 			: Network_Init
 * @brief				: Handle event network.
 * @parameter			: networkEventHandler
 * @return value		: None
 */
void networkInit(networkEventHandler pNetworkResult)
{
	g_pNetworkEventHandle = pNetworkResult;
}

/*
 * @function 			: networkFindAndJoin
 * @brief				: Find network
 * @parameter			: None
 * @return value		: None
 */
void networkFindAndJoin(void)
{
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberEventControlSetDelayMS(joinNetworkEventControl, 2000);
	}
}


/*
 * @function 			: networkStopFindAndJoin
 * @brief				: Stop find network
 * @parameter			: None
 * @return value		: None
 */
void networkStopFindAndJoin(void)
{
	emberAfPluginNetworkSteeringStop();
}


/*
 * @function 			: joinNetworkEventHandler
 * @brief				: Handle Event Join network
 * @parameter			: None
 * @return value		: None
 */
void joinNetworkEventHandler(void)
{
	emberEventControlSetInactive(joinNetworkEventControl);

	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberAfPluginNetworkSteeringStart();
		g_timeFindAndJoin++;
	}
}


/*
 * @function 			: emberAfStackStatusCallback
 * @brief				: Stack Status
 * @parameter			: EmberStatus
 * @return value		: True or false
 */
boolean emberAfStackStatusCallback(EmberStatus status){
	emberAfCorePrintln("emberAfStackStatusCallback\n");
	if(status == EMBER_NETWORK_UP){
		if(g_timeFindAndJoin>0){ // vao mang thanh cong
			networkStopFindAndJoin();
			if(g_pNetworkEventHandle != NULL){
				g_pNetworkEventHandle(NETWORK_JOIN_SUCCESS);
				emberAfCorePrintln("NETWORK_JOIN_SUCCESS");
			}
		}else {
			if(g_pNetworkEventHandle != NULL){
				g_pNetworkEventHandle(NETWORK_HAS_PARENT);
				emberAfCorePrintln("NETWORK_HAS_PARENT");
			}
		}
	}else {
		EmberNetworkStatus nwkStatusCurrent = emberAfNetworkState();
		if(nwkStatusCurrent == EMBER_NO_NETWORK){
			if(g_pNetworkEventHandle != NULL){
				g_pNetworkEventHandle(NETWORK_OUT_NETWORK);
				emberAfCorePrintln("NETWORK_OUT_NETWORK");
			}
		}else if(nwkStatusCurrent == EMBER_JOINED_NETWORK_NO_PARENT){
			emberAfCorePrintln("NETWORK_LOST_PARENT");
			g_pNetworkEventHandle(NETWORK_LOST_PARENT);
		}
	}
	if(status == EMBER_JOIN_FAILED){
		emberAfCorePrintln("NETWORK_JOIN_FAIL");
		g_pNetworkEventHandle(NETWORK_JOIN_FAIL);
	}

	return false;
}


