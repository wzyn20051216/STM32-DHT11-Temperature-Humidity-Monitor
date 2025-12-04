#include "key.h"
#include "delay.h"
#include "led.h"
extern unsigned char menu;
extern unsigned char temp_high;
extern unsigned char temp_low;
extern unsigned char humi_high;
extern unsigned char humi_low;

// 双击逻辑相关的变量
uint8_t k1_click_cnt = 0;   // Key1 点击次数
uint16_t k1_timer = 0;      // Key1 计时器
uint8_t k2_click_cnt = 0;   // Key2 点击次数
uint16_t k2_timer = 0;      // Key2 计时器

// 双击判定时间阈值 (根据你调用key_proc的频率调整)
// 假设每10ms-20ms调用一次，30次大约是300ms-600ms，适合双击间隔
#define DOUBLE_CLICK_TIME  30
unsigned int Key_Val, Key_Down, Key_Up, Key_Old;
void Key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t key_read(void)
{
    uint8_t temp = 0;
    
    // 注意：硬件是“按下为0”，为了配合你的算法习惯（按下为1），这里判断 == 0 就置 1
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0) temp |= 0x01; // K1
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0) temp |= 0x02; // K2
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0) temp |= 0x04; // K3
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0) temp |= 0x08; // K4
    
    return temp;
}

void key_proc(void)
{
    Key_Val  = key_read();                 // 读取当前硬件状态
    Key_Down = Key_Val & (Key_Val ^ Key_Old);  // 检测按下瞬间 (下降沿)
    Key_Up   = ~Key_Val & (Key_Val ^ Key_Old); // 检测抬起瞬间 (上升沿)
    Key_Old  = Key_Val;                    // 更新旧状态

    if (Key_Down) // 如果有任意键被按下
    {
        switch (Key_Down)
        {
            // ---------------- KEY 1 (PB12): 菜单/温度设置 ----------------
            case 0x01: 
                k1_click_cnt++; // 记录点击次数
                k1_timer = 0;   // 重置计时器
                
                if (k1_click_cnt == 2) // 触发【双击】
                {
                    // 双击逻辑：在温度设置界面切换 (Menu 2 <-> 3)
                    if(menu == 2) menu = 3;
                    else if(menu == 3) menu = 2;
                    else menu = 2; // 从其他界面直接进温度高设置
                    
                    k1_click_cnt = 0; // 清除计数，任务完成
                }
                break;

            // ---------------- KEY 2 (PB13): 湿度设置 ----------------
            case 0x02:
                k2_click_cnt++; 
                k2_timer = 0;
                
                if (k2_click_cnt == 2) // 触发【双击】
                {
                    // 双击逻辑：在湿度设置界面切换 (Menu 4 <-> 5)
                    if(menu == 4) menu = 5;
                    else if(menu == 5) menu = 4;
                    else menu = 4; // 从其他界面直接进湿度高设置
                    
                    k2_click_cnt = 0;
                }
                break;

            // ---------------- KEY 3 (PB14): 数值增加 (+) ----------------
            case 0x04:
                // 只有在设置模式下才有效
                if (menu == 2 && temp_high < 99) temp_high++;
                else if (menu == 3 && temp_low < 99) temp_low++;
                else if (menu == 4 && humi_high < 99) humi_high++;
                else if (menu == 5 && humi_low < 99) humi_low++;
                break;

            // ---------------- KEY 4 (PB15): 数值减少 (-) ----------------
            case 0x08:
                // 只有在设置模式下才有效
                if (menu == 2 && temp_high > 0) temp_high--;
                else if (menu == 3 && temp_low > 0) temp_low--;
                else if (menu == 4 && humi_high > 0) humi_high--;
                else if (menu == 5 && humi_low > 0) humi_low--;
                break;
        }
    }

    // ============================================================
    // 3. 超时处理 (处理单击逻辑)
    // ============================================================
    
    // --- 处理 Key1 的单击 (超时未发生第二次点击) ---
    if (k1_click_cnt == 1)
    {
        k1_timer++;
        if (k1_timer > DOUBLE_CLICK_TIME) // 超时了，说明是单击
        {
            // 单击逻辑：切换显示模式 (Menu 0 <-> 1)
				
             menu = 0;
            
            k1_click_cnt = 0; // 状态清零
            k1_timer = 0;
        }
    }

    // --- 处理 Key2 的单击 ---
    if (k2_click_cnt == 1)
    {
        k2_timer++;
        if (k2_timer > DOUBLE_CLICK_TIME) // 超时了
        {
            // 单击逻辑：也切换显示模式 (照抄你原代码逻辑)
           
             menu = 1;

            k2_click_cnt = 0;
            k2_timer = 0;
        }
    }
}