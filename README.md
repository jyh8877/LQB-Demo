# 蓝桥杯大模板
## [锁存器](#1)
## [定时器相关](#2)
## [pca捕获计数器](#3)
## [按键三行消除抖动](#4)
## [十六进制数转换十进制数](#5)
## [通讯协议](#6)
### <div id = '1'>锁存器 </div>

74HC138控制选择写入哪个寄存器(Y4,Y5,Y6,Y7)
真值表：
P25 P26 P27
![alt text](image.png)
==先把P2高三位清空==,再或上对应的真值
LED选择
```c
P2 = (P2 & 0x1f) | 0x80;
P2 &= 0x1f;
```
数码管COM口选择
```c
P2 = (P2 & 0x1f) | 0xC0;
P2 &= 0x1f;
```
数码管选择
```c
P2 = (P2 & 0x1f) | 0xE0;
P2 &= 0x1f;
```
蜂鸣器
```c
P2 = (P2 & 0x1f) | 0xA0;
P2 &= 0x1f;
```

### <div id = 2>定时器相关</div>
初始化代码用isp软件生成
定时器分配：定时器1用于轮询处理，定时器2用于串口通讯.
定时器1 选择1ms中断，16位自动重载，12T模式
定时器2 用于串口
#### 定时器以及中断相关寄存器
##### ==EA中断使能寄存器==初始化时候EA = 1；
##### ==ET1 定时器1中断使能==

### <div id = 3>ulSound-pca捕获相关</div>
#### 步骤:
1.发送8个方波40MHz方波(发送前关闭总中断,发送后开启) 
2.配置pca
3.等到收到方波(收到后Rx为0)
```c
while((Rx == 1) && (CF == 0));
```
4. 收到后time * 0.017(340 * time / 2 / 1000)
#### 重要定时器
##### CMOD:配置成0x00;
##### CF:溢出寄存器，溢出写完会变1，记得软件清零
##### CR：计数允许寄存器，写1才可以计数，用完记得写0
##### ==CH,CL==:计数器高八位和低八位，用前记得清零
```c
time = (CH << 8) | CL
```
得到计数器的值

### <div id = 4>按键三行消除抖动</div>
```c
static unsigned char KeyVal,KeyOld,KeyDown,KeyUp;
KeyVal = KeyRead();
KeyDown = KeyVal & (KeyVal ^ KeyOld);
KeyUp = ~KeyVal & (KeyVal ^ KeyOld);
KeyOld = KeyVal;
```
### <div id = 5>进制相关</div>
#### 10转换16进制
```c
ans = num / 10 << 4 |  num % 10;
```
#### 16转换10进制==务必加括号==
```c
ans = (tmp >>4 ) *10 + (tmp & 0x0F);
```
### <div id =6>通讯</div>
#### iic通讯 读模式
1.读数据前必须先在指定位置写一个空字符==写模式后不需要stop==
2.ack==写1不继续读==，写0继续读取
```c
    I2CStart();
    I2CSendByte(0x90);
    I2CWaitAck();
    I2CSendByte(addr);
    I2CWaitAck();

    I2CStart();
    I2CSendByte(0x91);
    I2CWaitAck();
    val=I2CReceiveByte();
    I2CSendAck(1);
    I2CStop();
```
#### 串口
##### 初始化
isp 生成串口1用定时器2作为波特率发生器，1T模式
ES = 1打开串口中断
```c
void Uart1_Init(void)	//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xC7;			//设置定时初始值
	T2H = 0xFE;			//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
    ES = 1;
    EA = 1;
}
```
##### 发送接收相关寄存器
RI:接收中断标志位，必须软件清零
TI:发送中断标志位,发送成功后硬件把TI写1,必须软件等待到TI写1后清零后发送下一字节

发送过程：
重写printf的底层putchar(stdio.h中)
```c
extern char putchar(char ch)
{
    SBUF = ch;
    while(TI == 0);
    TI = 0;
    return ch;
}
```
发送时如果要发送数字需要强转为unsigned int或int等，不能直接发送char作为数字,如
```c
unsigned char chh = 123;
printf("%d",(int)chh);
printf("%u",(unsigned int)chh);
```
接受过程
接收中断:
中断号为4，注意手动清除RI
```c
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
```
处理过程：
如果超时就清空接受数组
```c
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
```