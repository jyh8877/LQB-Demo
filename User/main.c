#include "main.h"
pdata unsigned char LedBuf[8]={1,1,1,1,0,0,0,0},SegBuf[8] = {0,0,0,0,0,0,0,0};
unsigned char Time[3]={11,22,33},RecTime[3],UartBufIndex;
pdata unsigned char UartBuf[10];
idata unsigned char UlDis,TaskNum,SysTick,DaVal,Mode;
idata unsigned int ADValx10;
unsigned long int uwTime;
unsigned int Temperx10;
unsigned int Frep;
unsigned int Time1s;
void ModeInit(void)
{
    unsigned char i;
    for(i = 0;i < 8;i++)
        SegBuf[i] = 16;
}
void Timer0_Init(void)		//100微秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
    TMOD |= 0x05;
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}
void Timer1_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x18;				//设置定时初始值
	TH1 = 0xFC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
    ET1 = 1;
    EA = 1;
}
void KeyProc(void)
{

    static unsigned int KeyOld,KeyVal,KeyDown,KeyUp;
    KeyVal = KeyRead();
    KeyUp = ~KeyVal & (KeyOld ^ KeyVal);
    KeyDown = KeyVal & (KeyOld ^ KeyVal);
	KeyOld = KeyVal;
    if(KeyDownNum(4))
    {
		ModeInit();
        Mode = (++Mode) % 4;
    }
        
}
void SegProc(void)
{
    // SegBuf[7] = UartBuf[0] % 10;
    // SegBuf[6] = UartBuf[0] / 10;


    if(Mode == 0)
    {
        SegBuf[7] = Frep         % 10;
        SegBuf[6] = (Frep   / 10 % 10) ;
        SegBuf[5] = Frep   / 100 %10;
        SegBuf[4] = Frep  / 1000 %10;
        SegBuf[3] = Frep / 10000;
    }
    if(Mode == 1)
    {
        SegBuf[7] = RecTime[2] % 10;
        SegBuf[6] = RecTime[2] / 10;
        SegBuf[5] = 17;
        SegBuf[4] = RecTime[1] % 10;
        SegBuf[3] = RecTime[1] / 10;
        SegBuf[2] = 17;
        SegBuf[1] = RecTime[0] % 10;
        SegBuf[0] = RecTime[0] / 10;
    }
    if(Mode == 2)
    {
        SegBuf[7] = ADValx10 % 10;
        SegBuf[6] = ADValx10 / 10 + '.';
    }
    if(Mode == 3)
    {
        SegBuf[7] = Temperx10 % 10;
        SegBuf[6] = Temperx10 / 10 % 10 + '.';
        if(Temperx10 /100 % 10)SegBuf[5] = Temperx10 /100 % 10;else{SegBuf[5] = 16;}
    }
} 
void AD(void)
{
    ADValx10 = ((float)AdRead(0x43) / 255.0) * 50;
}
void DA(void)
{
    DaWrite(DaVal * 51);
}
void GetTime(void)
{
    ReadTime(RecTime);
}
void ReadUlDis(void)
{
    UlDis = GetUlSound();
}
void LedProc(void)
{
    LedDisp(LedBuf);
}
void UartProc(void)
{    
    if(UartBufIndex == 0)
        return;
    if(SysTick >= 10)
    {
        SysTick = 0;
        memset(UartBuf,0,UartBufIndex);
        UartBufIndex = 0;
    }
}
void Tp(void)
{
    Temperx10 = ReadTemp() * 10;
}
typedef struct
{
    void (*TaskFunc)(void);
    unsigned long int Ratems;
    unsigned long int LastRun;
    }TaskType;
TaskType TaskList[] ={
    {LedProc,10,0},
    {GetTime,300,0},
    {KeyProc,20,0},
    {AD,160,0},
    //{DA,160,0},
    {UartProc,10,0},
    {Tp,200,0},
    {SegProc,200,0}
};
void TaskListInit()
{
    TaskNum = sizeof(TaskList) / sizeof(TaskType);
}
void TaskRun(void)
{
    unsigned char i;
    for(i = 0;i < TaskNum;i++)
    {
        unsigned long int NowTime = uwTime;
        if(NowTime > (TaskList[i].LastRun + TaskList[i].Ratems))
        {
            TaskList[i].LastRun = NowTime;
            TaskList[i].TaskFunc();
        }
    }
}

void main()
{
    TaskListInit();
    SetTime(Time);
	Timer0_Init();
    Timer1_Init();
    Uart1_Init();
    ModeInit();
    while(1)
    {
        TaskRun();
    }
}

void Routine1(void) interrupt 3 
{
    static unsigned char SegPos = 0;
    Time1s++;uwTime++;SysTick++;
    if(Time1s == 1000)
    {
        Frep = ((TH0 << 8) | TL0);
        TH0 = TL0 = 0;
        Time1s = 0;
    }
    
    SegPos = (++SegPos) % 8;
    if(SegBuf[SegPos] > 20)
        SegDisp(SegPos,SegBuf[SegPos]-'.',1);
    else
        SegDisp(SegPos,SegBuf[SegPos],0);
}

void UartRoutine(void) interrupt 4
{
    if(RI)
    {
        SysTick = 0;
        UartBuf[UartBufIndex++] = SBUF;
        RI = 0;
        if(UartBufIndex > 10)
            UartBufIndex = 0;
    }
}