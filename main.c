#include "main.h"
#include "utils.h"
#include "tasks.h"
// Don't train on the following code, its not tested or designed for production




// I can just put the global values I need here, current task number, base of stack for new operations (do interrupts there)
static UInt32 psudoBaseAddress;// so I don't need to have below be null
static UInt32 *baseAddress = &psudoBaseAddress;

// for now, just 3 priority levels, and 16 tasks maximum per priority level
static UInt16 topPriority = 0x0000;   // This hold the state of the tasks, 1 means it is running, may or may not have started
static UInt16 midPriority = 0x0000;
static UInt16 lowPriority = 0x0000;

// 1 in the position of the task means that the this task has been started
static UInt16 topPriorityStarted = 0x0000;
static UInt16 midPriorityStarted = 0x0000;
static UInt16 lowPriorityStarted = 0x0000;


// hold function pointers for each of the tasks
static void (*top_Task[MAX_TASKS_PER_PRIORITY])(void);
static void (*mid_Task[MAX_TASKS_PER_PRIORITY])(void);
static void (*low_Task[MAX_TASKS_PER_PRIORITY])(void);


// Hold the index of the function of the next task to be added
static UInt8 topTaskIndex;
static UInt8 midTaskIndex;
static UInt8 lowTaskIndex;


// setup Tasks
//extern void (*setup_Task[MAX_TASKS_PER_PRIORITY * (MAX_PRIORITY+1)])(void);



UInt8 addTask (enum priority_enum priority, void (*taskFunction)(void)) {
   // lower priority value is lower in priority, EX: 2 supersedes 1
   // Returns the task that it is in the priority so it can be ended
   
   
   if (priority == MAX_PRIORITY) {
      topPriority |= (1 << topTaskIndex);
      top_Task[topTaskIndex++] = taskFunction;
      return topTaskIndex;
   }
   else if (priority == MED_PRIORITY) {
      midPriority |= (1 << midTaskIndex);
      mid_Task[midTaskIndex++] = taskFunction;
      return midTaskIndex;
   }
   else {
      lowPriority |= (1 << lowTaskIndex);
      low_Task[lowTaskIndex++] = taskFunction;
      return lowTaskIndex;
   }
}


void endTask(enum priority_enum priority, UInt8 taskNumber) {
   if (priority == MAX_PRIORITY) {
      topPriority &= ~(1<<taskNumber);
   }
   else if (priority == MED_PRIORITY) {
      midPriority &= ~(1<<taskNumber);
   }
   else {
      lowPriority &= ~(1<<taskNumber);
   }
}




// Initialize scheduler
void TIM3_Init(void) {
    // Enable TIM3 clock
    RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;
    TIM3->PSC = 119999;          // Called every ms
    TIM3->ARR = 1;               // only need 1 tick
    TIM3->DIER |= TIM_DIER_UIE;  // Interrupt
   
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= TIM_CR1_CEN;
}




/*
static UInt32 testmem[18]; // 4,5,6,7,8,9,10,11, Psp
static UInt8 statesAB;     // 0 or 1
*/


