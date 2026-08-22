# OCD5010 新增 `ADC_DNL_Test` 测试项 —— 分析设计文档

> 任务：参考 `chips/ocd2810/tests.py` 中 `def ADC_DNL_Test():`（第 943~1111 行），
> 在 `chips/OCD5010/tests.py` 中新增同名的 `def ADC_DNL_Test()` 测试项。
> 依据：`ODS32M501xUM 260720.docx`（2026-07-20 版数据手册，CR 位域已修正）第 19 章 ADC。
>
> 本文档**仅做分析设计**，不直接修改任何源码。

---

## 1. 需求概述

| 项 | 内容 |
|---|---|
| 参考实现 | `chips/ocd2810/tests.py` → `ADC_DNL_Test()`（`@test_case`，5.9 ADC 测试） |
| 新增位置 | `chips/OCD5010/tests.py`（与现有 `@test_case` 函数并列） |
| 依据文档 | `ODS32M501xUM 260720.docx`（2026-07-20 新版）第 19 章 ADC（12bit SAR，2Msps，序列转换/校准/过采样）；旧版 `ODS32M501xUM.docx` 的 **CR 位域有整体 1 位偏移错误**，需以新版为准 |
| 硬件依据 | `test_configs/OCD5010/dohko2_Test_Plan_Guide.docx`（ADC 测试方案） |
| 规格来源 | `test_configs/OCD5010/test.xlsx`（需新增 `ADC_DNL` 行，参考 2810 为 `Min=-2 / Max=2 / Unit=LSB`） |
| 菜单注册 | `TEST_ITEMS` 字典（`main.py` 自动加载） |

---

## 2. 工程结构与运行机制

```
main.py                        # CHIP = "OCD5010"，动态 importlib 加载芯片模块
└── chips/OCD5010/
    ├── chip.py                # CHIP_LABEL、init_all() 初始化 SWD/串口/DMM/示波器
    ├── tests.py               # 测试函数 + TEST_ITEMS 映射（1323 行，目前 10 个菜单项）
    ├── get_data.py            # load_excel_data / get_32bit_reg_address
    └── trim/                  # 修调值组装与 OTP 烧写（与本文无关）
instrument/instrument_set.py   # 仪器统一接口（SWD/串口/DMM/示波器/信号发生器）
test_configs/OCD5010/
    ├── test.xlsx              # 测试规格（目前无 ADC 章节）
    ├── register.xlsx          # 寄存器表（仅 ANAC 模拟模块，无 ADC 数字模块）
    └── pin_configs.json       # GPIO5=15, GPIO6=31
Save_Results/OCD5010/          # 测试结果保存目录
```

**运行链路**（`main.py`）：

1. `CHIP="OCD5010"` → 加载 `chips.OCD5010.tests`，读取模块级 `TEST_ITEMS` 构建交互菜单；
2. 选中测试项 → 调用 `func()`；测试函数带 `@test_case` 装饰器，自动把 `print` 捕获并记录到 `Save_Results/OCD5010/item_results.xlsx`；
3. 测试函数内部通过 `save_item_result(test_name, passed, actual_value)` 写结果；
4. 规格与寄存器地址分别从 `df_test_data`（test.xlsx）、`df_register`（register.xlsx）读取。

`tests.py` 中可复用的基础设施：

| 元素 | 说明 |
|---|---|
| `@test_case` 装饰器 | 包裹 `TestContext`，捕获输出 + 记录结果 |
| `save_item_result()` | 写 item_results.xlsx |
| `save_trim_to_excel()` | 写 trim_best_value.xlsx（修调项用） |
| `global_use` 字典 | 跨测试函数共享变量（如最佳修调值） |
| `df_test_data` / `df_register` | 已加载的规格表 / 寄存器表 |
| `pin_cfg` / `eq_cfg` | 引脚（GPIO5/GPIO6）与设备（DMM_MODE=38 等）配置 |
| `frange()` | 浮点步进生成器 |

---

## 3. 2810 参考实现拆解（`chips/ocd2810/tests.py:943-1111`）

### 3.1 流程

```
① 读规格   : df_test_data 中"ADC_DNL"行的 Min/Max/Unit（Min=-2, Max=2, Unit=LSB）
② 取寄存器 : get_32bit_reg_address() 取 Pad/ADC_CLK/VREF/ADC_EN/CH6EN/ADCBSY/ADC_SCYC/ADC6_DR 等
③ 初始化   : 使能 Pad 模拟、VREF、ADC、通道 CH6、采样周期 ADC_SCYC=0x7
④ 接线确认 : 提示 "P20 接 DAC 输出线 + 数字万用表"，input() 等待
⑤ 选模式   : 1=固定1000mV / 2=随机10点 / 3=线性扫描(500~4000mV,步进默认100mV)
⑥ 逐点测量 : 串口 spi_cmd_set_val mV {vin} 设电压 → DMM 实测 vol → 触发 ADC(ADCBSY bit6) → 读 ADC6_DR & 0xFFF
⑦ 计算     : vreal = adc/4095 * Voffset_P（Voffset_P 为 VREF 实测电压,由 VREF_Test 存入 global_use）
             dnl_error_lsb = (vol - vreal) / (Voffset_P/4095)
⑧ 判定     : adc_dnl_max = max(ADC_LSB_list)；adc_min <= adc_dnl_max <= adc_max
⑨ 保存     : save_item_result("ADC_DNL", passed, round(adc_dnl_max,3))
⑩ 数据落盘 : 可选保存 xlsx 到桌面（列: 输入电压/万用表实测/ADC原始/ADC转换电压/DNL误差mV/DNL误差LSB）
⑪ 清理     : 关闭 CH6EN/ADC_EN → cmd_gpio_set_595d_test reset
```

### 3.2 关键实现要点（移植时需保留的"骨架"）

1. 测试项名称常量 `adc_test_item_name = "ADC_DNL"`，从 `df_test_data` 按 `测试项` 列匹配；
2. 对读取规格/寄存器统一 try/except，失败即 return；
3. 输入电压由板载 DAC 通过串口指令 `spi_cmd_set_val mV {vin}` 提供，DMM 与 ADC 输入端并联实测；
4. DNL 计算：以 **实测输入电压** 与 **ADC 转换电压** 之差折算 LSB（LSB = VREF/4095），取最大偏差；
5. 判定区间 `[Min, Max]`（±2 LSB），`save_item_result` 记录；
6. 交互式三选一电压模式（固定/随机/线性扫描），`random.seed(42)` 保证复现；
7. 支持把测量明细导出 xlsx（openpyxl，保存到桌面）；
8. 结尾关闭 ADC 通道并发送 `cmd_gpio_set_595d_test reset` 复位通路。

> ⚠️ 注意：2810 的 `max(ADC_LSB_list)` 取的是**最大值**（非绝对值），且其 DNL 定义是"实测电压-理想电压"的 LSB 偏差。
> OCD5010 移植时可沿用，也可按 dohko2 方案改用"输入-输出转移曲线最大斜率偏差"（见 §5.7）。

---

## 4. OCD5010 ADC 硬件特性（摘自 `ODS32M501xUM 260720.docx` 第 19 章，新版）

### 4.1 总体特性

- 12bit SAR-ADC，最高 2Msps，24 个外部通道 + 内部 CSA/温度/电源信号；
- 支持 16 个转换序列（4 段），支持模拟看门狗、软件/硬件触发、过采样求平均、增益/偏移校准；
- 校准公式：`ADC_CALDAT = ADC_RAW × GAIN + OFFSET`（GAIN 12bit 定点小数，OFFSET 12bit 有符号）；
- **数字 ADC 模块基地址：`0x4001_2400`**（与 ANAC 模拟控制 `0x40007C18` 分开）。

### 4.2 ADC 软件触发采样流程（4 步，CPU halt 下可由 SWD 完成）

> 以下内容已与 **SFR / ADC_Reg.h / ODS_5010.h** 三方核对，为权威配置。

