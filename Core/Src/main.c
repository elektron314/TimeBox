/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <math.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

RTC_AlarmTypeDef gAlarm;
RTC_DateTypeDef gDate;
RTC_TimeTypeDef gTime;

char time[9];
char date[9];

uint8_t alarm;

uint8_t SetAlarm;
uint8_t SetHours, SetMinutes, SetDate, SetSeconds;

uint8_t CharCounter;
uint32_t StartIgnoringTimer;
uint32_t MessageTimer;
uint8_t IgnoringFlag;
uint8_t Buffer[1];

//uint8_t NotSetMessage[] = "not set\0";
uint8_t Message[70];
uint8_t * Message2;
uint8_t * Message3;
uint8_t * Message4;
uint8_t AlarmIsAlreadyOn[] = "Alarm is already on\0";
uint8_t AlarmExpired[] = "You can't set expired alarm\0";
uint8_t AlarmNowIsOn[] = "Alarm is on\0";
uint32_t NowTime;
uint8_t DeleteAlarmFlag;
uint8_t GetAlarmFlag;

uint8_t K0isPressed, K1isPressed;
uint8_t DoorIsLockedFlag = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

uint8_t IsNewAlarmMoreFresh(void);
uint8_t IsAlarmSetBeforeNow(void);
uint8_t IsAlarmSetBeforeNow(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//Time is storing in format that being in Hex visually looks like the real time in decimals
//So this function translate this format into real decimal number of time digits
uint8_t TurnHexIntoDec(uint8_t hex)
{
	return ((hex & 0x000F) + ((hex >> 4) & 0x000F)*10);
}

uint8_t TurnDecIntoHex(uint8_t dec)
{
	return ((((uint8_t)(floor(dec/10))) << 4) + dec%10);
}

void HappyToggling(uint8_t longetivity)
{
	uint8_t TogglingDelay = 0;
	for (TogglingDelay = 0; TogglingDelay < longetivity; TogglingDelay++)
	{
	  HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
	  HAL_Delay(TogglingDelay);
	}
}

void SetTime(void)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	/** Initialize RTC and set the Time and Date*/
	sTime.Hours = 0x23;
	sTime.Minutes = 0x25;
	sTime.Seconds = 0x0;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler();
	}
	sDate.WeekDay = RTC_WEEKDAY_THURSDAY;
	sDate.Month = RTC_MONTH_FEBRUARY;
	sDate.Date = 0x25;
	sDate.Year = 0x21;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
}

void SetAlarmFunc(uint8_t SetInHours, uint8_t SetInMinutes, uint8_t SetInDate, uint8_t SetInSeconds)
{
	RTC_AlarmTypeDef sAlarm;

	sAlarm.AlarmTime.Hours = TurnDecIntoHex(SetInHours);
	sAlarm.AlarmTime.Minutes = TurnDecIntoHex(SetInMinutes);
	sAlarm.AlarmTime.Seconds = TurnDecIntoHex(SetInSeconds);
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = TurnDecIntoHex(SetInDate);
	sAlarm.Alarm = RTC_ALARM_A;

	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
	{
	Error_Handler();
	}
}

void GetTimeDate(void)
{
	/* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BCD);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BCD);

	/* Display time Format: hh:mm:ss */
	sprintf((char*)time,"%02x:%02x:%02x",gTime.Hours, gTime.Minutes, gTime.Seconds);

	/* Display date Format: yy-mm-dd */
	sprintf((char*)date,"%02x-%02x-%02x",gDate.Year, gDate.Month, gDate.Date);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	alarm = 1;
}

uint8_t IsAlarmSetBeforeNow(void)
{
	GetTimeDate();
	HAL_RTC_GetAlarm(&hrtc, &gAlarm, RTC_ALARM_A, RTC_FORMAT_BCD);
	if (gAlarm.AlarmTime.Seconds < gDate.Month)
	{
		return 1;
	} else if (gAlarm.AlarmTime.Seconds > gDate.Month)
	{
		return 0;
	} else
	{
		if (gAlarm.AlarmDateWeekDay < gDate.Date)
		{
			return 1;
		} else if (gAlarm.AlarmDateWeekDay > gDate.Date)
		{
			return 0;
		} else
		{
			if (gAlarm.AlarmTime.Hours < gTime.Hours)
			{
				return 1;
			} else if (gAlarm.AlarmTime.Hours > gTime.Hours)
			{
				return 0;
			} else
			{
				if (gAlarm.AlarmTime.Minutes < gTime.Minutes)
				{
					return 1;
				} else if (gAlarm.AlarmTime.Minutes > gTime.Minutes)
				{
					return 0;
				} else
				{
					return 1;
				}
			}
		}
	}
}

