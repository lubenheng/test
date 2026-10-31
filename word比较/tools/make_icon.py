"""生成 exe 图标：纯绘制，不依赖外部素材。

图案：深蓝底 + 左右两张文档页，左页红色删改条、右页绿色新增条，
中间用 ⇄ 表示"比较"。
"""

from __future__ import annotations

from pathlib import Path

from PIL import Image, ImageDraw

SIZE = 256
BG = (37, 99, 235)
PAPER = (255, 255, 255)
DEL = (220, 38, 38)
INS = (22, 163, 74)
LINE = (203, 213, 225)


def _rounded(draw, box, radius, fill):
    draw.rounded_rectangle(box, radius=radius, fill=fill)


def build(path: str | Path) -> Path:
    path = Path(path)
    img = Image.new("RGBA", (SIZE, SIZE), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    _rounded(draw, (8, 8, SIZE - 8, SIZE - 8), 46, BG + (255,))

    # 两张纸
    left = (36, 52, 122, 204)
    right = (134, 52, 220, 204)
    for box in (left, right):
        draw.rectangle((box[0] + 5, box[1] + 7, box[2] + 5, box[3] + 7),
                       fill=(30, 64, 175, 90))          # 阴影
        draw.rectangle(box, fill=PAPER + (255,))

    # 左页：灰色正文行 + 红色删除条
    y = left[1] + 16
    for i in range(5):
        width = 52 if i % 2 else 66
        color = DEL if i == 2 else LINE
        draw.rectangle((left[0] + 12, y, left[0] + 12 + width, y + 8), fill=color)
        if i == 2:
            draw.rectangle((left[0] + 12, y + 3, left[0] + 12 + width, y + 6),
                           fill=PAPER)
        y += 26

    # 右页：灰色正文行 + 绿色新增条
    y = right[1] + 16
    for i in range(5):
        width = 52 if i % 2 else 66
        color = INS if i == 2 else LINE
        draw.rectangle((right[0] + 12, y, right[0] + 12 + width, y + 8), fill=color)
        y += 26

    # 中间箭头
    cx, cy = SIZE // 2, SIZE - 40
    draw.line((cx - 26, cy, cx + 26, cy), fill=PAPER, width=9)
    draw.polygon([(cx + 26, cy - 15), (cx + 26, cy + 15), (cx + 44, cy)], fill=PAPER)
    draw.polygon([(cx - 26, cy - 15), (cx - 26, cy + 15), (cx - 44, cy)], fill=PAPER)

    path.parent.mkdir(parents=True, exist_ok=True)
    img.save(path, sizes=[(256, 256), (128, 128), (64, 64), (48, 48), (32, 32), (16, 16)])
    return path


if __name__ == "__main__":
    print(build("assets/wordcompare.ico"))