| 步骤 | 操作 | 寄存器 | 关键位 |
|------|------|--------|--------|
| 1 | 使能 ADC 时钟 | `RCC.ANACKE @ 0x4002005C` | `ADCCKE` bit0 = 1 |
| 2 | 配置采样序列 | `TGSR` / `TGNR` / `SEQR0..3` / `TCFG` | 触发源=软件、通道数、通道映射、采样时序 |
| 3 | 使能 ADC | `CR @ 0x40012460`（实测写入 `0x00310031`；可选 `ANAC_ENCR @ 0x40007800`） | `ADC_EN` bit0 置 1 |
| 4 | 软件触发 → 轮询 → 读结果 | `OTR` → `IR` → `DATA[n]` | 写 `OTR=0x5AA5`；`IR.S0CDIF` bit0；`DATA[0] @ 0x40012404` |

> **原理说明**：ADC 转换由外设时钟（PCLK/采样时钟）驱动，CPU 是否运行不影响转换完成；
> halt 后 SWD 直接写寄存器即可启动一次转换，`IR.S0CDIF` 置位后回读 `DATA[n]` 得到结果。
> 即**无需** ANAC `RW_ADC_START`（0→1→0）手工触发，直接用 `ADC_OTR` 写 `0x5AA5` 软件触发即可。

### 4.3 ADC 数字模块寄存器（`ODS32M501xUM 260720.docx` 表 19-4，地址已与 SFR/ADC_Reg.h 核对）

> 重要差异：**这些寄存器当前不在 `register.xlsx` 中**，要么补入寄存器表，要么在代码中硬编码地址（2810 已有硬编码先例 `0x40001424`）。

| 寄存器 | 地址 | 位域/说明 |
|---|---|---|
| `RCC.ANARST` | `0x4002_0014` | 模拟模块复位（bit0=0 默认已释放，一般无需处理） |
| `RCC.ANACKE` | `0x4002_005C` | **bit0 `ADCCKE` = 1 使能 ADC 时钟（必须）** |
| `ANAC.ENCR` | `0x4000_7800` | 模拟模块电源/基准使能（可选，参考 pga_demo，位域见 §4.4） |
| `ADC_DATA0` | `0x4001_2404` | `[11:0] DATA0` 序列 0 转换结果 |
| `ADC_DATA1`~`ADC_DATA15` | `0x4001_2408` ~ `0x4001_2440` | 序列 1~15 结果 |
| `ADC_AVGDAT` | `0x4001_2444` | 过采样求平均结果 |
| `ADC_TGSR` | `0x4001_2448` | `TRIG_EN[19:16]` 段触发使能；`TRIG_SEL0[3:0]` 段 0 触发源（`1000`=ADC_SOC 软件触发，复位 `0x000F_8888`） |
| `ADC_TGNR` | `0x4001_244C` | `TGN0[4:0]` 段 0 序列数（1~16，复位 `0x0404_0404`） |
| `ADC_SEQR0` | `0x4001_2450` | `CHSEQ0[4:0]` 序列 0 通道（复位 `0x0302_0100`） |
| `ADC_SEQR1` | `0x4001_2454` | CHSEQ4~7（复位 `0x0706_0504`） |
| `ADC_SEQR2` | `0x4001_2458` | CHSEQ8~11 |
| `ADC_SEQR3` | `0x4001_245C` | CHSEQ12~15 |
| `ADC_CR` | `0x4001_2460` | ⚠️ **新版 UM(260720) 位域**: `VREFH[21:20]`(00=1.5V/01=2V/10=2.5V/11=3V)，`VREFH_BYPS[17]`，`VREFH_EN[16]`，`AVG_SEQ[15:13]`，`AVG_MOD[11:9]`(000=不求平均)，`CLK[5:4]`(00=250K/01=500K/10=1M/11=2M)，`DME_EN[2]`，`CALIB_EN[1]`，`ADC_EN[0]`（复位 `0x0060_0060`） |
| `ADC_OTR` | `0x4001_2464` | `SOC[15:0]` 写 `0x5AA5` 产生一次 ADC_SOC **软件触发**。⚠️ SFR 文件将 OTR 标为 "Over Temperature Register" 是错误占位描述 |
| `ADC_TCFG` | `0x4001_2468` | 时序配置（INITIAL/START/SEQUENCE/SAMPLE/INTERVAL_PULSE） |
| `ADC_AWTH` / `ADC_AWE` | `0x4001_246C` / `0x4001_2470` | 模拟看门狗上下阈值 / 序列使能 |
| `ADC_IR` | `0x4001_2474` | `RDY[31]` 就绪；`S0CDIF[0]` 段 0 转换完成标志 |
| `ADC_IE` | `0x4001_2478` | 各中断使能 |
| `ADC_CALIB` | `0x4001_2480` | `GAIN[27:16]`、`OFFSET[11:0]`（复位 `0x0800_0000`） |
| `ADC_CALIBDAT` | `0x4001_2484` | `CAL_DATA[23:0]` 校准结果 |
| `ADC_ORIDAT` | `0x4001_2488` | `ORIG_DAT[11:0]` 当前转换原始数据 |

**ADC 通道映射（`CHSEQx` 编码，UM 表 19-2）**：

| CHSEQ 值 | 输入信号 | CHSEQ 值 | 输入信号 |
|---|---|---|---|
| 0b00000 (0) | CSA_VSEN | 0b10000 (16) | PA0 |
| 0b00011 (3) | CSA_VLPF | 0b10001 (17) | PA1 |
| 0b00100 (4) | **PB12（对应 GPIO28）** | 0b10010 (18) | PA2 |
| 0b00101 (5) | PB13 | 0b10111 (23) | **PA7（本文测试通道 ADC_CH23）** |
| 0b01101 (13) | VDD33_VSEN | 0b11110 (30) | PA14 |
| 0b01110 (14) | TJ_REF | — | — |
| 0b01111 (15) | VREF_1P20 | — | — |

### 4.4 关键位域定义（TGSR / TGNR / SEQR / CR / TCFG / IR / ANAC_ENCR）

**TGSR（0x40012448）**
| 位域 | 名称 | 说明 | 本配置 |
|------|------|------|--------|
| [3:0] | TRIG_SEL0 | 组 0 触发源 | **8 = 软件触发** |
| [7:4] | TRIG_SEL1 | 组 1 触发源 | 8（软件） |
| [11:8] | TRIG_SEL2 | 组 2 触发源 | 8（软件） |
| [15:12] | TRIG_SEL3 | 组 3 触发源 | 8（软件） |
| [19:16] | TRIG_EN | 各组触发使能 | **0x1 = 仅组 0**（SDK 中 `SampleGrpMode_1`） |

**TGNR（0x4001244C）**：`[7:0] TGN0`、`[15:8] TGN1`、`[23:16] TGN2`、`[31:24] TGN3`（各组通道数 1~16；各组之和 > 16 会置 `IR.TGNERRIF` bit16）

**SEQR0~3（0x40012450~0x5C）**：每 8 bit 一个序列通道选择（每 5 bit 有效）：
`SEQR0` 覆盖 SEQ0~3，`SEQR1` 覆盖 SEQ4~7，`SEQR2` 覆盖 SEQ8~11，`SEQR3` 覆盖 SEQ12~15

**CR（0x40012460，复位 0x00600060）** — ⚠️ **位域以新版手册 ODS32M501xUM 260720 为准**（相对旧版整体右移 1 位）
| 位 | 名称 | 复位 | 说明 |
|----|------|------|------|
| [21:20] | VREFH | 11 | 参考量程（00=1.5V/01=2V/10=2.5V/11=3V，默认 3V 无需改） |
| [17] | VREFH_BYPS | 0 | 参考旁路 |
| [16] | VREFH_EN | 0 | 参考电压使能（新版位置；**实测 CR=0x00310031 置位此位**） |
| [15:13] | AVG_SEQ | 0 | 平均序列数（不使用） |
| [11:9] | AVG_MOD | 0 | 平均模式（不使用） |
| [5:4] | CLK | 11 | 00=250K / 01=500K / 10=1M / 11=2M sps |
| [2] | DME_EN | 0 | DMA 请求（本方案不用） |
| [1] | CALIB_EN | 0 | 校准使能（可选项） |
| **[0]** | **ADC_EN** | **0** | **ADC 使能，必须置 1** |

