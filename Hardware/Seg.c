#include "Seg.h"

code unsigned char SegTable[] =
{
0xc0, //0
0xf9, //1
0xa4, //2
0xb0, //3
0x99, //4
0x92, //5
0x82, //6
0xf8, //7
0x80, //8
0x90, //9
0x88, //A
0x83, //b
0xc6, //C
0xa1, //d
0x86, //E
0x8e, //F
0xff, // 16 none
0xbf //17 -
};

void ComLock()
{
    P2 = (P2 & 0x1f) | 0xC0;
    P2 &= 0x1f;    
}
void SegLock()
{
    P2 = (P2 & 0x1f) | 0xE0;
    P2 &= 0x1f;
}
void SegDisp(unsigned char Add,unsigned char Str,unsigned char Point)
{
    P0 = 0xff;
    SegLock();
    P0 = 0x01<<Add;
    ComLock();
    P0 = SegTable[Str];
    if(Point)
        P0 &= 0x7f;
    SegLock();
}