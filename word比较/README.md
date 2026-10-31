# wordcompare — Word 文档比较引擎

对两份 `.docx` 做**结构级**比较：先对齐段落/表格，再在字符级标出文字修改，最后单独标注格式变化。
输出三种产物：**HTML 差异报告**、**带修订的 .docx**、**统计摘要**。

不依赖 Office，可在 Windows / Linux / 容器里运行。

---

## 快速开始

```bash
pip install -r requirements.txt

# 直接跑（无需安装包）
python run.py 旧稿.docx 新稿.docx -o output

# 或安装后使用命令行
pip install -e .
wordcompare 旧稿.docx 新稿.docx -o output
```

产物位于 `output/`：

| 文件 | 说明 |
| --- | --- |
| `<A>__vs__<B>.html` | 单文件双栏报告，内嵌 CSS/JS，可直接发邮件或归档 |
| `<A>__vs__<B>.md` / `.csv` | 统计摘要与变更明细 |
| `<A>__vs__<B>.修订.docx` | 可在 Word 中逐条接受/拒绝的修订稿 |

常用参数：

```bash
--threshold 0.5            # 块相似度阈值，越高越倾向"改判为新增+删除"
--ignore-case              # 忽略大小写
--normalize-punctuation    # 全角/半角标点视为相同
--keep-whitespace          # 不折叠空白
--no-text                  # 不比较文字，只报告格式变化
--no-format                # 不比较格式
--no-tables                # 不比较表格
--ignore-inherited         # 忽略"未设置 vs 显式设置"的格式差异
--no-html / --no-redline / --no-summary
--author "张三"            # 修订稿里的作者名
```

### 比较范围

文本 / 格式 / 表格 三项可独立开关（命令行用上面的 `--no-*`，图形界面是"比较范围"里的三个复选框）：

| 范围 | 关闭后的行为 |
| --- | --- |
| 文本 | 不再报告文字增删改，只保留格式变化；文字不同的段落因无法逐字对齐格式，整体忽略 |
| 格式 | 不报告加粗、字号、颜色、对齐、编号等格式差异 |
| 表格 | 表格整块不参与比较，只比较正文段落 |

默认三项全开，与完整比较等价。至少保留一项，否则没有可报告的内容。

---

## 设计思路：三层 diff

Word 比较的本质不是"全文跑一次 diff"，而是：

```
docx → 归一化 → 块切分/指纹 → 块级对齐 → 行内 diff → 格式 diff → 聚合渲染
                                    ↑ 表格递归走同一套流程
```

1. **块级对齐**：先用哈希做精确 LCS，把未变内容 100% 锚定，同时把文档切成若干"空洞"；
   再在每个空洞内做带相似度门槛的 DP 对齐，吸收"改了一句话"的局部变动。
   直接全文做字符 diff，一处改动会让后面所有文字错位，报告会变成大片无意义的红绿块。
2. **行内 diff**：只对判定为"修改"的块做字符级 diff（中文按字符、英文按词），
   并做语义清理，把"删一个词又加一个词"合并为一次替换。
3. **格式 diff**：只在文字相同的位置比较 run 属性，避免和文字修改重复计数。

---

## 打包与分发

```bash
python build_exe.py            # 单文件 dist/WordCompare.exe
python build_exe.py --onedir   # 便携目录 dist/WordCompare-便携版/ + 同名 zip
```

两种产物功能相同，区别只在启动方式：

| 产物 | 适用场景 |
| --- | --- |
| `WordCompare.exe`（单文件） | 只发一个文件最省事；每次启动需把运行时解压到 `%TEMP%`，启动稍慢 |
| `WordCompare-便携版.zip`（目录） | **发给别人更稳**：不解压到临时目录，不受临时目录策略 / 杀软拦截影响，启动也更快 |

发给别人时如果"双击没反应"，按顺序排查：

1. **文件被系统标记为"来自网络"**：右键 exe → 属性 → 勾选底部的"解除锁定" → 确定。
2. **SmartScreen 拦截**（"Windows 已保护你的电脑"）：点"更多信息" → "仍要运行"。
3. **杀软误报**：PyInstaller 打包的未签名程序常被误判，需要把程序加入信任/白名单。
   已用 `--noupx` 和版本信息资源降低误报率，但无法完全避免（没有代码签名证书）。
4. **换用便携版**：`python build_exe.py --onedir`，把 zip 发过去解压运行。
5. **系统位数 / 版本**：产物是 64 位程序，需要 Windows 10 及以上（Python 3.13 不支持 Win7/8）。

