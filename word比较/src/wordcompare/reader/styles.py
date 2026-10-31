"""样式解析：把 run 上"继承"来的格式还原成有效值。

不做这一步时，``w:rStyle="Strong"``（加粗来自字符样式）和直接加粗会被
判成不同，产生大量假阳性。
"""

from __future__ import annotations

from .flatten import _run_props, qn


class StyleResolver:
    def __init__(self, styles_root):
        self._rpr: dict[str, dict] = {}
        self._based: dict[str, str | None] = {}
        self._cache: dict[str, dict] = {}
        if styles_root is not None:
            self._parse(styles_root)

    def _parse(self, root) -> None:
        for style in root.findall(qn("w:style")):
            sid = style.get(qn("w:styleId"))
            if not sid:
                continue
            based = style.find(qn("w:basedOn"))
            self._based[sid] = based.get(qn("w:val")) if based is not None else None
            rPr = style.find(qn("w:rPr"))
            self._rpr[sid] = _run_props(rPr) if rPr is not None else {}

    def run_props(self, style_id: str | None) -> dict:
        if not style_id:
            return {}
        if style_id in self._cache:
            return self._cache[style_id]

        chain: list[str] = []
        seen: set[str] = set()
        current: str | None = style_id
        while current and current not in seen:
            seen.add(current)
            chain.append(current)
            current = self._based.get(current)

        merged: dict = {}
        for sid in reversed(chain):          # 从根样式往下覆盖
            for key, value in self._rpr.get(sid, {}).items():
                if value is not None:
                    merged[key] = value
        self._cache[style_id] = merged
        return merged
