#include "slide_render.h"
#include "oled.h"
#include <string.h>

/* 绘制标题栏：节点名称居中，8X16字体 */
static void draw_title_bar(const char *name)
{
    if (name == NULL) return;

    uint8_t len = 0;
    const char *p = name;
    while (p[len]) len++;

    /* 计算居中X坐标（8X16字体，每个字符宽8像素） */
    int16_t x = (128 - (int16_t)len * 8) / 2;
    if (x < 0) x = 0;

    OLED_ShowString(x, SLIDE_TITLE_Y, (char *)name, OLED_8X16);
}

/* 绘制两条分隔线 */
static void draw_separator_lines(void)
{
    uint8_t i;
    /* Y=11 分隔线 */
    for (i = 0; i < 128; i++) {
        OLED_DrawPoint(i, SLIDE_SEP_LINE1_Y);
    }
    /* Y=54 分隔线 */
    for (i = 0; i < 128; i++) {
        OLED_DrawPoint(i, SLIDE_SEP_LINE2_Y);
    }
}

/* 绘制导航指示器：左右箭头 + 圆点位置 */
static void draw_nav_indicator(Node *node)
{
    uint8_t total = count_siblings(node);
    uint8_t current = get_sibling_index(node);

    /* 左侧箭头 "<"：两条对角线交点在左侧 */
    if (node->prev != NULL) {
        OLED_DrawLine(7, 57, 3, 61);   // 上边到顶点
        OLED_DrawLine(7, 63, 3, 61);   // 下边到顶点
    }

    /* 右侧箭头 ">"：两条对角线交点在右侧 */
    if (node->next != NULL) {
        OLED_DrawLine(121, 57, 125, 61);   // 上边到顶点
        OLED_DrawLine(121, 63, 125, 61);   // 下边到顶点
    }

    /* 圆点指示器 */
    if (total > 1) {
        uint8_t dot_spacing = 12;
        uint8_t dots_width = (total - 1) * dot_spacing;
        uint8_t start_x = (128 - dots_width) / 2;
        uint8_t cy = 60;
        uint8_t i;

        for (i = 0; i < total; i++) {
            uint8_t cx = start_x + i * dot_spacing;
            if (i == current) {
                /* 当前节点：填充圆点（3x3） */
                OLED_DrawPoint(cx - 1, cy);
                OLED_DrawPoint(cx, cy - 1);
                OLED_DrawPoint(cx, cy);
                OLED_DrawPoint(cx, cy + 1);
                OLED_DrawPoint(cx + 1, cy);
            } else {
                /* 非当前节点：单点 */
                OLED_DrawPoint(cx, cy);
            }
        }
    }
}

/* 核心函数：将指定节点的完整页面渲染到OLED_DisplayBuf
 * 布局：
 *   Y=0~10:   标题栏（节点名称居中，8X16字体）
 *   Y=11:     分隔线
 *   Y=12~53:  内容区域（调用node->display()）
 *   Y=54:     分隔线
 *   Y=55~63:  导航指示器（箭头+圆点）
 */
void slide_draw_page(Node *node)
{
    if (node == NULL) return;

    /* 1. 清空OLED显存 */
    OLED_Clear();

    /* 2. 绘制标题栏 */
    draw_title_bar(node->name);

    /* 3. 绘制分隔线 */
    draw_separator_lines();

    /* 4. 调用节点的display函数绘制内容
     * 节点的display函数应该只绘制内容区域（Y=12~53），不绘制标题和导航 */
    if (node->display != NULL) {
        node->display();
    }

    /* 5. 绘制导航指示器（覆盖底部区域） */
    draw_nav_indicator(node);
}
