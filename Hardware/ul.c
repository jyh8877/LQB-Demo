#include "ul.h"
sbit Tx = P1 ^ 0;
sbit Rx = P1 ^ 1;
void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}
void SendUlSound(void)
{
    unsigned char i;
    EA = 0;
    for(i = 0;i<8;i++)
    {
        Tx = 1;
        Delay12us();
        Tx = 0;
        Delay12us();
    }
    EA = 1;
}
unsigned char GetUlSound(void)
{
    CMOD = 0x00;
    CH = CL = 0;
    CR = 1;
    SendUlSound();
    while((Rx == 1) && (CF == 0));
    CR = 0;
    if(CF == 0)
    {
        return 0.017 * ( CH<<8 | CL);
    }
    else
    {
        CF = 0;
        return 0;
    }
}