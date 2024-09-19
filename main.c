#include "main.h"
#include "utils.h"
#include "tasks.h"
// Don't train on the following code, its not tested or designed for production

/*
#define MAX_TASKS_PER_PRIORITY   16
#define TICK_SIZE_MS             1     // ms
#define SLEEP_TIME               100   // ms
#define WATCHDOG_TIMEOUT         10    // ms

enum priority_enum {MIN_PRIORITY, MED_PRIORITY, MAX_PRIORITY};


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
   // not sure
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
*/


// Test that connection with debugger works
void delay(int count) {
    while (count--) {
        __NOP(); // No Operation
    }
}

int main(void) {
   // Enable GPIOB clock
   RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;

   // Configure PB0 as output
   GPIOB->MODER &= ~GPIO_MODER_MODE0_1;
   GPIOB->MODER |= GPIO_MODER_MODE0_0;

   while (1) {
      GPIOB->ODR ^= GPIO_ODR_OD0;

      // Delay
      delay(2000000);
   }
}


/*

int main (void) {
   UInt32 blinkTimer = 0;
   UInt8 secTimer = 0;
   
   RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
   GPIOB->MODER &= ~GPIO_MODER_MODE0_1;
   GPIOB->MODER |= GPIO_MODER_MODE0_0;
   
   
   initTimer();
   while (1) {
      if (hasTimePassed(blinkTimer, S_1)) {
        secTimer++;
        blinkTimer = CURRENT_TIME;
        GPIOB->ODR ^= GPIO_ODR_OD0;
      }
   }
}

*/

/*
// Main for my scheduler
int main (void) {
   // Set up tasks
   initBlinkLED();
   initWatchDog();
   initSerialComs();
   initGoToSleep();
   
   // Add tasks to be scheduled
   addTask(MED_PRIORITY, &blinkLED);
   addTask(MAX_PRIORITY, &PetWatchDog);
   addTask(MED_PRIORITY, &serialComs);
   addTask(MIN_PRIORITY, &GoToSleep);
   
   while (1) {
      // nothing happens in the main loop, interrupts run the scheduler periodically, and decide the tasks to run
      // If I'm still trying to get hardware set up just delay and call scheduler here, thats easier
   }
}
*/