> **新旧版差异说明（重要）**：`ODS32M501xUM 260720.docx` 对 `CR` 位域做了修正——`VREFH`、`VREFH_BYPS`、`VREFH_EN`、`AVG_SEQ`、`CLK` 相对旧版 `ODS32M501xUM.docx` **整体右移 1 位**。
> 实测值 `0x00310031` 在新版位域下 = `ADC_EN[0]` + `CLK[5:4]=11(2M)` + `VREFH_EN[16]` + `VREFH[21:20]=11(3V)`，**完全正确**；
> 而旧版推导的 `0x00620061` 把 bit17 当作 VREFH_EN（新版该位是 VREFH_BYPS）、bit16=0（新版 VREFH_EN 未置位）→ 参考未使能，故实测不生效。
> `TCFG`、`TGSR`、`TGNR`、`SEQR0~3`、`IR`、`OTR`、`CALIB` 位域两版一致。

**ADC_CR 各位配置分析（DNL 测试场景，新版位域）**

| 位域 | 名称 | 复位 | 推荐值 | 实测 0x00310031 | 工程意义 |
|------|------|------|--------|----------------|----------|
| [21:20] | VREFH | 2'b11 | 11（3V） | 11（3V） | ADC 量程，决定可测输入范围与 LSB 大小（3V 档 LSB≈0.732mV）。DNL 测试用 3V 可覆盖最大输入跨度 |
| [17] | VREFH_BYPS | 0 | 0 | 0 | 0=量程按内部 VREFH。⚠️ 若误写 1，量程变 VDD33 且 VREFH 通路被旁路（旧版把此位当 VREFH_EN 的坑） |
| **[16]** | **VREFH_EN** | 0 | **1** | **1** | VREFH 参考使能，**必须置 1**，否则 ADC 无参考、结果无效。⚠️ 旧版误标在 bit17 |
| [15:13] | AVG_SEQ | 0 | 0 | 0 | 平均序列选择。DNL 测试置 0（配合 AVG_MOD=0 不平均） |
| [11:9] | AVG_MOD | 0 | 000 | 0 | 000=不求平均/001=2次/…/111=128次。⚠️ **DNL 测试必须关闭平均**，求平均会平滑非线性、掩盖真实 DNL |
| [5:4] | CLK | 2'b11 | 11（2M） | 11（2M） | 00=250K/01=500K/10=1M/11=2M。DNL 测试用 2M 提升效率；若输入源噪声大可降为 500K~1M |
| [2] | DME_EN | 0 | 0 | 0 | DMA 请求使能。本方案 SWD 直读寄存器，不用 DMA |
| [1] | CALIB_EN | 0 | 0 | 0 | 校准使能（GAIN/OFFSET）。⚠️ **DNL 测试建议关闭**，读原始数据；开启后 DATA 为校准结果会掩盖非线性 |
| [0] | **ADC_EN** | 0 | **1** | **1** | ADC 使能，**必须置 1** |

**实测值 `0x00310031` 位级展开**（`0000 0000 0011 0001 0000 0000 0011 0001b`）：

```
ADC_EN=1  CALIB_EN=0  DME_EN=0  CLK[5:4]=11(2M)  AVG_MOD[11:9]=000
AVG_SEQ[15:13]=0  VREFH_EN[16]=1  VREFH_BYPS[17]=0  VREFH[21:20]=11(3V)
```

结论：`0x00310031` = 单次转换（无平均、无校准）+ 2M 采样 + 3V 量程 + 参考使能，**正是 DNL 测试的理想配置**；其余保留位全部为 0。

**TCFG（0x40012468，复位 0x0A4020F4）**
| 位域 | ADC_Reg.h 字段 | 手册字段 | 复位 | 说明 |
|------|----------------|----------|------|------|
| [3:0] | STPT | START_PULSE | 0x4 | start 高电平时间 |
| [7:4] | COVT | SAMPLE_PULSE | 0xF | 采样/通道切换时间 |
| [15:8] | SAPT | INTERVAL_PULSE | 0x20 | 转换完成后读取等待时间 |
| [23:16] | INIT | INITIAL_PULSE | 0x40 | **ADC_EN 到 ADC_RDY 就绪时间（不能写 0）** |
| [31:24] | TDELY | SEQUENCE_PULSE | 0x0A | 每组首次采样前延时 |

**IR（0x40012474，全位只读）**：`[31] RDY` 就绪；`[20] AWDIF` 看门狗；`[17] TGTERRIF` 触发错误；`[16] TGNERRIF` 通道数错误；`[9] CALUPIF` 校准完成；`[4] SACDIF` 任意组完成；`[3:0] S3CDIF~S0CDIF`（**bit0 = S0CDIF 即组 0 转换完成**）

**ANAC.ENCR（0x40007800，可选）**：`[0] ANA_EN` 总使能；`[17] ANA_CSA_EN`；`[18] ANA_VREFH_EN`；`[23] ANA_BG_EN`；`[24] ANA_REF_EN`；`[25] ANA_LDO12_EN`

### 4.5 ANAC 模拟 ADC 控制寄存器（`register.xlsx` 内的工程对照表）

| 寄存器 | 地址 | 位域 | 说明 |
|---|---|---|---|
| `RW_ANA_EN` | `0x40007C00` | `[19] RW_ANA_ADC_EN` | 模拟 ADC 使能 |
| `RW_ANA_EN` | `0x40007C00` | `[18] RW_ANA_VREFH_EN` | 模拟 VREFH 使能 |
| `RW_ADC_START` | `0x40007C18` | `[0]` | ADC 启动：0→1 上升沿触发采样 |
| `RW_ADC_MODE` | `0x40007C18` | `[1]` | 0=单次，1=连续 |
| `RW_ADC_MUX_EN` | `0x40007C18` | `[2]` | 多路选择器使能 |
| `RW_ADC_MUX` | `0x40007C18` | `[7:3]` | 通道选择（同 §4.3 映射） |
| `RW_ADC_CLK` | `0x40007C18` | `[9:8]` | 00=250K/01=500K/10=1M/11=2M |
| `RW_ADC_VALID` | `0x40007C18` | `[10]` | ADC latch 有效信号使能 |
| `RW_ADC_IBIAS` | `0x40007C18` | `[11]` | 降低 ADC 偏置电流（省电） |
| `RW_ADC_BUF_TP` | `0x40007C18` | `[12]` | ADC Buffer 测试点 |
| `RW_ADC_VREFH` | `0x40007C18` | `[14:13]` | 量程 00=1.5V/01=2V/10=2.5V/11=3V（默认 3V） |
| `RW_ADC_VREFH_BYPS` | `0x40007C18` | `[15]` | 1=量程=VDD33 |
| `RW_ADC_VREFH_ATP` | `0x40007C18` | `[16]` | VREF 测试点使能（输出到 ATP） |
| `RW_ATP_MUX` | `0x40007C1C` | `[4:0]` | ATP 模拟测试点选择（`00110`=ADC_VREFH_OUT，`00111`=ADC_BUF_TP） |
| `RW_ATP_MUX_EN` | `0x40007C1C` | `[5]` | ATP MUX 使能（1=输出到 GPIO5） |
| `RW_ATP_TG_EN` | `0x40007C1C` | `[6]` | ATP 触发/保持 |
| `RW_ATP_BUF_EN` | `0x40007C1C` | `[7]` | ATP 缓冲器使能 |
| `PA_ANA_EN` | `0x48000050` | `[0:15]` | PA 口模拟使能（测试时先写 0x20） |
| `PA5_AFS14_EN` | `0x4800003C` | `[20:23]` | PA5 复用模拟功能使能 |

### 4.6 ADC 性能指标（`dohko2_Test_Plan_Guide.docx` 表 40）

| 参数 | Min | Typ | Max | Unit |
|---|---|---|---|---|
| ENOB | 10 | 11 | 12 | bits |
| input range | 5% VREFH | — | 95% VREFH | — |
| sampling rate | 250K | 1M | 2M | Hz |
| gain error (w/i VREFH) | — | 0.7% | — | — |
| offset error | — | 14 | — | mV |

