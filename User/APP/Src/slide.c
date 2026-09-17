#include "slide.h"
#include "slide_render.h"
#include "oled.h"
#include <string.h>

/* 缓冲区定义 */
static uint8_t old_screen_buf[8][128];   /* 滑动前（旧屏幕） */
static uint8_t new_screen_buf[8][128];   /* 目标页面（新屏幕） */

/* 滑动状态 */
static uint8_t slide_active = 0;
static uint8_t slide_direction = 0;
static uint8_t slide_frame = 0;

/* 输入锁 */
volatile uint8_t slide_input_locked = 0;

/* 启动滑动动画 */
void slide_start(Node *target, uint8_t direction)
{
    if (target == NULL) return;

    /* 1. 保存当前屏幕（旧屏幕） */
    memcpy(old_screen_buf, OLED_DisplayBuf, sizeof(old_screen_buf));

    /* 2. 渲染目标页面到 OLED_DisplayBuf */
    slide_draw_page(target);

    /* 3. 保存新屏幕 */
    memcpy(new_screen_buf, OLED_DisplayBuf, sizeof(new_screen_buf));

    /* 4. 初始化状态 */
    slide_active = 1;
    slide_direction = direction;
    slide_frame = 0;
    slide_input_locked = 1;
}

/* 执行一帧动画
 * 返回值：1 = 动画完成，0 = 还需继续
 *
 * 合成原理（以 SLIDE_LEFT 为例）：
 *   - 偏移量 offset = slide_frame * SLIDE_STEP_COLS
 *   - 列 [0, 128-offset)：取旧屏幕的列 [offset, 128) → 旧屏幕向左移出
 *   - 列 [128-offset, 128)：取新屏幕的列 [0, offset) → 新屏幕从右侧移入
 */
uint8_t slide_step(void)
{
    uint8_t offset;
    uint8_t col;

    if (!slide_active) return 1;

    slide_frame++;
    offset = slide_frame * SLIDE_STEP_COLS;

    if (offset >= 128) {
        /* 动画完成：将新屏幕完整写入 OLED_DisplayBuf */
        memcpy(OLED_DisplayBuf, new_screen_buf, sizeof(OLED_DisplayBuf));
        slide_active = 0;
        slide_input_locked = 0;
        return 1;
    }

    if (slide_direction == SLIDE_DIR_LEFT) {
        /* 向左滑动：旧屏幕左移，新屏幕从右入 */
        for (col = 0; col < 128 - offset; col++) {
            uint8_t page;
            for (page = 0; page < 8; page++) {
                OLED_DisplayBuf[page][col] = old_screen_buf[page][col + offset];
            }
        }
        for (col = 128 - offset; col < 128; col++) {
            uint8_t page;
            uint8_t src_col = col - (128 - offset);
            for (page = 0; page < 8; page++) {
                OLED_DisplayBuf[page][col] = new_screen_buf[page][src_col];
            }
        }
    } else {
        /* 向右滑动：旧屏幕右移，新屏幕从左入 */
        for (col = 0; col < offset; col++) {
            uint8_t page;
            uint8_t src_col = col + (128 - offset);
            for (page = 0; page < 8; page++) {
                OLED_DisplayBuf[page][col] = new_screen_buf[page][src_col];
            }
        }
        for (col = offset; col < 128; col++) {
            uint8_t page;
            uint8_t src_col = col - offset;
            for (page = 0; page < 8; page++) {
                OLED_DisplayBuf[page][col] = old_screen_buf[page][src_col];
            }
        }
    }

    return 0;
}

/* 查询动画是否正在进行 */
uint8_t slide_is_active(void)
{
    return slide_active;
}
