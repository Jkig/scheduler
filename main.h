
typedef char            Int8;
typedef unsigned char   UInt8;
typedef short           Int16;
typedef unsigned short  UInt16;
typedef long            Int32;
typedef unsigned long   UInt32;


void addTask (UInt8 priority, void (*taskFunction)(void), void (*taskSetup)(void));
void scheduler (void);