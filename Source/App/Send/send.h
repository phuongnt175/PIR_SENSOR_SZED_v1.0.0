/*
 * send.h
 *
 *  Created on: Mar 30, 2023
 *      Author: admin1
 */

#ifndef SOURCE_APP_SEND_SEND_H_
#define SOURCE_APP_SEND_SEND_H_

#define MAX_DATA_COMMAND_SIZE					50
#define SOURCE_ENDPOINT_PRIMARY					1
#define DESTINATTION_ENDPOINT					1
#define HC_NETWORK_ADDRESS						0x0000
 #define ZDO_MESSAGE_OVERHEAD 					1


void SEND_SendCommandUnicast(uint8_t source,
							 uint8_t destination,
							 uint8_t address);
void SendZigDevRequest(void);
void SEND_OnOffStateReport(uint8_t Endpoint, uint8_t value);
void SEND_PIRStateReport(uint8_t endpoint, uint8_t value);
void SEND_ReportInfoHc(void);
void SendZigDevRequest(void);

#endif /* SOURCE_APP_SEND_SEND_H_ */
