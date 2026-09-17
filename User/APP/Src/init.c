#include "init.h"
#include "tim.h"

void light_init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    /* PWM 输出不需要中断，关闭更新中断防止阻塞 FreeRTOS 任务调度 */
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
}
