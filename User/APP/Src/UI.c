#include "main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void UI_main(void)
{
    static uint8_t s_frame_count = 0;
    static uint8_t s_dot_phase = 0;
    static uint8_t s_scan_pos = 0;
    
    OLED_Clear();
    
    s_frame_count++;
    s_dot_phase = (s_frame_count / 15) % 2;
    s_scan_pos = (s_frame_count / 2) % 128;
    
    // 动态呼吸灯效果（通过改变圆大小模拟呼吸）
    uint8_t breathe_radius = 3 + (s_frame_count % 20 < 10 ? s_frame_count % 10 : 19 - s_frame_count % 10) / 3;
    
    // 外发光效果 - 多层圆叠加
    OLED_DrawCircle(8, 8, 7, OLED_UNFILLED);
    OLED_DrawCircle(8, 8, 5, OLED_UNFILLED);
    if (s_dot_phase == 0) {
        OLED_DrawCircle(8, 8, breathe_radius, OLED_FILLED);
    } else {
        OLED_DrawCircle(8, 8, 2, OLED_FILLED);
        OLED_DrawCircle(8, 8, 4, OLED_UNFILLED);
    }
    
    // 系统名称 - 带下划线装饰
    OLED_ShowString(20, 1, "SYSTEM", OLED_6X8);
    OLED_DrawLine(20, 10, 70, 10);  // 下划线
    
    // 版本号 - 带边框
    OLED_ShowString(96, 1, "v1.0", OLED_6X8);
    OLED_DrawRectangle(94, 0, 32, 12, OLED_UNFILLED);
    
    // ============================================
    // 2. 动态扫描线效果 (Y=18)
    // ============================================
    OLED_DrawLine(0, 18, 127, 18);
    // 扫描点沿分隔线移动
    OLED_DrawCircle(s_scan_pos, 18, 2, OLED_FILLED);
    OLED_DrawCircle(s_scan_pos - 3, 18, 1, OLED_UNFILLED);
    OLED_DrawCircle(s_scan_pos + 3, 18, 1, OLED_UNFILLED);
    
    // ============================================
    // 3. 中央大标题 - 双层阴影效果 (Y=22~42)
    // ============================================
    // 主标题
    OLED_ShowString(35, 25, "FREERTOS", OLED_6X8);
    // 装饰边框
    OLED_DrawRectangle(14, 20, 100, 18, OLED_UNFILLED);
    // 四个角装饰
    OLED_DrawCircle(16, 22, 1, OLED_FILLED);
    OLED_DrawCircle(112, 22, 1, OLED_FILLED);
    OLED_DrawCircle(16, 36, 1, OLED_FILLED);
    OLED_DrawCircle(112, 36, 1, OLED_FILLED);
    
    // ============================================
    // 4. 底部菜单栏 (Y=56~63)
    // ============================================
    
    // 显示MENU
    OLED_ShowString(2, 56, "[MENU]", OLED_6X8);
    

    OLED_ShowString(70, 56, "ENTER", OLED_6X8);
    OLED_ReverseArea(70, 55,33, 10);

    
    // 右下角装饰
    OLED_DrawLine(123, 61, 127, 61);
    OLED_DrawLine(127, 61, 127, 57);
}
void UI_back(void)
{
    OLED_Clear();
    OLED_ShowString(0, 32, "Back    ", OLED_6X8);
}
void UI_setting(void)
{
    OLED_Clear();
    OLED_ShowString(0, 32, "Setting", OLED_6X8);
}
void UI_light(void)
{
    OLED_Clear();
    OLED_ShowString(0, 32, "Light    ", OLED_6X8);
}
