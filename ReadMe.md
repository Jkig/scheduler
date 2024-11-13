# I’m building a basic scheduler:
* Uses 3 priority levels and round robin when multiple tasks (up to 16) at the same level
* Interrupt triggered by hardware timer does the work of the os/ scheduling tasks


## Memory layout:
![Memory](/MemoryDrawing.svg)


# Simple demonstrations:
1. Task switching for same level.
2. A high priority task stopping a task of lower priority until complete
3. blocking task doesn't actually block others


## How its used:
* Define a task by calling addTask and passing in priority and a function's address


# How this project could be expanded:
* Variable stack size
* Go to scheduler if you hit the end of a task before the end
* Make a timer that is interupt driven so you can wait for something and yeild processor
* More complex or choose your own priority system
* Hoisting priority so tasks don't get starved if higher priority tasks don't end after a period of time
* More automatic end task, so it can be called with no parameters and end automatically, or just have the task ending by getting to the end of the function that describes it. Ex: LR is to endtask that can detect which task, and yield to scheduler.

