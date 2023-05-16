/*
 * led.c
 *
 *  Created on: Apr 3, 2023
 *      Author: admin1
 */


#include "app/framework/include/af.h"
#include "led.h"
#include "em_timer.h"


typedef struct {
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
  bool 				  ledBlinkMode;
  ledColor_e		  color;
  uint32_t 			  onTime;
  uint32_t			  offTime;
  uint8_t			  blinkTime;
} ledArray_t;

EmberEventControl led1ToggleEventControl,led2ToggleEventControl;
EmberEventControl *ledEventControl[LED_RGB_COUNT];

ledArray_t led_Array[LED_RGB_COUNT][LED_RGB_ELEMENT] = {LED_RGB_1, LED_RGB_2};

ledArray_t ledAction[LED_RGB_COUNT];


/**
 * @func    ledInit
 * @brief   Initialize LED
 * @param   None
 * @retval  None
 */
void ledInit(void)
{
	CMU_ClockEnable(cmuClock_GPIO, true);
	for(int i = 0;i <LED_RGB_COUNT;i++)
	{
		for(int j = 0; j< LED_RGB_ELEMENT;j++)
		{
			GPIO_PinModeSet(led_Array[i][j].port, led_Array[i][j].pin,
							gpioModePushPull,0);
		}
	}
	turnOffRGBLed(LED1);
	turnOffRGBLed(LED2);
	ledEventControl[LED1] =(EmberEventControl *) &led1ToggleEventControl;
	ledEventControl[LED2] =(EmberEventControl *) &led2ToggleEventControl;
}

void turnOffRGBLed(ledNumber index)
{
	for(int j=0;j<LED_RGB_ELEMENT;j++)
	{
		GPIO_PinOutSet(led_Array[index][j].port, led_Array[index][j].pin);
	}
}

void turnOnLed(ledNumber index, ledColor_e color)
{
	for(int j=0;j<LED_RGB_ELEMENT;j++)
	{
		if(((color >> j) & 0x01) == 1)
		{
			GPIO_PinOutClear(led_Array[index][j].port, led_Array[index][j].pin);
		}
		else{
			GPIO_PinOutSet(led_Array[index][j].port, led_Array[index][j].pin);
		}
	}
}

/**
 * @func    toggleLed
 * @brief   toggled LED
 * @param   None
 * @retval  None
 */
void toggleLed(ledNumber ledIndex, ledColor_e color, uint8_t toggleTime, uint32_t onTimeMs, uint32_t offTimeMs)
{
	ledAction[ledIndex].ledBlinkMode = LED_TOGGLE;
	ledAction[ledIndex].color = color;
	ledAction[ledIndex].onTime = onTimeMs;
	ledAction[ledIndex].offTime = offTimeMs;
	ledAction[ledIndex].blinkTime = toggleTime*2;

	emberEventControlSetActive(*ledEventControl[ledIndex]);
}

/**
 * @func    toggleLedHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
void toggleLedHandle(ledNumber ledIndex)
{
	if(ledAction[ledIndex].blinkTime !=0)
	{
		if(ledAction[ledIndex].blinkTime % 2)
		{
			for( int i = 0; i<LED_RGB_ELEMENT; i++)
			{
				if(((ledAction[ledIndex].color >> i ) & 0x01) == 1 )
				{
					GPIO_PinOutClear(led_Array[ledIndex][i].port, led_Array[ledIndex][i].pin);
				}
				else
				{
					GPIO_PinOutSet(led_Array[ledIndex][i].port, led_Array[ledIndex][i].pin);
				}
			}
			emberEventControlSetDelayMS(*ledEventControl[ledIndex], ledAction[ledIndex].onTime);
		}else
		{
			for( int j = 0; j<LED_RGB_ELEMENT; j++)
			{
				GPIO_PinOutSet(led_Array[ledIndex][j].port, led_Array[ledIndex][j].pin);
			}
			emberEventControlSetDelayMS(*ledEventControl[ledIndex], ledAction[ledIndex].offTime);
		}
		ledAction[ledIndex].blinkTime--;
	}
	else
	{
		ledAction[ledIndex].ledBlinkMode = LED_FREE;
		for( int j = 0; j<LED_RGB_ELEMENT; j++)
		{
			GPIO_PinOutSet(led_Array[ledIndex][j].port, led_Array[ledIndex][j].pin);
		}
	}
}

/**
 * @func    led1ToggleEventHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
void led1ToggleEventHandle(void)
{
	emberEventControlSetInactive(led1ToggleEventControl);
	switch(ledAction[LED1].ledBlinkMode)
	{
	case LED_TOGGLE:
		toggleLedHandle(LED1);
		break;
	default:
		break;
	}
}


/**
 * @func    led2ToggleEventHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
void led2ToggleEventHandle(void)
{
	emberEventControlSetInactive(led2ToggleEventControl);
	switch(ledAction[LED2].ledBlinkMode)
	{
	case LED_TOGGLE:
		toggleLedHandle(LED2);
		break;
	default:
		break;
	}
}
