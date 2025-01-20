#include "Key.h"

unsigned int KeyRead(void)
{
    int col ,row;
    unsigned int KeyNum = 0;
    for(col = 0;col < 4;col++)
    {
        P44 = (col != 0);
        P42 = (col != 1);
        P35 = (col != 2);
        //P34 = (col != 3);
        for(row = 0;row<4;row++)
        {
            if( (P3 & (0x08 >> row)) == 0  )
            {
                KeyNum |=(1 << (col * 4 + row));
            }
        }
    }
    //P3 = 0xff;
    return KeyNum;
}