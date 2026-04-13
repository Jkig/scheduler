#ifndef SCHEDULE_H
#define SCHEDULE_H
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// I can just put the global values I need here, current task number, base of stack for new operations (do interrupts there)
extern uint32_t psudoBaseAddress;  // so I don't need to have below be null
extern uint32_t *baseAddress;

// for now, just 3 priority levels, and 16 tasks maximum per priority level
extern uint16_t topPriority;       // This hold the state of the tasks, 1 means it is running, may or may not have started
extern uint16_t midPriority;
extern uint16_t lowPriority;

// Hold the index of the function of the next task to be added
extern uint8_t topTaskIndex;
extern uint8_t midTaskIndex;
extern uint8_t lowTaskIndex;

extern uint16_t topFirstCall;      // This hold the state of the tasks, 1 means it is running, may or may not have started
extern uint16_t midFirstCall;
extern uint16_t lowFirstCall;

// Going from setup to operation
extern bool allSetup;
extern uint8_t lastTaskPriority;   // for offsets, 0 is the max, 1 is mid, 2 is low
extern uint8_t lastTaskNum;        // task to be ran// need to move this over to one for each priority level
extern uint32_t genStackBaseBeforeReg;
extern uint32_t sp_torestoreTo;
extern uint32_t lr_torestoreTo;

extern volatile TaskInfo Tasks[16*3];

void TIM3_Init(void);
void TIM3_IRQHandler(void);

#endif
