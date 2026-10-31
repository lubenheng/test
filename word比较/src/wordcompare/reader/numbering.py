"""自动编号还原。

自动编号不在 ``w:t`` 文本里，而是由 numbering.xml + 段落引用 + 计数器共同
决定。比较时如果把编号前缀算进正文，插入一个列表项会导致它后面所有条目
都被判为"修改"，因此这里把编号还原成展示前缀，但不参与正文比较。
"""

from __future__ import annotations

from .flatten import qn

_ROMAN = [
    (1000, "M"), (900, "CM"), (500, "D"), (400, "CD"),
    (100, "C"), (90, "XC"), (50, "L"), (40, "XL"),
    (10, "X"), (9, "IX"), (5, "V"), (4, "IV"), (1, "I"),
]

_CN_DIGITS = "零一二三四五六七八九"


def _roman(n: int) -> str:
    out = []
    for value, sym in _ROMAN:
        while n >= value:
            out.append(sym)
            n -= value
    return "".join(out)


def _letters(n: int, upper: bool) -> str:
    # 1 -> A, 26 -> Z, 27 -> AA
    if n <= 0:
        n = 1
    out = []
    while n > 0:
        n, rem = divmod(n - 1, 26)
        out.append(chr((ord("A") if upper else ord("a")) + rem))
    return "".join(reversed(out))


def _chinese(n: int) -> str:
    if n <= 0:
        return str(n)
    if n < 10:
        return _CN_DIGITS[n]
    if n < 20:
        return "十" + (_CN_DIGITS[n % 10] if n % 10 else "")
    if n < 100:
        tens, ones = divmod(n, 10)
        return _CN_DIGITS[tens] + "十" + (_CN_DIGITS[ones] if ones else "")
    return str(n)


def format_number(value: int, fmt: str) -> str:
    fmt = (fmt or "decimal").lower()
    if fmt in ("none",):
        return ""
    if fmt in ("bullet",):
        return ""
    if fmt == "decimalzero":
        return f"{value:02d}"
    if fmt in ("decimal", "decimalhalfwidth", "decimalfullwidth"):
        return str(value)
    if fmt == "lowerletter":
        return _letters(value, False)
    if fmt == "upperletter":
        return _letters(value, True)
    if fmt == "lowerroman":
        return _roman(value).lower()
    if fmt == "upperroman":
        return _roman(value)
    if fmt in ("chinesecounting", "chinesecountingthousand", "ideographdigital",
               "chinesecountingnotenthousand", "taiwanesecounting"):
        return _chinese(value)
    if fmt == "ideographtraditional":
        return _chinese(value)
    if fmt == "japanesecounting":
        return _chinese(value)
    return str(value)


class NumberingResolver:
    """按文档顺序推进的编号计数器。"""

    def __init__(self, numbering_root, styles_root):
        self.abstract: dict[str, dict[int, dict]] = {}
        self.num: dict[str, dict] = {}
        self.style_numpr: dict[str, tuple[str, int]] = {}
        self.counters: dict[str, dict[int, int]] = {}

        if numbering_root is not None:
            self._parse_numbering(numbering_root)
        if styles_root is not None:
            self._parse_styles(styles_root)

    # ---------------------------------------------------------------- parse
    def _parse_numbering(self, root) -> None:
        for absn in root.findall(qn("w:abstractNum")):
            aid = absn.get(qn("w:abstractNumId"))
            levels = {}
            for lvl in absn.findall(qn("w:lvl")):
                try:
                    ilvl = int(lvl.get(qn("w:ilvl")))
                except (TypeError, ValueError):
                    continue
                levels[ilvl] = self._parse_level(lvl)
            self.abstract[aid] = levels

        for num in root.findall(qn("w:num")):
            nid = num.get(qn("w:numId"))
            abs_ref = num.find(qn("w:abstractNumId"))
            overrides = {}
            for ov in num.findall(qn("w:lvlOverride")):
                try:
                    ilvl = int(ov.get(qn("w:ilvl")))
                except (TypeError, ValueError):
                    continue
                start = ov.find(qn("w:startOverride"))
                if start is not None:
                    try:
                        overrides[ilvl] = int(start.get(qn("w:val")))
                    except (TypeError, ValueError):
                        pass
            self.num[nid] = {
                "abstract": abs_ref.get(qn("w:val")) if abs_ref is not None else None,
                "overrides": overrides,
            }

    @staticmethod
    def _parse_level(lvl) -> dict:
        start_el = lvl.find(qn("w:start"))
        fmt_el = lvl.find(qn("w:numFmt"))
        text_el = lvl.find(qn("w:lvlText"))
        try:
            start = int(start_el.get(qn("w:val"))) if start_el is not None else 1
        except (TypeError, ValueError):
            start = 1
        return {
            "start": start,
            "fmt": fmt_el.get(qn("w:val")) if fmt_el is not None else "decimal",
            "text": text_el.get(qn("w:val")) if text_el is not None else "",
        }

    def _parse_styles(self, root) -> None:
        for st in root.findall(qn("w:style")):
            sid = st.get(qn("w:styleId"))
            if not sid:
                continue
            pPr = st.find(qn("w:pPr"))
            if pPr is None:
                continue
            numPr = pPr.find(qn("w:numPr"))
            if numPr is None:
                continue
            nid = numPr.find(qn("w:numId"))
            ilvl = numPr.find(qn("w:ilvl"))
            if nid is None:
                continue
            self.style_numpr[sid] = (
                nid.get(qn("w:val")),
                int(ilvl.get(qn("w:val"))) if ilvl is not None else 0,
            )

    # --------------------------------------------------------------- lookup
    def style_numbering(self, style_id: str) -> tuple[str, int] | None:
        return self.style_numpr.get(style_id)

    def _level(self, num_id: str, ilvl: int) -> dict:
        info = self.num.get(num_id) or {}
        aid = info.get("abstract")
        levels = self.abstract.get(aid, {}) if aid else {}
        lvl = levels.get(ilvl)
        if lvl is None and levels:
            # 缺失该层级时退化为最接近的层级定义
            nearest = min(levels, key=lambda k: abs(k - ilvl))
            lvl = levels[nearest]
        if lvl is None:
            return {"start": 1, "fmt": "decimal", "text": f"%{ilvl + 1}."}
        start_override = (info.get("overrides") or {}).get(ilvl)
        if start_override is not None:
            lvl = dict(lvl, start=start_override)
        return lvl

    # -------------------------------------------------------------- counters
    def next_prefix(self, num_id: str, ilvl: int) -> str:
        counters = self.counters.setdefault(num_id, {})
        for lv in [k for k in counters if k > ilvl]:
            del counters[lv]
        if ilvl in counters:
            counters[ilvl] += 1
        else:
            counters[ilvl] = self._level(num_id, ilvl)["start"]

        level = self._level(num_id, ilvl)
        template = level["text"] or f"%{ilvl + 1}."
        fmt = level["fmt"]

        def repl(match) -> str:
            idx = int(match.group(1)) - 1
            if idx not in counters:
                counters[idx] = self._level(num_id, idx)["start"]
            return format_number(counters[idx], self._level(num_id, idx)["fmt"])

        import re
        out = re.sub(r"%(\d)", repl, template)
        out = out.replace("\t", " ").strip()
        if fmt in ("bullet", "none"):
            return out
        if out and not out.endswith((" ", ".", "、", "。", "）", ")", ":")):
            out += " "
        elif out and not out.endswith(" "):
            out += " "
        return out