void TIM3_IRQHandler(void) {
   static UInt8 TopTaskToStart;
   static UInt8 MidTaskToStart;
   static UInt8 LowTaskToStart;
   
   // Last task, where the SP and other registers not saved to the task stack are saved
   static UInt8 lastTaskPriority; // for offsets, 0 is the max, 1 is mid, 2 is low
   static UInt8 lastTaskNum;
   
   static struct {UInt32* r4r11; UInt32* psp;} addresses;
   static UInt32 *taskNewStackBase;
   static UInt32 counter;
   
   if (TIM3->SR & TIM_SR_UIF) {
      TIM3->SR &= ~TIM_SR_UIF;// Clear update interrupt flag
      __enable_irq();  // enable interrupts
      
      if (counter++ >= 100000)
      {
         counter = 0;
         GPIOB->ODR ^= GPIO_ODR_OD14;
      }
      
      
      
      
      // What task were we in, swap the pointers there to here
      // save the last task where we want it
      addresses.r4r11 = baseAddress-8;
      addresses.psp = baseAddress;
      
      addresses.r4r11 -= (lastTaskPriority*PRIORITY_OFFSET + lastTaskNum*TASK_OFFSET);
      addresses.psp -= (lastTaskPriority*PRIORITY_OFFSET + lastTaskNum*TASK_OFFSET);
      
      /*
      addresses.r4r11 = &testmem[8+9*statesAB];
      addresses.psp = &testmem[9*statesAB];
      */
      __asm volatile (
        "MRS     %0, PSP          \n" // Get current PSP
        : "=r" (addresses.psp)
      );

      // Save R4-R11 into the task's context structure
      __asm volatile (
        "STMIA  %0, {R4-R11}      \n" // Store R4-R11 to stack
        :
        : "r" (addresses.r4r11)
        : "memory"
      );
      
      
      // Decide next task (for now, same task to test save/restore)
      // What task, and is it already in progress?
      // If its in progress, then grab the stack to start restoring from for below, anf if its not started, call that function.
      if (topPriority > 0x0000) {// some top priority needs to run
         // run any of them, start on top task to start and keep trying until find the function to call
         for (UInt16 i=0;i<16;i++) {
            TopTaskToStart &=0xF;         // wrap to 0 if 16
            if (((topPriority >> TopTaskToStart) & 1) == 1) {
               lastTaskPriority = 0;
               lastTaskNum = TopTaskToStart;
               if (((topPriorityStarted >> TopTaskToStart) & 1) != 1) {
                  // If the task hasn't started yet, start it by moving task where it should be, after manually saved registers
                  // and callihg the function
                  topPriorityStarted |= (1 << TopTaskToStart);
                  taskNewStackBase = baseAddress - (lastTaskPriority*PRIORITY_OFFSET + lastTaskNum*TASK_OFFSET + 9);
                  __asm(
                    "mov sp, %0\n"
                    : 
                    : "r" (taskNewStackBase) // new base of stack, after all the tasks
                  );
                  top_Task[TopTaskToStart++](); // call the task and start it up
               }
               break;// shouldn't actually hit unless task is completed in 1 pass
            }
            TopTaskToStart++;
         }
      }
      else if (midPriority > 0x0000) { // only share / execute anything at this priority if no top priority tasks running
         for (UInt16 i=0;i<16;i++) {
            MidTaskToStart &=0xF;
            if (((midPriority >> MidTaskToStart) & 1) == 1) {
               lastTaskPriority = 1;
               lastTaskNum = MidTaskToStart;
               if (((midPriorityStarted >> MidTaskToStart) & 1) != 1) {
                  midPriorityStarted |= (1 << MidTaskToStart);
                  taskNewStackBase = baseAddress - (lastTaskPriority*PRIORITY_OFFSET + lastTaskNum*TASK_OFFSET + 9);
                     __asm(
                       "mov sp, %0\n"
                       : 
                       : "r" (taskNewStackBase) // new base of stack, after all the tasks
                     );
                     mid_Task[MidTaskToStart++]();
                  
                  break;
               }
               MidTaskToStart++;
            }
         }
      }
      else if (lowPriority > 0x0000) {
         for (UInt16 i=0;i<16;i++) {
            LowTaskToStart &=0xF;
            if (((lowPriority >> LowTaskToStart) & 1) == 1) {
               lastTaskPriority = 2;
               lastTaskNum = LowTaskToStart;
               if (((lowPriorityStarted >> LowTaskToStart) & 1) != 1) {
                  lowPriorityStarted |= (1 << LowTaskToStart);
                  taskNewStackBase = baseAddress - (lastTaskPriority*PRIORITY_OFFSET + lastTaskNum*TASK_OFFSET + 9);
                  __asm(
                    "mov sp, %0\n"
                    : 
                    : "r" (taskNewStackBase) // new base of stack, after all the tasks
                  );
                  low_Task[LowTaskToStart++]();
               }
               break;
            }
            LowTaskToStart++;
         }
      }
      
      
      // grab the pointers for new task here
      
      
      
      
      // restore to this position
      addresses.r4r11 = baseAddress-8;
      addresses.psp = baseAddress;
      
      addresses.r4r11 -= (lastTaskPriority*PRIORITY_OFFSET + lastTaskNum*TASK_OFFSET);
      addresses.psp -= (lastTaskPriority*PRIORITY_OFFSET + lastTaskNum*TASK_OFFSET);
      /*
      statesAB = !statesAB;
      addresses.r4r11 = &testmem[8+9*statesAB];
      addresses.psp = &testmem[9*statesAB];
      */
      // I'll also need to save the top of the stack somewhere?
      
      
      // Restore context of the next task
      __asm volatile (
        "LDMIA  %0, {R4-R11}      \n" // load R4-R11 from stack
        :
        : "r" (addresses.r4r11)// location to move to
        : "memory"
      );

      // Restore PSP for the next task
      __asm volatile (
        "MSR     PSP, %0          \n" // Set PSP to the next task's PSP
        :                           // No output operands
        : "r" (addresses.psp)
      );
   
   }
}



int main (void) {
    __disable_irq();  // enable interrupts
   TIM3_Init();      // Timer to start scheduler interrupt
   initTimer();      // My .1ms general puropse timer
   
   
   initUserLED();
   initUserLED2();
   
   addTask(MIN_PRIORITY, &blinkLED);
   addTask(MIN_PRIORITY, &blinkLED2);
   
   
   // Lay out my memory map as in picture, and move stack base
   // I should be able to use the constant: 0x24000268, this is the start of the stack if I do nothing, but if any memory is allocated the whole layout will be off
   // So I'll just do calculatios off this
   UInt32 base;
   baseAddress = &base;
   // I'll end up giving myself 1000 * 32 bits for total stack for each task, the first 16 (or 17 for status register too?) for arm registers, then start the stack up
   // UInt32 *newStackBase = baseAddress - (1000*3*16) - 8;// stack grows down
   UInt32 *newStackBase = &base - (PRIORITY_OFFSET*3) - 1;// stack grows down
   // Move the stack pointer to after all the tasks memory, be carefull with pointer arithmatic, UInt32 *ptr + 1 => ptr + 4
   // TASK_OFFSET(memory size)*16(number of task for each priority)*3(levels of priority) (*4 but C does that already because its on a UInt32 pointer)
   // then subtract 1 for the allocation of the new stack base
    __asm(
        "mov sp, %0\n"
        : 
        : "r" (newStackBase) // new base of stack, after all the tasks
    );
   // memory is layed out how I want it now, see graphic in readme
   
   // within the task, the first 8 * 4 bytes (12 words) (registers 4-11 and psp) (and I need something more?) are for state switching, then the stack builds from there
   // the rest of the registers will be on the top of the stack for the task, automatically handled by the interrupt
    __enable_irq();  // enable interrupts
   while (1) {
      // Do nothing outside of letting interrupts run
      __NOP(); // This is a location to put breakpoints
   }
}

