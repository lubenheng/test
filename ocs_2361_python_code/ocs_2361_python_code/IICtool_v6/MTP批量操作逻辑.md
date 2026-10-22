# MTP 批量操作逻辑详细分析

> 分析对象：`IICtool_v6` 中 MTP 批量相关模块
> 核心文件：[mtp_batch_ops.py](mtp_batch_ops.py)（业务层）、[mtp_ops.py](mtp_ops.py)（单字节操作）、[crc.py](crc.py)（校验）、[IICtool.py](IICtool.py)（GUI 壳 + 线程/交互）
> 上游依赖：[iic_ops.py](iic_ops.py)（2361 报文与 Excel 解析工具）、[i2c_comm.py](i2c_comm.py)（DLL 通信层）

---

## 一、模块定位与分层

MTP 批量功能是"**Excel 驱动 + 逐字节 I2C 寄存器间接访问**"的三层结构：

```
┌─────────────────────────────────────────────────────────────┐
│ GUI 层  IICtool.py  class I2CGUI（MTP测设模式 页签）           │
│  · 文件对话框 / messagebox 确认 / 进度条 Toplevel / 日志输出   │
│  · 全部耗时逻辑在【主线程】同步执行（progress_win.update() 泵消息）│
└───────────────────────────┬─────────────────────────────────┘
                            │ 静态方法调用 + 回调(log_cb/progress_cb)
┌───────────────────────────▼─────────────────────────────────┐
│ 业务层  mtp_batch_ops.py  class MTPBatchLogic（全静态方法）     │
│  · 模板生成 / Excel 解析 / 512 字节 buffer 构建 / CRC 计算      │
│  · 全片烧录编排 / 读回比对 / 扇区读写 / 导出                    │
└───────────────────────────┬─────────────────────────────────┘
                            │ MTPController 实例（mtp_ctrl）
┌───────────────────────────▼─────────────────────────────────┐
│ 器件层  mtp_ops.py  class MTPController                       │
│  · 物理字节地址 → (ADDR, INDEX) 映射                           │
│  · 命令码封装：读触发 0x200 / 烧录 0x400 / 扇区擦 0x800 / 全擦 0x1000 │
└───────────────────────────┬─────────────────────────────────┘
                            │ write_func / read_func 回调
┌───────────────────────────▼─────────────────────────────────┐
│ 协议层  iic_ops.py execute_write/execute_read → i2c_comm.py   │
│  · 报头 bit-reversal、0x25 写帧、0xA5 读帧、DLL IICSendData     │
└──────────────────────────────────────────────────────────────┘
```

**解耦方式**：`MTPBatchLogic` 不持有任何 GUI/硬件对象，只通过参数接收 `mtp_ctrl`（器件层）与回调 `log_cb` / `progress_cb`，因此可被 GUI、测试脚本或命令行复用。

