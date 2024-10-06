# I’m building a basic scheduler:
* Uses 3 priority levels and round robin when multiple tasks (up to 16) at the same level
* Interrupt triggered by hardware timer does the work of the os/ scheduling tasks


## Memory layout:
![Memory](/MemoryDrawing.svg)


# Simple demonstrations:
1. task switching, and show with debugger its one then the other
2. A high priority task stopping a task of lower priority until complete
3. build a higer priority task to stop a lower priority task to block the lower, then complete
4. Show how saving state works with real computation (perhaps just increase count but wait with blocking timer and see that it continues) (maybe fibbinacci)


## How its used:
* Define a task by calling addTask and passing in priority and a function pointer


# How this project could be expanded:
* Variable stack size
* Go to scheduler if you hit the end of a task before the end
* More complex or choose your own priority system

