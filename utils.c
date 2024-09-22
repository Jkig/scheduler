#include "main.h"
#include "utils.h"


// function returns true if a specified amount of time has passed

void initTimer(void){
   UInt16 delay = 10000;
   RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;   // TIM2 is on APB1, 120 MHz max clock, which is where I set it
   while (delay--);
   delay = 10000;
   
   TIM2->PSC = 11999;          // .1 ms timer, this gives me good enough percision
   while (delay--);
   delay = 10000;
   TIM2->ARR = 0xFFFFFFFF;    // maximum range
   while (delay--);
   delay = 10000;
   CURRENT_TIME = 0;             // Clear the counter
   RCC->CFGR |= RCC_CFGR_TIMPRE;
   TIM2->EGR |= TIM_EGR_UG;
   TIM2->SR = 0;
   while (delay--);
   
   TIM2->CR1 |= TIM_CR1_CEN;  // Enable the counter
}



_Bool hasTimePassed (UInt32 start, UInt32 length) {
   // Some constraints, I can only store values up to ~5 days, 
   //    I'm not going to worry about filling this up, it won't run that long
   return ((CURRENT_TIME - start) >= length);
}

void initUserLED(void) {
   RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
   GPIOB->MODER &= ~GPIO_MODER_MODE0_1;
   GPIOB->MODER |= GPIO_MODER_MODE0_0;
}


void initUserLED2(void) {
   GPIOB->MODER &= ~GPIO_MODER_MODE14;
   GPIOB->MODER |= GPIO_MODER_MODE14_0;
}


/*
    // Set PB0 (LD2) as output
    GPIOB->MODER &= ~GPIO_MODER_MODE0;  // Clear MODER bits for PB0
    GPIOB->MODER |= GPIO_MODER_MODE0_0; // Set PB0 as output (01)
*/

