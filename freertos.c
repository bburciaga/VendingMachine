/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId coin_taskHandle;
osThreadId select_taskHandle;
osThreadId dispense_taskHandle;
osMessageQId selectQueueHandle;
osMessageQId coinQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);

extern void MX_USB_HOST_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */
  xSemaphoreHandle gatekeeper1 = 0;
  //xSemaphoreHandle gatekeeper2 = 0;

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	gatekeeper1 = xSemaphoreCreateMutex();

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of checkoutQueue */
  osMessageQDef(selectQueue, 3, uint16_t);
  selectQueueHandle = osMessageCreate(osMessageQ(selectQueue), NULL);
  osMessageQDef(coinQueue, 3, uint16_t);
  coinQueueHandle = osMessageCreate(osMessageQ(coinQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of sender_task */
  osThreadDef(coin_task, StartTask02, osPriorityNormal, 0, 128);
  coin_taskHandle = osThreadCreate(osThread(coin_task), NULL);

  /* definition and creation of reciever_task */
  osThreadDef(select_task, StartTask03, osPriorityHigh, 0, 128);
  select_taskHandle = osThreadCreate(osThread(select_task), NULL);
	
  /* definition and creation of reciever_task */
  osThreadDef(dispense_task, StartTask04, osPriorityLow, 0, 128);
  dispense_taskHandle = osThreadCreate(osThread(dispense_task), NULL);
	
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */

void StartDefaultTask(void const * argument)
{
	
  /* init code for USB_HOST */
  MX_USB_HOST_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
		
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
int coin = 0;

/**
* @brief Function implementing the sender_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
	int txCoin = 0;
  /* Infinite loop */
  for(;;)
  {
		if( xQueueReceive(coinQueueHandle, &txCoin, 1000)){
			txCoin = 0;
		}
		if(xSemaphoreTake(gatekeeper1, 20000)){
			if(txCoin == 0) {
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
				vTaskDelay(1000);
				if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){
					txCoin = 3;
				}
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
				vTaskDelay(1000);
				if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){
					txCoin = 5;
				}
			} else if(txCoin > 0) {
				vTaskDelay(1000);
			}
		} else {
			//deadlock
		}
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		if( !xQueueSend(coinQueueHandle, &txCoin, 1000)){
			
		}
		xSemaphoreGive(gatekeeper1);
	}
  osDelay(1);
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the reciever_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
	int txSelect = 0;
	int rxCoin = 0;
  /* Infinite loop */
  for(;;)
  {
		if(xSemaphoreTake(gatekeeper1,20000) && xQueueReceive(coinQueueHandle, &rxCoin, 1000)) {
			if(rxCoin > 0 && txSelect == 0) {
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
				vTaskDelay(1000);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) txSelect = 1;
				
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
				vTaskDelay(1000);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) txSelect = 2;
				
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
				vTaskDelay(1000);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
				if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) txSelect = 3;
			
			} else if (txSelect > 0) {
					if(! xQueueSend(selectQueueHandle, &txSelect, 1000) && ! xQueueSend(coinQueueHandle, &rxCoin, 1000)) {
						HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
					}
					for( int j = 0; j < rxCoin-2; j++){
						HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
						vTaskDelay(1000);
						HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
						vTaskDelay(1000);
					}
					vTaskDelay(4000);
					rxCoin = 0;
					xQueueSend(coinQueueHandle, &rxCoin, 1000);
					txSelect = 0;
					xSemaphoreGive(gatekeeper1);
				}
			
			} else {
				//deadlock
			}
		
		
		xSemaphoreGive(gatekeeper1);
    osDelay(1);
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the reciever_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */
	int rxSelect = 0;
  /* Infinite loop */
  for(;;)
  {
		if( xQueueReceive(selectQueueHandle, &rxSelect, 1000)){
			for( int i = 0; i < 4; i++ ){
				vTaskDelay(1000);
				if( rxSelect == 1 ){
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
					vTaskDelay(1000);
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				} else if( rxSelect == 2 ){
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
					vTaskDelay(1000);
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				} else if( rxSelect == 3 ){
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
					vTaskDelay(1000);
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
				}
			}
			rxSelect = 0;
		}
    osDelay(1);
  }
  /* USER CODE END StartTask04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
