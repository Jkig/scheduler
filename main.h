#include "stm32h743xx.h"

#ifndef MAIN_H
#define MAIN_H
typedef char            Int8;
typedef unsigned char   UInt8;
typedef short           Int16;
typedef unsigned short  UInt16;
typedef long            Int32;
typedef unsigned long   UInt32;



// Scheduler constants:
#define MAX_TASKS_PER_PRIORITY   16
#define TICK_SIZE_MS             1     // ms
#define SLEEP_TIME               100   // ms
#define WATCHDOG_TIMEOUT         10    // ms

// these numbers are all *4, but C does that under the hood for UInt32 pointers
#define TASK_OFFSET              1000        // size of task, *task number indexed with a 0
#define PRIORITY_OFFSET          TASK_OFFSET*16     // * 0 for top priority, *1 for middle, and *2 for lowest priority
#define TASK_STACK_BASE          17          // distance from the base of the memory of the task where the task's stack starts



enum priority_enum {MIN_PRIORITY, MED_PRIORITY, MAX_PRIORITY};

UInt8 addTask (UInt8 priority, void (*taskFunction)(void));
void endTask(enum priority_enum priority, UInt8 taskNumber);
void TIM3_Init(void);// initialize scheduler interrupt
void initScheduler (void);
void scheduler (void);
void delay(int count);
#endif
