 /* File name: send.c
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

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "app/framework/include/af.h"
#include "send.h"
#include "Source/App/Receive/receive.h"
#include "zigbee-framework/zigbee-device-common.h"

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
 * @func    sendCommandUnicast
 * @brief   Send uinicast command
 * @param   source, destination, address
 * @retval  None
 */
void sendCommandUnicast(uint8_t bySource,
						uint8_t byDestination,
						uint8_t byAddress)
{
	emberAfSetCommandEndpoints(bySource, byDestination);
	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, byAddress);
}


/**
 * @func    sendFillBufferGlobalCommand
 * @brief   Send fill buffer global command
 * @param   clusterID, attributeID, byGlobalCommand, pbyValue, byLength, byDataType
 * @retval  None
 */
static void sendFillBufferGlobalCommand(EmberAfClusterId clusterID,
										EmberAfAttributeId attributeID,
										uint8_t byGlobalCommand,
										uint8_t* pbyValue,
										uint8_t byLength,
										uint8_t byDataType)
{
	uint8_t byData[MAX_DATA_COMMAND_SIZE];
	byData[0] = (uint8_t)(attributeID & 0x00FF);
	byData[1] = (uint8_t)((attributeID & 0xFF00)>>8);
	byData[2] = EMBER_SUCCESS;
	byData[3] = (uint8_t)byDataType;
	memcpy(&byData[4], pbyValue, byLength);

	(void) emberAfFillExternalBuffer((ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
									  clusterID,
									  byGlobalCommand,
									  "b",
									  byData,
									  byLength + 4);
}

/**
 * @func    sendReportInfoHc
 * @brief   Send Report to HC
 * @param   None
 * @retval  None
 */
void sendReportInfoHc(void)
{
	uint8_t modelID[9] = {8,'L','M','1','_','P','I','R','1'};
	uint8_t manufactureID[5] = {4, 'L', 'u', 'm', 'i'};
	uint8_t byVersion = 1;

	if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
		return;
	}
	sendFillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								modelID,
								9,
								ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	sendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
					   DESTINATTION_ENDPOINT,
					   HC_NETWORK_ADDRESS);

	sendFillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								ZCL_MANUFACTURER_NAME_ATTRIBUTE_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								manufactureID,
								5,
								ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	sendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
					   DESTINATTION_ENDPOINT,
					   HC_NETWORK_ADDRESS);
	sendFillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								ZCL_APPLICATION_VERSION_ATTRIBUTE_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								&byVersion,
								1,
								ZCL_INT8U_ATTRIBUTE_TYPE);
	sendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
					   DESTINATTION_ENDPOINT,
					   HC_NETWORK_ADDRESS);
}

/**
 * @func    SEND_OnOffStateReport
 * @brief   Send On/Off State
 * @param   byEndpoint, byValue
 * @retval  None
 */
void sendOnOffStateReport(uint8_t byEndpoint, uint8_t byValue){
	sendFillBufferGlobalCommand(ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								(uint8_t*) &byValue,
								1,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	sendCommandUnicast(byEndpoint,
					   DESTINATTION_ENDPOINT,
					   HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(byEndpoint,
								ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								(uint8_t*) &byValue,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}

/**
 * @func    SEND_PIRStateReport
 * @brief   Send PIR State
 * @param   byEndpoint, byValue
 * @retval  None
 */
void sendPIRStateReport(uint8_t byEndpoint, uint8_t byValue){
	sendFillBufferGlobalCommand(ZCL_IAS_ZONE_CLUSTER_ID,
								ZCL_ZONE_STATUS_ATTRIBUTE_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								(uint8_t*) &byValue,
								1,
						   	   	ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	sendCommandUnicast(byEndpoint,
					   DESTINATTION_ENDPOINT,
					   HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(byEndpoint,
								ZCL_IAS_ZONE_CLUSTER_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								(uint8_t*) &byValue,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}

/**
 * @func    SendZigDevRequest
 * @brief   Send ZigDevRequest
 * @param   None
 * @retval  None
 */
void sendZigDevRequest(void)
{
	uint8_t contents[ZDO_MESSAGE_OVERHEAD + 1];
	contents[1] = 0x00;

	(void) emberSendZigDevRequest(HC_NETWORK_ADDRESS, LEAVE_RESPONSE, EMBER_AF_DEFAULT_APS_OPTIONS, contents, sizeof(contents));
}
