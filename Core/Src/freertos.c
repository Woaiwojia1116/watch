/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "queue.h"
#define tiaoshi 0

#if tiaoshi
#include "oled.h"
#endif

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
QueueHandle_t key_event_queue;
QueueHandle_t display_queue;
TimerHandle_t refresh_timer;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

static void refresh(void) {
    DisplayMsg_t msg = DISPLAY_MSG_REDRAW;
    xQueueOverwrite(display_queue, &msg);
}
void refresh_timer_callback(TimerHandle_t xTimer) {
    if(current_node == head_node)
    {
        (void)xTimer;
        refresh();
    }

}
void KeyScan_Task(void *arg)
{
  KeyEvent_t k[2];
  while (1)
  {
    key_scan(k);
    for(int i = 0;i<2;i++)
    {
        if(k[i].event != KEY_EVENT_NONE)
        {
            xQueueSend(key_event_queue,&k[i],0);
        }
    }
    vTaskDelay(10);
  }
}

void Display_Task(void *arg)
{
  DisplayMsg_t  refresh_flag = DISPLAY_NONE;
  while (1)
  {
    if(xQueueReceive(display_queue, &refresh_flag, portMAX_DELAY) == pdTRUE)
    {
      if(refresh_flag == DISPLAY_MSG_REDRAW)
      {
        refresh_flag = DISPLAY_NONE;
          if(current_node != NULL && current_node->display != NULL)
          {
            current_node->display();			
          }
          else{
            OLED_Clear();
            OLED_ShowString(0,0,"ERROR",OLED_8X16);
          }
          OLED_Update();
      }

    }
  }
}


void Menu_Task(void *arg)
{
  while(1)
  {
      KeyEvent_t k;
        if(xQueueReceive(key_event_queue, &k, portMAX_DELAY) == pdTRUE)
        {
            switch(k.event)
            {
                case KEY_EVENT_SHORT_PRESS:
                if(k.key == 0)
                {
                    if(current_node->move != NULL)
                    {
                        current_node->move();
                        refresh();
                    }
                }
                else if(k.key == 1)
                {
                    if(current_node->action != NULL)
                    {
                        current_node->action();
                        refresh();
                    }
                }
                break;
                case KEY_EVENT_LONG_PRESS:
                  if(k.key == 0)
                  {
                    move_np_up();
                    refresh();
                  }
                    
                break;
            }
        }

  }
}



/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  key_event_queue = xQueueCreate(10,sizeof(KeyEvent_t));
  display_queue = xQueueCreate(1, sizeof(DisplayMsg_t));
  TaskHandle_t KeyTask_attributes;
  xTaskCreate(KeyScan_Task,"KEY_SCAN_TASK",64,NULL,27,&KeyTask_attributes);

  TaskHandle_t DisplayTask_attributes;
  xTaskCreate(Display_Task,"DISPLAY_TASK",128,NULL,25,&DisplayTask_attributes);
  
  TaskHandle_t MenuTask_attributes;
  xTaskCreate(Menu_Task,"MENU_TASK",256,NULL,26,&MenuTask_attributes);

  refresh_timer = xTimerCreate("refresh_timer",pdMS_TO_TICKS(20),pdTRUE,NULL,refresh_timer_callback);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	creat_menu();
  xTimerStart(refresh_timer, 0);
	vTaskDelete(NULL);
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