uint8_t IsNewAlarmMoreFresh(void)
{
		HAL_RTC_GetAlarm(&hrtc, &gAlarm, RTC_ALARM_A, RTC_FORMAT_BCD);
		if (gAlarm.AlarmTime.Seconds < TurnDecIntoHex(SetSeconds))
		{
			return 1;
		} else if (gAlarm.AlarmTime.Seconds > TurnDecIntoHex(SetSeconds))
		{
			return 0;
		} else
		{
			if (gAlarm.AlarmDateWeekDay < TurnDecIntoHex(SetDate))
			{
				return 1;
			} else if (gAlarm.AlarmDateWeekDay > TurnDecIntoHex(SetDate))
			{
				return 0;
			} else
			{
				if (gAlarm.AlarmTime.Hours < TurnDecIntoHex(SetHours))
				{
					return 1;
				} else if (gAlarm.AlarmTime.Hours > TurnDecIntoHex(SetHours))
				{
					return 0;
				} else
				{
					if (gAlarm.AlarmTime.Minutes < TurnDecIntoHex(SetMinutes))
					{
						return 1;
					} else if (gAlarm.AlarmTime.Minutes >= TurnDecIntoHex(SetMinutes))
					{
						return 0;
					}
				}
			}
		}
}

uint8_t IsNewAlarmAfterNow(void)
{
		GetTimeDate();
		HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BCD);
		HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BCD);
		if (TurnDecIntoHex(SetSeconds) > gDate.Month)
		{
			return 1;
		} else if (TurnDecIntoHex(SetSeconds) < gDate.Month)
		{
			return 0;
		} else
		{
			if (TurnDecIntoHex(SetDate) > gDate.Date)
			{
				return 1;
			} else if (TurnDecIntoHex(SetDate) < gDate.Date)
			{
				return 0;
			} else
			{
				if (TurnDecIntoHex(SetHours) > gTime.Hours)
				{
					return 1;
				} else if (TurnDecIntoHex(SetHours) < gTime.Hours)
				{
					return 0;
				} else
				{
					if (TurnDecIntoHex(SetMinutes) > gTime.Minutes)
					{
						return 1;
					} else if (TurnDecIntoHex(SetMinutes) <= gTime.Minutes)
					{
						return 0;
					}
				}
			}
		}
}

void Rewind5Sec(void)
{
	if (K0isPressed)
	{
		RTC_TimeTypeDef gTime;

		HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BCD);
		gTime.Seconds = TurnDecIntoHex(TurnHexIntoDec(gTime.Seconds)-5);
		gTime.Hours = TurnDecIntoHex(TurnHexIntoDec(gTime.Hours)+0);

		if (HAL_RTC_SetTime(&hrtc, &gTime, RTC_FORMAT_BCD) != HAL_OK)
		{
		  Error_Handler();
		}

		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);

		K0isPressed = 0;
	}
}

void Forwardd5Sec(void)
{
	if (K1isPressed)
	{
		RTC_TimeTypeDef gTime;

		HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BCD);
		gTime.Seconds = TurnDecIntoHex(TurnHexIntoDec(gTime.Seconds)+5);
		gTime.Hours = TurnDecIntoHex(TurnHexIntoDec(gTime.Hours)+0);

		if (HAL_RTC_SetTime(&hrtc, &gTime, RTC_FORMAT_BCD) != HAL_OK)
		{
		  Error_Handler();
		}

		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
		K1isPressed = 0;
	}
}

