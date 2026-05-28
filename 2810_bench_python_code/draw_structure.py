"""Draw the project directory structure as an image."""
import os
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent

# Directories and files to exclude
EXCLUDE_DIRS = {'__pycache__', '.git', 'node_modules', '.egg-info', '__init__.py'}
EXCLUDE_SUFFIXES = {'.pyc'}
EXCLUDE_DIR_PATTERNS = {
    'instrument/debug/Devices',
    'instrument/debug/JFlash.exe',
    'instrument/debug/JLink.exe',
    'instrument/debug/JLinkDevices.xml',
    'instrument/debug/SA32A12_RESET_TEST.txt',
    'instrument/debug/readme.docx',
    'instrument/platform/USBIOX.DLL',
    'instrument/loader/USBIOX.DLL',
    'test_configs/~$',
    'test_configs/OCS-QA-Q2-010-07-OCD2810',
}

def should_exclude(path: Path) -> bool:
    rel = path.relative_to(PROJECT_ROOT).as_posix()
    if any(part.startswith('.') and part != '.' for part in path.parts):
        return True
    if any(p in EXCLUDE_DIRS for p in path.parts):
        return True
    if path.suffix in EXCLUDE_SUFFIXES:
        return True
    if any(rel.startswith(pat) for pat in EXCLUDE_DIR_PATTERNS):
        return True
    return False

def build_tree(paths):
    """Build a nested dict tree from file paths."""
    tree = {}
    for p in paths:
        parts = p.relative_to(PROJECT_ROOT).parts
        node = tree
        for part in parts[:-1]:
            node = node.setdefault(part, {})
        node[parts[-1]] = None  # file
    return tree

def collect_paths():
    paths = []
    for root, dirs, files in os.walk(PROJECT_ROOT):
        root_path = Path(root)
        # Skip excluded dirs
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS and not d.startswith('.')]
        for f in files:
            p = root_path / f
            if not should_exclude(p):
                paths.append(p)
    return sorted(paths)

def draw_tree(ax, tree, x, y, dx, dy, depth=0, parent_text=None):
    """Recursively draw tree."""
    items = sorted(tree.items(), key=lambda kv: (kv[1] is None, kv[0]))
    total = len(items)

    for i, (name, subtree) in enumerate(items):
        is_file = subtree is None
        cy = y - i * dy

        color = '#4A90D9' if is_file else '#E67E22'
        fontsize = 7 if is_file else 8
        fontweight = 'normal' if is_file else 'bold'

        # Draw connecting line from parent
        if parent_text is not None:
            ax.plot([x - dx, x], [cy, cy], color='#CCCCCC', linewidth=0.8)
            ax.plot([x, x], [cy, y], color='#CCCCCC', linewidth=0.8)

        # Icon (use simple markers to avoid font issues)
        label = f"  {name}"

        ax.text(x, cy, label, fontsize=fontsize, fontweight=fontweight,
                color=color, verticalalignment='center',
                fontfamily='DejaVu Sans')

        # Draw a marker for file vs directory
        marker = 'o' if is_file else 's'
        msize = 5 if is_file else 7
        ax.plot(x - 0.15, cy, marker, color=color, markersize=msize, zorder=5)

        if not is_file and subtree:
            # Draw vertical line for children
            children_y = [y - j * dy for j in range(total)]
            ax.plot([x, x], [children_y[0], children_y[-1]],
                    color='#DDDDDD', linewidth=1)

            nx = x + dx * 1.8
            draw_tree(ax, subtree, nx, cy, dx * 1.5, dy, depth + 1, name)

def main():
    paths = collect_paths()
    tree = build_tree(paths)

    # Count items for sizing
    def count_leaves(t):
        if t is None:
            return 1
        return sum(count_leaves(v) for v in t.values())

    leaf_count = count_leaves(tree)

    # Figure sizing
    height = max(8, leaf_count * 0.35)
    width = max(12, height * 1.5)

    fig, ax = plt.subplots(figsize=(width, height))
    ax.set_xlim(0, width)
    ax.set_ylim(-height * 0.1, height * 0.1 + leaf_count * 0.35)
    ax.axis('off')
    ax.set_facecolor('#FAFAFA')
    fig.patch.set_facecolor('#FAFAFA')

    # Title
    ax.text(width / 2, leaf_count * 0.35 + 0.3, '2810_Project Structure',
            fontsize=18, fontweight='bold', ha='center', va='bottom',
            color='#2C3E50')

    # Legend
    legend_elements = [
        mpatches.Patch(color='#E67E22', label='Directory'),
        mpatches.Patch(color='#4A90D9', label='File'),
    ]
    ax.legend(handles=legend_elements, loc='upper right',
              framealpha=0.9, fontsize=9)

    # Draw
    start_x = 0.8
    start_y = leaf_count * 0.35 - 0.5
    draw_tree(ax, tree, start_x, start_y, 1.5, 0.35)

    # Style
    for spine in ax.spines.values():
        spine.set_visible(False)

    plt.tight_layout()
    output_path = PROJECT_ROOT / 'project_structure.png'
    plt.savefig(output_path, dpi=150, bbox_inches='tight', facecolor=fig.get_facecolor())
    plt.close()
    print(f"Structure chart saved to: {output_path}")

if __name__ == '__main__':
    main()
