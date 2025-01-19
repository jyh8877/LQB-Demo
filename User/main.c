#include "main.h"
pdata unsigned char LedBuf[8]={1,1,1,1,0,0,0,0},SegBuf[8] = {16,16,16,16,16,16,16,16};
unsigned char Time[3]={11,22,33},RecTime[3],UartBufIndex;
pdata unsigned char UartBuf[10];
idata unsigned char UlDis,ADVal,DaVal,TaskNum,SysTick;
unsigned long int uwTime;
float Temper;
unsigned int TemperShow,Frep;
unsigned int Time1s;
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
void DataProc(void)
{
    if(Time1s % 150)return;
    //ReadTime(RecTime);
    Temper = ReadTemp() ;
	TemperShow = Temper * 10;
	
}
void KeyProc(void)
{

    static unsigned int KeyOld,KeyVal,KeyDown,KeyUp;
    if(Time1s % 20) return;
    KeyVal = KeyRead();
    KeyUp = ~KeyVal & (KeyOld ^ KeyVal);
    KeyDown = KeyVal & (KeyOld ^ KeyVal);
	KeyOld = KeyVal;
  
}
void SegProc(void)
{
    if(Time1s % 20) return;
    // SegBuf[7] = RecTime[2] % 10;
    // SegBuf[6] = RecTime[2] / 10;
    // SegBuf[5] = 17;
    // SegBuf[4] = RecTime[1] % 10;
    // SegBuf[3] = RecTime[1] / 10;
    // SegBuf[2] = 17;
    
    SegBuf[7] = Frep         % 10;
    SegBuf[6] = (Frep   / 10 % 10) ;
    SegBuf[5] = Frep   / 100 %10;
    SegBuf[4] = Frep  / 1000 %10;
    SegBuf[3] = Frep / 10000;
} 
void ADDA(void)
{
    ADVal = AdRead(0x41);
    DaWrite(DaVal);
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
    if(UartBufIndex <= 0)
        return;
    if(SysTick > 10)
    {
        SysTick = 0;
        memset(UartBuf,0,UartBufIndex);
        UartBufIndex = 0;
    }
}

typedef struct
{
    void (*TaskFunc)(void);
    unsigned long int Ratems;
    unsigned long int LastRun;
    }TaskType;
TaskType TaskList[] ={
    {GetTime,100,0},
    {ADDA,160,0},
    {SegProc,100,0},
    {UartProc,10,0},
    {LedProc,1,0}
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
        if(UartBufIndex > 10)UartBufIndex = 0;
            UartBufIndex = 0;
    }
}