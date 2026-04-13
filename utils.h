#ifndef UTILS_H
#define UTILS_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#define TEN_THOUSANTH_SEC  1
#define MS_1               10
#define MS_2               2*MS_1
#define MS_10              10*MS_1
#define S_1                1000*MS_1

#define CURRENT_TIME TIM2->CNT

void initTimer(void);
bool hasTimePassed (uint32_t start, uint32_t end);
void initUserLED(void);
void initUserLED2(void);

#endif
