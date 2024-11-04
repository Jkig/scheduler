#include "main.h"
#include "utils.h"
#include "tasks.h"
#include "scheduleInterrupt.h"


static _Bool firstcall = 0;
static _Bool thisPassFirstCall;
static UInt8 TopTaskToStart;
static UInt8 MidTaskToStart;
static UInt8 LowTaskToStart;

static struct {UInt32* r4r11; UInt32 psp;UInt32 *topOfStack;UInt32 *rawTopOfStack} addresses;
static volatile UInt32 *r4r11 = 0;// gcc crashes at runtime when returning from interrupt if this is in the struct above?
static UInt32 counter;
static UInt16 i=0;
UInt32 *newStackBase = 0;


// Initialize scheduler
void TIM3_Init(void) {
   UInt16 delay = 10000;
   // Enable TIM3 clock
   RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;
   while (delay--);
   delay = 10000;
   TIM3->PSC = 119999;          // Called every ms
   while (delay--);
   delay = 10000;
   TIM3->ARR = 1;               // only need 1 tick
   while (delay--);
   delay = 10000;
   TIM3->DIER |= TIM_DIER_UIE;  // Interrupt
   while (delay--);
   delay = 10000;

   NVIC_EnableIRQ(TIM3_IRQn);
   while (delay--);
   delay = 10000;
   TIM3->CR1 |= TIM_CR1_CEN;
}



void TIM3_IRQHandler(void) {// TODO: prep for recovery almost htere
   //static UInt8 TopTaskToStart;
   // static UInt8 MidTaskToStart;
   
   TIM3->SR &= ~TIM_SR_UIF;// Clear update interrupt flag
   __disable_irq();
   
   
   // What task were we in, swap the pointers there to here
   // save the last task where we want it
   if (!firstcall) {
      // Don't save the info if not coming from a call, I could seperately have a method to go back to this state if tasks came in during runtime
      firstcall = 1;
   }
   else {
      addresses.r4r11 = &Tasks[lastTaskPriority*16 + lastTaskNum].r4throu11[0];
      addresses.psp = Tasks[lastTaskPriority*16 + lastTaskNum].psp;
      addresses.topOfStack = &Tasks[lastTaskPriority*16 + lastTaskNum].topOfStack;

      __DSB();
      // Save R4-R11 into the task's context structure
      __asm volatile (
         "STMIA  %0, {R4-R11}      \n"
         :
         : "r" (addresses.r4r11)
         : "memory"
      );
      
      __DSB();
      
      __asm volatile (
         "str sp, [%0]\n"
         : 
         : "r" (addresses.topOfStack)
         : "memory"
      );
      __DSB();
   }
   if (topPriority > 0x0000) {// some top priority needs to run
      // run any of them, start on top task to start and keep trying until find the function to call
      for (i=0;i<16;i++) {
         TopTaskToStart &=0xF;         // wrap to 0 if 16
         if (((topPriority >> TopTaskToStart) & 1) == 1) {
            lastTaskPriority = MAX_PRIORITY;
            lastTaskNum = TopTaskToStart;
            TopTaskToStart++;
            break;// shouldn't actually hit unless task is completed in 1 pass
         }
         TopTaskToStart++;
      }
   }
   else if (midPriority > 0x0000) { // only share / execute anything at this priority if no top priority tasks running
      for (i=0;i<16;i++) {
         MidTaskToStart &=0xF;
         if (((midPriority >> MidTaskToStart) & 1) == 1) {
            lastTaskPriority = MED_PRIORITY;
            lastTaskNum = MidTaskToStart;
            MidTaskToStart++;
            break;
         }
         MidTaskToStart++;
      }
   }
   else if (lowPriority > 0x0000) {
      for (i=0;i<16;i++) {
         LowTaskToStart &=0xF;
         if (((lowPriority >> LowTaskToStart) & 1) == 1) {
            // This task is available to start
            lastTaskPriority = MIN_PRIORITY;
            lastTaskNum = LowTaskToStart;
            LowTaskToStart++;
            break; // TODO: make sure this is right again
         }
         LowTaskToStart++;
      }
   }
   
   
   __DSB();
   
   // restore to this position
   // if this task hasn't begun yet, then I need to adjust the stack manually.
   thisPassFirstCall = 0;
   if (lastTaskPriority == MAX_PRIORITY){
      if ((topFirstCall & (1 << lastTaskNum)) == 0) {
         topFirstCall |= (1 << lastTaskNum);
         thisPassFirstCall = 1;
      }
   }else if (lastTaskPriority == MED_PRIORITY){
      if ((midFirstCall & (1 << lastTaskNum)) == 0) {
         midFirstCall |= (1 << lastTaskNum);
         thisPassFirstCall = 1;
      }
   }
   else {
      if ((lowFirstCall & (1 << lastTaskNum)) == 0){
         lowFirstCall |= (1 << lastTaskNum);
         thisPassFirstCall = 1;
      }
   }
   
   
   if (thisPassFirstCall){
      // Hasn't been called yet, starts up the task
      addresses.rawTopOfStack = baseAddress - (PRIORITY_OFFSET*lastTaskPriority + (lastTaskNum)*TASK_OFFSET) - 8;// figure out how to do this based on correct count
      
      __enable_irq();
      __asm volatile (
         "mov sp, %0\n"
         "mov lr, #0xFFFFFFE9\n"
         "bx lr\n"
         :
         : "r" (addresses.rawTopOfStack)
         : "memory"
      );
   }
   else
   {
      addresses.r4r11 = &Tasks[lastTaskPriority*16 + lastTaskNum].r4throu11[0];
      addresses.psp = Tasks[lastTaskPriority*16 + lastTaskNum].psp;
      addresses.topOfStack = &Tasks[lastTaskPriority*16 + lastTaskNum].topOfStack;
      *addresses.topOfStack += 8;
      // Restore context of the next task
      
      __DSB();
      __asm volatile (
        "LDMIA  %0!, {R4-R11}      \n" // load R4-R11 from stack
        :
        : "r" (addresses.r4r11)// location to move to
        : "memory"
      );
      
      __DSB();
      __enable_irq();
      __asm volatile (
         "ldr sp, [%0]\n"
         "mov lr, #0xFFFFFFE9\n"
         "bx lr\n"
         :
         : "r" (addresses.topOfStack)
         : "memory"
      );
   }
}

