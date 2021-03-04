/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

RTC_HandleTypeDef hrtc;
RTC_AlarmTypeDef gAlarm;
RTC_DateTypeDef gDate;
RTC_TimeTypeDef gTime;

uint8_t JitterButtonK0 = 0;
uint8_t JitterButtonK1 = 0;
uint8_t RxData[1];
extern uint8_t CharCounter;
extern uint8_t Buffer[];
extern uint8_t Message[];
extern uint32_t StartIgnoringTimer;
extern uint32_t MessageTimer;
extern uint8_t IgnoringFlag;
uint8_t OpenMessage[] = "open\0";
uint8_t DelMessage[] = "del\0";
uint8_t GetAlarm[] = "get alarm\0";
//uint8_t AlarmNowIsOn[26] = "Alarm is on\0";
extern uint8_t AlarmNowIsOn[];

uint8_t Hstr[2], Mstr[2], Dstr[2], Sstr[2];
extern uint8_t SetAlarm;
extern uint8_t SetHours, SetMinutes, SetDate, SetSeconds;

extern uint8_t K0isPressed, K1isPressed;
extern uint8_t DoorIsLockedFlag;
extern uint8_t DeleteAlarmFlag;
extern uint8_t GetAlarmFlag;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

extern uint8_t IsAlarmSetBeforeNow(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line3 interrupt.
  */
void EXTI3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI3_IRQn 0 */

//	jitter solver
	if (HAL_GetTick() - JitterButtonK1 > 300)
	{
		JitterButtonK1 = HAL_GetTick();

//		here is our action on KEY1 button
		K1isPressed = 1;
	}

  /* USER CODE END EXTI3_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
  /* USER CODE BEGIN EXTI3_IRQn 1 */

  /* USER CODE END EXTI3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_IRQn 0 */

//	jitter solver
	if (HAL_GetTick() - JitterButtonK0 > 300)
	{
		JitterButtonK0 = HAL_GetTick();

//		here is our action on KEY0 button
		K0isPressed = 1;
	}

  /* USER CODE END EXTI4_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
  /* USER CODE BEGIN EXTI4_IRQn 1 */

  /* USER CODE END EXTI4_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  if (MessageTimer == 0)
	  MessageTimer = HAL_GetTick();
//  IgnoringFlag = 1;
  HAL_UART_Receive(&huart1, RxData, 1, 10);
  Buffer[CharCounter] = *RxData;
  CharCounter++;

  if ((CharCounter == strlen(DelMessage)) && (DoorIsLockedFlag))
  {
//	  StartIgnoringTimer = HAL_GetTick();
	  uint8_t loop;
	  for (loop = 0; loop < CharCounter; loop++)
	  {
		  if (Buffer[loop] == DelMessage[loop])
		  {
			  if (loop == CharCounter-1)
			  {
	//  				  __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
				  DeleteAlarmFlag = 1;
				  sprintf(Message, "Alarm deleted");
				  HAL_UART_Transmit(&huart1, Message, strlen(Message), strlen(Message));
			  }
			  continue;
		  }
		  else
			  break;
	  }
  }

  if (CharCounter == strlen(OpenMessage))
  {
//	  StartIgnoringTimer = HAL_GetTick();
	  uint8_t loop;
	  for (loop = 0; loop < CharCounter; loop++)
	  {
		  if (Buffer[loop] == OpenMessage[loop])
		  {
			  if (loop == CharCounter-1)
			  {
//				  __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
				  if (IsAlarmSetBeforeNow())
				  {
					  HAL_GPIO_WritePin(OpenDoor_GPIO_Port, OpenDoor_Pin, RESET);
					  HAL_UART_Transmit(&huart1, Buffer, CharCounter, CharCounter);
					  HAL_GPIO_WritePin(OpenDoor_GPIO_Port, OpenDoor_Pin, SET);
				  } else
				  {
					  HAL_RTC_GetAlarm(&hrtc, &gAlarm, RTC_ALARM_A, RTC_FORMAT_BCD);
					  sprintf(Message, "%s %02x:%02x of %02x.%02x", AlarmNowIsOn, gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds, gAlarm.AlarmDateWeekDay);
					  HAL_UART_Transmit(&huart1, Message, strlen(Message), strlen(Message));
				  }
			  }
			  continue;
		  }
		  else
			  break;
	  }
  }

  if (CharCounter == strlen(GetAlarm))
  {
	  uint8_t loop;
	  for (loop = 0; loop < CharCounter; loop++)
	  {
		  if (Buffer[loop] == GetAlarm[loop])
		  {
			  if (loop == CharCounter-1)
			  {
//				  __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
				  GetAlarmFlag = 1;
//				  HAL_RTC_GetAlarm(&hrtc, &gAlarm, RTC_ALARM_A, RTC_FORMAT_BCD);
//				  HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BCD);
//				  HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BCD);
//				  sprintf(Message, "%s %02x:%02x of %02x.%02x. Time now: %02x:%02x %02x.%02x", AlarmNowIsOn, gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds, gAlarm.AlarmDateWeekDay, gTime.Hours, gTime.Minutes, gDate.Month, gDate.Date);
//				  HAL_UART_Transmit(&huart1, Message, strlen(Message), strlen(Message));
			  }
			  continue;
		  }
		  else
			  break;
	  }
  }

  if (CharCounter == 8)
  {
	  uint8_t loop;
	  for (loop = 0; loop < CharCounter; loop++)
	  {
		  if isdigit((uint8_t)Buffer[loop])
		  {
			  if (loop == CharCounter-1)
			  {
				  __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);

//				  check if all the received numbers are valid to set for alarm
				  Hstr[0] = (uint8_t)Buffer[0];
				  Hstr[1] = (uint8_t)Buffer[1];
				  SetHours = atoi(Hstr);
				  if ( (SetHours >= 0) && (SetHours <= 23) )
				  {
					  Mstr[0] = (uint8_t)Buffer[2];
					  Mstr[1] = (uint8_t)Buffer[3];
					  SetMinutes = atoi(Mstr);
					  if ( (SetMinutes >= 0) && (SetMinutes <= 59) )
					  {
						  Dstr[0] = (uint8_t)Buffer[6];
						  Dstr[1] = (uint8_t)Buffer[7];
						  SetDate = atoi(Dstr);
						  if ( (SetDate >= 1) && (SetDate <= 30) )
						  {
							  Sstr[0] = (uint8_t)Buffer[4];
							  Sstr[1] = (uint8_t)Buffer[5];
							  SetSeconds = atoi(Sstr);
							  SetAlarm = 1;
						  }
					  }
				  }

			  }
			  continue;
		  }
		  else
			  break;
	  }
  }

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles RTC alarms A and B interrupt through EXTI line 17.
  */
void RTC_Alarm_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_Alarm_IRQn 0 */

  /* USER CODE END RTC_Alarm_IRQn 0 */
  HAL_RTC_AlarmIRQHandler(&hrtc);
  /* USER CODE BEGIN RTC_Alarm_IRQn 1 */

  /* USER CODE END RTC_Alarm_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
