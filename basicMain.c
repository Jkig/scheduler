// Test that connection with debugger works
void delay(int count) {
    while (count--) {
        __NOP(); // No Operation
    }
}

int main(void) {
   initUserLED();

   while (1) {
      GPIOB->ODR ^= GPIO_ODR_OD0;

      // Delay
      delay(2000000);
   }
}

/*
// prove that interrupt is running 

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
      }
   }
}

*/


/*
// Goal Main for my scheduler
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