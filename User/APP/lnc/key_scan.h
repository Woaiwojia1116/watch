#ifndef __KEY_SCAN_H
#define __KEY_SCAN_H

#include "main.h"

typedef enum {
    KEY_EVENT_NONE = 0,      // 无事件
    KEY_EVENT_SHORT_PRESS,   // 短按（按下后释放）
    KEY_EVENT_LONG_PRESS,    // 长按
} event;
  typedef struct {
      uint8_t key;      // 0 = 按键1, 1 = 按键2
      event event; 
} KeyEvent_t;

typedef struct {
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    uint8_t key_value;
    uint8_t debounce_cnt;//消抖次数
    uint8_t long_press_cnt;//长按计数 
} key_info;

void key_scan(KeyEvent_t* event);


#endif
