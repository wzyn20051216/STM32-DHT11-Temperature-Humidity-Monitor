#ifndef DHT11_H
#define DHT11_H

#include "delay.h"
#include "sys.h"
//PA11
#define DHT11_IO_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
#define DHT11_IO_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;} 
//IO操作函数   
#define	DHT11_DQ_OUT PBout(8)
#define	DHT11_DQ_IN  PBin(8) 

u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取数据
u8 DHT11_Read_Byte(void);//读取一个字节
u8 DHT11_Read_Bit(void);//读取一位
u8 DHT11_Check(void);//检测DHT11
void DHT11_Rst(void);//复位DHT11   

#endif
