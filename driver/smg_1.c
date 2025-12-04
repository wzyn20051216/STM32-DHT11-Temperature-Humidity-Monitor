#include "smg.h"
#include "timer.h"
#include "DHT11.h"

// 数码管段码表 (0-9, 共阴极)
const unsigned char smg_code[] = {
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D,  // 6
    0x07,  // 7
    0x7F,  // 8
    0x6F,  // 9
    0x00   // 熄灭(10)
};

// 从main.c引入的全局参数
extern unsigned char temp;
extern unsigned char humi;
extern unsigned char menu;
extern unsigned char temp_high;
extern unsigned char temp_low;
extern unsigned char humi_high;
extern unsigned char humi_low;

const uint16_t DIG_PINS[] = {GPIO_Pin_11, GPIO_Pin_10, GPIO_Pin_1, GPIO_Pin_0};

// 1. 数码管初始化
void SMG_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 开启 GPIOA 和 GPIOB 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // --- 配置段选 PA0-PA7 (推挽输出) ---
    GPIO_InitStructure.GPIO_Pin = 0x00FF; // Pin_0 到 Pin_7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // --- 配置位选 PB0, PB1, PB10, PB11 (推挽输出) ---
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 初始状态：全部熄灭
    GPIOA->ODR &= 0xFF00; // 段选清零
    GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11);
}

// 2. 显示单个数字函数（修正小数点生效+段码表错误）
// pos: 位置 0-3
// num: 数字 0-9
// dp_en: 1=显示小数点，0=不显示
void SMG_Display_Single(uint8_t pos, uint8_t num, uint8_t dp_en)
{
    
    if(pos > 3 || num > 9) return;

    // A. 消隐 (先把4个位选全关了，防止残影)
    GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11);
    
    // B. 送段码（叠加小数点）
    uint8_t seg_code = smg_code[num];
    if(dp_en == 1) {
        seg_code |= 0x80; 
    }
    GPIOA->ODR = (GPIOA->ODR & 0xFF00) | seg_code; 

    // C. 选中当前位 (PB口输出高电平，三极管导通)
    GPIO_SetBits(GPIOB, DIG_PINS[pos]);
    

}


// 数码管数据处理（根据菜单显示不同内容）
void seg_proc(void)
{
    unsigned char display_value = 0; // 初始化，防止乱码

    // 1. 根据菜单状态选择要显示的数值
    switch(menu)
    {
        case 0: display_value = temp;      break; // 显示当前温度
        case 1: display_value = humi;      break; // 显示当前湿度
        case 2: display_value = temp_high; break; // 显示温度上限
        case 3: display_value = temp_low;  break; // 显示温度下限
        case 4: display_value = humi_high; break; // 显示湿度上限
        case 5: display_value = humi_low;  break; // 显示湿度下限
        default: display_value = 0;        break; // 异常情况归零
    }

    
    
    // 显示十位 
    smg_buf[0] = display_value / 10; 
    
    // 显示个位
    smg_buf[1] = display_value % 10;      

   
    smg_buf[2] = 10; 
    smg_buf[3] = 10; // 熄灭
}