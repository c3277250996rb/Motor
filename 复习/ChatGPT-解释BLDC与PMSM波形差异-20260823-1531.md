# 解释BLDC与PMSM波形差异

**User:** Anonymous  
**Created:** 8/23/2026 15:30:46  
**Updated:** 8/23/2026 15:30:47  
**Exported:** 8/23/2026 15:31:54  
**Link:** [https://chatgpt.com/share/6a8aa1a6-3d24-83ee-af49-838f715b8486](https://chatgpt.com/share/6a8aa1a6-3d24-83ee-af49-838f715b8486)  

## Prompt:
2026/8/23 15:05:58

为什么bldc的反电动势和电流是方波而pmsm的是正弦波

## Response:
2026/8/23 15:06:00

核心原因其实就一句话：

> **BLDC 和 PMSM 的反电动势、电流波形不同，根本上是因为它们的绕组分布和磁场设计不同，从而导致气隙磁密与绕组产生的反电动势谐波不同。**

不过这里有个很重要的点：**BLDC 和 PMSM 在物理结构上其实没有绝对界限，它们本质上都是永磁同步电机。**只是通常：

- **BLDC** → 设计成反电动势接近**梯形波/方波**
- **PMSM** → 设计成反电动势接近**正弦波**

---

# 1. 先看反电动势是怎么来的

一相绕组的反电动势可以理解为：

$$
e = \frac{d\lambda}{dt}
$$

其中磁链：

$$
\lambda = N\Phi
$$

而转子永磁体转动时，定子绕组看到的磁场是在变化的。

所以：

> **绕组看到的磁通变化是什么形状，最终产生的反电动势就会接近什么形状。**

---

# 2. PMSM：为什么是正弦反电动势？

PMSM 的目标通常是让气隙磁场尽可能接近正弦分布。

比如转子磁场：

$$
B(\theta)=B_m\sin\theta
$$

同时，定子绕组也采用比较典型的**分布式绕组**。

可以想象成一个相绕组不是集中在一个槽里，而是分布在多个槽中：

```text
槽：

A相绕组分布：

  +A    +A    +A
---|-----|-----|----------------

            -A    -A    -A
------------|-----|-----|--------
```

这样做的一个非常重要的作用是：

> **把很多空间谐波平均掉。**

最终得到比较接近：

$$
\lambda_A=\lambda_m\sin\theta
$$

因此：

$$
e_A=\frac{d\lambda_A}{dt}
$$

得到：

$$
e_A=E_m\cos\theta
$$

也就是：

```text
反电动势

       /¯¯\      /¯¯\
      /    \    /    \
_____/      \__/      \____
```

接近正弦波。

所以 PMSM 通常采用：

```text
正弦反电动势
        ↓
正弦电流
        ↓
产生比较平滑的电磁转矩
```

---

# 3. BLDC：为什么是梯形波？

BLDC 常采用**集中绕组或者比较少的槽极配合**。

例如某一相绕组可能集中在几个齿上：

```text
      A+
       │
───────┼───────

             A-
              │
──────────────┼──────
```

这样绕组在空间上不像 PMSM 那样进行了大量平均。

再加上永磁体磁场通常也会设计成比较“平顶”的分布。

于是某相绕组转过磁极时，磁链可能接近：

```text
磁链 λ

      ┌─────────┐
     /           \
____/             \____
```

那么：

$$
e=\frac{d\lambda}{dt}
$$

磁链变化率在一段区域内比较稳定，于是反电动势就接近：

```text
      ______
     /      \
____/        \____
    \        /
     \______/
```

也就是常说的：

> **梯形波反电动势（Trapezoidal Back-EMF）**

严格来说，BLDC 的反电动势通常是**梯形波**，不是完美的方波。

---

# 4. 为什么 BLDC 的电流又是方波？

这里就和控制方式有关了。

BLDC 最经典的是 **六步换相（Six-step Commutation）**。

例如：

```text
60°      60°      60°
 |--------|--------|
 A+ B-    A+ C-    B+ C-
```

每一个 60° 电角度，只让两相通电：

```text
A相： +I ──────── 0 ──────── -I

B相： -I ──────── +I ──────── 0

C相： 0 ──────── -I ──────── +I
```

所以电流看起来是：

```text
        ┌───────┐
        │       │
────────┘       └────────
        └───────┘
```

接近方波。

因此 BLDC 的经典搭配是：

$$
\boxed{\text{梯形反电动势} + \text{方波电流}}
$$

---

# 5. 为什么要这样搭配？

因为电磁转矩和反电动势、电流有关。

可以粗略理解成：

$$
T_e \propto e_ai_a+e_bi_b+e_ci_c
$$

如果 BLDC 是：

```text
反电动势：梯形
电流：    方波
```

那么在有效导通区域：

```text
e × i ≈ 常数
```

例如：

```text
反电动势

      ┌───────┐
──────┤       ├──────
      └───────┘

电流

      ┌───────┐
──────┤       ├──────
      └───────┘
```

两者相乘：

$$
T\approx 常数
$$

所以虽然波形看起来“方方的”，但是只要换相配合得好，平均转矩仍然可以比较稳定。

---

# 6. PMSM 为什么必须配正弦电流？

PMSM 的反电动势是：

$$
e_a=E_m\sin\theta
$$

如果电流也是：

$$
i_a=I_m\sin\theta
$$

三相合起来：

$$
e_ai_a+e_bi_b+e_ci_c
$$

会得到一个基本恒定的结果。

所以：

$$
\boxed{\text{正弦反电动势}+\text{正弦电流}}
$$

可以获得非常平滑的转矩。

这就是为什么 PMSM 特别适合：

- 伺服电机
- 电动车驱动
- 高性能电机控制
- FOC
- 对低噪声、低转矩脉动要求高的场合

---

# 7. 你可以这样理解两者

## BLDC

```text
转子磁场
    ↓
绕组分布 / 磁场设计
    ↓
梯形反电动势
    ↓
六步换相
    ↓
方波 / 矩形电流
    ↓
保持较稳定转矩
```

## PMSM

```text
转子磁场
    ↓
分布式绕组
    ↓
正弦反电动势
    ↓
FOC / SVPWM
    ↓
正弦电流
    ↓
非常平滑的旋转磁场和转矩
```

---

# 8. 最关键的区别其实不是“一个方一个正弦”

很多人容易记成：

> BLDC = 方波  
> PMSM = 正弦波

但这个说法其实不够准确。

更准确应该是：

| | BLDC | PMSM |
|---|---|---|
| 本质 | 永磁同步电机 | 永磁同步电机 |
| 反电动势 | 通常梯形 | 通常正弦 |
| 典型控制 | 六步换相 | FOC |
| 相电流 | 通常矩形/120°导通 | 正弦 |
| 绕组 | 常见集中绕组 | 常见分布式绕组 |
| 转矩脉动 | 相对较大 | 更小 |

甚至你可以把 BLDC 看成：

> **为了适配六步换相，而故意设计成“梯形反电动势”的 PMSM。**

而 PMSM 则是：

> **为了获得平滑转矩，而设计成“正弦磁场 + 正弦电流”的永磁同步电机。**

如果你愿意，我还可以直接给你画一张 **“转子磁场 → 绕组 → 反电动势 → 电流 → 转矩”** 的对比图，把 BLDC 和 PMSM 为什么一个梯形、一个正弦完整串起来。

## Prompt:
2026/8/23 15:13:22

为什么要分出pmsm和bldc这两个类别, 不是一个东西吗

## Response:
2026/8/23 15:13:23

对，**从电机的物理本质来说，你完全可以把它们看成“一类东西”**。

更准确地说：

> **BLDC 是 PMSM 的一个特殊设计/控制分支，而不是和 PMSM 完全并列的两种物理原理。**

之所以后来分成两个名字，主要是因为**设计目标、反电动势波形和控制方法不同**。

### 先看“家族关系”

可以这么画：

```text
                 永磁同步电机
                    PMSM
                     │
          ┌──────────┴──────────┐
          │                     │
   正弦型设计                梯形型设计
          │                     │
       常称 PMSM              常称 BLDC
          │                     │
       FOC控制              六步换相
          │                     │
       正弦电流              矩形电流
```

所以你说“不是一个东西吗？”——**从电磁原理上说，是的，非常接近，甚至很多时候就是同一种电机换了一套叫法。**

---

## 那为什么非要叫两个名字？

因为工程上需要区分**“这个电机是怎么设计、怎么驱动的”**。

比如你拿到一个电机：

### 情况 A

它的反电动势：

```text
       /\
      /  \
_____/    \_____
```

比较接近正弦波。

通常：

```text
PMSM
 ↓
三相正弦电流
 ↓
FOC
 ↓
SVPWM
```

---

### 情况 B

反电动势：

```text
    ┌──────┐
____│      │____
    └──────┘
```

比较接近梯形波。

通常：

```text
BLDC
 ↓
六步换相
 ↓
120°导通
 ↓
矩形电流
```

所以工程师一听：

> “这是 BLDC。”

大概就知道：

**哦，它是梯形反电动势，通常采用六步换相。**

听到：

> “这是 PMSM。”

通常就会想到：

**正弦反电动势、正弦电流、FOC。**

这其实是一个**工程分类名称**。

---

# 最有意思的地方来了

实际上，**BLDC 这个名字有点“名不副实”**。

BLDC =

> Brushless DC Motor  
> 无刷直流电机

但它实际上：

- 转子是永磁体
- 三相绕组
- 反电动势随转子位置变化
- 本质上是交流电机
- 需要电子换相

所以从电机理论来看，它实际上属于：

$$
\boxed{\text{永磁同步电机}}
$$

而不是传统意义上的“直流电机”。

它之所以叫 **DC motor**，主要是因为它继承了传统直流电机的**电子换相 + 近似恒转矩控制思路**。

---

# 你甚至可以这样理解

传统有刷直流电机：

```text
直流电源
   ↓
机械换向器
   ↓
绕组
   ↓
转矩
```

BLDC：

```text
直流电源
   ↓
逆变器
   ↓
电子换相
   ↓
三相绕组
   ↓
转矩
```

所以 BLDC 相当于：

> **把有刷直流电机的“机械换向器”拿掉，然后用电子开关代替。**

因此才叫：

**无刷直流电机 BLDC。**

---

# 而 PMSM 是另一种思路

PMSM 从一开始就是按照交流同步电机来设计：

```text
直流母线
   ↓
逆变器
   ↓
三相正弦电流
   ↓
旋转磁场
   ↓
永磁转子同步旋转
```

所以它更接近传统的：

**交流同步电机 → 永磁化 → PMSM**

---

# 所以最准确的关系是

如果从**物理本质**分类：

$$
\boxed{
BLDC \subset \text{永磁同步电机}
}
$$

如果从**工程产品/控制方式**分类：

$$
\boxed{
BLDC \neq PMSM
}
$$

这两个说法都可以成立，只是分类角度不同。

---

而且这也解释了一个你前面问的 **SPWM / SVPWM** 问题：

**SVPWM 并不是“PMSM 专属技术”，BLDC 也完全可以用 SVPWM 驱动。**

同样：

**PMSM 也不是绝对只能用正弦电流。**

真正的核心其实是：

> **电机本身的反电动势长什么样 + 你希望采用什么样的电流波形和换相策略。**

这才是 BLDC 和 PMSM 区分最关键的地方。

## Prompt:
2026/8/23 15:15:03

那为什么foc算法在bldc也适用

## Response:
2026/8/23 15:15:05

对，而且这正好能把你前面几个问题串起来。

**FOC 能用于 BLDC，根本原因是：BLDC 和 PMSM 的电磁本质都是“三相永磁同步电机”，FOC 控制的是“定子电流产生的磁场”，而不是专门针对“正弦反电动势”。**

不过这里有一个很关键的细节：

> **FOC 可以用于 BLDC，但 BLDC 的梯形反电动势并不是 FOC 最理想的情况。**

---

## 1. FOC 到底在控制什么？

FOC（磁场定向控制）的核心不是：

> “我要产生正弦电压。”

而是：

> **我要控制定子电流矢量的方向和大小。**

比如转子永磁体产生一个磁场：

```text
                  转子 N
                    ↑
                    │
                    │  ψf
                    │
                    ●────────→
                 转子磁场
```

定子三相电流合成一个磁场：

```text
                定子磁场
                   ↗
                  /
                 /
                ●
                │
                │ 转子磁场
                ↓
```

FOC 做的事情就是：

**让定子磁场按照我希望的方向和大小旋转。**

---

# 2. 为什么 BLDC 也可以这么干？

因为 BLDC 仍然有：

- 三相定子绕组
- 永磁转子
- 转子位置
- 三相电流

所以你完全可以把：

$$
i_a,i_b,i_c
$$

转换成：

$$
i_\alpha,i_\beta
$$

再转换成：

$$
i_d,i_q
$$

也就是：

```text
三相电流
 ia ib ic
    │
    ↓ Clarke
 iα iβ
    │
    ↓ Park
 id iq
```

然后控制：

```text
id → 磁链
iq → 转矩
```

这跟 PMSM 是一样的。

---

# 3. 你可以把 FOC 想成“旋转坐标系”

这是理解 FOC 最重要的一步。

假设转子永磁体磁场一直在转：

```text
      t1          t2          t3

       ↑           ↗           →
       │          /           /
       ●          ●           ●
```

如果你站在地面看，它一直变。

但是如果你**跟着转子一起转**：

```text
                d轴
                ↑
                │
                ●────→ q轴
```

这个坐标系跟着转子走。

于是原本变化的三相交流电流，被转换成：

$$
i_d
$$

和

$$
i_q
$$

两个相对简单的量。

其中：

$$
i_q \rightarrow 转矩
$$

$$
i_d \rightarrow 磁链
$$

这就是 FOC 最核心的思想。

---

# 4. 那 BLDC 的“梯形反电动势”怎么办？

这才是重点。

理想 PMSM：

```text
反电动势：

      /\
     /  \
____/    \____
```

理想 BLDC：

```text
反电动势：

    ┌──────┐
____│      │____
```

所以严格来说：

$$
\boxed{\text{正弦反电动势 → FOC 最自然}}
$$

而：

$$
\boxed{\text{梯形反电动势 → 六步换相更自然}}
$$

但**FOC 并不要求反电动势必须是完美正弦波才能运行。**

---

# 5. 为什么还能运行？

因为电机真正产生转矩的根本关系是：

$$
T=\mathbf{\psi}\times\mathbf{i}
$$

也就是：

> **转子磁链和定子电流之间的夹角决定转矩。**

只要你知道转子位置，就可以让定子电流产生一个合适的旋转磁场。

所以即使 BLDC 的反电动势有梯形特征：

```text
BLDC

反电动势
    ┌─────┐
────┘     └────
       ↓
      FOC
       ↓
三相电流被控制成旋转电流矢量
       ↓
产生转矩
```

仍然可以工作。

---

# 6. 但是有一个坑

如果你拿一个**典型梯形反电动势 BLDC**，然后：

> 强行用“标准正弦 FOC”

那么它通常不会像正弦 PMSM 那么漂亮。

因为：

$$
T \approx e_a i_a+e_bi_b+e_ci_c
$$

如果：

$$
e_a,e_b,e_c
$$

本身不是正弦，而你又让：

$$
i_a,i_b,i_c
$$

严格正弦，那么瞬时转矩可能出现比较明显的**转矩脉动**。

也就是说：

```text
理想 PMSM：

正弦反电动势
      ↓
正弦电流
      ↓
平滑转矩
```

而典型 BLDC：

```text
梯形反电动势
      ↓
正弦电流
      ↓
可能产生转矩脉动
```

---

# 7. 那为什么现在很多“BLDC”都在用 FOC？

因为现在市场上所谓的“BLDC”这个词，实际上经常被用得非常宽泛。

例如一个电机厂家可能说：

> “这是 BLDC 电机。”

但实际上它可能是：

- 永磁同步电机
- 反电动势接近正弦
- 三相绕组
- 位置传感器/编码器
- 用 FOC + SVPWM 驱动

这种情况下，**从控制算法角度，它和你理解的 PMSM 几乎没什么区别。**

所以现在你经常会看到：

```text
“BLDC 电机”
      ↓
FOC
      ↓
Clarke
      ↓
Park
      ↓
PI
      ↓
SVPWM
```

完全没问题。

---

# 8. 甚至可以这样理解

你前面问：

> “BLDC 和 PMSM 不是一个东西吗？”

现在可以进一步得到：

### 电机本体

```text
              永磁同步电机
                   │
          ┌────────┴────────┐
          ↓                 ↓
    梯形反电动势          正弦反电动势
          ↓                 ↓
       BLDC典型            PMSM典型
```

### 控制方式

却是另一条维度：

```text
                  永磁电机
                     │
        ┌────────────┼────────────┐
        ↓            ↓            ↓
      六步换相       FOC         其他控制
        │            │
      矩形电流      正弦电流
```

所以：

> **“BLDC / PMSM”是在描述电机的电磁设计特征；“六步 / FOC”是在描述你怎么控制它。**

这两个概念**不是一一对应的**。

---

## 最后给你一个非常重要的对应关系

| 电机 | 反电动势 | 经典控制 | 也能不能 FOC |
|---|---|---|---|
| 典型 BLDC | 梯形 | 六步换相 | ✅ 能 |
| 典型 PMSM | 正弦 | FOC | ✅ 非常适合 |
| 正弦型 BLDC | 接近正弦 | 六步/FOC | ✅ |
| 梯形 PMSM | 接近梯形 | 六步/其他 | ✅ |

所以你现在可以把整个逻辑记成一句话：

> **FOC 不关心你叫它 BLDC 还是 PMSM，它真正关心的是：我能不能获得转子位置，并通过三相电流控制定子磁场矢量。**

而这也是为什么你学习 **Clarke → Park → $i_d/i_q$ → PI → SVPWM** 的时候，实际上是在学习一套非常通用的**永磁同步电机矢量控制方法**。

## Prompt:
2026/8/23 15:18:52

那pmsm能用六步换向吗

## Response:
2026/8/23 15:18:53

可以。**PMSM 完全可以用六步换向。**

而且这正好说明：**“PMSM / BLDC”和“FOC / 六步换向”不是一一对应的。**

### 先把关系理清

```text
                 永磁同步电机
                      │
          ┌───────────┴───────────┐
          │                       │
      正弦反电动势             梯形反电动势
          │                       │
       常称 PMSM                常称 BLDC
          │                       │
     ┌────┴────┐             ┌────┴────┐
     │         │             │         │
    FOC      六步换向        FOC      六步换向
```

所以：

**PMSM + 六步换向 → 能工作。**

---

## 但为什么通常不用？

因为**六步换向最适合梯形反电动势电机**。

假设 PMSM 的反电动势是正弦：

```text
e

       /\
      /  \
_____/    \_____
```

而六步换向给它的电流大致是：

```text
i

    ┌──────┐
____│      │____
```

那么：

$$
T \propto e\cdot i
$$

由于：

- 反电动势是正弦
- 电流是矩形

两者不能很好地匹配。

结果就是：

> **平均转矩可以有，但转矩脉动会比较大，而且电流利用率、噪声、振动等方面通常不如 FOC。**

---

## 为什么 BLDC 六步就比较合适？

典型 BLDC：

```text
反电动势：

       ┌───────┐
───────┘       └───────
```

六步控制：

```text
电流：

       ┌───────┐
───────┘       └───────
```

两者形状比较匹配。

于是：

$$
e\times i\approx 常数
$$

所以转矩比较容易保持稳定。

这就是为什么经典 BLDC 喜欢：

$$
\boxed{\text{梯形反电动势 + 六步换向}}
$$

而 PMSM 喜欢：

$$
\boxed{\text{正弦反电动势 + FOC}}
$$

---

## 但是有个特别容易混淆的地方

**六步换向 ≠ 只能给方波电流。**

六步换向的本质是：

> **每隔 60° 电角度改变一次导通的两相。**

比如：

```text
60°区间：

A+ B-

A+ C-

B+ C-

B+ A-

C+ A-

C+ B-
```

至于每个区间里面电流到底是：

- 恒定矩形
- PWM 调制
- 电流闭环控制

可以有不同实现。

所以你以后看到：

> **PMSM + 六步控制**

不要觉得奇怪。

它当然能转，只是对于一个设计成正弦反电动势的 PMSM 来说，**通常不是最优方案**。

---

### 一句话记忆

> **电机决定“反电动势长什么样”，控制算法决定“我给它什么样的电流”。**

所以：

**PMSM 能六步，BLDC 能 FOC。**

只是：

$$
\boxed{
\text{梯形} \leftrightarrow \text{六步}
}
$$

和

$$
\boxed{
\text{正弦} \leftrightarrow \text{FOC}
}
$$

是**最经典、最匹配的组合**，不是硬性规定。

## Prompt:
2026/8/23 15:19:44

那bldc的foc和pmsm的foc会有什么区别

## Response:
2026/8/23 15:19:46

有区别，但这个区别非常有意思：

> **BLDC 的 FOC 和 PMSM 的 FOC，底层的 Clarke、Park、$i_d/i_q$、PI、SVPWM 这些框架基本可以完全一样；真正的区别主要来自“电机的反电动势/磁链到底是不是正弦”。**

你可以把它理解成：

**FOC 算法框架一样，但“被控制的电机模型”不完全一样。**

---

## 1. 先看最核心的区别

典型 PMSM：

$$
\boxed{\text{正弦磁链} \rightarrow \text{正弦反电动势}}
$$

所以 FOC 的模型非常漂亮：

```text id="wcz5xx"
三相电流
  ↓
Clarke
  ↓
iα、iβ
  ↓
Park
  ↓
id、iq
  ↓
PI电流环
  ↓
SVPWM
  ↓
逆变器
  ↓
PMSM
```

而典型 BLDC：

$$
\boxed{\text{梯形磁链} \rightarrow \text{梯形反电动势}}
$$

这时候你仍然可以：

```text id="6zqjvi"
ia ib ic
  ↓
Clarke
  ↓
iα iβ
  ↓
Park
  ↓
id iq
  ↓
PI
  ↓
SVPWM
```

**但是 $d/q$ 模型不再像理想 PMSM 那么“干净”。**

---

# 2. 为什么？

这是关键。

标准 PMSM 的一个经典模型是：

$$
u_d=R_si_d+L_d\frac{di_d}{dt}-\omega_eL_qi_q
$$

$$
u_q=R_si_q+L_q\frac{di_q}{dt}
+\omega_e(L_di_d+\psi_f)
$$

这里有一个非常漂亮的东西：

$$
\psi_f
$$

可以近似认为是一个**恒定的转子永磁磁链**。

所以 FOC 做 Park 变换以后：

```text id → 磁链
iq → 转矩
```

两者可以比较干净地分开。

---

# 3. BLDC 就没这么理想

如果 BLDC 的反电动势是梯形的：

```text id="72lqjg"
       ┌───────┐
───────┘       └───────
```

那么它对应的转子磁链也不是标准的正弦：

```text id="90w8vc"
梯形反电动势
       ↓ 积分
非正弦磁链
```

这意味着：

> **你用 Park 变换以后，$d/q$ 轴上的量可能还会带有明显的周期性谐波。**

于是就不再是理想 PMSM 那种：

$$
i_q \rightarrow \text{几乎纯粹的转矩}
$$

而会出现：

$$
i_q \rightarrow \text{平均转矩 + 转矩脉动}
$$

---

# 4. 所以最明显的区别：电流波形

### PMSM FOC

通常希望：

```text id="4v3x4y"
ia
      /¯\      /¯\
_____/   \____/   \____

ib
    /¯\      /¯\
___/   \____/   \____

ic
  /¯\      /¯\
_/   \____/   \____
```

三相电流非常接近正弦。

---

### BLDC FOC

如果你直接使用普通 PMSM FOC：

```text id="4q6i0s"
ia
      /¯\      /¯\
_____/   \____/   \____
```

依然可以是正弦电流。

但是由于电机本身反电动势是梯形：

```text id="6j8z9e"
反电动势
    ┌───────┐
____│       │____
```

就可能出现：

```text id="1qz3bs"
转矩
    ~~~~~~~~
  ~~        ~~
~~            ~~
```

也就是**转矩脉动**。

---

# 5. 所以 BLDC FOC 往往会做“特殊处理”

如果只是：

> “让 BLDC 转起来”

那普通 PMSM FOC 就可以。

但如果你希望：

> “让梯形反电动势 BLDC 也获得很好的转矩性能”

就可能需要针对它的非正弦反电动势做补偿。

例如：

### 方法一：反电动势补偿

建立实际的：

$$
e_a(\theta),e_b(\theta),e_c(\theta)
$$

模型。

而不是假设：

$$
e_a=E\sin\theta
$$

---

### 方法二：谐波补偿

BLDC 的梯形波可以看成：

$$
\text{基波}+\text{3次、5次、7次……谐波}
$$

所以可以针对这些谐波进行补偿。

---

### 方法三：改变电流波形

这个非常重要。

如果电机反电动势是梯形：

```text id="j5q4wz"
e：
    ┌───────┐
____│       │____
```

你并不一定非要给它正弦电流。

理论上可以设计：

$$
i(\theta)
$$

让：

$$
e(\theta)i(\theta)
$$

尽可能保持恒定。

于是：

```text id="4oq4fc"
梯形 e
   ×
特殊电流 i
   ↓
恒定转矩
```

这实际上就已经开始接近**“针对 BLDC 的优化控制”**了。

---

# 6. 但是还有一个非常重要的现实问题

现在很多厂家把电机都叫：

> BLDC

但实际上它的反电动势可能已经非常接近正弦。

例如：

```text id="4fbd89"
厂家：
“BLDC Motor”

实际：
反电动势 ≈ 正弦
       ↓
FOC
       ↓
SVPWM
```

这种情况下：

$$
\boxed{\text{BLDC FOC} \approx \text{PMSM FOC}}
$$

你在代码里甚至很难找到什么本质区别。

---

# 7. 所以如果你正在看 STM32 的 FOC 代码

这点尤其重要。

你可能会看到：

```c
Clarke();
Park();
PID_Current();
SVPWM();
```

然后问：

> “这是 BLDC 的 FOC 还是 PMSM 的 FOC？”

**光看这些函数基本判断不出来。**

因为：

```text
Clarke
Park
PI
SVPWM
```

这些是**通用三相永磁电机矢量控制工具**。

真正要看的是：

### ① 电机参数

比如：

$$
R_s
$$

$$
L_d,L_q
$$

$$
\psi_f
$$

---

### ② 反电动势模型

有没有假设：

$$
e=E\sin\theta
$$

还是使用实际梯形反电动势。

---

### ③ 转矩模型

PMSM 常见：

$$
T_e=
\frac{3}{2}p
[\psi_f i_q+(L_d-L_q)i_di_q]
$$

而对于非正弦 BLDC，可能需要更加完整的：

$$
T_e(\theta,i_a,i_b,i_c)
$$

模型。

---

# 8. 最后给你一个非常关键的结论

你现在可以把整个关系理解成：

```text id="9q3xk4"
                 永磁三相电机
                       │
             ┌─────────┴─────────┐
             ↓                   ↓
       正弦反电动势           梯形反电动势
             │                   │
           PMSM                BLDC
             │                   │
             └───────┬───────────┘
                     ↓
                    FOC
                     │
          ┌──────────┼──────────┐
          ↓          ↓          ↓
       Clarke      Park       SVPWM
          │          │
          └──── id / iq ───────┘
```

**算法骨架基本一样。**

区别在于：

$$
\boxed{\text{PMSM：电机模型非常适合标准正弦 FOC}}
$$

而：

$$
\boxed{\text{BLDC：如果反电动势明显梯形，标准 FOC 只是“能用”，需要优化才能用得漂亮}}
$$

所以以后你看到“BLDC FOC”，**不要理解成一套完全不同于 PMSM FOC 的算法**。更准确地说，它是在**FOC 这个框架下针对非正弦永磁电机进行控制**。

而你前面一直在问的 **SVPWM 为什么能控制 BLDC、为什么 BLDC 也能做 FOC**，现在其实就串起来了：**SVPWM 只是逆变器产生三相电压矢量的手段，FOC 是控制电流矢量的方法，它们都不要求电机必须被厂家叫作“PMSM”。**

## Prompt:
2026/8/23 15:22:15

那现在有一个成熟且广泛专门针对梯形带电动势的foc方案吗

## Response:
2026/8/23 15:22:16

> **正在搜索网页**
>
>
> **正在搜索 4 个网站**
>
> 
>
> 思考了两秒

有，但这里要稍微纠正一下我前面给你的一个印象：

> **目前并没有一个像“标准 PMSM FOC”那样、行业内高度统一的“梯形反电动势 BLDC 专用 FOC”标准方案。**

这恰恰是因为：**对于真正的梯形反电动势 BLDC，工程上最成熟、最广泛的方案其实仍然是六步换相，而不是 FOC。** Microchip 也明确把典型 BLDC 定义为针对六步梯形控制设计的 PMSM。([Microchip](https://www.microchip.com/en-us/solutions/technologies/motor-control-and-drive/motor-control-algorithms?utm_source=chatgpt.com))

### 为什么没有形成一个统一的“BLDC-FOC”？

因为标准 FOC 的核心假设是：

$$
\boxed{\text{气隙磁场/反电动势近似正弦}}
$$

而真正的梯形 BLDC：

$$
e(\theta)=\text{非正弦}
$$

如果你硬套标准：

```text
Clarke
   ↓
Park
   ↓
id / iq
   ↓
PI
   ↓
SVPWM
```

得到的主要是**正弦电流**。

但梯形反电动势 + 正弦电流并不是最佳匹配，因此会留下明显的转矩脉动。TI 的工程讨论甚至直接指出，梯形 BEMF 电机用传统 FOC 会产生约 6 倍电频率的转矩脉动，并建议通过谐波电流注入等方式处理。([TI E2E](https://e2e.ti.com/support/microcontrollers/c2000-microcontrollers-group/c2000/f/c2000-microcontrollers-forum/493902/bldc-dq-transform?utm_source=chatgpt.com))

---

# 那真正的“梯形 BLDC FOC”怎么做？

比较有代表性的思路是：

### ① 还是用 FOC 的框架

```text
ia ib ic
   ↓
Clarke
   ↓
iα iβ
   ↓
Park
   ↓
id iq
   ↓
电流控制
   ↓
SVPWM
```

### ② 但不能再简单假设电机是“完美正弦 PMSM”

需要把实际的：

$$
e_a(\theta),e_b(\theta),e_c(\theta)
$$

考虑进去。

然后设计**非正弦电流参考**，使：

$$
T_e(\theta)
$$

尽可能恒定。

也就是：

```text
梯形反电动势
       ↓
计算需要的电流波形
       ↓
非正弦电流参考
       ↓
电流闭环
       ↓
SVPWM
       ↓
低转矩脉动
```

这类方法通常会涉及：

- 反电动势模型
- 谐波电流注入
- 6次转矩脉动补偿
- 非正弦电流参考
- 非正弦反电动势观测器
- 转矩脉动补偿

而不是简单地把 PMSM FOC 原封不动搬过去。

---

# 但它为什么没有像 PMSM FOC 那样普及？

因为有一个非常现实的工程问题：

**你费这么大劲把梯形 BLDC 做成高级 FOC，为什么不直接把它当普通永磁同步电机来设计？**

如果电机本身可以重新设计，那么：

```text
重新设计绕组/磁钢
       ↓
让 BEMF 接近正弦
       ↓
标准 PMSM FOC
       ↓
搞定
```

这比针对一个梯形电机专门做一套复杂的非正弦 FOC 更容易形成标准化方案。

所以市场上非常常见的是：

### 梯形 BLDC

$$
\boxed{\text{六步换相}}
$$

### 正弦型永磁电机

$$
\boxed{\text{标准 FOC}}
$$

而不是：

$$
\boxed{\text{梯形 BLDC + 高级非正弦 FOC}}
$$

---

# 但是，确实有成熟商用方案支持 BLDC + FOC

这点现在已经很明确了。

例如 TI 有同时支持 BLDC/PMSM 的 FOC 参考设计；Microchip 也有明确支持 BLDC 的 FOC 开发方案。([Texas Instruments](https://www.ti.com/tool/TIDM-THREEPHASE-BLDC-LC?utm_source=chatgpt.com))

甚至 Microchip 现在的资料直接把 **BLDC FOC** 单独列出来，说明 FOC 可以用于 BLDC。([Microchip Developer Help](https://developerhelp.microchip.com/xwiki/bin/view/applications/motors/control-algorithms/bldc/foc/?utm_source=chatgpt.com))

但这里有一个坑：

> **“支持 BLDC 的 FOC” ≠ “专门针对理想梯形反电动势优化过的非正弦 FOC”。**

很多所谓的 BLDC FOC，实际上是：

```text
BLDC这个产品名称
        ↓
电机实际BEMF可能已经比较接近正弦
        ↓
标准PMSM FOC
```

这在工业产品里非常常见。

---

# 你如果是想自己在 STM32 上做

那我反而建议你不要一上来搞“梯形 BLDC 专用 FOC”。

可以分成三个阶段：

### 第一阶段：标准六步

```text
Hall / BEMF
   ↓
六步换相
   ↓
PWM
```

先把 BLDC 跑起来。

### 第二阶段：标准 FOC

```text
Clarke
 ↓
Park
 ↓
Id/Iq PI
 ↓
SVPWM
```

把你的 BLDC 当作 PMSM 跑。

### 第三阶段：针对梯形 BEMF 优化

再研究：

$$
\boxed{\text{谐波电流注入 + 转矩脉动补偿}}
$$

这时候你就真正进入了“**梯形 BEMF 的非正弦 FOC**”这个领域。

---

而且我觉得这第三阶段**非常适合你现在正在学的东西**。

因为你前面一直在问：

**SPWM → SVPWM → Clarke/Park → FOC → BLDC/PMSM**

其实下一步最值得搞明白的就是：

> **为什么梯形反电动势 + 正弦电流会产生 6 次转矩脉动，以及怎样通过“电流谐波注入”把这个 6 次转矩脉动抵消掉。**

这个问题一旦搞懂，你就会真正理解**为什么“普通 PMSM FOC”不能完美解决梯形 BLDC 的问题**。

---

**Sources:**

- [Motor Control Algorithms | Microchip Technology](https://www.microchip.com/en-us/solutions/technologies/motor-control-and-drive/motor-control-algorithms?utm_source=chatgpt.com)
- [BLDC dq transform - C2000 microcontrollers forum - C2000™︎ microcontrollers - TI E2E support forums](https://e2e.ti.com/support/microcontrollers/c2000-microcontrollers-group/c2000/f/c2000-microcontrollers-forum/493902/bldc-dq-transform?utm_source=chatgpt.com)
- [TIDM-THREEPHASE-BLDC-LC reference design | TI.com](https://www.ti.com/tool/TIDM-THREEPHASE-BLDC-LC?utm_source=chatgpt.com)
- [Field-Oriented Control (FOC) Brushless DC (BLDC) Motor Algorithms - Developer Help](https://developerhelp.microchip.com/xwiki/bin/view/applications/motors/control-algorithms/bldc/foc/?utm_source=chatgpt.com)

## Prompt:
2026/8/23 15:23:43

所以实际性能上bldc是不是还是使用六步环形最好

## Response:
2026/8/23 15:23:44

如果你说的是**“真正设计成梯形反电动势的 BLDC”**，那么基本可以这么说：

> **是的，六步换相通常是最合适、最成熟、性价比最高的方案。**

但“性能最好”要加一个前提：**看你追求什么性能。**

### 典型梯形 BLDC

它本来就是按照：

$$
\boxed{\text{梯形反电动势}+\text{矩形电流}}
$$

来设计的。

六步换相正好匹配：

```text
反电动势
      ┌───────┐
──────┘       └──────

电流
      ┌───────┐
──────┘       └──────
```

所以它的优点非常明显：

- 控制简单
- MCU 运算量低
- 换相逻辑简单
- 成本低
- 低成本位置传感器（如 Hall）就能做
- 调试相对容易
- 对典型 BLDC 来说效率也可以很好

因此风扇、电动工具、水泵、压缩机等大量产品都采用这种思路。

---

### 但如果你说“综合性能最好”，就不一定了

六步换相的最大问题就是：

$$
\boxed{\text{换相瞬间 + 非正弦电流} \rightarrow \text{转矩脉动}}
$$

典型表现：

```text
六步 BLDC：

转矩
      /¯\    /¯\
_____/   \__/   \____
```

所以它可能有：

- 转矩脉动
- 噪声
- 振动
- 换相电流尖峰
- 高速时换相性能下降

如果你要求：

> **特别平滑、低噪声、低振动、高动态性能**

那 FOC 通常更有优势。

---

### 所以真正应该这样选

| 需求 | 更适合 |
|---|---|
| 低成本 | **BLDC 六步** |
| 控制简单 | **BLDC 六步** |
| 风扇/水泵 | **六步很合适** |
| 成本敏感 | **六步** |
| 对转矩脉动不敏感 | **六步** |
| 极低噪声 | **FOC** |
| 低振动 | **FOC** |
| 高动态响应 | **FOC** |
| 高精度伺服 | **FOC + PMSM** |
| 高性能电动车 | **FOC + PMSM** |

---

不过有一个特别重要的概念：

> **六步换相并不一定“性能低”，它是“针对梯形 BLDC 最匹配的控制方法”。**

就像：

**赛车发动机不一定比家用发动机“更适合所有场景”。**

如果你的电机本来就是梯形 BEMF，要求又只是：

> 转得快、效率高、便宜、控制简单

那么你花大力气搞高级 FOC，**很可能是在拿复杂度换一个用户根本感觉不到的收益。**

反过来，如果你想要：

> **特别安静、转矩特别平滑、精确控速/控转矩**

那就值得上 FOC。

而如果从**电机设计阶段**就能选择，那么很多时候更直接的办法其实是：

$$
\boxed{\text{把电机设计成正弦 BEMF + FOC}}
$$

而不是：

$$
\boxed{\text{梯形 BEMF + 非正弦 FOC}}
$$

这也是为什么你现在看到的高性能永磁电机，很多最终都会走向**正弦 BEMF + FOC + SVPWM**这条路线。

## Prompt:
2026/8/23 15:25:16

针对bldc设计的foc是不是性能上比六步换向低但是稳定性上比六步换向高

## Response:
2026/8/23 15:25:17

不完全是。**“FOC 性能低、但稳定性高”这个判断反过来了一部分。**

如果我们限定讨论的是**真正的梯形反电动势 BLDC**：

> **六步换向通常是最匹配、最简单、最鲁棒的；针对 BLDC 优化过的 FOC 则有更高的性能潜力，但控制复杂度和调试难度也更高。**

可以这么看：

| 项目 | 六步换向 | BLDC 专用/优化 FOC |
|---|---|---|
| 控制复杂度 | ⭐ | ⭐⭐⭐⭐ |
| 算力需求 | 低 | 高 |
| 参数依赖 | 少 | 多 |
| 调试难度 | 低 | 高 |
| 启动 | 很成熟 | 需要更完善的启动策略 |
| 低速稳定性 | 很好 | 做好后也很好 |
| 转矩脉动 | 较大 | **可以明显降低** |
| 噪声振动 | 较大 | **可以降低** |
| 转矩控制精度 | 一般 | **高** |
| 动态性能 | 好 | **通常更好** |
| 电流利用率 | 好 | **可以进一步优化** |
| 故障/参数异常容忍度 | **通常更强** | 相对依赖模型和参数 |
| 实现成本 | **低** | 高 |

### 为什么你会觉得 FOC “稳定性更高”？

这个要区分两种“稳定”。

如果你说的是：

> **转速稳定、转矩平滑**

那确实，FOC 往往更好。

因为 FOC 有电流闭环：

$$
i_q^* \rightarrow i_q
$$

可以精确控制转矩。

而六步换向主要是：

```text
Hall位置
 ↓
判断60°区间
 ↓
切换导通状态
```

所以换相的时候容易产生转矩波动。

---

但如果你说的“稳定性”是：

> **不容易死机、不容易失控、参数变了还能正常运行、抗干扰强**

那反而通常是**六步换向更鲁棒**。

因为它根本不需要那么多电机参数。

六步只需要知道：

```text
现在在哪个60°区间？
        ↓
该哪两相导通？
```

而 FOC 要处理：

$$
R_s,L_d,L_q,\psi_f
$$

还涉及：

- 转子角度
- 电流采样
- Clarke/Park
- PI 参数
- SVPWM
- 速度环
- 参数辨识
- 保护逻辑

所以 FOC 的“性能高”，是以**控制系统复杂度**换来的。

---

### 最关键的一句话

如果是**普通梯形 BLDC**：

$$
\boxed{\text{六步 = 简单、鲁棒、匹配度高}}
$$

而优化后的 FOC：

$$
\boxed{\text{FOC = 更平滑、更精确、更高性能，但更复杂}}
$$

所以不是：

> FOC 性能低，稳定性高。

而更接近：

> **六步：基础性能已经很好，而且非常鲁棒；FOC：性能上限更高，但需要更复杂的控制和更好的参数/采样/位置估计。**

---

还有一个很有意思的结论：

如果你所谓的“BLDC FOC”只是**普通 PMSM FOC + SVPWM**，而电机又是明显的梯形 BEMF，那么它**不一定比六步性能好**。

但如果是专门针对梯形 BEMF 做了：

$$
\boxed{\text{非正弦电流参考 + 谐波补偿 + 转矩脉动补偿}}
$$

那么它的性能上限就可以超过传统六步。

所以真正的比较应该是：

$$
\boxed{
\text{六步 BLDC}
\quad VS \quad
\text{针对梯形 BEMF 优化的 FOC}
}
$$

而不是简单的：

$$
\text{六步 VS 普通 FOC}
$$

这两个概念差别挺大的。

## Prompt:
2026/8/23 15:26:22

那是不是针对bldc优化的foc事实上并不成熟性价比并不高

## Response:
2026/8/23 15:26:23

对，**如果你说的是“真正针对梯形反电动势 BLDC 做深度优化的 FOC”**，这个判断基本是成立的：

> **它不是没有成熟方案，而是相比“六步换相”，在大量普通 BLDC 应用里性价比并不高。**

原因不是 FOC 不好，而是**它解决的问题，很多 BLDC 应用根本没必要花这么大代价去解决。**

### 你可以把它理解成这样

```text
典型梯形 BLDC
      │
      ├── 六步换相
      │      ↓
      │   简单、成熟
      │   便宜、鲁棒
      │   已经很好用
      │
      └── 非正弦 FOC
             ↓
        更复杂的控制
             ↓
       降低转矩脉动
       优化噪声/振动
       提高转矩控制能力
             ↓
        但成本也上去了
```

所以对于：

- 风扇
- 水泵
- 普通电动工具
- 小型执行器
- 成本敏感产品

六步换相往往就是**最合理的工程方案**。

---

## 但不能说“BLDC FOC 不成熟”

这一点要稍微严谨一点。

现在：

$$
\boxed{\text{BLDC + FOC}}
$$

本身已经是可以商用的，TI、ST、Microchip、NXP 等都有相关方案。

真正不那么“标准化”的是：

$$
\boxed{\text{针对明显梯形 BEMF 的非正弦 FOC}}
$$

也就是你前面一直问的那种：

> **我明明知道这个 BLDC 的反电动势是梯形的，我还要用矢量控制，而且我要专门优化它的电流波形，让转矩尽可能恒定。**

这种方案当然有研究和产品应用，但**没有像标准 PMSM FOC 那样形成一个非常统一、简单、广泛复制的工业套路。**

---

## 为什么？

因为它有一个很尴尬的工程问题：

假设你现在有：

```text
梯形 BEMF BLDC
      ↓
六步换相
```

已经：

- 能跑
- 效率不错
- 控制简单
- MCU 要求低
- 成本低
- 鲁棒性好

现在老板说：

> “我们想把转矩脉动再降低一点。”

你有两个选择。

### 方案 A：优化六步

例如：

- PWM 策略优化
- 换相提前角
- 换相补偿
- 电流控制
- 换相重叠
- Hall 位置补偿

成本增加不多。

### 方案 B：上高级 FOC

需要：

```text
电流采样
 ↓
Clarke
 ↓
Park
 ↓
位置角
 ↓
Id/Iq控制
 ↓
非正弦电流参考
 ↓
谐波补偿
 ↓
SVPWM
```

然后 MCU 算力、软件复杂度、参数调试、采样精度要求全部上升。

如果最终只是：

> **“转矩脉动从 10% 降到 5%。”**

老板可能会问：

> **“那我为什么不直接把六步优化一下？” 😂**

这就是性价比问题。

---

# 真正值得上 FOC 的场景

如果目标变成：

> **低噪声 + 低振动 + 精确转矩 + 高动态 + 宽速域**

这时候就不一样了。

因为六步换相的先天问题：

$$
\boxed{\text{离散换相}}
$$

很难彻底消除。

这时候 FOC 的价值就出来了。

尤其是：

```text
高速
 ↓
中速
 ↓
低速
 ↓
堵转附近
```

都需要比较精细的电流/转矩控制时，FOC 的优势越来越明显。

---

# 所以你现在可以得到一个非常完整的结论

### 传统 BLDC

$$
\boxed{
\text{梯形 BEMF}
+
\text{六步换相}
}
$$

**成熟、便宜、鲁棒、性价比高。**

---

### 正弦 PMSM

$$
\boxed{
\text{正弦 BEMF}
+
\text{FOC}
+
\text{SVPWM}
}
$$

**成熟、性能高、工业应用广。**

---

### 梯形 BLDC + 专用非正弦 FOC

$$
\boxed{
\text{梯形 BEMF}
+
\text{非正弦 FOC}
}
$$

**能做，也有价值，但复杂度更高，应用场景更窄。**

所以如果让我给它们一个非常粗略的工程评价：

```text
                    性能上限
                       ↑
                       │       非正弦BLDC FOC
                       │          ●
                       │
                       │   PMSM FOC
                       │      ●
                       │
                       │  六步BLDC
                       │     ●
                       └────────────────→
                            实现复杂度
```

**六步不是“落后技术”，而是对于梯形 BLDC 来说，它本身就是一个非常合理的最优解。**

而 FOC 真正大放异彩的地方，恰恰是**正弦型 PMSM**——因为电机本身就是为这种控制方式准备的。

## Prompt:
2026/8/23 15:27:47

网上有没有梯形和正弦型反电动势的示波器对比图

## Response:
2026/8/23 15:27:49

> **正在搜索网页**
>
>
> **正在搜索 13 个网站**
>
> 
>
> 思考了几秒

有，而且我刚查了一下，**确实有非常直观的示波器实测图**。甚至有一张 TI 论坛的图，正好把不同测量方式下的波形放在一起，很适合你现在这个问题。

### 1. 最推荐：TI 的真实示波器截图

![Image](https://images.openai.com/static-rsc-4/BtHNtlzuzEEQMYPPPkconRtp1Ij8UUVwJDUunH1Z6oYWrszYPx6IETL0TeyiNwQtWGBJ8W0jAkHgQpi-VXLXLxCqxfVU05Ekftd2R1yyEyNUhPXZvZeAhKft6GyUIflKP9_9nQ4C-CrxxO7OH7wDKCo7-WF_KSe0qw-3i7rPGXVIgwR1PmEj-Dm3eewDJw6J?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/2nNdiOGBOUrd2TZwoaB_UO_bQyLNGMFFfOTIx8NZDpEDNhrpAc_OL5y4q6cLBVFKqiOy2Ush-aUOPBN7ToLtzcRCrAL3h4vq50fheRFfoBiXuJIPEtkfkGFzbpjFU25Af0vDZzN7ipQvNkw7QfH8U360HT7ROwr8JCanJaWxpm1cLQqJfgZLmy1pbomYPwaZ?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/npYLaniRZqJrRmO2YnNCbIdUFHTlTU-BELx2AGSPmSDUV8xwaUamwQdIXPDG58cLcu1S98Lk9PmCK-bxqNrjwYSodkPNflLVe8WQSuKcV5bDjbloiAEpEeYKIUITzESpSoYywBTbq5qfwi4EO_GUValo6_g1YYxHB4EVkgtEl1KMeAbtJ4vLmNMwvkkqECqE?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/9OJ5JBFIo8ticKhkmO6vXc0lY4jwC_tGW8gUAXSV8q9AMYVAGynBStlq6N9J8vrR9-iT9-K2bn3ZCC7fodtHs-lijGGS4RGSujRt5OzEX3NQIC-TUJkZ339u5q9p1Y4Cqfqh4Ty7AUV7yVg4qY6EnZMyrFnwHZzBqGHsRyJ-3jtx9pX8owxlVGD8Vw4fcrIQ?purpose=fullsize)

TI 的这个案例是实际用示波器测 BLDC 电机反电动势。黄色通道能看到比较明显的**梯形特征**，另外的通道呈现更接近正弦的波形。TI 的讨论还专门分析了为什么**线电压和相电压的测量结果可能看起来不一样**。([TI E2E](https://e2e.ti.com/support/microcontrollers/c2000/f/171/t/756653?utm_source=chatgpt.com))

如果你想直接看原图：

[TI C2000 论坛：Back EMF of BLDC motors](https://e2e.ti.com/support/microcontrollers/c2000-microcontrollers-group/c2000/f/c2000-microcontrollers-forum/756653/back-emf-of-bldc-motors-trapezoidal-or-sinusoidal?utm_source=chatgpt.com)

---

### 2. 还有一个特别适合对比的 TI 资料

TI 的资料里直接把：

**正弦反电动势**

和

**梯形反电动势**

画在一起：

[TI：正弦波与台形波逆起电力对比资料](https://www.ti.com/content/dam/videos/external-videos/en-us/8/3816841626001/6243256559001.mp4/subassets/bldc_motor1_basic_jp.pdf?utm_source=chatgpt.com)

它不是示波器实测，而是理论波形，但非常清楚：

```text
正弦型：

       /\
      /  \
_____/    \_____
    /        \
___/          \___

梯形：

      ┌──────┐
_____/        \_____
    \          /
     └────────┘
```

---

### 3. 如果你想看“真正示波器屏幕上的梯形 BEMF”

这个也很不错：

![Image](https://images.openai.com/static-rsc-4/aQx4hQgakuMgqh_alD_tQOhe_Uu6VEls2QdgBF2tQkWr99NPouiNiB8YNd0DLSvWUnOUAgc3KNo3ecb7S-gr5loLqiijTI_XYUl3YXMkXdWW5wJc8Pr0UWz_glHENnVboL--dQpwfh6zUhmHZet4X-5a8WSTWXKo0kmDHMFu79N_puPFHD5VaxZeoiwXLIth?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/26zsjRM9viM5dERYbwcPFkUQ1XSWVmT5iVwHt8fxR0MDSRHtuQhb4vYoG3gWaFaruToYJ77HWZR9QkpezPw5HINpeoGfaW6y-xHtaT8str_8xkTX8XbjBDj7UsmEaPPk6wEpuwqlXgHi6goy8bW-MwockxSuaDvycyYcadulq9DNtY20XRlTNWAXmrYTXbma?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/m-VxEH4wWel6G9ho6UHK8Sgup5QY-ZHPAinhAvjKuSYIGmmRLWA34GGJQ03lE3-T0v2saTkeqhgqp71qA9Ia7VQ7-320fbyfSvFL92UOYDwZcL6FOWH-UwXkPyzA5WYQok3Fx9H7bKrlD7R-HBECwykmR-RlOSWuMcqbYwxU1cC7qACfhsp6WIGOxWzvtUJ_?purpose=fullsize)

比如 Zubax 的电机测试资料就有真实 Rigol 示波器截图，可以看到两相反电动势的**平顶/梯形特征**。

---

## 4. 还有一个很有意思的实测结果

我建议你特别看看这个。

有工程师实际测了 **3 个所谓的 BLDC 电机**，结果发现：

- 一个非常接近梯形
- 一个并不太像梯形
- 还有一个几乎是完美正弦

也就是说：

> **现实中的“BLDC”反电动势根本没有理论图那么标准。**

[Electrical Engineering Stack Exchange：3 个 BLDC 实测反电动势波形讨论](https://electronics.stackexchange.com/questions/347683/how-can-one-achieve-sinusoidal-back-emf-in-pmsm-and-trapezoidal-back-emf-in-bldc?utm_source=chatgpt.com)

这其实也正好解释了我们前面一直讨论的：

> 为什么“BLDC + FOC”也完全可以做。

因为现实中的电机可能根本不是一个漂亮的理想梯形波。

---

### 一个非常重要的测量注意事项

如果你自己以后拿示波器测，**不要直接把示波器地夹夹到三相电机某一相上，然后随便测**，尤其是电机已经接在逆变器上的时候。

最好是在：

**电机断开驱动器 → 外力匀速拖动转子 → 测开路反电动势**

这种条件下观察。

而且要区分：

$$
\boxed{\text{相电压 BEMF}}
$$

和

$$
\boxed{\text{线电压 BEMF}}
$$

两者波形可能会让你产生很大的误解。TI 的那个实测案例恰好就遇到了这个问题。([TI E2E](https://e2e.ti.com/support/microcontrollers/c2000/f/171/t/756653?utm_source=chatgpt.com))

**如果你愿意，我还可以帮你找一组“同一个示波器画面里，左边梯形 BLDC、右边正弦 PMSM”的真实实测图**，这种最适合你直接拿来对照理解。

---

**Sources:**

- [Back Emf of BLDC motors (trapezoidal or sinusoidal) - C2000 microcontrollers forum - C2000™︎ microcontrollers - TI E2E support forums](https://e2e.ti.com/support/microcontrollers/c2000/f/171/t/756653?utm_source=chatgpt.com)



---
Powered by [ChatGPT Exporter](https://www.chatgptexporter.com)