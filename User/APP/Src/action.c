#include "main.h"
#include "action.h"
#include "tim.h"

static uint8_t s_duty = 0;       // 当前占空比 0-100
static const uint8_t STEP = 20;  // 步进值

void light_adjust(void)
{
    s_duty += STEP;
    if (s_duty > 100)
    {
        s_duty = 0;
    }
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, s_duty);
}
uint8_t light_get_duty(void)
{
    return s_duty;
}
