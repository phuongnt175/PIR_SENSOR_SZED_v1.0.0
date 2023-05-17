 /* File name: reiceive.c
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
#include "Source/Mid/Led/led.h"
#include "Source/App/Send/send.h"
#include "receive.h"

/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

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
/**
 * @func    emberAfPreCommandReceivedCallback
 * @brief   Process Command Received
 * @param   EmberAfClusterCommand
 * @retval  boolean
 */
boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* pCmd)
{
	if(pCmd->clusterSpecific){
		switch(pCmd->apsFrame->clusterId)
		{
			case ZCL_ON_OFF_CLUSTER_ID:
				receiveHandleOnOffCluster(pCmd);
				break;

			case ZCL_LEVEL_CONTROL_CLUSTER_ID:
//				RECEIVE_HandleLevelControlCluster(pCmd);
				break;

			default:
				break;
		}
	}

	return false;
}

/**
 * @func    emberAfPreMessageReceivedCallback
 * @brief   Process Pre message received
 * @param   EmberAfIncomingMessage
 * @retval  None
 */
boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* pIncommingMessage)
{
	if(pIncommingMessage->apsFrame->clusterId == ACTIVE_ENDPOINTS_RESPONSE){
		return true;
	}

 return false;
}

/**
 * @func    receiveHandleOnOffCluster
 * @brief   Handler On/Off command
 * @param   EmberAfClusterCommand
 * @retval  bool
 */
bool receiveHandleOnOffCluster(EmberAfClusterCommand* pCmd)
{
	uint8_t commandID = pCmd->commandId;
	uint8_t localEndpoint = pCmd ->apsFrame -> destinationEndpoint;
	uint8_t remoteEndpoint = pCmd->apsFrame -> sourceEndpoint;
	uint16_t IgnoreNodeID = pCmd->source;
/************************************ON-OFF LED******************************************************************************************/
	emberAfCorePrintln("receiveHandleOnOffCluster SourceEndpoint = %d, RemoteEndpoint = %d, commandID = %d, nodeID %2X\n",remoteEndpoint,localEndpoint,commandID,IgnoreNodeID);
	switch(commandID)
	{
		case ZCL_OFF_COMMAND_ID:
			emberAfCorePrintln("Turn off LED");

			switch (pCmd->type) {

				case EMBER_INCOMING_UNICAST:
					if(localEndpoint == 1)
					{
						turnOffRGBLed(LED1);
						sendOnOffStateReport(localEndpoint, LED_OFF);
					}
					if(localEndpoint == 2)
					{
						turnOffRGBLed(LED2);
						sendOnOffStateReport(localEndpoint, LED_OFF);
					}
					break;

				case EMBER_INCOMING_MULTICAST:
					emberAfCorePrintln("Multicast");
					turnOnLed(LED1, ledOff);
					turnOnLed(LED2, ledOff);
					break;

				default:
					break;
			}
			break;

		case ZCL_ON_COMMAND_ID:

			emberAfCorePrintln("Turn on LED");
			switch (pCmd->type) {

				case EMBER_INCOMING_UNICAST:
					if(localEndpoint == 1){
						turnOnLed(LED1, ledBlue);
						sendOnOffStateReport(localEndpoint, LED_ON);
					}
					if(localEndpoint == 2){
						turnOnLed(LED2, ledBlue);
						sendOnOffStateReport(localEndpoint, LED_ON);
					}
					break;

				case EMBER_INCOMING_MULTICAST:
					emberAfCorePrintln("Multicast");
					turnOnLed(LED2, ledGreen);
					break;

				default:
					break;
			}
			break;

		default:
			break;
		}
	return false;
}

