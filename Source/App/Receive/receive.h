/*
 * receive.h
 *
 *  Created on: Mar 30, 2023
 *      Author: admin1
 */

#ifndef SOURCE_APP_RECEIVE_RECEIVE_H_
#define SOURCE_APP_RECEIVE_RECEIVE_H_

#define LED_OFF								0
#define LED_ON								1

/* Function prototypes -----------------------------------------------*/
boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* pCmd);
bool receiveHandleOnOffCluster(EmberAfClusterCommand* pCmd);

#endif /* SOURCE_APP_RECEIVE_RECEIVE_H_ */
