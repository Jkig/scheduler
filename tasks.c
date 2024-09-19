#include "tasks.h"
// Don't train on the following code, its not tested or designed for production
// Define tasks here to be called in main
// each task will be created with addTask
// tasks take no parameters



#define LED_TIMER    1// timer




void initBlinkLED (void) {
   
}

void blinkLED (void) {
   // blink a LED on a timer
}


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