### 4.7 dohko2 ADC 测试方案（表 42，DNL/INL 相关摘录）

> **ADC_DATA<11:0> 细测步骤 3（DNL/INL 参考方法）**：
> "以尽可能小的步长给 GPIO28 递增的电压，实时记录 ADC DATA，
> 得到一条输入 vs 输出的曲线，**根据斜率最大处得到 ADC 的 DNL**，
> 根据曲线偏离拟合直线最大处得到 INL；记录 ADC 实际增益，
> 对比 VREFH 误差造成的增益误差；记录 ADC 实际失调。"
>
> 初测路径配置：`RW_ADC_EN=1`、`RW_ANA_VREFH_EN=1`、`RW_ADC_BUF_TP=1`、
> `RW_ADC_MUX<4:0>=00100`（GPIO28）、`RW_ATP_MUX<4:0>=00111`、`RW_ATP_TG_EN=1`；
> 连续模式：`RW_ADC_MODE=1`，触发 `D_ADC_START 0→1→0`。
>
> 注：dohko2 原文以 GPIO28（`RW_ADC_MUX<4:0>=00100`）为例；本文实际测试通道采用 **ADC_CH23(PA7)（SEQ0=0x17）**，通路配置原理相同。

### 4.8 串口指令集（继电器通路 / 外部 DAC 电压控制）

串口由 CH343(COM14) 承载（`SERIAL_BAUDRATE=115200`），测试板固件解析以下指令：

| 指令 | 格式 | 说明 |
|------|------|------|
| **打开通路** | `cmd_gpio_set_595d_test set <通路号1> <通路号2> ...` | 同时打开多条通路，**每个参数为一个独立通路编号**：38=万用表测量、7=DAC→母线、\<bench_pin_bit\>=母线→ADC 通道。例 PA7→`set 38 7 11` |
| **复位通路** | `cmd_gpio_set_595d_test reset` | 复位所有继电器通路（测试开始/结束调用） |
| **设 DAC 电压** | `spi_cmd_set_val mV <设置电压mV>` | 设置 DAC 输出电压。**DAC1(编号6)输出=设置值×1；DAC2(编号7)输出=设置值×2**，如 `spi_cmd_set_val mV 1000` → DAC2 输出 2000mV |
| 设 DAC 原始值 | `spi_cmd_set_val Ori <0~65535>` | 设置 DAC 原始码（`other_test/dac_test` 使用） |
| GPIO 切换 | `cmd_gpio_toggle <port> <pin>` | GPIO 翻转（`GPIO_toggle_test` 使用） |

常用通道编号（`instrument/equip_configs.json`）：

| 编号 | 含义 | 编号 | 含义 |
|------|------|------|------|
| 38 | 万用表测量通路（测母线电压） | 6 | DAC1_CMD（输出=设置值×1） |
| 7 | DAC2→母线通路（输出=设置值×2） | 39 | GND |
| 32~35 | SCOPE_CH1~4 | 36 / 37 | PWM_CH1 / PWM_CH2 |
| 27 | 母线→通道通路(例 GPIO11/PA11) | 28 | 母线→通道通路(例 GPIO37/PC5) |

> ⚠️ `set` 的每个参数为一个**独立通路编号**（非"通道-值"对），可一次携带多个通路号同时打开。
> **通路拓扑**：`DAC →(7)→ 母线(bus) →(bench_pin_bit)→ ADC 通道`，万用表经 `38` 测母线电压；
> 例 `set 38 7 27 28` = 万用表(38) + DAC→母线(7) + 母线→通道(27) + 母线→通道(28)。
> 测试流程惯例：开始时 `reset` → 逐项 `set` 打开所需通路 → 测试 → `reset` 复位。

**ADC 采样通道 ↔ bench_pin_bit 映射**（来源：`ADC通道_socket_bench.xlsx`，bench_pin_bit 序号用于 `set` 开启 DAC 通路）：

| GPIO | Pin | ADC通道(CHSEQ) | socket序号 | bench_pin序号 | **bench_pin_bit序号** |
|------|-----|---------------|-----------|--------------|----------------------|
| GPIO2 | PA2 | ADC_IN18 | 2 | PIN21 | 5 |
| GPIO5 | PA5 | ADC_IN21 | 5 | PIN28 | 15 |
| GPIO6 | PA6 | ADC_IN22 | 6 | PIN30 | 31 |
| **GPIO7** | **PA7** | **ADC_IN23** | 7 | PIN14 | **11（当前测试通道）** |
| GPIO8 | PA8 | ADC_IN24 | 8 | PIN17 | 4 |
| GPIO9 | PA9 | ADC_IN25 | 9 | PIN19 | 20 |
| GPIO10 | PA10 | ADC_IN26 | 10 | PIN13 | 3 |
| GPIO11 | PA11 | ADC_IN27 | 11 | PIN16 | 27 |
| GPIO12 | PA12 | ADC_IN28 | 12 | PIN18 | 12 |
| GPIO13 | PA13 | ADC_IN29 | 13 | PIN12 | 26 |
| GPIO14 | PA14 | ADC_IN30 | 14 | PIN15 | 19 |
| GPIO30 | PB14 | ADC_IN6 | 30 | PIN22 | 13 |
| GPIO35 | PC3 | ADC_IN7 | 35 | PIN24 | 29 |
| GPIO36 | PC4 | ADC_IN8 | 36 | PIN26 | 22 |
| GPIO37 | PC5 | ADC_IN9 | 37 | PIN20 | 28 |
| GPIO38 | PC6 | ADC_IN10 | 38 | PIN23 | 21 |

> 说明：ADC 通道号 = `ADC_INxx` 的编号（如 ADC_IN23 → CHSEQ=23，写 `SEQR0`）。`GPIO28~30(PB12~14)`、`GPIO39~40(PC7~8)` 未在表中列出 bench_pin_bit（无对应继电器通路）。

---

## 5. OCD5010 `ADC_DNL_Test` 实现设计

### 5.1 测试方案（移植 2810 流程 + OCD5010 寄存器）

```
测试项名      : "ADC_DNL"
被测信号      : ADC 序列0 = ADC_CH23（PA7）
参考电压      : CR=0x00310031（实测有效值, ADC_EN 置1; 参考电压通路以实测为准）
输入电压源    : 外部 DAC2→母线（**输出=设置值×2**，串口指令见 §4.8: set 38 7 <bench_pin_bit> 开通路 + spi_cmd_set_val mV 设电压）
实测手段      : 外部万用表经 `set 38` 通路实测母线电压（DAC 输出经母线接 ADC 输入）
ADC 时钟      : RCC.ANACKE bit0 ADCCKE=1（必须）
ADC 触发      : 单次转换，ADC_OTR 写 0x5AA5 → 轮询 IR.S0CDIF
ADC 结果      : 读 DATA[0] @ 0x40012404 低 12 位
判定规格      : 参考 2810 的 Min=-2 / Max=2 LSB（test.xlsx 需新增）
```

### 5.2 配置值计算（对齐工程，单通道最小可验证）

| 寄存器 | 地址 | 计算过程 | 写入值 |
|--------|------|----------|--------|
| `RCC.ANACKE` | 0x4002005C | bit0 `ADCCKE`=1（使能 ADC 时钟） | 0x00000001 |
| `TGSR` | 0x40012448 | TRIG_SEL0=8（软件）+ TRIG_EN[19:16]=0x1 | 0x00010008 |
| `TGNR` | 0x4001244C | TGN0[7:0]=1（1 个通道） | 0x00000001 |
| `SEQR0` | 0x40012450 | SEQ0 = ADC_CH23(PA7) = 23 | 0x00000017 |
| `TCFG` | 0x40012468 | 复位默认时序（INIT=0x40 不可写 0） | 0x0A4020F4 |
| `CR` | 0x40012460 | **新版UM(260720)位域正确值**: ADC_EN[0] + CLK[5:4]=11(2M) + VREFH_EN[16] + VREFH[21:20]=11(3V)；旁路VDD33时加 VREFH_BYPS[17]=1 → 0x00330031 | 0x00310031 |
| `ANAC.ENCR`(可选) | 0x40007800 | ANA_EN\|VREFH_EN\|BG_EN\|REF_EN\|LDO12_EN（按需） | 0x03840001 |

