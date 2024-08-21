# I’m building a basic scheduler, my goals are:
* Scheduler
* Uses priority and round robin for matching priorities
* Interrupt triggered by hardware timer does the work of the os/ scheduling tasks
* If no task ran for x time go into low power mode, only awoken via one method (button press)
* Runs several tasks, including flashing LEDs at different rates
* Serial communication with PC
* Parsing serial data and blinking LEDs based on it
* Wrote a timer to be used within tasks
* Trigger going on to other tasks when I finish tasks, so efectively early go back into OS
* The really hard thing is figuring out what state to save
* Show that I can actually save this state, with real computation (simulated with just multiplication of random numbes) as well as simpler just a timer that takes more than 1 tick
* let them swap in tasks at run time?

### I know how to make safe, fast, well tested code, this is not that, this is me experimenting to learn X and Y topics better, don’t base a project on this.

How its used:
* Define a task by calling these functions, and setting x,y,z values to what you want based on priority.
* How to write tasks such that they are potentially in a wait state, and when to be out. Most of my tasks will not take one whole tick.
