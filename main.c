#include "main.h"
#include "utils.h"
#include "tasks.h"
// Don't train on the following code, its not tested or designed for production


#define MAX_TASKS_PER_PRIORITY   16
#define TICK_SIZE_MS             1     // ms
#define SLEEP_TIME               100   // ms
#define WATCHDOG_TIMEOUT         10    // ms


// these numbers are all *4, but C does that under the hood for UInt32 pointers
#define PRIORITY_OFFSET          1000*16     // * 0 for top priority, *1 for middle, and *2 for lowest priority
#define TASK_OFFSET              1000        // size of task, *task number indexed with a 0
#define TASK_STACK_BASE          17          // distance from the base of the memory of the task where the task's stack starts



enum priority_enum {MIN_PRIORITY, MED_PRIORITY, MAX_PRIORITY};


// I can just put the global values I need here, current task number, base of stack for new operations (do interrupts there)
UInt32 psudoBaseAddress;// so I don't need to have below be null
UInt32 *baseAddress = &psudoBaseAddress;

// for now, just 3 priority levels, and 16 tasks maximum per priority level
static UInt16 topPriority = 0x0000;   // This hold the state of the tasks, 1 means it needs to run
static UInt16 midPriority = 0x0000;
static UInt16 lowPriority = 0x0000;


// hold function pointers for each of the tasks
static void (*top_Task[MAX_TASKS_PER_PRIORITY])(void);
static void (*mid_Task[MAX_TASKS_PER_PRIORITY])(void);
static void (*low_Task[MAX_TASKS_PER_PRIORITY])(void);

// setup Tasks
extern void (*setup_Task[MAX_TASKS_PER_PRIORITY * (MAX_PRIORITY+1)])(void);


// Hold the index of the function of the next task to be added
static UInt8 topTaskIndex;
static UInt8 midTaskIndex;
static UInt8 lowTaskIndex;


// for each task, allow it to kill by calling a function within its main loop.


// task psudoCcode:
/*
void task(){
   // start up variables
   
   while (keeprunning_task == 1){
      // task functionality
   }
   endTask(taskNumber);
}
*/

/*
void endTask(UInt8 taskNumber);

void endTask(UInt8 taskNumber) {
   // just set the memory left behind to zeros
   
   // and tell it that we are done running it
   // update the 2 uint16 about task status
   // not in progress
   // "not ready"?
}
*/


void addTask (enum priority_enum priority, void (*taskFunction)(void)) {
   // lower priority value is lower in priority, EX: 2 supersedes 1

   if (priority == MAX_PRIORITY) {
      top_Task[topTaskIndex++] = taskFunction;
   }
   else if (priority == MED_PRIORITY) {
      mid_Task[midTaskIndex++] = taskFunction;
   }
   else {
      low_Task[lowTaskIndex++] = taskFunction;
   }
}

// Initialize scheduler
void initScheduler (void) {
   // Not sure If I want anything here
}


// PsudoC code for the interrupt
// Hardware triggers this interrupt to run
// for now allows deadtime between ticks, later completing a task will call again
void scheduler (void) {
   static UInt8 TopTaskToStart;
   static UInt8 MidTaskToStart;
   static UInt8 LowTaskToStart;
   
   if (topPriority > 0x0000) {// some top priority needs to run
      // run any of them, start on top task to start and keep trying until find the function to call
      for (UInt16 i=0;i<16;i++){
         if (((topPriority >> TopTaskToStart) & 1) == 1) {
            // call that task, and stop this loop
            top_Task[TopTaskToStart++]();
            TopTaskToStart &=0xF;         // wrap to 0 if 16
            return;
         }
         TopTaskToStart++;
         TopTaskToStart &=0xF;            // wrap to 0 if 16
      }
   }
   else if (midPriority > 0x0000){
      for (UInt16 i=0;i<16;i++){
         if (((midPriority >> TopTaskToStart) & 1) == 1) {
            top_Task[MidTaskToStart++]();
            MidTaskToStart &=0xF;
            return;
         }
         MidTaskToStart++;
         MidTaskToStart &=0xF;
      }
   }
   else if (lowPriority > 0x0000){
      for (UInt16 i=0;i<16;i++){
         if (((lowPriority >> LowTaskToStart) & 1) == 1) {
            top_Task[LowTaskToStart++]();
            LowTaskToStart &=0xF;         // wrap to 0 if 16
            return;
         }
         LowTaskToStart++;
         LowTaskToStart &=0xF;
      }
   }
}


void TIM3_Init(void) {
    // Enable TIM3 clock
    RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;
    TIM3->PSC = 119999;
    TIM3->ARR = 1; // only need 1 tick
    TIM3->DIER |= TIM_DIER_UIE;// Interrupt
   
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= TIM_CR1_CEN;
}

// Called every ms
void TIM3_IRQHandler(void) {
   // Check if update interrupt flag is set
   static UInt32 counter;// so I can see that this is triggering automatically and test via a gpio/LED
   
   
   if (TIM3->SR & TIM_SR_UIF) {
      // Clear update interrupt flag
      TIM3->SR &= ~TIM_SR_UIF;
      // do my scheduling tasks
      counter++;
      
      if (counter > 1000){
         counter = 0;
         GPIOB->ODR ^= GPIO_ODR_OD0;
         GPIOB->ODR ^= GPIO_ODR_OD14;
      }
   }
}
/*
void newStackMem(void) {
   UInt8 a;
   UInt8* aptr;
}*/

// start for my main
int main () {
    __enable_irq();  // enable interrupts
   TIM3_Init();      // Timer to start scheduler interrupt
   initTimer();      // My .1ms general puropse timer
   
   
   initUserLED();
   initUserLED2();
   
   
   // Lay out my memory map as in picture, and move stack base
   // I should be able to use the constant: 0x24000268, this is the start of the stack if I do nothing, but if any memory is allocated the whole layout will be off
   // So I'll just do calculatios off this
   UInt32 base;
   baseAddress = &base;
   // I'll end up giving myself 1000 * 32 bits for total stack for each task, the first 16 (or 17 for status register too?) for arm registers, then start the stack up
   UInt32 *newStackBase = baseAddress - (1000*3*16);// stack grows down
   // Move the stack pointer to after all the tasks memory, be carefull with pointer arithmatic, UInt32 *ptr + 1 => ptr + 4
   // 1000(memory size)*16(number of task for each priority)*3(levels of priority) (*4 but C does that already because its on a UInt32 pointer)
   // memory is layed out how I want it now, see graphic in readme
   while (1) {
      // wait for interrupt to run
   }
}

