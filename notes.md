#define saveState() __asm volatile (“PUSH {R0-R15};”)
#define restoreState() __asm volatile (“idk {R0-R15};”)// get all of them from an address, and restore the registers to these values, this is a bit harder
Restore to program counter last?


Use pointer math to reserve space in memory for each of my stacks, put the main loop on top of all this, then anything in the interrupt would be in here. Know where the start of these should be
For now, make each sub stack take up a fixed amount of memory.
Under the bottom of the stack for each task, store the registers at the time that I’m moving on, maybe just all 16 registers, as well as the stack pointer at the time of execution, but I think this is automatic


