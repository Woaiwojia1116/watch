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
#include "node.h"
#include "slide.h"
#include "slide_render.h"
#include "oled.h"
#define tiaoshi 0

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
    (void)xTimer;
    /* 仅在根节点且无滑动动画时自动刷新（用于main界面动画） */
    if(current_node == head_node && !slide_is_active())
    {
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
    vTaskDelay(6);
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
        if(current_node == head_node) {
          /* 根节点：独立显示（保持原有动画效果） */
          if(current_node != NULL && current_node->display != NULL)
          {
            current_node->display();
          }
          else{
            OLED_Clear();
            OLED_ShowString(0,0,"ERROR",OLED_8X16);
          }
          OLED_Update();
        } else {
          /* 非根节点：使用统一渲染器 */
          if(current_node != NULL && current_node->name != NULL)
          {
            slide_draw_page(current_node);
          }
          else if(current_node != NULL && current_node->display != NULL)
          {
            current_node->display();
          }
          OLED_Update();
        }
      }
      else if(refresh_flag == DISPLAY_MSG_SLIDE_LEFT || refresh_flag == DISPLAY_MSG_SLIDE_RIGHT)
      {
        /* 滑动动画 */
        uint8_t dir = (refresh_flag == DISPLAY_MSG_SLIDE_RIGHT) ? SLIDE_DIR_RIGHT : SLIDE_DIR_LEFT;
        refresh_flag = DISPLAY_NONE;
        slide_start(current_node, dir);
        /* 执行动画帧 */
        while(!slide_step())
        {
          OLED_Update();
          vTaskDelay(pdMS_TO_TICKS(SLIDE_FRAME_MS));
        }
        /* 最后一帧 */
        OLED_Update();
        slide_input_locked = 0;
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
            /* 滑动动画期间忽略按键 */
            if(slide_input_locked) continue;

            switch(k.event)
            {
                case KEY_EVENT_SHORT_PRESS:
                if(k.key == 0)
                {
                    /* Key0短按：前进 */
                    if(current_node == head_node) {
                        /* 根节点：进入子节点（即时切换） */
                        if(current_node->child != NULL)
                        {
                            current_node = current_node->child;
                            refresh();
                        }
                    } else if(current_node->next != NULL) {
                        /* 有下一兄弟：向左滑动切换 */
                        current_node = current_node->next;
                        DisplayMsg_t msg = DISPLAY_MSG_SLIDE_LEFT;
                        xQueueOverwrite(display_queue, &msg);
                    } 
                }
                else if(k.key == 1)
                {
                    /* Key1短按：执行当前节点的功能 */
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
                    /* Key0长按：返回 */
                    if(current_node == head_node) {
                        /* 根节点：忽略 */
                    } else if(current_node->prev != NULL) {
                        /* 有上一兄弟：向右滑动切换 */
                        current_node = current_node->prev;
                        DisplayMsg_t msg = DISPLAY_MSG_SLIDE_RIGHT;
                        xQueueOverwrite(display_queue, &msg);
                    }else if(current_node->father != NULL) {
                        /* 没有上一兄弟：返回父节点（即时切换） */
                        current_node = current_node->father;
                        refresh();
                    }
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