`MTPController` 在 [IICtool.py:235](IICtool.py#L235) 构造，注入的是 GUI 的两个薄封装：

| 注入回调 | GUI 实现 | 最终落到 |
|---|---|---|
| `write_func` | `I2CGUI._do_write` ([IICtool.py:211](IICtool.py#L211)) | `execute_write`（`iic_ops`）→ `I2CController.send_custom_write_frame` |
| `read_func` | `I2CGUI._do_read` ([IICtool.py:214](IICtool.py#L214)) | `execute_read`（两阶段 0x25+0xA5 读时序） |

---

## 二、MTP 地址模型（理解全部批量逻辑的基础）

MTP 是 **512 字节**空间，物理地址 `0~511`。器件不暴露字节地址，而是把 512 字节切成 **16 个扇区 × 32 字节**，并通过寄存器 `(ADDR, INDEX)` 间接寻址。

**字节地址 → 寄存器地址映射**（[mtp_ops.py:14](mtp_ops.py#L14)）：

```python
addr  = (mtp_byte_addr >> 6) & 0x1F   # ADDR  : 0~7   （每 64 字节换一个 ADDR）
index =  mtp_byte_addr       & 0x3F   # INDEX : 0~63
```

| 概念 | 范围 | 说明 |
|---|---|---|
| MTP 物理字节地址 | 0 ~ 511 | 512 B，每次操作 1 字节 |
| ADDR（基地址） | 0 ~ 7 | `byte >> 6`，共 8 个基地址 |
| INDEX（Word 偏移） | 0 ~ 63 | `byte & 0x3F` |
| 扇区 sector | 0 ~ 15 | 32 B/扇区，`sector = byte_addr >> 5` |
| Excel 侧"物理地址" | `ADDR + INDEX*4` | **注意：这不是字节地址**，是"字起始地址" |

> ⚠️ 关键歧义点：Excel 的 `ADDR / INDEX` 两列组合出的 `phys_addr = ADDR + INDEX*4` 在代码里被**当作 MTP 字节地址直接使用**（[mtp_batch_ops.py:191](mtp_batch_ops.py#L191)、[:331](mtp_batch_ops.py#L331)）。也就是说模板约定 `ADDR=0, INDEX=n` 时 `phys_addr = 4n`，一次 PROGRAM 覆盖字节 `4n ~ 4n+3`。这与设备侧 `(ADDR,INDEX)` 的真实映射（`byte>>6` / `byte&0x3F`）**并不是同一套数学**，实际是"模板行号 → 字节地址"的业务约定，靠 `program_byte` 内部再映射一次落到真实寄存器。理解这一点是看懂整个流程的前提。

**命令码**（写进 `(ADDR, INDEX)` 的寄存器值）：

| 操作 | 寄存器写入值 | 代码位置 | 触发方式 |
|---|---|---|---|
| 读触发 | `0x200` | [mtp_ops.py:72](mtp_ops.py#L72) | 写 `0x200` 后同址读回 4 字节 |
| 烧录 1 字节 | `0x400 \| (val & 0xFF)` | [mtp_ops.py:60](mtp_ops.py#L60) | **连写 2 次**（内部冗余） |
| 扇区擦除 | `0x800` | [mtp_ops.py:51](mtp_ops.py#L51) | 写在扇区首字节对应的 `(ADDR,INDEX)` |
| 全片擦除 | `0x1000` | [mtp_ops.py:55](mtp_ops.py#L55) | 写在 `(0,0)` |
| 使能开/关 | `0x5A5A5A5A` / `0xA5A500A5` | [mtp_ops.py:22-43](mtp_ops.py#L22-L43) | 多寄存器握手序列 |

读回的 4 字节列表 `dt` 中，**有效字节固定在 `dt[3]`**（代码统一取 `[3]`），这是所有导出/校验取值的位置。

---

## 三、五条批量业务流程

MTP 页签共 5 个按钮（[IICtool.py:143-154](IICtool.py#L143-L154)），对应五条流程。其中"批量 MTP 命令执行"这条**没有接线**（见第五节）。

```
┌── 按钮 ──────────────────────┬── GUI 方法 ────────────────┬── 业务方法 ───────────────────────┐
│ ① 生成MTP模板                 │ generate_mtp_template      │ generate_mtp_template             │
│ ② 全片擦写+校验               │ mtp_full_program_with_crc  │ _crc_gui + execute_mtp_burn       │
│                               │                            │        + verify_mtp_burn          │
│ ③ 读出MTP并导出               │ read_mtp_and_export_template│ read_mtp_and_export_template     │
│ ④ 写本扇区(不擦除)            │ write_mtp_sector_data      │ write_mtp_sector_data             │
│ ⑤ 读本扇区并导出              │ read_and_export_mtp_sector │ read_and_export_mtp_sector        │
│ ⑥ (无按钮) 批量MTP命令        │ import_and_execute_mtp_batch│ import_and_execute_mtp_batch ✗未接线│
└──────────────────────────────┴────────────────────────────┴───────────────────────────────────┘
```

### 3.1 ① 生成 MTP 模板 — `generate_mtp_template`

[mtp_batch_ops.py:120-159](mtp_batch_ops.py#L120-L159)

生成一个固定骨架的 Excel，**默认预填 78 行 PROGRAM**（`for i in range(1, 79)`，[mtp_batch_ops.py:140](mtp_batch_ops.py#L140)），即覆盖 `INDEX=1~78` → 字节地址 `4~315`。

| 列 | 表头 | 备注行说明 |
|---|---|---|
| A | OPERATION | `READ / PROGRAM / ERASE_SECTOR` |
| B | ADDR(基地址) | 如 `0~511` |
| C | INDEX(Word偏移) | 1 代表偏 4 字节 |
| D | [31:24] | 物理地址 `ADDR+INDEX*4+3` |
| E | [23:16] | `+2` |
| F | [15:8] | `+1` |
| G | [7:0] | `+0` |
| H | 物理地址(计算) | 自动公式 `=B{行}+C{行}*4` |

- 第 1 行 = 表头（加粗），第 2 行 = 说明行，**数据从第 3 行开始**（所有解析逻辑统一 `min_row=3`）。
- 列宽自适应（最长 25）。

### 3.2 ② 全片擦写 + CRC 校验（主流程）

这是最复杂的一条，拆成 **计算 → 确认 → 烧录 → 校验** 四阶段。

**阶段 0：UI 触发**（[IICtool.py:605-616](IICtool.py#L605-L616)）

```python
filename = askopenfilename(...)                 # 选输入 Excel
crc_start = int(self.crc_start_var.get() or 4)  # ⚠️ 无对应输入控件，恒为 4
crc_end   = int(self.crc_end_var.get() or 315)  # ⚠️ 恒为 315
MTPBatchLogic.mtp_full_program_with_crc_gui(filename, self.mtp, self._do_write, self._do_read,
                                            IICLogic.parse_hex_byte, self.log, crc_start, crc_end)
```

**阶段 1：解析 Excel + 构建 512 字节 buffer + 算 CRC**
`mtp_full_program_with_crc_gui`（[mtp_batch_ops.py:161-227](mtp_batch_ops.py#L161-L227)）

```
1) 选工作表：多 sheet 且含 "MTP批量模板" → 用该 sheet，否则 active
2) 从第 3 行起逐行扫描，遇 A/B/C 三列全空即 break（数据块结束）
3) 仅处理 OPERATION == "PROGRAM" 的行：
      phys = ADDR + INDEX*4                       (必须 0~511，否则跳过)
      buffer[phys+0] = [7:0]     ← b7
      buffer[phys+1] = [15:8]    ← b15
      buffer[phys+2] = [23:16]   ← b23
      buffer[phys+3] = [31:24]   ← b31
      并把这 4 个地址并入 phys_addrs_set（供后续去重/排序）
4) 确定 CRC 覆盖范围（见下方"⚠️"）
5) data_to_crc = 按 phys_addrs_sorted 顺序逐字节取 buffer  (或 fallback 用 crc_start..crc_end)
   final_crc = calculate_crc32_iter(data_to_crc)[0]
6) CRC 以小端写入保留区：buffer[507]=crc&0xFF, [508]=crc>>8, [509]=crc>>16, [510]=crc>>24
```

返回 `(True, buffer, phys_addrs_sorted, final_crc, None)`，**buffer 是 512 字节的完整镜像**，`phys_addrs_sorted` 是"本次需要烧录的字节地址清单"。

**阶段 2：人工确认**（[IICtool.py:621-623](IICtool.py#L621-L623)）

```python
messagebox.askyesno("全片烧录 CRC", f"算出CRC校验为: 0x{final_crc:08X}\n\n"
                    "确认你已经手动【擦除+Open MTP EN】后再点击是开始执行。")
```

工具**不会自动擦除、不会自动 Open MTP EN**，完全依赖操作者手工在单步区完成。这是流程中最脆弱的一环。

**阶段 3：两遍烧录** — `execute_mtp_burn`（[mtp_batch_ops.py:229-248](mtp_batch_ops.py#L229-L248)）

```
total_bytes = (len(phys_addrs_sorted) + 4) * 2      # +4 = CRC 区 507~510
for pass_idx in range(2):                           # ★ 两遍全片重写
    for offset in phys_addrs_sorted:                #   数据字节
        mtp_ctrl.program_byte(offset, buffer[offset])
    for offset in range(507, 511):                  #   CRC 字节
        mtp_ctrl.program_byte(offset, buffer[offset])
```

**每字节的实际 I2C 写次数 = 2 遍 × `program_byte` 内部 2 次 = 4 次**（[mtp_ops.py:62](mtp_ops.py#L62)）。冗余重写是 MTP 单元提高写入可靠性的常规手段，不依赖额外的状态轮询。

**阶段 4：逐字节读回比对** — `verify_mtp_burn`（[mtp_batch_ops.py:250-278](mtp_batch_ops.py#L250-L278)）

```
verify_offsets = phys_addrs_sorted + [507,508,509,510]（去重）
逐地址 read_byte(off) → read_val = rv[3]
  相等 → log 校验通过
  不等/读回 None/抛异常 → 记入 mismatches[(off, expected, actual)] 并 log
返回 mismatches 列表
```

GUI 侧（[IICtool.py:656-678](IICtool.py#L656-L678)）：有 mismatch 弹 `showwarning` 并列出前 20 条；全通过弹 `showinfo` 并复述最终 CRC。

**全流程时序**：

```
用户                         GUI(I2CGUI)                 MTPBatchLogic           MTPController/I2C
 │ 点【全片擦写+校验】          │                              │                        │
 ├────────────────────────────>│ askopenfilename              │                        │
 │                             ├── _crc_gui ─────────────────>│ 解析 Excel             │
 │                             │                              │ 构建 buffer(512)       │
 │                             │                              │ CRC32 → 507~510        │
 │                             │<── (buffer, addrs, crc) ─────│                        │
 │<── askyesno(显示CRC) ────────│                              │                        │
 ├── 是 ───────────────────────>│ 弹进度窗(Toplevel+grab_set)   │                        │
 │                             ├── execute_mtp_burn ─────────>│ 2 遍循环               │
 │                             │                              ├── program_byte ───────>│ 写 0x400|val ×2
 │                             │<── progress_cb ──────────────│    (每 10 字节刷新)     │
 │                             │ 销毁进度窗                    │                        │
 │                             ├── verify_mtp_burn ──────────>│ 逐字节               │
 │                             │<── progress_cb ──────────────│  read_byte(b)          │
 │                             │  destroy 进度窗               │  (先写0x200再读)        │
 │<── showinfo/warning ────────│ mismatches 汇总               │                        │
```

### 3.3 ③ 读出 MTP 并导出 — `read_mtp_and_export_template`

[mtp_batch_ops.py:442-553](mtp_batch_ops.py#L442-L553)、GUI [IICtool.py:751-779](IICtool.py#L751-L779)

用途：把芯片里的 MTP 内容整体读成"可再次烧录的模板"，即流程 ② 的逆向 + 自检。

```
循环 INDEX = 1..78  (ADDR 固定 0，物理地址 4..315)      ← 78 个 4 字节字 = 312 字节
    v0..v3 = read_byte(phys+0..+3)  → b0..b3 = v[3]
    写一行 PROGRAM / ADDR=0 / INDEX=n / [31:24]=b3 … [7:0]=b0 / H列公式
    同时 read_buffer.extend([b0,b1,b2,b3])
    ✗ 任一步抛异常 → 该行记 "READ" 且全 0，read_buffer 补 0，继续（不中断）

再读物理 507 起 4 字节 = 器件中存储的 CRC → 写一行 "PROGRAM(存储的CRC)"
最后用 read_buffer(4~315 共 312 字节) 就地算 CRC32 → 追加一行
        "CALCULATED(本次数据算得的新CRC)"
```

产出的最后一行就是**校验锚点**：把它与上一行"存储的CRC"对比即可判断芯片内容是否被篡改/写入不完整。

CRC 就地实现（[mtp_batch_ops.py:526-533](mtp_batch_ops.py#L526-L533)）：

```python
crc = 0xFFFFFFFF
for byte in read_buffer:
    crc ^= (byte << 24)
    for _ in range(8):
        crc = ((crc << 1) ^ 0x04C11DB7) & 0xFFFFFFFF if crc & 0x80000000 else (crc << 1) & 0xFFFFFFFF
```

> 🔴 **与 `crc.py` 的 `calculate_crc32_iter` 并不等价**（经实测验证，详见 [MTP批量模板CRC校验分析.md](MTP批量模板CRC校验分析.md)）：
> - 本段内联循环按 **MSB 优先**（`byte << 24` 使 `bit7` 先参与移位）→ 实测为标准 **CRC-32/MPEG-2**（`"123456789" → 0x0376E6E7` 吻合）；
> - `crc.py` 按 **LSB 优先**（`bit = (b >> i) & 1`，`i=0..7`）→ 等价于"标准 MPEG-2 作用于**逐字节位反转后**的数据"。
>
> 两者**仅当输入字节全部是位回文**（`0x00 / 0xFF / 0xA5 / 0x5A / 0x81 / 0x42 / 0x24` …）时才相等；真实数据必然不等。
> 后果：**烧录流程写入的 CRC 与读出流程算出的 CRC 永远对不上，CRC 校验锚点当前完全失效**（烧录用 `crc.py` [:219](mtp_batch_ops.py#L219)，回读用本段内联）。详见第五节 P4 / P12。

### 3.4 ④ 写本扇区 — `write_mtp_sector_data`

[mtp_batch_ops.py:373-390](mtp_batch_ops.py#L373-L390)、GUI [IICtool.py:716-731](IICtool.py#L716-L731)

```
校验 sector ∈ [0,15]
start = sector*32, end = start+31
for addr in start..end: mtp_ctrl.program_byte(addr, byte_val)   # 32 字节同值
```

GUI 弹确认框提示"请确认已【擦除】且已【开启 MTP_EN】"。

**特点**：只写一遍（`program_byte` 内部 2 次），**不擦除、不读回校验**，结束后只 `showinfo`。属于"快速单扇区调试"用途，非量产路径。

### 3.5 ⑤ 读本扇区并导出 — `read_and_export_mtp_sector`

[mtp_batch_ops.py:392-440](mtp_batch_ops.py#L392-L440)

```
读 start..end 共 32 字节 → results[32]（read_byte(b)[3]，None 记 0）
导出 Excel：8 行 × 每行 1 个 Word(4 字节)
   ADDR/INDEX 由 mtp_ctrl._addr_index_from_mtp(sector<<5) 反查基址 (w > 0 时 INDEX 递增取模 64)
   [31:24]=b3 … [7:0]=b0，H 列写物理起始地址（数值，非公式）
```

### 3.6 ⑥ 通用批量 MTP 命令执行 — `import_and_execute_mtp_batch`（⚠️ 当前无 UI 入口）

[mtp_batch_ops.py:280-371](mtp_batch_ops.py#L280-L371)

设计上是最通用的一条：读一张"命令表 Excel"，按行执行 `READ / PROGRAM / ERASE_SECTOR`。

**区间与注释规则**：

| 语法 | 行为 |
|---|---|
| A 列 `START` | 其**下一行**为起始行（[mtp_batch_ops.py:297](mtp_batch_ops.py#L297)） |
| A 列 `END` | 其**上一行**为结束行（[:305](mtp_batch_ops.py#L305)） |
| 找不到 START/END | 默认第 3 行 ~ 末行 |
| 空行 / `#` / `//` / 以 `注` 开头 | 跳过（[:317](mtp_batch_ops.py#L317)） |

**各操作语义**：

| OPERATION | 动作 | 备注 |
|---|---|---|
| `ERASE_SECTOR` | `sector = phys_addr >> 5` → `erase_sector(sector)` | 擦除的是**该物理地址所在扇区** |
| `READ` | 连读 `phys_addr ~ phys_addr+3` 共 4 字节 | 仅 log `[b3 b2 b1 b0]`，**不写回 Excel** |
| `PROGRAM` | 2 遍 × 4 字节 `program_byte` | 与 ② 的单字节写法一致，但**无 CRC、无读回校验** |

**异常策略**：任意一行失败立即 `return False, total_count, success_count, error_msg`（**快速失败，不回滚**），已执行的指令不会撤销。

---

## 四、数据流与关键算法汇总

### 4.1 CRC 的位置与口径（两个流程必须一致才对得上）

| 项目 | 烧录流程（写 CRC） | 读出流程（算 CRC） | 出处 |
|---|---|---|---|
| 多项式 / 初值 | `0x04C11DB7` / `0xFFFFFFFF` | 同左 | [crc.py:8](crc.py#L8) |
| **算法位序** | **LSB 优先**（`bit=(b>>i)&1`, i=0..7） | **MSB 优先**（`byte<<24` 后移位） | [crc.py:17](crc.py#L17) / [:526](mtp_batch_ops.py#L526) |
| **算法鉴定** | = 标准 MPEG-2 作用于**位反转字节流** | = **标准 CRC-32/MPEG-2** | 实测，见 [CRC 分析文档](MTP批量模板CRC校验分析.md) |
| **输入集合** | `phys_addrs_sorted`（Excel 声明为 PROGRAM 的地址） | 固定字节 `4 ~ 315`（INDEX 1..78） | [:216](mtp_batch_ops.py#L216) / [:491](mtp_batch_ops.py#L491) |
| 取值顺序 | 字节地址升序 | 字节地址升序 | 同 |
| 数据区 | 字节 `4 ~ 315`（默认模板覆盖范围） | 同左 | 模板 `range(1,79)` |
| 保留区 | 字节 `507 ~ 510`，小端存放 | 同左（作为比对目标读出） | [:220-223](mtp_batch_ops.py#L220-L223) |

> 🔴 **两个流程的 CRC 在所有层面都口径不一致**（经实测，详见 [MTP批量模板CRC校验分析.md](MTP批量模板CRC校验分析.md)）：
> 1. **算法不同（致命）**：烧录用 `crc.py`（LSB 优先），回读用内联循环（MSB 优先）。**即使模板完全正确、输入字节流完全相同，两侧算出的 CRC 也必然不同**。实测：字节 4~315 填 `i & 0xFF` 时，烧录侧 `0x4D1D7A77` vs 回读侧 `0x61FF70B5`。
> 2. **输入集合不同**：烧录按"Excel 声明集合"，回读按"固定 4~315"。模板有空洞（部分行未填/非 PROGRAM）或写到 315 之后时，两侧输入流也不同。
>
> 二者叠加的结果：导出模板里的 `CALCULATED(本次数据算得的新CRC)` **永远不等于** `PROGRAM(存储的CRC)`，**CRC 校验锚点当前完全失效**。
>
> 唯一"看起来正常"的情况是数据恰好只由位回文字节（`0x00/0xFF/0xA5/0x5A/…`）构成——此时两算法退化为同一结果（例如全 `0xFF` 时两侧都是 `0x1BE4E76F`）。这很可能就是该缺陷长期未被发现的原因。

### 4.2 容量与边界

| 项 | 值 |
|---|---|
| MTP 总容量 | 512 B（地址 0~511） |
| 扇区 | 16 × 32 B |
| `program_byte` 合法地址 | 0~511（无显式校验） |
| `read_byte` 合法地址 | 0~511（显式抛 `ValueError`，[mtp_ops.py:66](mtp_ops.py#L66)） |
| 数据区 | 4~506 |
| CRC 保留区 | 507~510 |
| 标准模板覆盖 | 4~315（78 字） |

### 4.3 写入次数（以默认模板 78 字全填为例）

```
phys_addrs_sorted = 312 字节 (+3 后到 315)
步骤② 全片烧录: (312 + 4) × 2 遍 × 2 次内部重写 = 1264 次 I2C 写
步骤② 校验:     316 次 read_byte = 316(触发写 0x200) + 316(读) = 632 次 I2C 事务
流程③ 全量导出: 79 次 read_byte → 158 次 I2C 事务
```

---

## 五、问题清单与风险评估

### 🔴 高风险

**P1 — Excel 物理地址 509~511 会造成 buffer 越界（IndexError）**
[mtp_batch_ops.py:192](mtp_batch_ops.py#L192) 只校验 `phys_addr > 511` 才跳过，但随后无条件写 `buffer[phys_addr + 3]`：
```python
if phys_addr < 0 or phys_addr > 511: continue      # 511 通过
...
buffer[phys_addr + 3] = b31                        # buffer[514] → IndexError
```
`bytearray(512)` 合法下标 0~511，故 `phys_addr ∈ [509,511]` 必然抛异常。异常被外层 `try` 捕获后返回 `(False, None, None, None, "bytearray index out of range")`，GUI 弹出 `showerror("全片烧录失败", ...)`——**错误信息完全不指向真实原因（是哪一行 Excel）**，排查成本高。
→ 建议：上界改为 `phys_addr > 508`，并在报错中带上 Excel 行号。

**P2 — 主线程执行全部批量 I/O，UI 冻结且进度窗可能卡死**
- `execute_mtp_burn` / `verify_mtp_burn` / `read_mtp_and_export_template` / `import_and_execute_mtp_batch` 均在 **GUI 主线程**被直接调用（[IICtool.py:643](IICtool.py#L643)、[:664](IICtool.py#L664)、[:776](IICtool.py#L776)、[:702](IICtool.py#L702)），只用 `progress_win.update()` 手动泵一次消息。
- 烧录窗的刷新条件是 `current_byte % 10 == 0`，校验窗是 `current % 10 == 0`，读导出窗是 `current % 5`——**两次刷新之间界面完全无响应**；默认模板下总时长可能达十几秒到数十秒（1600+ 次串口事务）。
- 进度窗设了 `grab_set()`（模态），一旦业务抛未捕获异常，`progress_win.destroy()` 在烧录流程中**没有放在 `finally` 里**（[IICtool.py:643-644](IICtool.py#L643-L644)），窗口会永久残留并锁住主窗口。
→ 建议：批量流程放后台线程，进度经 `root.after` 回主线程；销毁窗口统一 `try/finally`。

**P3 — 烧录流程不做写完成确认（相对旧实现的回归）**
新版 `execute_mtp_burn` 只调 `program_byte`，**既不轮询状态寄存器 `(31,0)`、也不插入任何延时**。而被弃用的旧实现 `MTPBatchLogic.mtp_full_program_with_crc`（[mtp_batch_ops.py:52-61](mtp_batch_ops.py#L52-L61)）有完整的 `mtp_retry_exec`：

```python
def mtp_retry_exec(cmd_name, cmd_seq, delay, exp_v, exp_m, max_r=5):
    for _ in range(max_r):
        for iw in cmd_seq: execute_write_cb(...)     # 下发命令
        time.sleep(delay)                            # ERASE/WRITE 等 0.5s
        st = execute_read_cb(31, 0)                  # 轮询状态寄存器
        if (st[3] & exp_m) == exp_v: return st       # 期望 bit0=1
    return None
```
新版完全依赖末尾的读回比对兜底：**写入未完成时读回会失败，但失败发生在几百次写入之后，无法重试、只能整片重来**。
→ 建议：至少恢复 CRC 区/关键字节的状态轮询，或在两遍之间加固定延时。

### 🔴 高风险（补充）

**P4 — CRC 两处实现算法不等价，校验锚点失效（🔴 致命，实测确认）**
不是"口径"差异这么轻——**烧录侧与回读侧用的是两个不同的算法**：
- 烧录 `mtp_full_program_with_crc_gui` 调 [crc.py](crc.py) `calculate_crc32_iter`：逐字节 **LSB 优先**（`bit=(b>>i)&1`）；实测等价于"标准 MPEG-2 作用于位反转字节流"；
- 回读 `read_mtp_and_export_template` 用内联循环 [:526-533](mtp_batch_ops.py#L526-L533)：逐字节 **MSB 优先**；实测为标准 **CRC-32/MPEG-2**（`"123456789" → 0x0376E6E7` 吻合）。

两者**仅当输入字节全是位回文时才相等**。真实数据下即使模板完全正确、输入字节流完全相同，结果也必然不同——实测字节 4~315 填 `i & 0xFF` 时：烧录侧 `0x4D1D7A77`，回读侧 `0x61FF70B5`。

**后果**：导出模板中的 `CALCULATED(本次数据算得的新CRC)` 恒不等于 `PROGRAM(存储的CRC)`，**CRC 校验锚点当前完全失效**。仅当数据恰为全 `0xFF` 之类回文字节时因两算法退化而"巧合通过"。

> **已定口径：以 [crc.py](crc.py) 为准**。`crc.py` 保持不变，只需把**回读侧内联循环**改为调用 `calculate_crc32_iter`（该模块第 6 行已导入）：
> ```python
> # mtp_batch_ops.py:526-533  修改后
> crc, _ = calculate_crc32_iter(bytes(read_buffer))
> ```
> **不改变写入芯片的 CRC 值**（烧录侧本就用 `crc.py`），故不影响在途批次。修复后标准模板场景两侧由 `0x4D1D7A77` / `0x61FF70B5` 收敛为**同一个** `0x4D1D7A77`。

> 另有第二类独立差异：烧录按"Excel 声明地址集合"、回读按"固定 4~315 区间"，模板有空洞或超出 315 时输入流也不同。**两类叠加；只改算法不解决第二类**（模板非"INDEX 1~78 连续填满"时仍对不上）。

详细的实测数据、场景仿真与修复方案见 **[MTP批量模板CRC校验分析.md](MTP批量模板CRC校验分析.md)**。

### 🟡 中风险

**P5 — `crc_start` / `crc_end` 是无效参数（三重失效）**
1. `IICtool.py` 中 `crc_start_var` / `crc_end_var` 只有 `StringVar` 定义（[:43-44](IICtool.py#L43-L44)），**没有任何 Entry/Spinbox 控件**，用户无法修改；
2. 传入 `mtp_full_program_with_crc_gui` 后，只要 Excel 中存在 PROGRAM 行，函数内部就用 `phys_addrs_sorted` 覆盖这两个值（[:212-213](mtp_batch_ops.py#L212-L213)），**入参被丢弃**；
3. 只有"Excel 无有效数据"时才回落到 `4 / 315` 的默认值（[:207-210](mtp_batch_ops.py#L207-L210)）。

→ 结论：这两参数目前是纯噪声，应从签名与 GUI 中移除，或真正接线成输入框。

**P6 — 读回/导出失败被静默降级为 0**
- `read_byte` 内部 `try/except` 失败返回 `None`（[mtp_ops.py:88-90](mtp_ops.py#L88-L90)）；
- 调用方普遍把 `None` 折算成 `0`：`read_and_export_mtp_sector`（[:407](mtp_batch_ops.py#L407)）、`read_mtp_and_export_template`（[:487-490](mtp_batch_ops.py#L487-L490) 等）；
- 结果：**通信失败产出的是一张"全 0 但格式正确"的 Excel**，且 `read_and_export_mtp_sector` 仍返回 `success=True`（只统计 `total/success` 的流程会显示"成功"）。
→ 建议：`None` 与 `0x00` 必须区分，失败应计数并最终回报。

**P7 — `import_and_execute_mtp_batch` 无 UI 入口，属于死路径**
[IICtool.py:680](IICtool.py#L680) 定义了方法，但 `IICtool.py` 与 `gui_mtp.py` 中**均无 `command=self.import_and_execute_mtp_batch` 绑定**（对比：其余 5 个流程都有按钮）。功能完整却不可达。

**P8 — `import_and_execute_mtp_batch` 的 PROGRAM 不擦除、不自校验**
与 ② 不同，该路径既不提示用户先擦除，也不做 CRC/读回校验，且失败即中断（已写入部分不回滚）。若未来接线使用，需补齐语义。

**P9 — 跨类访问私有方法**
[mtp_batch_ops.py:421](mtp_batch_ops.py#L421) 直接调用 `mtp_ctrl._addr_index_from_mtp(...)`，并在异常时**静默回落 `addr_base, index_base = 0, 0`**——若映射逻辑变更，导出的 ADDR/INDEX 列会整体错误而无人察觉。该映射应提升为 `MTPController` 的公开方法。

**P10 — 日志洪泛**
`program_byte` / `read_byte` 每次调用都 `self.log(...)`（[mtp_ops.py:58](mtp_ops.py#L58)、[:68](mtp_ops.py#L68)），默认模板一次全片流程会产生 **2000+ 行**日志，写入 Tkinter Text 控件是主要耗时来源之一（远超串口本身）。旧实现用 `execute_write` 的 `suppress_log` 参数刻意抑制了这一类输出。
→ 建议：批量路径改用静默写（`suppress_log=True`），只保留里程碑级日志。

### 🟢 低风险 / 清理项

**P11 — `MTPBatchLogic.mtp_full_program_with_crc`（旧版）是死代码**
[mtp_batch_ops.py:16-118](mtp_batch_ops.py#L16-L118)，全仓无调用点（`IICtool.py`／`gui_mtp.py` 里 `command=app.mtp_full_program_with_crc` 指向的是 **GUI 层的同名方法** [IICtool.py:605](IICtool.py#L605)，内部走 `_crc_gui` 新路径）。约 100 行、含唯一的 `mtp_retry_exec` 状态机实现。删除前建议先把其中的状态轮询能力移植到新流程（见 P3）。

**P12 — CRC 算法重复实现（⚠️ 且两实现不等价，见 P4）**
[mtp_batch_ops.py:526-533](mtp_batch_ops.py#L526-L533) 内联了 CRC 循环，与 [crc.py](crc.py) 的 `calculate_crc32_iter` 并存。**两者并非等价重复，而是两个不同算法**（MSB 优先 vs LSB 优先）。
**已定口径：以 [crc.py](crc.py) 为准** → 删除内联循环，回读侧改为调用 `calculate_crc32_iter`（算法与覆盖差异详见 [MTP批量模板CRC校验分析.md](MTP批量模板CRC校验分析.md)）。
另：`calculate_crc32_iter` 的第二个返回值 `iter_results`（逐字节中间结果）**无任何调用方**，属历史遗留。

**P13 — 函数签名噪声**：`mtp_full_program_with_crc_gui` 的参数 `mtp_ctrl` 与 `log_cb` 在函数体内从未使用（[mtp_batch_ops.py:162-163](mtp_batch_ops.py#L162-L163)）。

**P14 — 未使用变量**：`min_addr` / `max_addr` 的维护结果仅在"Excel 无 PROGRAM 行"的分支里被间接使用，正常路径下是死计算。

**P15 — 扇区导出时的 INDEX 回绕**：`idx_mod = (index_base + w) & 0x3F`（[:426](mtp_batch_ops.py#L426)）在扇区跨越 ADDR 边界时会出现 INDEX 回绕而 ADDR 未递增的展示不一致（当前扇区整体位于同一 ADDR 内，暂不触发，但依赖了隐含前提）。

---

## 六、结论与改进优先级建议

MTP 批量逻辑的**分层与解耦是干净的**：业务规则集中在 `MTPBatchLogic`，器件时序集中在 `MTPController`，GUI 只做交互壳，回调式进度/日志便于复用。Excel 模板与 `ADDR/INDEX → 物理地址` 的约定贯穿 5 条流程，语义自洽。

主要隐患集中在**鲁棒性与一致性**上，建议按以下顺序处理：

| 优先级 | 事项 | 关联 |
|---|---|---|
| **1** | **以 [crc.py](crc.py) 为准统一 CRC 算法**：保留 `crc.py` 不动，把回读侧内联循环改为 `calculate_crc32_iter(bytes(read_buffer))`，恢复校验锚点。**不改动写入芯片的 CRC 值，不影响在途批次** | **P4 / P12** |
| 2 | **两侧 CRC 范围统一为 `ADDR + INDEX*4 < 416`（地址 4~415）**：烧录侧加行过滤，回读导出侧把固定 INDEX 1~78 改成同一范围。算法同样用 `crc.py`。不改此项则第 1 项只在"模板恰好覆盖 4~415"时有效 | P4 / P5 |
| 3 | 修复 `phys_addr` 上界（509~511 越界）并让报错带 Excel 行号 | P1 |
| 4 | 批量流程移入后台线程 + 进度窗 `try/finally` 释放 | P2 |
| 5 | 恢复写入完成确认（状态寄存器轮询或延时），或明确文档化"靠读回兜底" | P3 |
| 6 | 区分"读回失败"与"读回 0"，失败必须上报 | P6 |
| 7 | 清理死代码（旧 `mtp_full_program_with_crc`）、重复 CRC、无效参数 | P11 / P12 / P13 |
| 8 | 决定 `import_and_execute_mtp_batch` 的去留：接线并补齐语义，或删除 | P7 / P8 |
| 9 | 批量路径静默日志，避免日志洪泛拖慢主线程 | P10 |

> 说明：CRC 相关两项被提到最前，因为它们是**唯一会导致"校验通过"结论本身不可信**的问题——其余各项影响的是稳定性、可观测性与可维护性，而 P4 直接影响**出货判定**。第 1、2 项建议**同一批修改一起落地**，只做第 1 项在模板非标准时会留下隐性陷阱。

---

## 附录 A：MTP 批量相关文件与行数索引

| 文件 | 关键符号 | 行 |
|---|---|---|
| [mtp_batch_ops.py](mtp_batch_ops.py) | `mtp_full_program_with_crc`（旧/死） | 16 |
| | `generate_mtp_template` | 120 |
| | `mtp_full_program_with_crc_gui` | 161 |
| | `execute_mtp_burn` | 229 |
| | `verify_mtp_burn` | 250 |
| | `import_and_execute_mtp_batch` | 280 |
| | `write_mtp_sector_data` | 373 |
| | `read_and_export_mtp_sector` | 392 |
| | `read_mtp_and_export_template` | 442 |
| [mtp_ops.py](mtp_ops.py) | `_addr_index_from_mtp` | 14 |
| | `open_mtp_en` / `close_mtp_en` | 22 / 34 |
| | `erase_sector` / `erase_chip` | 45 / 53 |
| | `program_byte` / `read_byte` | 57 / 65 |
| [crc.py](crc.py) | `calculate_crc32` / `calculate_crc32_iter` | 1 / 8 |
| [IICtool.py](IICtool.py) | 按钮绑定（MTP 批量区） | 143-154 |
| | `mtp_full_program_with_crc`（GUI 壳） | 605 |
| | `import_and_execute_mtp_batch`（未接线） | 680 |
| | `write_mtp_sector_data` / `read_and_export_mtp_sector` | 716 / 733 |
| | `read_mtp_and_export_template` | 751 |

## 附录 B：Excel 模板字段速查

| 列 | ② 全片烧录读取 | ③ 导出写入 | ⑥ 批量命令 |
|---|---|---|---|
| A `OPERATION` | 只认 `PROGRAM` | 写 `PROGRAM` / `READ` / `PROGRAM(存储的CRC)` / `CALCULATED(...)` | `READ` / `PROGRAM` / `ERASE_SECTOR`（另识别 `START`/`END` 标记） |
| B `ADDR` | int，与 C 组合成物理地址 | 0 | int |
| C `INDEX` | int | 1~78 | int |
| D~G `[31:24]…[7:0]` | 1~2 位 HEX（空按 0） | 输出 HEX 文本 | 仅 PROGRAM 读取 |
| H 物理地址 | **不读取**（用公式仅为人工核对） | 写公式或数值 | **不读取** |

> 注意：解析统一使用 `data_only=True` 加载（[:170](mtp_batch_ops.py#L170)、[:287](mtp_batch_ops.py#L287)），因此 **H 列公式必须有缓存值**（即文件曾被 Excel 打开并保存过），否则读到 `None`；不过 H 列本身不参与任何计算，所以实际影响为零。