> **单通道流程**：`RCC.ANACKE=1` → `TGSR/TGNR/SEQR0/TCFG` → `CR` → `OTR=0x5AA5` → 轮询 `IR.S0CDIF` → 读 `DATA[0]`。
> 全部寄存器均可由 SWD 在 CPU halt 下直接写，无需 CPU 运行。
>
> 16 通道参考（`adc_demo/main.c`）：TGNR=0x00000010，SEQR0=0x17161512，SEQR1=0x1B1A1918，
> SEQR2=0x061E1D1C，SEQR3=0x0A090807，TCFG=0x0A4020FA，CR=0x01720061。
>
> 注：`DATA[0]` 等数字模块寄存器不在 `register.xlsx` 中，建议代码中**硬编码地址常量**并加注释；
> 或补入 register.xlsx 后用 `get_32bit_reg_address()` 获取（推荐，便于维护）。

### 5.3 交互式选择：参考电压量程 + ADC 采样通道

在采样开始前，允许交互选择 `VREFH` 量程与 `ADC` 通道（当前代码固定 3V / ADC_CH23，后续集成此交互）。

**① 参考电压量程选择（`CR[21:20] VREFH`）**

| 选项 | 量程 | CR 写入值 | LSB(≈) | 有效输入范围(5%~95%) |
|------|------|-----------|--------|----------------------|
| 1 | 1.5V | `0x00010031` | 0.366 mV | 0.075~1.425 V |
| 2 | 2.0V | `0x00110031` | 0.488 mV | 0.100~1.900 V |
| 3 | 2.5V | `0x00210031` | 0.611 mV | 0.125~2.375 V |
| 4 | 3.0V(默认) | `0x00310031` | 0.732 mV | 0.150~2.850 V |
| 5 | **旁路VDD33(≈3.3V)** | `0x00330031` | 0.806 mV | 0.165~3.135 V |

> 推导：`CR = 0x00000031`（`ADC_EN[0]` + `CLK[5:4]=11` 2M）` | (vrefh_code<<20) | 0x00010000`（`VREFH_EN[16]`）。
> **选项 5（旁路 VDD33）**：额外置 `VREFH_BYPS[17]=1`（`0x00020000`）→ 量程=VDD33(≈3.3V)，`VREFH[21:20]` 位被忽略；满量程 `VREFH_mV` 取 3300（实际以万用表实测 VDD33 为准）。
> 量程直接影响 DNL 计算的满量程 `VREFH_mV` 与 LSB（`VREFH_mV/4095`），必须与所选档位一致。

**② ADC 采样通道选择（`SEQR0[4:0] SEQ0`）**

新版手册表 19-2 通道映射（CHSEQ 0~30，与旧版一致）：

| CHSEQ | 通道 | CHSEQ | 通道 | CHSEQ | 通道 |
|---|---|---|---|---|---|
| 0~2 | CSA_VSEN | 11 | PC7 | 22 | PA6 |
| 3 | CSA_VLPF | 12 | PC8 | **23** | **PA7（默认）** |
| 4 | PB12 | 13 | VDD33_VSEN | 24 | PA8 |
| 5 | PB13 | 14 | TJ_REF | 25 | PA9 |
| 6 | PB14 | 15 | VREF_1P20 | 26 | PA10 |
| 7 | PC3 | 16 | PA0 | 27 | PA11 |
| 8 | PC4 | 17 | PA1 | 28 | PA12 |
| 9 | PC5 | 18 | PA2 | 29 | PA13 |
| 10 | PC6 | 19~21 | PA3~PA5 | 30 | PA14 |

**③ 参考代码框架（集成进 `ADC_DNL_Test`）**

```python
# 以下为 ADC_DNL_Test 函数体内的"交互式选择"片段（集成时保持函数缩进）
# ---------- 0. 交互式选择参考量程与采样通道 ----------
VREFH_OPT = {  # 选项: (VREFH码[21:20], 量程mV)
    "1": (0b00, 1500), "2": (0b01, 2000),
    "3": (0b10, 2500), "4": (0b11, 3000),
}
print("\n请选择 ADC 参考电压量程 (VREFH):")
print("  1. 1.5V   2. 2.0V   3. 2.5V   4. 3.0V (默认)")
vrefh_code, VREFH_mV = VREFH_OPT.get(input("请输入选项(1/2/3/4): ").strip(), (0b11, 3000))
print(f"  已选择量程: {VREFH_mV} mV")

try:
    ch = int(input("请输入 ADC 采样通道号 (0~30, 默认23=PA7): ").strip() or "23")
    ch = 0 if ch < 0 else (30 if ch > 30 else ch)
except ValueError:
    ch = 23
ADC_CH = ch
print(f"  已选择通道: ADC_CH{ADC_CH}")

# 按选择写 CR：ADC_EN + CLK=2M + VREFH_EN(bit16) + VREFH码
CR_VAL = 0x00000031 | (vrefh_code << 20) | 0x00010000
instr.swd_write_register(ADC_CR, CR_VAL)
# 按选择写 SEQR0[4:0]
instr.swd_modify_register(ADC_SEQR0, ADC_CH, 0, 4)
```

**④ 对 DNL 测试的影响**
- 量程决定 LSB 与满量程：DNL 计算 `VREFH_mV / 4095` 必须用所选档位实际电压（建议实测 VREFH，见 §5.6）；
- 通道决定输入引脚：电压源需接到所选通道对应引脚，DMM 与输入节点并联实测；
- 内部通道（0~3 CSA、13 VDD33_VSEN、14 TJ_REF、15 VREF_1P20）无需外部输入，可作通路自检。

### 5.4 JLink Commander 上机验证脚本（先手动验证配置，再写自动化）

> 以下脚本可直接在 J-Link Commander（`JLink.exe`）交互模式下逐行执行，用于**上电后先人工验证** ADC 配置与软件触发通路是否打通（CPU 处于 halt/复位状态即可）。命令：`w4 <addr>,<val>` 写 32 位；`mem32 <addr>,<count>` 回读。

```
; ===== 第 1 步：使能 ADC 时钟 =====
w4 0x4002005C, 0x00000001    ; RCC.ANACKE |= ADCCKE(bit0)

; ===== 第 2 步：配置采样序列（单通道 CH23/PA7） =====
w4 0x40012448, 0x00010008    ; TGSR = 软件触发 + 仅组0
w4 0x4001244C, 0x00000001    ; TGNR = 组0 共 1 个通道
w4 0x40012450, 0x00000017    ; SEQR0 = SEQ0 -> ADC_CH23(PA7)
w4 0x40012468, 0x0A4020F4    ; TCFG = 复位默认时序（INIT=0x40 不能为0）

; ===== 第 2 步（可选）：模拟电源域上电 =====
w4 0x40007800, 0x03840001    ; ANAC_ENCR: ANA_EN|VREFH_EN|BG_EN|REF_EN|LDO12_EN

; ===== 第 3 步：使能 ADC + 参考电压 =====
w4 0x40012460, 0x00310031    ; CR = 0x00310031 (实测有效值, ADC_EN 置1)

; ===== 验证配置已生效（回读，正常应为写入值） =====
mem32 0x4002005C, 4          ; 期望 0x00000001 (ADCCKE)
mem32 0x40012448, 4          ; 期望 0x00010008 (TGSR)
mem32 0x4001244C, 4          ; 期望 0x00000001 (TGNR)
mem32 0x40012450, 4          ; 期望 0x00000017 (SEQR0)
mem32 0x40012468, 4          ; 期望 0x0A4020F4 (TCFG)
mem32 0x40012460, 4          ; 期望 0x00310031 (CR 实测值)
mem32 0x40012474, 4          ; IR: bit31(RDY) 应=1

; ===== 第 4 步：软件触发一次 =====
w4 0x40012464, 0x00005AA5    ; OTR = 0x5AA5 -> 产生 ADC_SOC 触发

; ===== 轮询完成标志（手动重复执行，直到 bit0=1） =====
mem32 0x40012474, 4          ; IR.S0CDIF(bit0)=1 表示组0转换完成；bit31 RDY=1

; ===== 读取转换结果 =====
mem32 0x40012404, 4          ; DATA[0] 低12位 = 采样值（0~4095）
```

