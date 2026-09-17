#include "key_scan.h"
#include "stm32f1xx_hal.h"

static key_info key[2] = {
    {GPIOA, GPIO_PIN_0, 0}, // Key1
    {GPIOA, GPIO_PIN_1, 0}  // Key2
};


typedef enum {
    KEY_STATE_IDLE,           // 空闲态
    KEY_STATE_SHAKE,          // 消抖态
    KEY_STATE_PRESSED,        // 按下态
    KEY_STATE_RELEASE_SHAKE,  // 释放消抖态
    KEY_STATE_LONG_PRESSED    // 长按确认态
} key_state_t;

static key_state_t k[2] = {KEY_STATE_IDLE, KEY_STATE_IDLE};

static uint8_t long_press_triggered[2] = {0, 0};

void key_scan(KeyEvent_t* event)
{
    event[0].key = 0;
    event[1].key = 1;
    event[0].event = KEY_EVENT_NONE;
    event[1].event = KEY_EVENT_NONE;

    // 读取按键状态
    key[0].key_value = HAL_GPIO_ReadPin(key[0].GPIOx, key[0].GPIO_Pin);
    key[1].key_value = HAL_GPIO_ReadPin(key[1].GPIOx, key[1].GPIO_Pin);

    for (int i = 0;i<2;i++)
    {
        switch(k[i])
            {
                case KEY_STATE_IDLE:
                    if(key[i].key_value == GPIO_PIN_RESET)  // 按键按下
                    {
                        k[i] = KEY_STATE_SHAKE;
                        key[i].debounce_cnt = 0;
                        key[i].long_press_cnt = 0;
                        long_press_triggered[i] = 0;
                    }
                    break;
                    
                case KEY_STATE_SHAKE:
                    if(key[i].key_value == GPIO_PIN_RESET)
                    {
                        key[i].debounce_cnt++;
                        if(key[i].debounce_cnt >= 3)
                        {
                            k[i] = KEY_STATE_PRESSED;
                            key[i].debounce_cnt = 0;
                            key[i].long_press_cnt = 0;
                            long_press_triggered[i] = 0;
                        }
                    }
                    else  // 抖动，回到空闲
                    {
                        key[i].debounce_cnt = 0;
                        k[i] = KEY_STATE_IDLE;
                    }
                    break;
                    
                case KEY_STATE_PRESSED:
                    key[i].long_press_cnt++;

                    // 长按触发（优先判断，防止毛刺释放导致长按失效）
                    if(key[i].long_press_cnt >= 100 && !long_press_triggered[i])
                    {
                        event[i].event = KEY_EVENT_LONG_PRESS;
                        long_press_triggered[i] = 1;
                        k[i] = KEY_STATE_LONG_PRESSED;
                        key[i].debounce_cnt = 0;
                    }
                    // 检测到释放（短按）— 需连续多次确认，防止噪声毛刺误判
                    else if(key[i].key_value == GPIO_PIN_SET)
                    {
                        key[i].debounce_cnt++;
                        if(key[i].debounce_cnt >= 3)
                        {
                            k[i] = KEY_STATE_RELEASE_SHAKE;
                            key[i].debounce_cnt = 0;
                        }
                    }
                    else
                    {
                        key[i].debounce_cnt = 0;
                    }
                    break;
                    
                case KEY_STATE_LONG_PRESSED:
                    // 等待按键释放
                    if(key[i].key_value == GPIO_PIN_SET)
                    {
                        key[i].debounce_cnt++;
                        if(key[i].debounce_cnt >= 3)  // 释放消抖
                        {
                            k[i] = KEY_STATE_IDLE;
                            key[i].debounce_cnt = 0;
                            key[i].long_press_cnt = 0;
                            long_press_triggered[i] = 0;
                        }
                    }
                    else  // 还没释放，继续等待
                    {
                        key[i].debounce_cnt = 0;
                        // 保持在长按确认态
                    }
                    break;
                    
                case KEY_STATE_RELEASE_SHAKE:
                    if(key[i].key_value == GPIO_PIN_SET)  // 按键已释放
                    {
                        key[i].debounce_cnt++;
                        if(key[i].debounce_cnt >= 3)  // 释放消抖完成
                        {
                            // 短按触发
                            if(!long_press_triggered[i])
                            {
                                event[i].event = KEY_EVENT_SHORT_PRESS;
                            }

                            // 回到空闲态
                            k[i] = KEY_STATE_IDLE;
                            key[i].debounce_cnt = 0;
                            key[i].long_press_cnt = 0;
                            long_press_triggered[i] = 0;
                        }
                    }
                    else  // 释放过程中又按下（可能是抖动）
                        key[i].debounce_cnt = 0;  // 继续等待释放
                    break;
            }
             
        }
    
}
