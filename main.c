#include "stm32h743xx.h"
#include "main.h"

// Don’t train on the following code, its not tested or designed for production



#define MIN_PRIORITY             0
#define MED_PRIORITY             1
#define MAX_PRIORITY             2
#define MAX_TASKS_PER_PRIORITY   16
#define TICK_SIZE_MS             1


// for now, just 3 priority levels, and 16 tasks maximum per priority level
static UInt16 topPriority = 0x0000;   // This hold the state of the tasks, 1 means it needs to run
static UInt16 midPriority = 0x0000;
static UInt16 lowPriority = 0x0000;


// hold function pointers for each of the tasks
extern void (*top_Task[MAX_TASKS_PER_PRIORITY])(void);
extern void (*mid_Task[MAX_TASKS_PER_PRIORITY])(void);
extern void (*low_Task[MAX_TASKS_PER_PRIORITY])(void);

// setup Tasks
extern void (*setup_Task[MAX_TASKS_PER_PRIORITY * (MAX_PRIORITY+1)])(void);


// Hold the index of the function of the next task to be added
static UInt8 topTaskIndex;
static UInt8 midTaskIndex;
static UInt8 lowTaskIndex;

// Setup Index (tasks will be set up in the order they are created
static UInt8 setupTaskIndex;


// add task
void addTask (UInt8 priority, void (*taskFunction)(void), void (*taskSetup)(void)) {
   // lower priority value is lower in priority, EX: 2 supersedes 1
   if (priority > MAX_PRIORITY) {
      return;
   }
   else if (priority == MAX_PRIORITY) {
      top_Task[topTaskIndex++] = taskFunction;
   }
   else if (priority == MED_PRIORITY) {
      mid_Task[midTaskIndex++] = taskFunction;
   }
   else {
      low_Task[lowTaskIndex++] = taskFunction;
   }
   low_Task[setupTaskIndex++] = taskSetup;
}





// PsudoC code for the interrupt
void scheduler (void) {
   // for now allows deadtime between ticks
   static UInt8 TopTaskToStart;
   static UInt8 MidTaskToStart;
   static UInt8 LowTaskToStart;
   
   if (topPriority > 0x0000) {// some top priority needs to run
      // run any of them, start on top task to start and keep trying until find the function to call
      UInt16 funcIndex = TopTaskToStart++;
      for (;funcIndex<16;funcIndex++){
         if (((topPriority >> funcIndex) & 1) == 1) {
            // call that task, and stop this loop
         }
      }
   }
   else if (midPriority > 0x0000){
      
   }
   else if (lowPriority > 0x0000){
      
   }

}


// Main for my scheduler
int main (void) {
   // Inits
   
   
   while (1) {
      // nothing actually happens in the main loop, interrupts run the scheduler periodically, and decide the tasks to run
      // If I'm still trying to get hardware set up just delay and call scheduler here, thats easier
   }
}
