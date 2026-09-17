#ifndef __ACTION_H
#define __ACTION_H

#include "main.h"

void light_adjust(void);         // 循环步进: 0→20→40→...→100→0
uint8_t light_get_duty(void);

#endif
