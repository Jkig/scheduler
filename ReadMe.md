# I’m building a basic scheduler, my goals are:
* Scheduler
* Uses priority and round robin for matching priorities
* Interrupt triggered by hardware timer does the work of the os/ scheduling tasks
* If no task ran for x time go into low power mode, only awoken via one method (button press)


## Sample project to demonstrate the scheduler works:
* Runs several tasks, including flashing LEDs at different rates
* Serial communication with PC
* Parsing serial data and blinking LEDs based on it (command to change duration)
* Wrote a timer to be used within tasks
* Trigger going on to other tasks when I finish tasks, so efectively early go back into OS
* The really hard thing is figuring out what state to save
* Show that I can actually save this state, with real computation (simulated with just multiplication of random numbes) as well as simpler just a timer that takes more than 1 tick
* let them swap in tasks at run time?


## How its used:
* Define a task by calling these functions, and setting x,y,z values to what you want based on priority.
* How to write tasks such that they are potentially in a wait state, and when to be out. Most of my tasks will not take one whole tick.


## Constraints:
* I know how to make safe, fast, well tested code, this is not that, this is me experimenting to learn X and Y topics better, don’t base a project on this.
* I'm not letting us delete tasks, or change anything about them at run time, I'm may not let engineer change size of stack for any task, just have a fixed level?



# TODO:
* finish the timer
* test that timer works correctly by blinking a LED 
* get debugger running
* blink led on a timer bare metal
* each of the tasks for the sample project
* building the stacks for each of the tasks
* (assembly code?) in scheduler to move the stack pointer to where I want it specifically
* Code in scheduler to save the last stack pointer, and for each task

## Milestones:
* Blink led on a timer with the scheduler
* Blink 2 leds on a timer with different prority levels, and again with same priority levels
* Get sleep to work based only on button presses - go in if no button presses for 10 sec, go out based on pressing , or maybe check every sec if its pressed, and if not go back to sleep, in this case it can be awoken by holding for 1 sec.
* Full Sample project working
