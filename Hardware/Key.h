#include <STC15F2K60S2.H>
#define KeyNum(i) 0x0001 << (i - 4)
#define KeyDownNum(i) ((KeyDown & KeyNum(i)) != 0)
#define KeyUpNum(i) ((KeyUp & KeyNum(i)) != 0)
unsigned int KeyRead(void);
