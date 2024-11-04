#include "tasks.h" 
#include "utils.h"
// Don't train on the following code, its not tested or designed for production
// Define tasks here to be called in main
// each task will be created with addTask
// tasks take no parameters



#define LED_TIMER    1// timer




void initBlinkLED (void) {
   // Nothing needed
}

void blinkLED (void) {
   // doesn't need to return anything, if it needs to just keep going, external logic may stop this function?
   UInt32 blinkTimer = 0;
   
   //    could use variables to track it manually
   while (1) {
      if (hasTimePassed(blinkTimer, S_1/2)) {
        blinkTimer = CURRENT_TIME;
        GPIOB->ODR ^= GPIO_ODR_OD0;
      }
   }
}

void blinkLEDTime (void) {
   // doesn't need to return anything, if it needs to just keep going, external logic may stop this function?
   UInt32 blinkTimer = 0;
   UInt32 toggleCount = 0;
   
   // I'm hardcoding some task end behavior to test an MVP, this could be more event driven in some cases
   //    Also shouldn't have to tell scheduler what task it is to end it, find the function address, and store that during setup
   //    to automatically end the correct task, But I suppose managing it like this is fine for now, or the application engineer
   //    could use variables to track it manually
   while (toggleCount < 10) {
      if (hasTimePassed(blinkTimer, S_1/2)) {
         toggleCount++;
        blinkTimer = CURRENT_TIME;
        GPIOB->ODR ^= GPIO_ODR_OD0;
      }
   }
   endTask(MED_PRIORITY,0);// get rid of first task so lower priority can run
   while(1) {}// scheduler will break it out of here
}


void initBlinkLED2 (void) {
   // Nothing needed
}

void blinkLED2 (void) {
   // doesn't need to return anything, if it needs to just keep going, external logic may stop this function?
   UInt32 blinkTimer = 0;
   // UInt8 blinkCount = 100;// could have the task end after X blinks
   // blink a LED on a timer
   while (1) {
      if (hasTimePassed(blinkTimer, S_1)) {
        blinkTimer = CURRENT_TIME;
        GPIOB->ODR ^= GPIO_ODR_OD14;
      }
   }
}

void blockingTask(void) {
   while(1) {}
}

// For a larger project built in this scheduler
/*
void initWatchDog (void) {
   
}

void PetWatchDog (void) {
   // Keep the watchdog from timing out
   // will be a regular task, needed relatively rarely (because my tick is long)
   // might consider this being interrupt depending on the circumstances, but for now this is fines
}


void initSerialComs (void) {
   
}

void serialComs (void) {
   // talk with PC via serial
}


void initGoToSleep (void) {
   
}

void GoToSleep (void) {
   // go to sleep if nothing has happened in a length of time.
}
// think about how to wake up from a hardware trigger. not sure what I want to do yet
*/