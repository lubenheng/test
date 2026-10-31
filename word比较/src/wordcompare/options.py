from dataclasses import dataclass


@dataclass
class Options:
    # --- 归一化（只影响相似度与相等判定，不影响展示文本） ---
    normalize_width: bool = True       # 全角字母/数字 → 半角
    normalize_punctuation: bool = False  # 中文标点 → 英文标点（默认关，可能是刻意修改）
    ignore_whitespace: bool = True     # 连续空白折叠为一个空格并去首尾
    ignore_case: bool = False

    # --- 比较范围（三项可独立开关） ---
    compare_text: bool = True          # 文字内容：行内 diff、段落/行的增删
    compare_tables: bool = True        # 表格：不比较时整块跳过
    format_diff: bool = True           # 格式：run / 段落格式

    # --- 对齐 ---
    similarity_threshold: float = 0.5  # 高于此值才认为"同一段被修改"
    max_dp_cells: int = 250_000        # DP 对齐的规模上限，超过后退化为贪心
    detect_move: bool = False          # 移动检测（M4）

    # --- 格式细节 ---
    report_inherited: bool = True      # "显式格式 vs 继承" 也视为格式变更

    # --- 产物 ---
    redline_author: str = "文档比较"
    redline_date: str | None = None    # ISO 时间，None 表示使用当前时间
