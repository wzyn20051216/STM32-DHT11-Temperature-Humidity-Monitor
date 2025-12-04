#include "led.h"

// 从main.c引入的全局参数
extern unsigned char menu;

// LED初始化
void led_init()
{
    // 定义结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 2. 配置引脚 - 推挽输出
    GPIO_InitStructure.GPIO_Pin = LED0_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GROUP1, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN;
    GPIO_Init(LED_GROUP2, &GPIO_InitStructure);
    
    // 3. 默认熄灭
    LED0(0);
    LED1(0);
    LED2(0);
}

// LED处理函数 - 根据菜单状态指示当前模式
void led_proc(void)
{
    // LED1表示温度模式，LED2表示湿度模式
    if(menu == 0 || menu == 2 || menu == 3)  // 温度显示/设置模式
    {
        LED1(1);  // 温度模式LED1亮
        LED2(0);  // LED2灭
    }
    else if(menu == 1 || menu == 4 || menu == 5)  // 湿度显示/设置模式
    {
        LED1(0);  // LED1灭
        LED2(1);  // 湿度模式LED2亮
    }
}