程序启动后会把每次启动和任何启动期异常写入
`%LOCALAPPDATA%\WordCompare\启动日志.txt`，并弹出错误框。
对方说"打不开"时，让他把这个文件发回来即可定位：日志不存在 = 程序根本没被执行（被拦），
有报错 = 是程序自身的问题。

---

## 目录结构

```
src/wordcompare/
├─ model.py              # 数据模型：Paragraph / Table / DiffItem / DiffResult
├─ options.py            # 所有影响判定结果的开关
├─ normalize.py          # 归一化（只影响 norm，不影响展示文本）
├─ engine.py             # compare(left, right, options) -> DiffResult
├─ cli.py                # 命令行
├─ reader/
│  ├─ docx_reader.py     # zip + python-docx + lxml → 模型
│  ├─ flatten.py         # run 抽取（穿透 ins/hyperlink/sdt/域）
│  ├─ numbering.py       # 自动编号还原（计数器 + 多级模板）
│  └─ styles.py          # 样式继承解析（rStyle / pStyle）
├─ align/
│  ├─ fingerprint.py     # 哈希 + 分词 + 相似度
│  ├─ block_align.py     # 两段式块对齐（精确锚点 + 空洞 DP）
│  └─ table_align.py     # 行级对齐 + 单元格递归
├─ diff/
│  ├─ inline_diff.py     # diff-match-patch + 语义清理
│  └─ format_diff.py     # run / 段落格式差异
├─ output/
│  ├─ html_report.py     # 双栏报告
│  ├─ docx_redline.py    # 修订稿写回
│  └─ summary.py         # Markdown / CSV
└─ templates/report.html.j2
```

三个输出器彼此独立，可直接调用：

```python
from wordcompare import compare, Options
from wordcompare.output.html_report import write_report
from wordcompare.output.docx_redline import write_redline
from wordcompare.output.summary import write_summary

result = compare("旧稿.docx", "新稿.docx", Options(similarity_threshold=0.55))
print(result.stats.to_dict())

write_report(result, "out/report.html")
write_summary(result, "out/summary.md", "out/summary.csv")
write_redline(result, "out/redline.docx")
```

---

## 三个关键决策

**1. 自动编号不参与正文比较。**
编号不在 `w:t` 里，而是由 `numbering.xml` + 计数器决定。若把"1."算进正文，
插入一条列表项会导致其后所有条目被判为"修改"。这里把编号还原成展示前缀，
只比较正文，编号层级变化作为段落格式变更单独报告。

**2. 字符样式继承会被解析。**
`w:rStyle="Strong"` 和直接加粗在视觉上完全相同。读取时把段落样式 / 字符样式里
继承的格式补到 run 上，避免这类假阳性。

**3. 修订稿以"新稿"为底稿打补丁。**
重新生成文档会丢掉主题、表格宽度、页眉页脚等原始信息。
因此修订稿复用读取阶段那棵 XML 树，只在需要的位置插入 `w:ins` / `w:del`。

**验收不变量**：修订稿在 Word 中"接受全部修订"后，必须与新稿完全一致。
测试里用 XML 级模拟接受全部修订来校验这一点。

---

## 测试

```bash
python tests/test_engine.py
```

覆盖：块对齐与统计、仅格式变化（加粗）、自动编号稳定性、HTML 报告、
统计摘要、修订稿接受后一致性、同一文档无差异、相似度阈值行为。

样本由 `tests/make_fixtures.py` 动态生成，不依赖二进制固定资产。

---

## 已知限制

| 项 | 现状 |
| --- | --- |
| 页眉/页脚/脚注/批注 | 未纳入比较 |
| 文本框、艺术字 | 仅当作图片存在性处理 |
| 图片 | 按内容哈希判断增删/替换，不做像素级比较 |
| 合并单元格 | 能读取，但跨行单元格内部的精细对齐有限 |
| 修订稿中的超链接/图片 | 被修改的段落重写时只保留文字与文字格式 |
| 旧版 `.doc` | 不支持，需先另存为 `.docx`（可用 LibreOffice headless 批量转换） |
| 移动检测 | 预留 `Options.detect_move`，当前按"删除 + 新增"处理 |

---

## 路线图

- **M1（已完成）** 读取 + 段落对齐 + 行内 diff + HTML 报告
- **M2（已完成）** 表格、编号、格式 diff、统计摘要
- **M3（已完成）** 正文与表格的修订 docx，接受修订后与新稿一致
- **M4** 移动检测、页眉页脚、脚注、批量比较、Web 服务化
