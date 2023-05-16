/*
 * receive.c
 *
 *  Created on: Mar 30, 2023
 *      Author: admin1
 */

#include "app/framework/include/af.h"
#include "Source/Mid/Led/led.h"
#include "Source/App/Send/send.h"
#include "receive.h"

/**
 * @func    emberAfPreCommandReceivedCallback
 * @brief   Process Command Received
 * @param   EmberAfClusterCommand
 * @retval  boolean
 */
boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd)
{
	if(cmd->clusterSpecific)
	{
		switch(cmd->apsFrame->clusterId)
		{
			case ZCL_ON_OFF_CLUSTER_ID:
				RECEIVE_HandleOnOffCluster(cmd);
				break;
			case ZCL_LEVEL_CONTROL_CLUSTER_ID:
//				RECEIVE_HandleLevelControlCluster(cmd);
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
boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incommingMessage)
{
	if(incommingMessage->apsFrame->clusterId == ACTIVE_ENDPOINTS_RESPONSE)
	{
		return true;
	}
 return false;
}


/**
 * @func    RECEIVE_HandleOnOffCluster
 * @brief   Handler On/Off command
 * @param   EmberAfClusterCommand
 * @retval  bool
 */
bool RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd)
{
	uint8_t commandID = cmd->commandId;
	uint8_t localEndpoint = cmd ->apsFrame -> destinationEndpoint;
	uint8_t remoteEndpoint = cmd->apsFrame -> sourceEndpoint;
	uint16_t IgnoreNodeID = cmd->source;
/************************************ON-OFF LED******************************************************************************************/
	emberAfCorePrintln("RECEIVE_HandleOnOffCluster SourceEndpoint = %d, RemoteEndpoint = %d, commandID = %d, nodeID %2X\n",remoteEndpoint,localEndpoint,commandID,IgnoreNodeID);
	switch(commandID)
	{
	case ZCL_OFF_COMMAND_ID:
		emberAfCorePrintln("Turn off LED");

		switch (cmd->type) {
			case EMBER_INCOMING_UNICAST:
				if(localEndpoint == 1)
				{
				turnOffRGBLed(LED1);
				SEND_OnOffStateReport(localEndpoint, LED_OFF);
				}
				if(localEndpoint == 2)
				{
					turnOffRGBLed(LED2);
					SEND_OnOffStateReport(localEndpoint, LED_OFF);
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
		switch (cmd->type) {
			case EMBER_INCOMING_UNICAST:
				if(localEndpoint == 1)
				{
					turnOnLed(LED1, ledBlue);
					SEND_OnOffStateReport(localEndpoint, LED_ON);
				}
				if(localEndpoint == 2)
				{
					turnOnLed(LED2, ledBlue);
					SEND_OnOffStateReport(localEndpoint, LED_ON);
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

