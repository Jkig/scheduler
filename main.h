#include "stm32h743xx.h"

#ifndef MAIN_H
#define MAIN_H
typedef char            Int8;
typedef unsigned char   UInt8;
typedef short           Int16;
typedef unsigned short  UInt16;
typedef long            Int32;
typedef unsigned long   UInt32;


void addTask (UInt8 priority, void (*taskFunction)(void));
void initScheduler (void);
void scheduler (void);
void delay(int count);
#endif
