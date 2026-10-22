# MTP 批量执行代码分析

> 分析对象：MTP「全片擦写+校验」批量执行链路
> 参考口径：**算法以 [crc.py](crc.py) 为准**；**CRC 只计算模板地址 0~416**；**416~510 直接烧写（不进 CRC）**
> 相关代码：[mtp_batch_ops.py](mtp_batch_ops.py)（业务层）、[mtp_ops.py](mtp_ops.py)（器件层）、[i2c_comm.py](i2c_comm.py)（通信层）、[IICtool.py](IICtool.py)（GUI）
> 分析日期：2026-10-14
> 验证方式：本报告数值结论均由**实际调用生产函数**得到（见 §6 附录）

---

## 一、本次确认的口径

| 项 | 规定 |
|---|---|
| 算法 | [crc.py](crc.py) `calculate_crc32_iter`：`poly=0x04C11DB7`、`init=0xFFFFFFFF`、**LSB 优先**、`xorout=0` |
| CRC 计入范围 | **模板地址 0 ~ 416**（即 `ADDR + INDEX*4 < 416`，数据区上限 **地址 415**） |
| 地址 416 ~ 510 | **直接烧写**（按模板值原样落盘），**不参与 CRC 计算** |
| CRC 结果存放 | 地址 **507~510，小端**：`[507]=crc&0xFF, [508]=crc>>8, [509]=crc>>16, [510]=crc>>24` |
| 参考实现 | [CRC_Test.py](ocs_2361_python_code/IICtool_v6/CRC_Test.py)（`CRC_ADDR_LIMIT = 416`，[CRC_Test.py:67](ocs_2361_python_code/IICtool_v6/CRC_Test.py#L67)） |
| **最终确认** | **声明行 ∩ `phys < 416`** —— 模板中声明为 `PROGRAM` 且 `ADDR + INDEX*4 < 416` 的行字节，按地址升序去重拼接；与 `CRC_Test.py` 的 ★ 主输出完全同值 |

> 已排除"连续区间 `buffer[0:416]`"读法（含未声明字节）：会与 `CRC_Test.py` 算出不同值，使既有交叉核对失效，详见 §6.3。
| 历史口径 | 本口径与 [MTP批量模板CRC校验分析.md](MTP批量模板CRC校验分析.md) 的「已定口径（2026-10-10）」一致 |

**概念澄清**：所谓"416 直接烧写"不是"416 之前不烧"，而是——
**416 以下的字节既烧录、又计入 CRC；416 及以上的字节只烧录、不进 CRC。**

---

## 二、批量执行代码地图

```
【入口】IICtool.py  MTP测设模式 页签
├── 全片擦写+校验        IICtool.py:605  mtp_full_program_with_crc()      ← 本文重点
│     └─ MTPBatchLogic.mtp_full_program_with_crc_gui   mtp_batch_ops.py:162   解析+建 buffer+算 CRC
│     ├─ messagebox.askyesno                             IICtool.py:622        人工确认（擦除+Open EN）
│     ├─ MTPBatchLogic.execute_mtp_burn                mtp_batch_ops.py:230   两遍烧录
│     └─ MTPBatchLogic.verify_mtp_burn                 mtp_batch_ops.py:251   逐字节读回比对
├── 读出MTP并导出        IICtool.py:751  read_mtp_and_export_template()
│     └─ MTPBatchLogic.read_mtp_and_export_template    mtp_batch_ops.py:443   读回+算 CRC 并导出
├── 生成MTP模板          IICtool.py:594  → MTPBatchLogic.generate_mtp_template      mtp_batch_ops.py:121
├── 写本扇区(不擦除)      IICtool.py:716  → MTPBatchLogic.write_mtp_sector_data      mtp_batch_ops.py:374
├── 读本扇区并导出        IICtool.py:733  → MTPBatchLogic.read_and_export_mtp_sector mtp_batch_ops.py:393
└── (无按钮) 批量执行     IICtool.py:680  import_and_execute_mtp_batch()             ← 不可达，见 §4.6
                              └─ MTPBatchLogic.import_and_execute_mtp_batch  mtp_batch_ops.py:281

【公共底座】mtp_ops.py  MTPController
   program_byte(mtp_addr, val)  mtp_ops.py:57   写 0x400|val，**固定写 2 次**
   read_byte(mtp_addr)          mtp_ops.py:65   先写触发 0x200，再读 4 字节（**2 次 I2C 事务**）
   erase_sector / erase_chip    mtp_ops.py:45 / :53
   → 注入的 write_i2c/read_i2c = IICtool.py:211/214 的 _do_write/_do_read
     → iic_ops.py execute_write / execute_read（0x25 / 0xA5 报文）
```

### 烧录时的真实 I/O 量（以模板声明 416 字节为例）

| 阶段 | 调用 | 单次成本 | 合计 I2C 事务 |
|---|---|---:|---:|
| 烧录（2 遍 × 416 字节 + CRC 4 字节） | `(416+4)×2 = 840` 次 `program_byte` | 2 次写 | **1680** |
| 校验（416 + CRC 4 字节） | `420` 次 `read_byte` | 1 写 + 1 读 | **840** |
| | | | **≈ 2520** |

再叠加 [mtp_batch_ops.py:275](mtp_batch_ops.py#L275) 每个字节一次 `log_cb` → **420 次 GUI 文本插入 + 420 条 `log_records`**（导出日志时全部落 Excel）。这是 100k 速率下"点一次按钮等几分钟、界面无响应"的直接原因。

---

## 三、逐函数剖析（对照规定口径）

### 3.1 `mtp_full_program_with_crc_gui` — [mtp_batch_ops.py:162-227](mtp_batch_ops.py#L162-L227)

这是**唯一的 CRC 写入点**，也是与规定偏差最集中的地方。

| 行 | 行为 | 对照规定 | 判定 |
|---|---|---|---|
| [171-174](mtp_batch_ops.py#L171-L174) | 优先选 `MTP批量模板` 工作表，否则 `active` | — | ✅ |
| [184-186](mtp_batch_ops.py#L184-L186) | **遇首个空行即 `break`**，其后所有行丢弃 | 现场控制台 `bp` 会烧录空行之后的行 | ⚠️ 见 §4.5 |
| [191](mtp_batch_ops.py#L191) | `phys_addr = ADDR + INDEX*4` | 与模板列定义一致 | ✅ |
| [192](mtp_batch_ops.py#L192) | `if phys < 0 or phys > 511: continue` | 上界应为 508 | ❌ **509~511 越界缝隙** |
| [198-201](mtp_batch_ops.py#L198-L201) | `buffer[phys+0..3] = b7,b15,b23,b31` | 小端铺放 | ✅ |
| [212-213](mtp_batch_ops.py#L212-L213) | `crc_start/crc_end` 被数据范围覆盖 | 应为固定 `416` 上限 | ❌ 配置项失效 |
| [**214**](mtp_batch_ops.py#L214) | `phys_addrs_sorted = sorted(x for x in phys_addrs_set if 0 <= x <= 511)` | 应为 `x < 416` | ❌ **核心偏差** |
| [216-218](mtp_batch_ops.py#L216-L218) | CRC 输入 = 声明集合（升序） | 应为"0~416" | ❌ 口径分裂 |
| [220-223](mtp_batch_ops.py#L220-L223) | CRC → `buffer[507..510]` 小端 | 一致 | ✅ |

#### ★ 核心偏差：CRC 输入范围没有任何 416 上限

[mtp_batch_ops.py:214](mtp_batch_ops.py#L214) 把**模板声明的全部地址**（`0 <= x <= 511`）纳入 CRC 输入，**没有 416 截断**。

**实测**（构造含 416~510 直烧区的模板，模板结构：INDEX 1~103 → 地址 4~415；INDEX 104~126 → 地址 416~507）：

```
成功=True  CRC=0xDF4893FC  烧录地址数=504  区间 4~507
   → 416 及以上被计入烧录集合的字节数 = 92
   → CRC 存储字节 buffer[507:511] = [252, 147, 72, 223]   (= 0xDF4893FC 小端 ✅)
```

**结论**：当前实现把 **92 字节的直烧区混进了 CRC 输入流**，算出的 `0xDF4893FC` 不是规定口径的值。按 §6 的三种口径仿真，差异是**恒定的、与数据无关**的：

| 口径 | CRC | 与规定比 |
|---|---|---|
| 规定：`phys < 416` | `0x997C72C8` | — |
| 现生产：全部声明集合（无 416 上限） | `0x1BDB7BE3` | ❌ 不一致 |
| 若读作连续 `buffer[0:416]` | `0xF7755BFD` | ❌ 不一致 |
| 现回读：固定 4~315 | `0x95A161D4` | ❌ 不一致 |

**四种口径两两皆不相同** —— 这正是"CRC 校验锚点失效"的完整成因，而不仅仅是算法差异。

### 3.2 `execute_mtp_burn` — [mtp_batch_ops.py:230-248](mtp_batch_ops.py#L230-L248)

```python
for pass_idx in range(2):                              # 两遍
    for offset in phys_addrs_sorted:                   # 数据区
        mtp_ctrl.program_byte(offset, buffer[offset])
        ...
    for offset in range(507, 511):                     # CRC 区
        mtp_ctrl.program_byte(offset, buffer[offset])
```

| 项 | 判定 |
|---|---|
| 两遍烧录 | ✅ 与量产余量要求一致（但见 §4.4 的写 4 次问题） |
| CRC 区 507~510 每遍都写 | ✅ |
| `total_bytes = (len+4)*2` | ✅ 与循环次数自洽 |
| **416~510 直烧区** | ❌ **依赖 `phys_addrs_sorted` 里"恰好包含"它们** |

#### ⚠️ 修复时的陷阱：加 416 过滤会让 416~510 不再被烧录

[mtp_batch_ops.py:214](mtp_batch_ops.py#L214) 当前是**一个集合同时承担两种职责**：

- 作为 **CRC 输入**（应排除 ≥416）
- 同时作为 **烧录清单**（应包含 ≥416）

因此 [MTP批量模板CRC校验分析.md](MTP批量模板CRC校验分析.md) 中建议的一行修复：

```python
phys_addrs_sorted = sorted(x for x in phys_addrs_set if x < 416)   # ⚠️ 会连同"烧录"一起截断
```

**会把 416~510 整个直烧区从烧录清单里删掉**，与规定"416~510 直接烧写"直接冲突，属于**静默功能回归**（不会报错，只是芯片上那一段没烧）。

**正确做法是拆成两个集合**：

```python
crc_addrs     = sorted(x for x in phys_addrs_set if x < 416)      # 进 CRC + 烧录
literal_addrs = sorted(x for x in phys_addrs_set if x >= 416)     # 只烧录，不进 CRC
# CRC 输入：bytes(buffer[x] for x in crc_addrs)
# 烧录清单：crc_addrs + literal_addrs
```

### 3.3 `verify_mtp_burn` — [mtp_batch_ops.py:251-278](mtp_batch_ops.py#L251-L278)

| 项 | 判定 |
|---|---|
| 逐字节读回并与 `buffer` 比对 | ✅ 逻辑正确 |
| 追加校验 507~510 | ✅ |
| **与 CRC 算法的关系** | 无关 —— 它把 CRC 字节也当普通数据比对 |
| I/O 放大 | ❌ 每字节 1 次 `read_byte`（= 1 写 + 1 读）+ 1 次日志 |

**关键结论**：`.mtp_full_program_with_crc` 结束时提示的「校验全部通过」，**只代表写入落位正确，不代表 CRC 值正确**。这是使用说明与弹窗文案容易误导的地方。

**可优化 4 倍**：`read_byte` 一次返回 4 字节（4 字节对应一个 word），按 word 成组校验即可把 420 次读降到 ~105 次。

### 3.4 `read_mtp_and_export_template` — [mtp_batch_ops.py:443-553](mtp_batch_ops.py#L443-L553)

回读侧，与烧录侧口径**三重不一致**：

| 项 | 现状 | 规定 | 判定 |
|---|---|---|---|
| 读取范围 | 硬编码 `range(1, 79)` → 地址 **4~315**（[mtp_batch_ops.py:475](mtp_batch_ops.py#L475)） | 地址 0~416 | ❌ 缺 316~416，多算了本不该进 CRC 的低端？ |
| CRC 输入 | `read_buffer` = 312 字节（[mtp_batch_ops.py:491](mtp_batch_ops.py#L491)） | 0~416 的字节流 | ❌ |
| **算法** | 内联 **MSB 优先**循环（[mtp_batch_ops.py:526-533](mtp_batch_ops.py#L526-L533)） | [crc.py](crc.py) **LSB 优先** | ❌ **必须改为 `calculate_crc32_iter`** |
| CRC 行读取位置 | ADDR=507 INDEX=0 → 地址 507~510（[mtp_batch_ops.py:503-505](mtp_batch_ops.py#L503-L505)） | 507~510 | ✅ |
| 比对方式 | 只追加 `CALCULATED` 行供**人工**比对 | — | ⚠️ 无程序化判定 |

修复（`calculate_crc32_iter` 已在 [mtp_batch_ops.py:6](mtp_batch_ops.py#L6) 导入）：

```python
# 修改前（mtp_batch_ops.py:526-533，MSB 优先，与 crc.py 不等价）
crc = 0xFFFFFFFF
for byte in read_buffer:
    crc ^= (byte << 24)
    for _ in range(8):
        crc = ((crc << 1) ^ 0x04C11DB7) & 0xFFFFFFFF if crc & 0x80000000 else (crc << 1) & 0xFFFFFFFF

# 修改后（以 crc.py 为准）
crc, _ = calculate_crc32_iter(bytes(read_buffer))
```

同时把读取范围从"固定 INDEX 1~78"改为"地址 0~416 连续区间"，才能与烧录侧对齐。

> **注意**：改这里**不改变写入芯片的 CRC 值**（烧录侧本就用 [crc.py](crc.py)），因此不影响在途批次，无需量产评估。

### 3.5 `write_mtp_sector_data` / `read_and_export_mtp_sector` — [mtp_batch_ops.py:374-440](mtp_batch_ops.py#L374-L440)

与 CRC 无关，但导出的 Excel **列不自洽**（详见 [分析报告.md](分析报告.md) P1-4）：实际读的是 `range(sector*32, sector*32+32)`，但 INDEX 列按**字节粒度**模型写 `(index_base + w)`，而模板公式按 **Word 粒度** `phys = ADDR + INDEX*4` 解释 → 导出文件若回灌会操作错误地址区间。

sector=1 的反例：实际读 32~63，导出 INDEX 写 32~39，按公式反推 = 128~156。

### 3.6 `import_and_execute_mtp_batch` — [mtp_batch_ops.py:281-371](mtp_batch_ops.py#L281-L371)（不可达）

- **GUI 无按钮**：[IICtool.py:680](IICtool.py#L680) 的实现完整（进度条、结果弹窗俱全），但 `create_widgets` 从未绑定 → 生产不可达
- **完全不做 CRC**：PROGRAM 行原样烧 2 遍，无 CRC 计算、无 CRC 写盘
- **工作表选择不一致**：用 `wb.active`（[mtp_batch_ops.py:287](mtp_batch_ops.py#L287)），而另外两处用 `MTP批量模板` 优先

含义：如果"416~510 直接烧写"应当由模板行驱动，**本函数是唯一"按模板原样落盘"的实现**，但它同时也把数据区原样落盘、绕过了 CRC 机制。规定的意图更可能是：在 `mtp_full_program_with_crc_gui` 内区分两个区间，而非启用此函数。

### 3.7 `mtp_ops.py` 器件层

| 位置 | 行为 | 判定 |
|---|---|---|
| [mtp_ops.py:14-17](mtp_ops.py#L14-L17) | `addr=(byte>>6)&0x1F`, `index=byte&0x3F` | ⚠️ 与模板的 Word 粒度模型不同源（见 [分析报告.md](分析报告.md) P1-4） |
| [mtp_ops.py:57-63](mtp_ops.py#L57-L63) | `program_byte`：**固定写 2 次** | ⚠️ 无 0~511 范围校验；越界地址会被 `&0x1F`/`&0x3F` **静默取模**到别的单元 |
| [mtp_ops.py:65-90](mtp_ops.py#L65-L90) | `read_byte`：触发写 + 读，**异常被吞返回 None** | ❌ 有范围校验，但读失败静默 → 上层按 `0x00` 记账 |
| [mtp_ops.py:31-32](mtp_ops.py#L31-L32) / [42-43](mtp_ops.py#L42-L43) | `open/close_mtp_en` **异常被吞** | ❌ 失败照常进入烧录流程 |

---

## 四、偏差清单

| # | 位置 | 偏差 | 严重度 |
|---|---|---|:---:|
| M-1 | [mtp_batch_ops.py:214](mtp_batch_ops.py#L214) | CRC 输入无 416 上限，把直烧区（416~510）计入 → 算出的 CRC 不是规定值 | 🔴 |
| M-2 | [mtp_batch_ops.py:526-533](mtp_batch_ops.py#L526-L533) | 回读侧用内联 MSB 算法，与 [crc.py](crc.py) 不等价 | 🔴 |
| M-3 | [mtp_batch_ops.py:475](mtp_batch_ops.py#L475) | 回读范围硬编码 4~315，与规定的 0~416 不符 | 🔴 |
| M-4 | [mtp_batch_ops.py:214](mtp_batch_ops.py#L214) | 单一集合兼顾"CRC 输入"与"烧录清单"→ 按 M-1 修复若只加过滤，会**静默漏烧 416~510** | 🔴 |
| M-5 | [mtp_batch_ops.py:192](mtp_batch_ops.py#L192) | `phys > 511 continue` 留下 509~511 缝隙 → `IndexError`，且不报 Excel 行号（实测证实） | 🟠 |
| M-6 | [mtp_batch_ops.py:184-186](mtp_batch_ops.py#L184-L186) | 遇空行 `break`，其后行**既不烧也不校验**，与现场 `bp` 行为不一致 | 🟠 |
| M-7 | [mtp_batch_ops.py:275](mtp_batch_ops.py#L275) | 逐字节打日志 + 逐字节 `read_byte` → 2520 次 I2C + 420 次 GUI 插入 | 🟠 |
| M-8 | [mtp_ops.py:88-90](mtp_ops.py#L88-L90) | 读失败返回 `None`，上层 [mtp_batch_ops.py:487](mtp_batch_ops.py#L487) 按 `0` 记账 → 坏点被伪装成 `0x00` | 🟠 |
| M-9 | [mtp_ops.py:31](mtp_ops.py#L31)、[42](mtp_ops.py#L42) | MTP 使能失败被吞，GUI 的错误弹窗是死分支 | 🟠 |
| M-10 | [IICtool.py:680](IICtool.py#L680) | 批量执行无按钮入口，90 行逻辑不可达 | 🟡 |
| M-11 | [mtp_batch_ops.py:212-213](mtp_batch_ops.py#L212-L213) | `crc_start/crc_end` 被覆盖，且 GUI 侧无控件绑定 → 配置项整体失效 | 🟡 |
| M-12 | [mtp_batch_ops.py:426-435](mtp_batch_ops.py#L426-L435) | 扇区导出的 INDEX 列与实际读取地址不自洽 | 🟡 |
| M-13 | [mtp_batch_ops.py:287](mtp_batch_ops.py#L287) | 工作表选择用 `active`，与另两处不一致 | 🟢 |
| M-14 | [mtp_ops.py:57](mtp_ops.py#L57) | `program_byte` 无范围校验，越界静默取模 | 🟢 |
| M-15 | [mtp_batch_ops.py:16-118](mtp_batch_ops.py#L16-L118) | 旧版 `mtp_full_program_with_crc` 表头校验为 6 列（模板是 8 列）→ 必然失败，且无调用者 | 🟢 |

---

## 五、修复方案（按调用链顺序落地）

### 第一组：让 CRC 口径与规定一致 —— ✅ **已实施（2026-10-14）**

**① [mtp_batch_ops.py](ocs_2361_python_code/IICtool_v6/mtp_batch_ops.py) — 新增口径常量 + 拆分集合**

```python
MTP_SIZE       = 512
CRC_ADDR_LIMIT = 416     # 只计入 phys = ADDR + INDEX*4 < 416 的声明行
CRC_RSV_LO, CRC_RSV_HI = 507, 510
PHYS_MAX_SAFE  = 508     # 单行落位 4 字节: phys+3 <= 511
CRC_SCAN_INDEX_LO, CRC_SCAN_INDEX_HI = 0, 103   # 回读扫描 → 物理地址 0~415
```

`mtp_full_program_with_crc_gui` 主体改为：

```python
burn_addrs = sorted(x for x in phys_addrs_set if 0 <= x < MTP_SIZE)   # 烧录清单(含 >=416)
crc_addrs  = [x for x in burn_addrs if x < CRC_ADDR_LIMIT]            # 进 CRC
data_to_crc = bytes(buffer[x] for x in crc_addrs)
final_crc, _ = calculate_crc32_iter(data_to_crc)
for i, off in enumerate(range(CRC_RSV_LO, CRC_RSV_HI + 1)):
    buffer[off] = (final_crc >> (8 * i)) & 0xFF
return True, buffer, burn_addrs, final_crc, None
```

**关键点**：`burn_addrs` 与 `crc_addrs` **拆成两个集合**。若只把原 `phys_addrs_sorted` 改成 `x < 416`，`execute_mtp_burn` 会**静默漏烧整个 416~510 直烧区**（见 M-4）。

同时修复 M-5（越界缝隙）：

```python
if not (0 <= phys_addr <= PHYS_MAX_SAFE):        # 原 `> 511` 会放行 509~511
    log_cb(f"!! 第 {idx} 行 phys={phys_addr} 超出可写范围 (0~{PHYS_MAX_SAFE})，该行已跳过")
    continue
```

并删除了失效的 `crc_start/crc_end` 归一化逻辑（原 `if crc_start < 4` 在参数变为 `None` 后会抛 `TypeError`）。

**② [mtp_batch_ops.py](ocs_2361_python_code/IICtool_v6/mtp_batch_ops.py) — 回读侧算法与范围同时对齐**

```python
for index_mod in range(CRC_SCAN_INDEX_LO, CRC_SCAN_INDEX_HI + 1):   # 原 range(1,79)
    ...
crc, _ = calculate_crc32_iter(bytes(read_buffer))                   # 取代内联 MSB 循环
```

**③ [IICtool.py](ocs_2361_python_code/IICtool_v6/IICtool.py) — 同步 GUI 层**

- 删除无控件绑定的 `crc_start_var` / `crc_end_var`（M-11）
- 不再向业务层传 `crc_start/crc_end`；局部变量改名 `phys_addrs_sorted` → `burn_addrs`
- 确认弹窗展示口径明细（算法、计入字节数、直烧字节数）

**④ [CRC_Test.py](ocs_2361_python_code/IICtool_v6/CRC_Test.py) — 交叉核对文案修正**

`prod_msg` 原先按"声明集合总字节数"报数，现区分"计入 CRC 字节数 / 只烧录字节数"。

**⑤ 验收结果（全部 PASS）**

| 验收项 | 结果 |
|---|---|
| 生产烧录器 vs `CRC_Test.py` ★ 主输出（模板含 416~507 声明行） | `0xD36A982D` == `0xD36A982D` ✅ |
| 计入 CRC 字节数（两侧） | 412 == 412 ✅ |
| CRC 落盘小端 | `2D 98 6A D3` ✅ |
| 越界缝隙 `phys=509` | 不再抛 `IndexError`；带行号跳过 ✅ |
| 回读侧算法与范围 | 导出的 `CALCULATED` == `crc.py(0~415)` ✅ |
| 端到端（模板声明 0~415）：烧录侧 CRC == 回读 `CALCULATED` == 芯片 CRC 字节 | 三者同为 `0x99E0623B` ✅ |
| 不含 ≥416 声明行的模板 | CRC 不受直烧区影响（与旧口径同值）✅ |

### 遗留项（本次未改，需决策）

**`generate_mtp_template` 生成的默认模板仍是 `INDEX 1~78`（地址 4~315）**（[mtp_batch_ops.py:161](ocs_2361_python_code/IICtool_v6/mtp_batch_ops.py#L161)）。

- 烧录侧对它是自洽的（CRC = `crc.py(4~315)`）
- 但回读侧固定扫 0~415 → **两侧 CRC 不等**

由于已确认采用"声明行集合"语义且模板有空洞时不报错，这个不一致是**口径本身的固有代价**：回读无法从芯片反推原模板声明了哪些行。

若要开箱即用，需二选一：
- **(a)** 把 `generate_mtp_template` 改为生成 `INDEX 0~103`（地址 0~415），新模板即完全对齐；
- **(b)** 强制校验模板恰好声明 0~415，缺失/越界直接拒绝并提示 Excel 行号。

### 第二组：正确性与可观测性

4. `read_byte` 失败改为显式报错或返回 `(ok, value)`，不再以 `0` 记账（M-8）
5. `open_mtp_en` / `close_mtp_en` 去掉 `except`，让失败真正弹窗（M-9）
6. 校验结果除与 `buffer` 比对外，**追加一次 CRC 判定**：读回 0~415 → 算 CRC → 与 507~510 比对，并明确提示"CRC 一致/不一致"（当前只有人工看 Excel）
7. 消歧文案：`全片擦写+校验` 完成弹窗应写明"逐字节落位校验通过"，与 CRC 校验区分

### 第三组：性能与体验

8. 校验按 word 成组读（4 字节/次），I2C 事务从 420 降到 ~105
9. 逐字节日志改为按扇区/每 N 条汇总，日志量从 420 行降到 ~15 行
10. 长任务移出主线程（工作线程 + `queue` + `after` 轮询），去掉 `update()` 泵消息

### 第四组：结构清理

11. 补上「批量执行」按钮，或删除不可达的 `import_and_execute_mtp_batch`（M-10）
12. 删除失效的 `crc_start/crc_end` 与旧版 `mtp_full_program_with_crc`（M-11、M-15）
13. 修正扇区导出的 INDEX 列（M-12）
14. 统一工作表选择逻辑（M-13）

---

## 六、附录：验证方法与实测数据

> **§6.1 / §6.2 为修复前（2026-10-14 之前）的基准测量**，用于定位缺陷；修复后的验收结果见 §5 第一组。
> 两处使用的模板不同（§6.1/6.2 为构造模板，§5 为验收模板），故 CRC 数值不可直接对比。

### 6.1 直接调用生产函数（修复前）

```python
from mtp_batch_ops import MTPBatchLogic
from iic_ops import IICLogic
ok, buf, addrs, crc, err = MTPBatchLogic.mtp_full_program_with_crc_gui(
    path, None, None, None, IICLogic.parse_hex_byte, print)
```

模板 A（INDEX 1~103 → 地址 4~415；INDEX 104~126 → 地址 416~507）：

```
成功=True  CRC=0xDF4893FC  烧录地址数=504  区间 4~507
   → 416 及以上被计入烧录集合的字节数 = 92
   → CRC 存储字节 buffer[507:511] = [252, 147, 72, 223]      # 小端正确 ✅
```

模板 B（单行 `PROGRAM, ADDR=509`）：

```
成功=False  err=bytearray index out of range                 # 缝隙越界证实 ❌
```

### 6.2 四种 CRC 口径仿真（C.2）

构造含直烧区的完整模板（数据区 4~415 + 直烧区 416~504 + 模板 CRC 行）：

| 口径 | CRC32 | 与规定 |
|---|---|---|
| 规定：`phys < 416` | `0x997C72C8` | — |
| 现生产：全部声明集合 | `0x1BDB7BE3` | ❌ |
| 连续 `buffer[0:416]` | `0xF7755BFD` | ❌ |
| 现回读：固定 4~315 | `0x95A161D4` | ❌ |

补充验证（不含直烧区的模板，即全部地址 < 416）：

**此时"现生产"与"规定"相等** —— 这解释了该缺陷为何能长期存活：只要模板不声明 416 以上的行，两种口径偶然一致，问题被完全掩盖。

### 6.3 口径决策（已定稿 2026-10-14）

| 问题 | 决策 |
|---|---|
| **416 是否含端点** | `phys < 416`（地址 0~415），与 [CRC_Test.py](ocs_2361_python_code/IICtool_v6/CRC_Test.py) 的 `CRC_ADDR_LIMIT = 416` 一致 |
| **"0~416" 是连续区间还是"声明行集合"** | **声明行集合** —— 只计入模板中声明为 `PROGRAM` 且 `phys < 416` 的行字节，按地址升序去重拼接 |

**为何不选连续区间**：连续读法会在 buffer 中取到**未声明的字节**（默认模板从 INDEX=1 起，地址 0~3 永不声明 → `buffer[0:4] = 0x00`，而芯片擦除后为 `0xFF`）→ CRC 输入含芯片上不存在的字节，读回永远复现不了；且会与 `CRC_Test.py` 的 ★ 主输出产生不同值，使既有的工具/烧录器交叉核对失效。

**该决策的代价**（已接受）：模板有空洞时，烧录侧（声明集合）与回读侧（固定 0~415）的 CRC 仍会不等。这是口径本身的固有代价 —— 回读无法从芯片反推原模板声明了哪些行。如需闭环，见 §5「遗留项」。

---

## 附：与既有文档的关系

| 文档 | 定位 |
|---|---|
| [MTP批量模板CRC校验分析.md](ocs_2361_python_code/IICtool_v6/MTP批量模板CRC校验分析.md) | CRC 口径定稿 + 算法鉴定 + 场景仿真（**口径来源**） |
| [CRC_Test.py](ocs_2361_python_code/IICtool_v6/CRC_Test.py) | 口径的可运行落地版，纯计算不改文件（**参考实现**） |
| **本文件** | MTP 批量**执行链路**的代码级剖析、偏差定位、修复方案与实施记录 |
| [分析报告.md](分析报告.md) | 全代码库架构与问题总览 |
| [代码架构.md](代码架构.md) | 既有的架构说明（**注意**：其中的 `iic_ops` 模块级函数与 `mtp_full_program_with_crc` 旧版描述已与当前代码不符） |
