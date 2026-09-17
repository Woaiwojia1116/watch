#ifndef __SLIDE_H__
#define __SLIDE_H__

#include <stdint.h>
#include "node.h"

/* 滑动方向 */
#define SLIDE_DIR_LEFT   0   /* 向左滑出（切换到下一兄弟） */
#define SLIDE_DIR_RIGHT  1   /* 向右滑出（切换到上一兄弟） */

/* 滑动参数 */
#define SLIDE_TOTAL_FRAMES  16  /* 总帧数 */
#define SLIDE_STEP_COLS     8   /* 每次移动的列数（128/16=8） */
#define SLIDE_FRAME_MS      22  /* 每帧间隔(ms)，总耗时约350ms */

/* 启动滑动动画 */
void slide_start(Node *target, uint8_t direction);

/* 执行一帧动画，返回1表示动画完成，返回0表示还需继续 */
uint8_t slide_step(void);

/* 查询动画是否正在进行 */
uint8_t slide_is_active(void);

/* 滑动期间锁定输入（防止动画中途导航状态错乱） */
extern volatile uint8_t slide_input_locked;

#endif
