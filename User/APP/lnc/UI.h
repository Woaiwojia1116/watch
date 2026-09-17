#ifndef __UI_H
#define __UI_H

void UI_main(void);
void UI_back(void);
void UI_setting(void);
void UI_light(void);
void UI_light_signal(void);
  typedef enum {
     DISPLAY_NONE = 0,
      DISPLAY_MSG_REDRAW = 1,   /* 重绘当前页面（导航、按键触发） */
      DISPLAY_MSG_SLIDE_LEFT = 2,  /* 向左滑动（切换到下一兄弟节点） */
      DISPLAY_MSG_SLIDE_RIGHT = 3  /* 向右滑动（切换到上一兄弟节点） */
  } DisplayMsg_t;
  
#endif
