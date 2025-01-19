#include <STC15F2K60S2.H>
#include <INTRINS.H>
unsigned char AdRead(unsigned char Add);
void DaWrite(unsigned char DaVal);
void EEPROMWrite(unsigned char *str,unsigned char Num,unsigned char Add);
void EEPROMRead(unsigned char* Str,unsigned char Num,unsigned char Add);