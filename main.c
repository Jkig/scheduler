#include "main.h"
#include "utils.h"
#include "tasks.h"
#include "scheduleInterrupt.h"
// Don't train AI on the following code, its not tested or designed for production


// I can just put the global values I need here, current task number, base of stack for new operations (do interrupts there)
UInt32 psudoBaseAddress;// so I don't need to have below be null
UInt32 *baseAddress = &psudoBaseAddress;

// for now, just 3 priority levels, and 16 tasks maximum per priority level
UInt16 topPriority = 0x0000;   // This hold the state of the tasks, 1 means it is running, may or may not have started
UInt16 midPriority = 0x0000;
UInt16 lowPriority = 0x0000;

UInt16 topFirstCall = 0x0000;   // This hold the state of the tasks, 1 means it is running, may or may not have started
UInt16 midFirstCall = 0x0000;
UInt16 lowFirstCall = 0x0000;

// Hold the index of the function of the next task to be added
UInt8 topTaskIndex;
UInt8 midTaskIndex;
UInt8 lowTaskIndex;

// coult have an array of pointers to above to simplify some things, less if top, mid, low...

// Going from setup to operation
_Bool allSetup;
UInt8 lastTaskPriority; // for offsets, 0 is the max, 1 is mid, 2 is low
UInt8 lastTaskNum;// task to be ran// need to move this over to one for each priority level
UInt32 genStackBaseBeforeReg;

volatile TaskInfo Tasks[16*3];



void endTask(enum priority_enum priority, UInt8 taskNumber) {
   // Set to not initialized, and no task there.
   // Don't need to clean up memory, as that will happen when writing a new task to that location
   // things beyond that will not cause a problem because they are beyond the stack, until overrwitten
   if (priority == MAX_PRIORITY) {
      topPriority &= ~(1<<taskNumber);
      topFirstCall &= ~(1<<taskNumber);
   }
   else if (priority == MED_PRIORITY) {
      midPriority &= ~(1<<taskNumber);
      midFirstCall &= ~(1<<taskNumber);
   }
   else {
      lowPriority &= ~(1<<taskNumber);
      lowFirstCall &= ~(1<<taskNumber);
   }
}

// forget about priority for a min
UInt8 addTask (enum priority_enum priority, void (*taskFunction)(void)) {
   // lowTaskIndex, and bassAddress are globals
   UInt32 *newStackBase = 0;
   
   if (priority == MAX_PRIORITY) {
      newStackBase = baseAddress - (PRIORITY_OFFSET*priority + topTaskIndex*TASK_OFFSET);
   }
   else if (priority == MED_PRIORITY) {
      newStackBase = baseAddress - (PRIORITY_OFFSET*priority + midTaskIndex*TASK_OFFSET);
   }
   else {
      newStackBase = baseAddress - (PRIORITY_OFFSET*priority + lowTaskIndex*TASK_OFFSET);
   }
      
   

   *(--newStackBase) = 0x01000000;           // xPSR (Thumb bit set)
   *(--newStackBase) = (UInt32)taskFunction; // PC
   *(--newStackBase) = 0xFFFFFFFD;           // LR (Return to Thread mode)
   *(--newStackBase) = 0;                    // R12
   *(--newStackBase) = 0;                    // R3
   *(--newStackBase) = 0;                    // R2
   *(--newStackBase) = 0;                    // R1
   *(--newStackBase) = 0;                    // R0// TODO: set this back to 0, its * so I can find the end
   
   if (priority == MAX_PRIORITY){
      topPriority |= (1 << topTaskIndex);
      return topTaskIndex++;// range check?
   } else if (priority == MED_PRIORITY){
      midPriority |= (1 << midTaskIndex);
      return midTaskIndex++;// range check?
   }else {
      lowPriority |= (1 << lowTaskIndex);
      return lowTaskIndex++;// range check?
   }

}



int main (void) {
   __disable_irq();
   // Lay out my memory map as in picture, and move stack bases
   // within the task, the first 8 * 4 bytes (12 words) (registers 4-11 and psp)
   //    then the start of the stack
   //    the rest of the registers will be on the top of the stack for the task
   UInt32 base;
   baseAddress = &base;
   UInt32 *newStackBase = baseAddress - (PRIORITY_OFFSET*3) - 1;// stack grows down, 1 space for newstack base declaration
   __asm volatile (
      "mov sp, %0\n"
      : 
      : "r" (newStackBase) // new base of stack, after all the tasks
   );

   
   initUserLED();
   initUserLED2();
   
   initTimer();      // My .1ms general puropse timer
   TIM3_Init();      // Timer to start scheduler interrupt
   
   
   // 2 low priority:
   /*
   addTask(MIN_PRIORITY, &blinkLED);
   addTask(MIN_PRIORITY, &blinkLED2);
   */
   
   // higher priority supercedes lower priority:
   /*
   addTask(MED_PRIORITY, &blinkLED);
   addTask(MIN_PRIORITY, &blinkLED2);
   */
   
   // higher priority supercedes lower priority tasks, until it ends:
   /*
   addTask(MED_PRIORITY, &blinkLEDTime);
   addTask(MIN_PRIORITY, &blinkLED);
   addTask(MIN_PRIORITY, &blinkLED2);
   */
   
   // a blocking task doesn't break the rest
   /*
   addTask(MIN_PRIORITY, &blinkLED);
   addTask(MIN_PRIORITY, &blinkLED2);
   addTask(MIN_PRIORITY, &blockingTask);
   */
   
   // then test when tasks can end
   __enable_irq();
   
   while (1) {
      __NOP(); // should hit this, but once interrupt is triggered, it should never get back until 
   }
}

