#include "main.h"
#include "utils.h"

#define TEN_THOUSANTH_SEC  1
#define MS_1               10
#define MS_2               2*MS_1
#define MS_10              10*MS_1
#define S_1                1000*MS_1


// function returns true if a specified amount of time has passed

void initTimer(void){
   RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
   
   // TIM2 is on APB1, 120 MHz max clock, which is where I set it
               
   TIM2->PSC = 1199;          // .1 ms timer, this gives me good enough percision
   TIM2->ARR = 0xFFFFFFFF;    // maximum range
   TIM2->CNT = 0;             // Clear the counter

   
   TIM2->CR1 |= TIM_CR1_CEN;  // Enable the counter
}



_Bool hasTimePassed (UInt32 start, UInt32 length) {
   // Some constraints, I can only store values up to ~5 days, 
   //    I'm not going to worry about filling this up, it won't run that long
   return (start+length) <= TIM2->CNT;
}
