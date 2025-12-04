#include "stm32f10x.h"                  // Device header
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "smg.h"
#include "beep.h"
#include "DHT11.h"
#include "timer.h"

// ========== 宏定义 ==========
// 默认阈值
#define DEFAULT_TEMP_HIGH    30
#define DEFAULT_TEMP_LOW     15
#define DEFAULT_HUMI_HIGH    70
#define DEFAULT_HUMI_LOW     30

// ========== 全局变量 ==========
// 系统参数
unsigned char menu = 0;                // 当前菜单状态（0=温度显示 1=湿度显示 2=温度上限 3=温度下限 4=湿度上限 5=湿度下限）
unsigned char temp = 0;                // 当前温度值
unsigned char humi = 0;                // 当前湿度值
unsigned char temp_high = DEFAULT_TEMP_HIGH;  // 温度上限阈值
unsigned char temp_low = DEFAULT_TEMP_LOW;    // 温度下限阈值
unsigned char humi_high = DEFAULT_HUMI_HIGH;  // 湿度上限阈值
unsigned char humi_low = DEFAULT_HUMI_LOW;    // 湿度下限阈值

// 系统时基
extern unsigned long int uwtick;



// ========== 任务调度器 ==========
// 任务结构体
typedef struct
{
    void (*func_task)(void);     // 任务函数指针
    unsigned long int rate_ms;   // 任务执行周期(ms)
    unsigned long int last_run;  // 上次运行时间
} task_t;

// 任务函数声明
void dht11_task(void);

// 任务列表
task_t sch_task[] = {
    {key_proc,   10,  0},   // 按键扫描 - 10ms
    {seg_proc,   20,  0},   // 数码管显示 - 20ms
    {led_proc,   100, 0},   // LED指示 - 100ms
    {dht11_task, 500, 0},   // 温湿度采集 - 500ms
    {alarm_proc, 500, 0},   // 报警检测 - 500ms（跟随采集）
    {uart_proc,  500, 0}    // 串口回显 - 500ms（跟随采集）
};

unsigned char task_num;

// 调度器初始化
void sch_init(void)
{
    task_num = sizeof(sch_task) / sizeof(task_t);
}

// 调度器运行
void sch_run(void)
{
    unsigned char i;
    for(i = 0; i < task_num; i++)
    {
        unsigned long int now_time = uwtick;
        if(now_time >= sch_task[i].rate_ms + sch_task[i].last_run)
        {
            sch_task[i].last_run = now_time;
            sch_task[i].func_task();
        }
    }
}

void dht11_task(void)
{
	DHT11_Read_Data(&temp,&humi);
}

// ========== 主函数 ==========
int main(void)
{
    // 硬件初始化
    led_init();
    USART1_Init();
    Key_init();
    SMG_Init();
    DHT11_Init();
    TIM3_Init();
    Init_tiemr4();

	  printf("         ==== 温湿度采集 =====\r\n");
	  printf("             == YYDS ==\r\n");
    sch_init();
    
    while(1)
    {
       sch_run();
    }
}
