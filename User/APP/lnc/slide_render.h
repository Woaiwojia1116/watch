#ifndef __SLIDE_RENDER_H__
#define __SLIDE_RENDER_H__

#include <stdint.h>
#include "node.h"

/* 页面布局常量（128x64 OLED） */
#define SLIDE_TITLE_Y       0     /* 标题栏起始Y */
#define SLIDE_TITLE_HEIGHT  11    /* 标题栏高度（8X16字体占2页，这里只占1页8像素+间距） */
#define SLIDE_SEP_LINE1_Y   20    /* 第一条分隔线Y坐标 */
#define SLIDE_CONTENT_Y     21    /* 内容区域起始Y */
#define SLIDE_CONTENT_END_Y 63    /* 内容区域结束Y */
#define SLIDE_SEP_LINE2_Y   54    /* 第二条分隔线Y坐标 */
#define SLIDE_NAV_Y         56    /* 导航指示器起始Y */

/* 核心函数：将指定节点的完整页面渲染到OLED_DisplayBuf（标题+内容+导航） */
void slide_draw_page(Node *node);

#endif
