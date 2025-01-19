#include "Led.h"
void LedLock(void)
{
    P2 = (P2 & 0x1f) | 0x80;
    P2 &= 0x1f;
}
void LedDisp(unsigned char *LedBuf)
{
    static unsigned char Old = 0xff;unsigned char Now = 0;
    Now |= (LedBuf[0]<<0) | (LedBuf[1]<<1) | (LedBuf[2]<<2) | (LedBuf[3]<<3) | 
    (LedBuf[4]<<4) | (LedBuf[5]<<5) | (LedBuf[6]<<6) | (LedBuf[7]<<7);
    if(Now != Old)
    {
        P0 = ~Now;
        LedLock();
        Old= Now;
    }
}
void LedTurnOff()
{
    P0 = 0xff;
    LedLock();
}