void OpenTheDoor(void)
{
	HAL_GPIO_WritePin(OpenDoor_GPIO_Port, OpenDoor_Pin, RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(OpenDoor_GPIO_Port, OpenDoor_Pin, SET);
}

void ToDoOnAlarm(void)
{
	OpenTheDoor();
	HappyToggling(150);
}

//void DeleteAlarm(void)
//{
//	SetAlarmFunc(0,0,1,1);
//}

  /* USER CODE END 6 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2)
  {
	  SetTime();
  }

  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);

  if (IsAlarmSetBeforeNow())
  {
	  OpenTheDoor();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//	!!
//	if I upload confifugartion from CubeMX Configurator I need to delete set time from MX_RTC_Init()!
//	!!

	  if (HAL_GPIO_ReadPin(MagnetDoor_GPIO_Port, MagnetDoor_Pin) == 0)
	  {
		  HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
		  DoorIsLockedFlag = 1;
	  } else
		  DoorIsLockedFlag = 0;

	  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	  HAL_Delay(200);

	  if (SetAlarm)
	  {
		  if (DoorIsLockedFlag)
		  {
			  if (IsNewAlarmMoreFresh())
			  {
				  if (IsNewAlarmAfterNow())
				  {
					  sprintf(Message, "Set alarm to %02d:%02d of %02d.%02d\0",SetHours, SetMinutes, SetDate, SetSeconds);
					  HAL_UART_Transmit(&huart1, Message, strlen(Message), strlen(Message));
					  HappyToggling(100);
					  SetAlarmFunc(SetHours, SetMinutes, SetDate, SetSeconds);
				  } else
				  {
	//				  HAL_RTC_GetAlarm(&hrtc, &gAlarm, RTC_ALARM_A, RTC_FORMAT_BCD);
	//				  sprintf(AlarmIsAlreadyOn, "%s %02x:%02x of %02x.%02x", AlarmIsAlreadyOn, gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds, gAlarm.AlarmDateWeekDay);
					  HAL_UART_Transmit(&huart1, AlarmExpired, strlen(AlarmExpired), strlen(AlarmExpired));
	//				  HAL_UART_Transmit(&huart1, AlarmIsAlreadyOn, strlen(AlarmIsAlreadyOn), 35);
				  }
			  } else
			  {
	//			  memset(Message, 0, sizeof(Message));
				  sprintf(Message, "%s %02x:%02x of %02x.%02x\0", AlarmIsAlreadyOn, gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds, gAlarm.AlarmDateWeekDay);
	  //		  	  sprintf(Message2, "%s %02x:%02x of %02x.%02x\0", AlarmIsAlreadyOn, gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds, gAlarm.AlarmDateWeekDay);
	  //		  	  sprintf(*Message3, "%s %02x:%02x of %02x.%02x\0", AlarmIsAlreadyOn, gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds, gAlarm.AlarmDateWeekDay);
	  //		  	  sprintf(&Message4, "%s %02x:%02x of %02x.%02x\0", AlarmIsAlreadyOn, gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds, gAlarm.AlarmDateWeekDay);
				  HAL_UART_Transmit(&huart1, Message, strlen(Message), strlen(Message));
			  }
		  } else
		  {
			  sprintf(Message, "Set alarm to %02d:%02d of %02d.%02d\0",SetHours, SetMinutes, SetDate, SetSeconds);
			  HAL_UART_Transmit(&huart1, Message, strlen(Message), strlen(Message));
			  HappyToggling(100);
			  SetAlarmFunc(SetHours, SetMinutes, SetDate, SetSeconds);
		  }
		  SetAlarm = 0;
	  }

	  GetTimeDate();

	  if (alarm)
	  {
		  ToDoOnAlarm();
		  alarm = 0;
	  }

	  if (DeleteAlarmFlag)
	  {
		  SetAlarmFunc(0,0,1,1);
		DeleteAlarmFlag = 0;
	  }

	  if (GetAlarmFlag)
	  {
		  HAL_RTC_GetAlarm(&hrtc, &gAlarm, RTC_ALARM_A, RTC_FORMAT_BCD);
		  HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BCD);
		  HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BCD);
		  sprintf(Message, "%s %02x:%02x of %02x.%02x. Time now: %02x:%02x %02x.%02x", AlarmNowIsOn, gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds, gAlarm.AlarmDateWeekDay, gTime.Hours, gTime.Minutes, gDate.Month, gDate.Date);
		  HAL_UART_Transmit(&huart1, Message, strlen(Message), strlen(Message));
		  GetAlarmFlag = 0;
	  }

//	  1000 milliseconds is just a time, taken from nothing, during that RX interrupts will be disabled
	  NowTime = HAL_GetTick();
//	  if (((IgnoringFlag == 1) || (NowTime - MessageTimer > 5000)) && ((NowTime - StartIgnoringTimer) > 1000))
	  if ((CharCounter >= 8) && (NowTime - MessageTimer > 5000))
//	  if (NowTime - StartIgnoringTimer > 1000)
	  {
		  CharCounter = 0;
//		  IgnoringFlag = 0;
		  __HAL_UART_CLEAR_OREFLAG(&huart1);
		  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
		  HappyToggling(30);
		  MessageTimer = 0;
	  }

	  Rewind5Sec();
	  Forwardd5Sec();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

//  RTC_TimeTypeDef sTime = {0};
//  RTC_DateTypeDef sDate = {0};
//  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
//  sTime.Hours = 1;
//  sTime.Minutes = 57;
//  sTime.Seconds = 0x0;
//  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
//  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
//  sDate.Month = RTC_MONTH_FEBRUARY;
//  sDate.Date = 0x10;
//  sDate.Year = 0x21;
//
//  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /** Enable the Alarm A
//  */
//  sAlarm.AlarmTime.Hours = 0x0;
//  sAlarm.AlarmTime.Minutes = 0x0;
//  sAlarm.AlarmTime.Seconds = 0x0;
//  sAlarm.AlarmTime.SubSeconds = 0x0;
//  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
//  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
//  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
//  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
//  sAlarm.AlarmDateWeekDay = 0x1;
//  sAlarm.Alarm = RTC_ALARM_A;
//  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
//  {
//    Error_Handler();
//  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, LED0_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OpenDoor_GPIO_Port, OpenDoor_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : KEY1_Pin KEY0_Pin */
  GPIO_InitStruct.Pin = KEY1_Pin|KEY0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : LED0_Pin LED1_Pin */
  GPIO_InitStruct.Pin = LED0_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : OpenDoor_Pin */
  GPIO_InitStruct.Pin = OpenDoor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OpenDoor_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MagnetDoor_Pin */
  GPIO_InitStruct.Pin = MagnetDoor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(MagnetDoor_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
