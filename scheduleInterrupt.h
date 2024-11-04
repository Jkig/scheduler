#ifndef SCHEDULE_H
#define SCHEDULE_H
#include "main.h"

// I can just put the global values I need here, current task number, base of stack for new operations (do interrupts there)
extern UInt32 psudoBaseAddress;  // so I don't need to have below be null
extern UInt32 *baseAddress;

// for now, just 3 priority levels, and 16 tasks maximum per priority level
extern UInt16 topPriority;       // This hold the state of the tasks, 1 means it is running, may or may not have started
extern UInt16 midPriority;
extern UInt16 lowPriority;

// Hold the index of the function of the next task to be added
extern UInt8 topTaskIndex;
extern UInt8 midTaskIndex;
extern UInt8 lowTaskIndex;

extern UInt16 topFirstCall;      // This hold the state of the tasks, 1 means it is running, may or may not have started
extern UInt16 midFirstCall;
extern UInt16 lowFirstCall;

// Going from setup to operation
extern _Bool allSetup;
extern UInt8 lastTaskPriority;   // for offsets, 0 is the max, 1 is mid, 2 is low
extern UInt8 lastTaskNum;        // task to be ran// need to move this over to one for each priority level
extern UInt32 genStackBaseBeforeReg;
extern UInt32 sp_torestoreTo;
extern UInt32 lr_torestoreTo;

extern volatile TaskInfo Tasks[16*3];

void TIM3_Init(void);
void TIM3_IRQHandler(void);

#endif
