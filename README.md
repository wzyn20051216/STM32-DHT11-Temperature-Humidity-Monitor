# STM32温湿度采集监测系统

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![STM32](https://img.shields.io/badge/STM32-F103C8T6-brightgreen.svg)]()
[![Language](https://img.shields.io/badge/language-C-orange.svg)]()

基于STM32F103C8T6的温湿度采集监测系统，支持实时显示、阈值报警、参数设置等功能。

![项目封面](docs/images/cover.jpg)

## 功能特性

- ✅ **实时采集**：DHT11传感器，500ms采集周期
- ✅ **数码管显示**：4位动态扫描，显示温度/湿度/阈值
- ✅ **智能报警**：超出阈值自动蜂鸣器报警
- ✅ **阈值设置**：4个独立阈值（温度/湿度 上限/下限）
- ✅ **按键控制**：单击/双击识别，实现多级菜单
- ✅ **串口通信**：实时回显系统状态
- ✅ **任务调度**：基于时间片的多任务调度架构

## 硬件清单

| 组件 | 规格 | 数量 |
|------|------|------|
| MCU | STM32F103C8T6 | 1 |
| 温湿度传感器 | DHT11 | 1 |
| 数码管 | 4位共阴极 | 1 |
| 按键 | 轻触开关 | 4 |
| 蜂鸣器 | 无源蜂鸣器 | 1 |
| LED | 红/绿LED | 2 |
| 电阻 | 10KΩ, 330Ω | 若干 |

## 引脚定义

```
DHT11:       PB8
数码管段选:  PA0-PA7
数码管位选:  PB0, PB1, PB10, PB11
按键:        PB12-PB15
LED:         PB6, PB7
蜂鸣器:      PA6 (TIM3_CH1 PWM)
串口:        PA9(TX), PA10(RX)
```

## 项目结构

```
温湿度采集/
├── user/
│   └── main.c              # 主程序、任务调度器
├── driver/
│   ├── DHT11.c/h           # DHT11温湿度传感器驱动
│   ├── led.c/h             # LED指示灯驱动
│   ├── key.c/h             # 按键驱动（支持单击/双击）
│   ├── smg.c/h             # 数码管动态扫描驱动
│   ├── beep.c/h            # 蜂鸣器PWM驱动
│   └── usart.c/h           # 串口通信
├── system/
│   ├── delay.c/h           # 微秒级延时
│   ├── timer.c/h           # 定时器配置
│   └── sys.c/h             # 系统初始化
└── project/
    └── LED.uvprojx         # Keil工程文件
```

## 快速开始

### 1. 开发环境

- Keil uVision5
- STM32F1xx标准外设库
- ST-Link驱动

### 2. 编译下载

```bash
# 1. 使用Keil打开工程文件
打开 project/LED.uvprojx

# 2. 编译项目
Project -> Build Target (F7)

# 3. 下载到开发板
Flash -> Download (F8)
```

### 3. 硬件连接

按照引脚定义连接各模块，注意：
- DHT11的DATA引脚需要4.7K上拉电阻
- 数码管共阴极，注意位选和段选接线
- 按键采用上拉输入模式，按下时为低电平

## 使用说明

### 操作指南

| 按键 | 单击操作 | 双击操作 |
|------|---------|---------|
| KEY1 | 温度 ↔ 湿度显示切换 | 温度上限 ↔ 温度下限设置 |
| KEY2 | 温度 ↔ 湿度显示切换 | 湿度上限 ↔ 湿度下限设置 |
| KEY3 | 数值增加 (+1) | - |
| KEY4 | 数值减少 (-1) | - |

### 菜单说明

```
Menu 0: 显示当前温度 (默认)
Menu 1: 显示当前湿度
Menu 2: 设置温度上限 (KEY3/KEY4调节)
Menu 3: 设置温度下限 (KEY3/KEY4调节)
Menu 4: 设置湿度上限 (KEY3/KEY4调节)
Menu 5: 设置湿度下限 (KEY3/KEY4调节)
```

### 默认阈值

```c
温度上限：30°C
温度下限：15°C
湿度上限：70%
湿度下限：30%
```

## 软件架构

### 任务调度器

采用基于时间片的协作式多任务调度：

```c
task_t sch_task[] = {
    {key_proc,   10,  0},   // 按键扫描 - 10ms
    {seg_proc,   20,  0},   // 数码管显示 - 20ms
    {led_proc,   100, 0},   // LED指示 - 100ms
    {dht11_task, 500, 0},   // 温湿度采集 - 500ms
    {alarm_proc, 500, 0},   // 报警检测 - 500ms
    {uart_proc,  500, 0}    // 串口回显 - 500ms
};
```

### 核心模块

#### 1. DHT11驱动
- 单总线协议通信
- 精确时序控制（20ms复位，40us采样）
- 校验和验证

#### 2. 按键双击识别
- 计数器+定时器方式
- 300ms双击时间窗口
- 状态机防抖

#### 3. 数码管动态扫描
- 消隐-送段码-选位三步显示
- 20ms刷新周期
- 支持小数点显示

#### 4. 蜂鸣器PWM报警
- TIM3 CH1输出1kHz PWM
- 50%占空比
- 超阈值自动触发

## 调试经验

### 常见问题

1. **DHT11无响应**
   - 检查DATA引脚焊接
   - 确认复位函数延时时间（必须≥18ms）
   - 验证上拉电阻是否正常

2. **数码管显示不全**
   - 逐段测试段选引脚
   - 检查位选三极管是否导通
   - 确认共阴/共阳类型

3. **按键误触发**
   - 增加硬件滤波电容（0.1uF）
   - 调整软件去抖时间
   - 检查上拉电阻阻值

### 关键BUG修复记录

#### BUG #1: DHT11初始化失败
```c
// 错误代码
delay_us(20);  // 只有0.02ms，不满足18ms要求

// 修复后
delay_ms(20);  // 正确：20ms
```

#### BUG #2: 数据偶尔异常
```c
// 添加校验和验证
if((buf[0]+buf[1]+buf[2]+buf[3]) == buf[4])
{
    *temp = buf[0];
    *humi = buf[2];
}
```

## 扩展功能

可以在此基础上实现：

- [ ] EEPROM存储阈值设置
- [ ] LCD12864图形化显示
- [ ] ESP8266 WiFi远程监控
- [ ] 历史数据记录（SD卡）
- [ ] RTC实时时钟
- [ ] 低功耗休眠模式

## 贡献指南

欢迎提交Issue和Pull Request！

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开Pull Request

## 参考资料

- [STM32F103C8T6数据手册](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [DHT11传感器手册](docs/DHT11.pdf)
- [STM32标准外设库](https://www.st.com/en/embedded-software/stsw-stm32054.html)

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 联系方式

- 作者：刘立烨
- 学校：电信233
- CSDN：[详细技术博客](https://blog.csdn.net/...)

## 致谢

感谢所有为本项目提供帮助的老师和同学！

---

如果这个项目对你有帮助，请给一个⭐️Star支持一下！
