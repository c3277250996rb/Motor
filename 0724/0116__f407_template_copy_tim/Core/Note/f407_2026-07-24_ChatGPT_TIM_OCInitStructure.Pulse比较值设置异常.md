# F407 TIM2 TIM8频率

**User:** Anonymous  
**Created:** 7/23/2026 23:38:04  
**Updated:** 7/24/2026 0:42:19  
**Exported:** 7/24/2026 0:44:40  
**Link:** [https://chatgpt.com/c/6a62355b-5c68-83ee-9668-27e1f24d146a](https://chatgpt.com/c/6a62355b-5c68-83ee-9668-27e1f24d146a)  

## Response:
2026/7/24 00:42:12

如果你的意思是：

```c
TIM_OCInitStructure.Pulse = (TIM_PWMOUTPUT_Handle.Init.Period + 1) / 2;
```

运行后发现 **PWM没有变成5000对应的占空比**，但是：

```c
TIM_OCInitStructure.Pulse = 5000;
```

就正常，那就不是“优化”的问题。

原因在于：**你给 `Pulse` 赋值的时机不对。**

看你的代码：

```c
TIM_PWMOUTPUT_Handle.Init.Period = 10000-1;

TIM_PWMOUTPUT_Handle.Init.Prescaler = 84-1;
```

然后：

```c
TIM_OCInitStructure.Pulse = (TIM_PWMOUTPUT_Handle.Init.Period + 1) / 2;
```

理论结果应该：

```c
(9999 + 1) / 2 = 5000
```

完全没问题。

但是要注意：

`TIM_PWMOUTPUT_Handle.Init.Period` 是 HAL 句柄里的软件变量，不一定等于最终 TIM2 的 ARR。

---

## 建议这样写

不要依赖 Handle：

```c
uint32_t period;

period = 10000 - 1;

TIM_PWMOUTPUT_Handle.Init.Period = period;

TIM_OCInitStructure.Pulse = (period + 1) / 2;
```

完整：

```c
TIM_PWMOUTPUT_Handle.Init.Period = 10000 - 1;
TIM_PWMOUTPUT_Handle.Init.Prescaler = 84 - 1;

TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;
TIM_OCInitStructure.Pulse = (TIM_PWMOUTPUT_Handle.Init.Period + 1) / 2;
```

---

## 如果还是不行，重点检查初始化结构体有没有清零

你这里：

```c
TIM_OC_InitTypeDef TIM_OCInitStructure;
```

是局部变量。

建议：

```c
TIM_OC_InitTypeDef TIM_OCInitStructure = {0};
```

否则里面可能有随机值。

HAL很多结构体没有初始化全部成员时，会出现奇怪问题。

---

## 另外一个可能

如果你开启了：

```c
#define USE_FULL_ASSERT
```

或者调试发现：

```c
TIM_OCInitStructure.Pulse
```

不是5000。

可以直接：

```c
printf("%d\r\n", TIM_OCInitStructure.Pulse);
```

或者 Keil 断点看。

---

不过从你的描述：

> 写死5000正常，表达式赋值不行

我第一怀疑是：

```c
TIM_OC_InitTypeDef TIM_OCInitStructure;
```

没有初始化导致的，而不是优化。

改：

```c
TIM_OC_InitTypeDef TIM_OCInitStructure = {0};
```

通常就好了。你这个 HAL 工程里很容易踩这个坑。



---
Powered by [ChatGPT Exporter](https://www.chatgptexporter.com)