**验证要点**：
1. 各寄存器回读值与写入值一致（若某位回读为 0，说明写保护/时钟未使能）；
2. 触发后 `IR` 的 bit31(RDY)=1、bit0(S0CDIF)=1，`DATA[0]` 低 12 位随 ADC_CH23(PA7) 输入电压变化；
3. 手动验证通过后，再将该流程翻译为 `instrument.instrument_set.swd_write_register / swd_modify_register` 调用（见 §5.5 参考代码）。

### 5.5 函数框架（参考代码，写入本文档供实现人员使用）

```python
@test_case
def ADC_DNL_Test():
    """ADC DNL 测试（参考 2810 实现，基于 ODS32M501xUM 第19章 ADC）"""
    adc_test_item_name = "ADC_DNL"
    row = df_test_data[df_test_data["测试项"] == adc_test_item_name]
    if row.empty:
        print(f"未找到测试项: {adc_test_item_name}")
        return
    try:
        adc_min_val = float(row["Min."].values[0])
        adc_max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return

    # ---------- 寄存器地址（已与 SFR / ADC_Reg.h / ODS_5010.h 三方核对） ----------
    RCC_ANACKE  = 0x4002005C   # 模拟时钟使能: bit0 ADCCKE=1（必须）
    ANAC_ENCR   = 0x40007800   # 模拟电源/基准使能(可选, 参考 pga_demo)
    ADC_CR      = 0x40012460   # 控制(新版UM 260720): [0]ADC_EN [16]VREFH_EN [21:20]VREFH [5:4]CLK
    ADC_OTR     = 0x40012464   # 软件触发: 写 0x5AA5 (SFR 标 OTR 为温度寄存器是错误占位)
    ADC_TGSR    = 0x40012448   # 触发源: [19:16]TRIG_EN [3:0]TRIG_SEL0(=8 软件)
    ADC_TGNR    = 0x4001244C   # 组0通道数: [7:0]TGN0
    ADC_SEQR0   = 0x40012450   # 通道选择: [4:0]SEQ0
    ADC_TCFG    = 0x40012468   # 时序: [23:16]INIT(不可为0) [15:8]SAPT [7:4]COVT [3:0]STPT
    ADC_IR      = 0x40012474   # 状态(只读): [31]RDY [0]S0CDIF
    ADC_DATA0   = 0x40012404   # 序列0转换结果: [11:0]

    # CHSEQ 通道名映射（新版 UM 260720 表 19-2）
    CH_NAME = {0: "CSA_VSEN", 1: "CSA_VSEN", 2: "CSA_VSEN", 3: "CSA_VLPF",
               4: "PB12", 5: "PB13", 6: "PB14", 7: "PC3", 8: "PC4", 9: "PC5",
               10: "PC6", 11: "PC7", 12: "PC8", 13: "VDD33_VSEN", 14: "TJ_REF",
               15: "VREF_1P20", 16: "PA0", 17: "PA1", 18: "PA2", 19: "PA3",
               20: "PA4", 21: "PA5", 22: "PA6", 23: "PA7", 24: "PA8", 25: "PA9",
               26: "PA10", 27: "PA11", 28: "PA12", 29: "PA13", 30: "PA14"}

    # ADC 通道(CHSEQ) → bench_pin_bit 序号（来源 ADC通道_socket_bench.xlsx, 见 §4.8）
    BENCH_PIN_BIT = {18: 5, 21: 15, 22: 31, 23: 11, 24: 4, 25: 20, 26: 3,
                     27: 27, 28: 12, 29: 26, 30: 19, 6: 13, 7: 29, 8: 22,
                     9: 28, 10: 21}

    # ---------- 0. 交互式选择参考量程与采样通道（见 §5.3） ----------
    VREFH_OPT = {"1": (0b00, 1500, 0), "2": (0b01, 2000, 0), "3": (0b10, 2500, 0),
                 "4": (0b11, 3000, 0), "5": (0b11, 3300, 1)}   # 5=旁路VDD33(≈3.3V), VREFH_BYPS[17]=1
    print("\n请选择 ADC 参考电压量程 (VREFH): 1=1.5V 2=2.0V 3=2.5V 4=3.0V 5=旁路VDD33")
    vrefh_code, VREFH_mV, vrefh_byps = VREFH_OPT.get(input("请输入选项(1/2/3/4/5): ").strip(), (0b11, 3000, 0))
    try:
        ch = int(input("请输入 ADC 采样通道号 (0~30, 默认23=PA7): ").strip() or "23")
        ch = 0 if ch < 0 else (30 if ch > 30 else ch)
    except ValueError:
        ch = 23
    ADC_CH, ch_name = ch, CH_NAME.get(ch, "ADC_CH%02d" % ch)
    print(f"  量程: {VREFH_mV} mV, 通道: ADC_CH{ADC_CH} ({ch_name})")

    # ---------- 1. 初始化 ADC 通路（4 步，CPU halt 下 SWD 可完成） ----------
    # ① 使能 ADC 时钟
    instr.swd_modify_register(RCC_ANACKE, 0x01, 0, 0)            # ADCCKE=1
    # ② 配置采样序列（单通道: SEQ0 = 所选通道, 软件触发, 复位默认时序）
    instr.swd_modify_register(ADC_SEQR0, ADC_CH, 0, 4)           # SEQ0 = 所选通道
    instr.swd_modify_register(ADC_TGNR,   1, 0, 7)               # TGN0 = 1（组0通道数）
    instr.swd_modify_register(ADC_TGSR,   0b0001, 16, 19)        # TRIG_EN = 仅组0
    instr.swd_modify_register(ADC_TGSR,   0b1000,  0,  3)        # TRIG_SEL0 = 8 (软件)
    instr.swd_write_register(ADC_TCFG,    0x0A4020F4)            # 复位默认时序(INIT≠0)
    # ③ 按所选量程写 CR：ADC_EN + CLK=2M + VREFH_EN(bit16) + VREFH码 + (旁路时 VREFH_BYPS[17]=1)
    CR_VAL = 0x00000031 | (vrefh_code << 20) | 0x00010000
    if vrefh_byps:
        CR_VAL |= 0x00020000                    # 旁路VDD33: 量程=VDD33(≈3.3V)
    instr.swd_write_register(ADC_CR, CR_VAL)
    # (可选) 模拟模块电源/基准使能, 参考 pga_demo
    # instr.swd_write_register(ANAC_ENCR, 0x03840001)
    time.sleep(0.1)

    # ---------- 2. 接线确认 + 打开外部 DAC 通路 ----------
    print(f"⚠️ 请确认 ADC_CH{ADC_CH} ({ch_name}) 输入与外部 DAC 输出已连接。")
    input()
    dac_bit = BENCH_PIN_BIT.get(ADC_CH)                        # bench_pin_bit 映射(见 §4.8), PA7→11
    if dac_bit is not None:
        instr.serial_send(f"cmd_gpio_set_595d_test set 38 7 {dac_bit}\r\n")  # 万用表+DAC→母线+母线→通道
    else:
        instr.serial_send("cmd_gpio_set_595d_test set 38 7\r\n")  # 仅万用表+DAC→母线
        print(f"⚠️ ADC_CH{ADC_CH}({ch_name}) 无 bench_pin_bit 映射，未开启母线→通道通路。")
    time.sleep(0.2)

    # ---------- 3. 电压模式选择(与2810一致) ----------
    # DAC2 增益: 输出=设置值×2; 目标输入电压上限 = 量程 95%
    DAC_GAIN = 2
    vin_max = int(VREFH_mV * 0.95)
    print("\n请选择 ADC DNL 测试电压模式:")
    print("1. 固定测试电压")
    print("2. 随机测试电压 (10组循环)")
    print("3. 线性扫描测试电压")
    choice = input("请输入选项 (1/2/3): ").strip()
    random.seed(42)

    # 总采样点数：每个电压采样一次，连续采样 N 次（无"点"概念）
    N = max(1, min(int(input("请输入总采样点数 (默认 100): ").strip() or "100"), 5000))

    if choice == '1':
        # 固定测试电压：固定电压值，采样 N 次
        fix_mv = int(input("请输入固定测试电压 (mV, 默认1000): ").strip() or "1000")
        fix_mv = max(0, min(fix_mv, vin_max))
        vin_seq = [fix_mv] * N
    elif choice == '2':
        # 随机测试电压：10 组随机电压循环，每个电压采样一次，连续到 N 次
        rnd_list = [int(round(random.uniform(vin_max * 0.05, vin_max), -1)) for _ in range(10)]
        vin_seq = [rnd_list[i % 10] for i in range(N)]
    else:
        # 线性扫描：起始/终点/步进生成电压序列，循环采样到 N 次
        v_start = int(input("请输入起始电压 (mV, 默认150): ").strip() or "150")
        v_stop  = int(input(f"请输入终点电压 (mV, 默认{vin_max}): ").strip() or str(vin_max))
        step    = int(input("请输入步进值 (mV, 默认100): ").strip() or "100")
        v_start = max(0, min(v_start, vin_max))
        v_stop  = max(0, min(v_stop, vin_max))
        if v_stop < v_start:
            v_start, v_stop = v_stop, v_start
        sweep = list(range(v_start, v_stop + 1, max(1, step)))
        vin_seq = [sweep[i % len(sweep)] for i in range(N)]
    print(f"  (量程 {VREFH_mV/1000.0:.1f}V, 目标输入上限 {vin_max} mV; DAC2 实际输出 = 设置值×2; 总采样 {N} 次)")

    # ---------- 4. 连续采样（设 DAC → 万用表实测 → ADC 采样，每个电压采样一次） ----------
    # 实测间隔：万用表每 N 点实测一次（0=不实测只换算[快速]，1=每点，默认0）
    # ⚠️ DMM 实测(VISA通信)每次可达数百ms，为主要耗时项，默认关闭以保采样速度
    dmm_interval = max(0, int(input("请输入万用表实测间隔 (0=不实测[快速], 1=每点, 默认0): ").strip() or "0"))
    last_vol = None
    save_data_list = []
    dnl_lsb_list   = []
    for i, vin in enumerate(vin_seq):
        # 设置 DAC2：目标输入电压/2（2 倍增益），上限 VREFH×0.95/2
        dac_mv = max(0, min(vin // DAC_GAIN, int(VREFH_mV * 0.95 / DAC_GAIN)))
        instr.serial_send(f"spi_cmd_set_val mV {dac_mv}\r\n")   # 只发送不等待响应(避免串口读阻塞)
        time.sleep(0.005)                                # DAC 输出/母线建立

        # 万用表实测母线电压 (mV)（经通路 38；按间隔，间隔>1 时沿用最近实测值）
        if dmm_interval > 0 and (i % dmm_interval == 0):
            last_vol = instr.dmm_measure_single_voltage()
        vol = last_vol

        # ④ 软件触发 → 轮询转换完成 → 读结果（先等转换完成再轮询, 减少 J-Link 往返）
        instr.swd_write_register(ADC_OTR, 0x5AA5)                # 写 0x5AA5 触发一次 ADC_SOC
        time.sleep(0.001)
        for _ in range(20):                                     # 轮询 IR.S0CDIF (bit0), 超时保护
            if instr.swd_read_register(ADC_IR) & 0x1:
                break
            time.sleep(0.0005)
        get_adc_val = instr.swd_read_register(ADC_DATA0) & 0xFFF

        # 计算
        adc_lsb = VREFH_mV / 4095.0
        vreal   = get_adc_val * adc_lsb                 # ADC 转换电压 mV
        dnl_lsb = (vol - vreal) / adc_lsb               # DNL 偏差(LSB)
        dnl_lsb_list.append(dnl_lsb)
        save_data_list.append({
            "输入电压(mV)":       vin,
            "万用表实测电压(mV)": round(vol, 3),
            "ADC原始数据":        get_adc_val,
            "ADC转换电压(mV)":    round(vreal, 3),
            "DNL误差(mV)":        round(vol - vreal, 3),
            "DNL误差(LSB)":       round(dnl_lsb, 3),
        })
        print(f"  [{i+1:>4}/{len(vin_seq)}] adc={get_adc_val:4d} 转换={vreal:7.2f}mV 实测={vol:7.2f}mV DNL={dnl_lsb:.3f}LSB")

    # ---------- 5. DNL / INL 计算与判定（转移曲线法） ----------
    # 按 adc_code 排序、相邻 code 步距归一化；逐点 DNL 打印并保存，INL 只保存不打印
    lsb_mv = VREFH_mV / 4096.0
    pts = sorted((r["万用表实测电压(mV)"] if r["万用表实测电压(mV)"] else float(r["输入电压(mV)"]),
                  r["ADC原始数据"]) for r in save_data_list)
    dnl_points, inl_points, dnl_by_code, inl_by_code, acc = [], [], {}, {}, 0.0
    print("\n[DNL 逐点结果]")
    for i in range(len(pts) - 1):
        v1, c1 = pts[i]
        v2, c2 = pts[i + 1]
        dc = c2 - c1
        if dc <= 0:
            continue
        dnl = ((v2 - v1) / lsb_mv) / dc - 1.0
        acc += dnl
        dnl_points.append((c1, c2, dnl))
        inl_points.append((c2, acc))
        dnl_by_code[c1] = dnl
        inl_by_code[c2] = acc
        print(f"  code {c1}→{c2}: DNL={dnl:+.3f} LSB")
    adc_dnl_max = max(dnl_points, key=lambda x: abs(x[2]))[2] if dnl_points else 0.0
    adc_inl_max = max(inl_points, key=lambda x: abs(x[1]))[1] if inl_points else 0.0
    print(f"  DNL max : {adc_dnl_max:+.3f} LSB")           # INL 只保存不打印
    adc_passed  = (adc_min_val <= adc_dnl_max <= adc_max_val)
    save_item_result(adc_test_item_name, adc_passed, round(adc_dnl_max, 3))
    print("✅ ADC_DNL 测试通过!" if adc_passed else "❌ ADC_DNL 测试失败! 超出规格范围!")

    # ---------- 6. 可选保存 xlsx 到桌面 ----------
    save_choice = input("\n是否保存测量数据为 xlsx 文件? (y/n, 默认n): ").strip().lower()
    if save_choice in ('y', 'yes'):
        save_filename = input("请输入文件名 (默认 ADC_DNL_测量数据): ").strip() or "ADC_DNL_测量数据"
        if not save_filename.endswith(".xlsx"):
            save_filename += ".xlsx"
        save_path = os.path.join(os.path.expanduser("~"), "Desktop", save_filename)
        try:
            wb = openpyxl.Workbook()
            ws = wb.active
            ws.title = "ADC_DNL测量数据"
            headers = ["计数", "目标电压(mV)", "DAC2设置(mV)", "adc_code", "转换电压(mV)", "实测电压(mV)", "DNL(LSB)", "INL(LSB)"]
            ws.append(headers)
            for i, r in enumerate(save_data_list):
                ws.append([i + 1, r["输入电压(mV)"], "", r["ADC原始数据"], round(r["ADC转换电压(mV)"], 3),
                           round(r["万用表实测电压(mV)"], 3),
                           round(dnl_by_code.get(r["ADC原始数据"]), 3) if r["ADC原始数据"] in dnl_by_code else "",
                           round(inl_by_code.get(r["ADC原始数据"]), 3) if r["ADC原始数据"] in inl_by_code else ""])
            ws.append([])
            ws.append(["ADC DNL最大误差(LSB)", round(adc_dnl_max, 3)])
            ws.append(["ADC INL最大误差(LSB)", round(adc_inl_max, 3)])
            ws.append(["测试结果", "PASS" if adc_passed else "FAIL"])
            wb.save(save_path)
            print(f"✅ 测量数据已保存到: {save_path}")
        except Exception as e:
            print(f"❌ 保存文件失败: {e}")

    # ---------- 7. 清理 ----------
    instr.swd_modify_register(ADC_CR, 0x00, 0, 0)     # ADC_EN 关闭
    instr.serial_send("cmd_gpio_set_595d_test reset\r\n")  # 只发送不等待响应; 复位继电器通路(含外部 DAC)
```

