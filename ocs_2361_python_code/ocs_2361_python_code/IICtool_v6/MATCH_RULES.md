# 匹配规则说明（MATCH_RULES.md）

## **概览**

此文档描述 `convert_2361_to_motor.py` 中的匹配逻辑、输入/输出列含义、匹配优先级与回退策略，便于阅读源码或对模板/配置进行调整。

参见实现： [convert_2361_to_motor.py](convert_2361_to_motor.py#L1-L400)、[config.py](config.py#L1-L300)、[convert_report.csv](convert_report.csv#L1-L20)。

## **输出模板列（Template Columns）**

- **`OPERATION` / `op`**: 输出操作列。代码变量：`cols['op']`。查找关键字：`operation`, `op`, `操作`。默认列号：1。脚本在覆盖/追加时会把该列写为 `PROGRAM`。
- **`ADDR(基地址)` / `addr`**: 模块基地址（MTP 地址空间基地址）。代码变量：`cols['addr']`。关键字：`addr`, `基地址`, `地址`。默认列号：2。匹配时用于 `addr + index` 方式的比较。
- **`INDEX(Word偏移)` / `index`**: 逻辑序号（word 偏移），脚本内表示 Word 偏移（计算物理地址时乘 4）。代码变量：`cols['index']`。关键字：`index`, `偏移`, `word`。默认列号：3。
- **字节列 `31:24` / `23:16` / `15:8` / `7:0`**: 分别对应 `cols['b4']`,`cols['b3']`,`cols['b2']`,`cols['b1']`，默认列号 4..7。脚本会用 `to_hex_byte()` 将源值标准化为 2 位大写十六进制字符串再写入模板。
- **`寄存器index` / `regindex`**: 目标模板中的寄存器序号（或索引列），代码变量：`cols['regindex']`。关键字：`寄存器index`, `寄存器 index`, `regindex`。默认列号：11。用于更精确的匹配（同模块时与 module_addr 一起使用）。
- **`寄存器模块` / `regmodule`**: 目标模板中的模块标识列，代码变量：`cols['regmodule']`。关键字：`寄存器模块`, `模块`。默认列号：12。写入值为规范模块键（如 `motor_top` 或 `driver`），或源文件的模块候选字符串。
- **`寄存器模块地址(WORDS)` / `module_addr`**: 目标模板中表示模块地址编号（非 MTP 基地址），代码变量：`cols['module_addr']`。关键字：`寄存器模块地址`, `模块地址`, `words`。默认列号：13。此列与 `MODULE_MAP.word_addr` 对应（见 `config.py`）。
- **`MATCHED` / `matched`**: 可选列，用于标记是否匹配到模块（`YES` / `NO`）。代码变量：`cols['matched']`。关键字：`matched`, `匹配`, `是否匹配`。若模板无该列，脚本会在末尾追加一列并重新计算列索引。

说明：`cols['header_len']` 表示模板表头的列数，脚本用它在 `append` 时构建新行长度。

## **源数据列（Source columns / 输入表约定）**

- 源行必须包含 4 个字节字段（脚本在第 4..7 列查找字节）。即脚本期望源行至少有 7 列（1-based 列号）：
  - 列 4: 源字节 `[31:24]`（脚本中映射为 `b4`）
  - 列 5: 源字节 `[23:16]`（`b3`）
  - 列 6: 源字节 `[15:8]`（`b2`）
  - 列 7: 源字节 `[7:0]`（`b1`）
- 源表的第 2 列（B 列）被解释为 `src_word`（脚本变量名），通过 `as_int_or_none()` 转为整数后：
  - 若数值存在且与 `config.MODULE_MAP[*]['word_addr']` 匹配，则得到 `module_key`（规范模块名）与 `module_info`。
  - 如果 `MODULE_MAP` 中无匹配，脚本把 `src_word` 作为 `base_addr` 的候选值（回退）。
- 源表的第 3 列（C 列）被优先当作 `index_candidate`（Word 偏移）。若第 3 列不可用，则回退到第 2 列的数值作为 index。最终 `index_candidate` 若为 None 则设为 0。
- 脚本还会在第 8..12 列（H..L，1-based）扫描第一个非空单元作为 `module_candidate`（模块显示名称候选），用于覆盖 `regmodule` 当 `module_key` 不可用时显示来源信息。

## **匹配流程与优先级（Match flow）**

匹配总体策略：优先用模板中的 `module_addr`（即 `MODULE_MAP.word_addr`）+ `regindex` 精确匹配；若失败则回退到 `addr + index`（即 `base_addr` + `index_candidate`）匹配；若模板搜索均无结果则追加（append）。

具体步骤：
1. 读取源行，计算 `b4/b3/b2/b1`（使用 `to_hex_byte()`），获取 `src_word`（B列）、`index_candidate`（C列 或 B列 回退）、`module_candidate`（第 8..12 列首个非空）。
2. 用 `src_word` 匹配 `config.MODULE_MAP` 中的 `word_addr`，若匹配则得到 `module_key` 与 `module_info`（含 `base` 与 `word_addr`）。
3. 决定 `base_addr`：若 `module_info` 存在，使用 `module_info['base']`；否则若 `src_word` 为数字则用 `src_word`，否则回退为 0。
4. 通过 `REGISTER_MAP` 试图找到 `reg_index`：计算 `phys = base_addr + index_candidate * 4`，在 `REGISTER_MAP[canonical_module]['regs']` 的第三项（MTP 地址）中匹配 `phys`，如果找到则 `reg_index = idx`（索引位置），用于提高匹配成功率。
5. 在目标模板中查找匹配行（若存在模板文件 `电机.xlsx`，脚本会先打开模板文件以定位模板行）：
   - 优先匹配条件（精确匹配）：模板行的 `module_addr` 等于 `MODULE_MAP.word_addr` 且模板行的 `regindex` 等于 `index_candidate`（或等于上一步得到的 `reg_index`）。若命中则 `match_method = 'template_file_row'`。  
   - 回退匹配条件：模板行的 `addr` 等于 `base_addr` 且 `regindex` 等于 `index_candidate`；若命中则也视为 `template_file_row`。
6. 若读取模板文件失败或模板不存在，脚本会在输出工作簿（`out_ws`）内存副本中扫描，匹配逻辑相同，但 `match_method = 'out_ws_scan'`。
7. 如果找到匹配行（`found_cells` 不为 None）：脚本执行覆盖（overlay）：
   - 写入 `OPERATION = 'PROGRAM'`，`ADDR` 写为 `base_addr`；覆盖字节列 `b4..b1`（空源则写 None）；写入 `regindex`（`reg_index` 或 `index_candidate`）、写入 `regmodule`（`module_key` 或 `module_candidate`）、写入 `module_addr`（`module_info['word_addr']`）。并把 `matched` 标记为 `YES` 或 `NO`（取决于是否找到了 `module_key`）。
   - 记录 `before/after` 差异（`diffs`），并在 `trace` 与可选的 `report.csv` 中输出差异摘要。
8. 若未找到匹配行：脚本会 `append` 一行，长度为 `cols['header_len']`，并在相应列填入上述字段（`op/addr/index/b4..b1/regindex/regmodule/module_addr/matched`），`match_method = 'append'`。

常见的 `match_method` 值：
- `template_file_row`：在模板文件中定位到行并使用该行覆盖输出副本。
- `out_ws_scan`：在输出工作簿（内存）中扫描到匹配行（模板文件读取失败或模板不存在时）。
- `append`：未找到匹配行，新增一行。

## **辅助函数与行为细节**

- `find_header_cols(ws)`：根据表头第一行字符串关键词识别输出模板的列索引，支持的关键词见上面每列的说明；若未找到关键字则使用代码中的默认列位置（见 `or <n>`）。返回字典 `cols`。
- `find_module_key(name, register_map)`：对源字符串进行规范化（小写、去空格），先尝试与 `REGISTER_MAP` 的键精确匹配，再尝试子串匹配，最后使用一组内置别名映射（例如 `'motor' -> 'motor_top'`, `'foc' -> 'foc_top'`, `'diver' -> 'driver'` 等）来找到规范模块键。
- `to_hex_byte(v)`：把各种形式的数值/字符串规范化为两位大写十六进制字符串（例如 `15` -> `0F`, `0xA` -> `0A`），无法解析或为空时返回空字符串 `''`（覆盖写入时会写 `None` 从而清空目标单元）。
- `as_int_or_none(v)`：尝试把值解释为整数，支持十进制、十六进制（`0x..` 或纯十六进制字符）以及混合带注释字符串（会提取前导数字）。失败返回 `None`。

## **报告字段（`report.csv`）说明**

脚本可通过 `--report` 输出一份 CSV 报告（参见仓库中的 `convert_report.csv` 示例）。CSV 列如下：

- **`sheet`**: 源工作表名（例如 `批量操作模板`）。
- **`src_row`**: 源表中行号（1-based）。
- **`src_word`**: 源表 B 列（经 `as_int_or_none` 后的数值，用于与 `MODULE_MAP.word_addr` 匹配）。
- **`index_candidate`**: 选择的 index（优先 C 列，否则 B 列，最终回退为 0）。
- **`module_key`**: 通过 `MODULE_MAP` 或 `find_module_key()` 解析出的规范模块键（如 `motor_top`）；找不到时为空。
- **`module_candidate`**: 源表中搜到的模块名字符串候选（第 8..12 列首个非空值），仅作为显示或回退值。
- **`base_addr`**: 最终用于匹配或写入的基地址（来自 `MODULE_MAP.base` 或回退为 `src_word`）。
- **`reg_index`**: 如果能在 `REGISTER_MAP` 对应模块中解析到物理地址，则为该寄存器在 `regs` 列表中的索引；否则为空。
- **`match_method`**: 匹配到的方式（`template_file_row`、`out_ws_scan`、`append`）。
- **`found_row_idx`**: 在模板或输出工作簿中找到的行号（用于 overlay），`append` 时为新追加行号。
- **`matched_flag`**: 布尔，表示是否找到了 `module_key`（即是否解析出模块）。
- **`action`**: `overlay` 或 `append`，表示对输出模板采取的操作。
- **`diffs`**: 差异摘要，格式为 `col:old->new;...`。示例见仓库中 [convert_report.csv](convert_report.csv#L1-L10)。

## **示例**

- 覆盖（overlay）示例（摘自 `convert_report.csv`）:

  > `批量操作模板,10,1,0,motor_top,MOTOR,288,0,template_file_row,53,True,overlay,...`  
  说明：源行 B 列 `1` 对应 `MODULE_MAP['motor_top'].word_addr==1`，找到模板行（行号 53），对该行做 `overlay`。

- 追加（append）示例（摘自 `convert_report.csv`）:

  > `批量操作模板,4,1,32,motor_top,MOTOR,288,,append,85,True,append,...`  
  说明：未在模板中找到合适行，追加到输出工作簿第 85 行（`match_method='append'`）。

## **注意事项与建议**

- 如果模板 `电机.xlsx` 存在且列标题不规范，建议先在模板中把表头改为标准关键字（例如 `ADDR(基地址)` / `INDEX(Word偏移)` / `[31:24]` 等），以提高自动匹配成功率。
- `MODULE_MAP.word_addr` 应与模板中 `module_addr` 列使用的编号保持一致（否则会导致优先匹配失效）。配置位于 `config.py` 的 `MODULE_MAP` 节（参见 [config.py](config.py#L150-L220)）。
- 若源表有不同列布局，可在导入前用 Excel 把列对齐到脚本期望的位置（源字节位于 4..7，B 列为模块/word 值，C 列为索引）。脚本具有一定容错，但过度偏离会导致匹配失败并被追加。

---

文件位置：`MATCH_RULES.md`（已写入仓库根目录）。

若需要我把说明更改为“中文+英文并列”或把说明嵌入到 `README` 中，请告诉我需要的格式。 
