#ifndef __UI_H
#define __UI_H

void UI_main(void);
void UI_back(void);
void UI_setting(void);
void UI_light(void);
  typedef enum {
     DISPLAY_NONE = 0,
      DISPLAY_MSG_REDRAW = 1,   // 重绘当前页面（导航、按键触发）
      // 未来扩展：DISPLAY_MSG_TOAST, DISPLAY_MSG_PAGE_JUMP 等
  } DisplayMsg_t;
  
#endif