### 5.6 VREFH 实测（保证 DNL 计算精度）

2810 用 `global_use["Voffset_P"]`（VREF 实测值）做满量程参考。OCD5010 对应方案（三选一）：

1. **推荐**：在测试开头复用现有 VREFH 通路实测——`RW_ADC_VREFH_ATP[16]=1`、
   `RW_ATP_MUX[4:0]=0b00110`、`RW_ATP_MUX_EN[5]=1`，`cmd_gpio_set_595d_test set {pin_cfg.GPIO5} {eq_cfg.DMM_MODE}`，
   用 `instr.dmm_measure_single_voltage()` 读 `ADC_VREFH_OUT`（GPIO5）。
   前提：`CR` 写入实测值 `0x00310031`（`ADC_EN` 置 1）；参考电压量程/使能通路以实测与 SFR 为准，
   若需置位 `ANAC.ENCR.ANA_VREFH_EN`（bit18）以工程实测为准；
2. 从 `global_use["ADC_VREFH_VTRM_BEST"]` / 已修调 VREFH 档位推算（若测试顺序固定，先跑 `ADC_VREFH_VTRM`）；
3. 简化为标称值（量程 3V → 3000mV），但会引入 VREFH 修调误差（dohko2 增益误差指标 0.7%）。

### 5.7 DNL 严格算法（可选增强，dohko2 表 42 细测步骤 3）

若要与 dohko2 的 DNL 定义一致，可改用**小步长递增扫描**：

```
step = 1LSB 左右(如 VREFH/4096)
对 v 从 0.15V 到 VREFH-0.15V 递增:
   设置电压 → 读 ADC_DATA0 → 记录 (v, code)
求转移曲线: 对每个 code 记录首次达到该 code 的输入电压
DNL(n) = (V[n+1]-V[n])/1LSB - 1   # 相邻 code 步距与 1LSB 的偏差
INL(n) = Σ DNL(k)                  # 或曲线对拟合直线的最大偏离
结果取 max|DNL| 与规格比较
```

该方案精度更高但耗时更长，建议作为细测模式；首版移植先沿用 2810 的"实测电压-转换电压"方法。

---

## 6. 需要同步修改的文件清单

| 文件 | 修改内容 | 优先级 |
|---|---|---|
| `chips/OCD5010/tests.py` | 新增 `@test_case def ADC_DNL_Test()`（已支持交互选择 VREFH 量程与 ADC 通道，默认 3V/ADC_CH23(PA7)）；已在 `TEST_ITEMS` 注册为 "11" | 必须 |
| `test_configs/OCD5010/test.xlsx` | 新增 `ADC_DNL` 行：`Min=-2, Max=2, Unit=LSB`（参考 2810 规格，供 `df_test_data` 读取） | 必须 |
| `test_configs/OCD5010/register.xlsx` | （推荐）补充 `ADC_CR / ADC_OTR / ADC_TGSR / ADC_TGNR / ADC_SEQR0 / ADC_IR / ADC_DATA0` 数字模块寄存器；否则代码中硬编码地址 | 可选 |
| `test_configs/OCD5010/pin_configs.json` | 如需新增 ADC_CH23(PA7) 等引脚定义 | 可选 |
| `instrument/instrument_set.py` | 无需改动（已有 `sig_*`、`swd_*`、`dmm_*` 接口） | 无需 |

---

## 7. 注意事项 / 待确认问题

1. **输入电压源（已确认）**：外部 DAC2→母线→ADC 通道串口指令——`cmd_gpio_set_595d_test set 38 7 <bench_pin_bit>`
   （每个参数为独立通路：**38=万用表测量、7=DAC2→母线、\<bench_pin_bit\>=母线→ADC 通道**，映射见 §4.8，PA7→`set 38 7 11`）
   + `spi_cmd_set_val mV {电压}` 设置输出电压；**DAC2 输出 = 设置值×2**（DAC1 为×1），故设置值时需按 2 倍折算，
   实际输入以万用表经 38 通路实测为准；已集成进 `ADC_DNL_Test`（见 §5.5）；
   信号发生器 DG1022Z（`sig_*` 接口）保留为备选方案。
2. **触发与读数的通路（已确认）**：`ADC_OTR` 写 `0x5AA5` 软件触发 → 轮询 `IR.S0CDIF` → 读 `DATA[0]`，
   全程可由 SWD 在 CPU halt 下完成；**无需** ANAC `RW_ADC_START` 手工触发。
   ⚠️ SFR 中 `OTR` 标注为 "Over Temperature Register" 是错误占位，实际为软件触发寄存器。
3. **数字 ADC 模块寄存器未入 register.xlsx**：建议补表（测试章节列填 `ADC`），或按 2810 硬编码地址先例。
4. **VREFH 取值**：DNL 换算依赖满量程，建议实测（§5.6 方案 1），避免把 VREFH 修调误差带入结果。
5. **测试点数量**：3V 量程下 ADC 全程有效输入约 0.15~2.85V（5%~95% VREFH），
   线性扫描默认 0.5~4.0V 会超量程，建议限制在 0.2~2.8V 之间。
6. **输入/实测并联**：2810 中 DMM 与 ADC 输入在同一节点并联；OCD5010 需在 DUT 板上保证
   ADC_CH23(PA7) 同时可被信号源驱动和万用表测量（不能把该引脚切到 `DMM_MODE` 继电器，否则输入断开）。
7. **交互式 input()**：测试含接线确认与模式选择，`main.py` 的"执行全部"会自动跳过含
   "TRIM/修调"字样的项，`ADC DNL 测试` 名称不含关键词会被全量执行，符合预期（注意耗时可观）。
8. **ADC 时钟与 CR 配置（已确认）**：`RCC.ANACKE.ADCCKE`(bit0)=1 是转换前提；
   `CR` 实测有效值为 `0x00310031`（`ADC_EN` 置 1；按手册位域推导的 `0x00620061` 实测不生效，
   参考电压使能通路以实测/SFR 为准）；`TCFG.INIT`(INITIAL_PULSE) 不可写 0，
   否则 ADC 不会输出有效结果（已核对 SFR / ADC_Reg.h / ODS_5010.h + 上机实测）。

---

## 8. 参考资料（工程内文件路径）

| 资料 | 位置 |
|---|---|
| 2810 参考函数 | `chips/ocd2810/tests.py` 第 943~1111 行（`ADC_DNL_Test`），第 1761 行注册 |
| 2810 规格 | `test_configs/ocd2810/test.xlsx` → `ADC_DNL`: Min=-2 / Max=2 / LSB |
| OCD5010 用户手册 | **`ODS32M501xUM 260720.docx`（新版，CR 位域已更新）** 第 19 章 ADC（第 2423~2637 段），表 19-4；旧版 `ODS32M501xUM.docx` 的 CR 位域已过时 |
| OCD5010 测试计划 | `test_configs/OCD5010/dohko2_Test_Plan_Guide.docx` 表 38~42（VREFH/ADC 测试方案） |
| OCD5010 FT 规格 | `test_configs/OCD5010/SDS5010  Final Test specifications V0.0.0.docx`（ADC 测试项 65/69） |
| OCD5010 测试代码 | `chips/OCD5010/tests.py`（现有 ADC_VREFH_VTRM 第 854~970 行可作寄存器写法参照） |
| 寄存器表 | `test_configs/OCD5010/register.xlsx`（ANAC 模块 `0x40007C00~7FFF`） |
| 仪器接口 | `instrument/instrument_set.py`（`swd_modify_register` / `dmm_measure_single_voltage` / `sig_*`） |
| 固件 SDK 参考 | `adc_demo/main.c`（16 通道配置示例，见 §5.2）；`ADC_Reg.h` / `ODS_5010.h` / SFR（寄存器权威定义） |

---

*文档生成时间：2026-08-21。本文件仅为分析与设计参考，未修改任何工程源码。*
