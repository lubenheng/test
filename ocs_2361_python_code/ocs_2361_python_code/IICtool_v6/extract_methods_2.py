import ast, os

def get_args(node):
    args = [a.arg for a in node.args.args]
    if node.args.vararg: args.append('*' + node.args.vararg.arg)
    if node.args.kwarg: args.append('**' + node.args.kwarg.arg)
    return ", ".join(args)

with open("arch_out.txt", "w", encoding="utf-8") as out:
    for f in os.listdir('.'):
        if f.endswith('.py') and f != 'extract_methods_2.py' and f != 'extract_methods.py':
            try:
                tree = ast.parse(open(f, encoding='utf-8').read())
                out.write(f'\n### {f}\n')
                for n in tree.body:
                    if isinstance(n, ast.FunctionDef):
                        out.write(f'- `def {n.name}({get_args(n)})`\n')
                    elif isinstance(n, ast.ClassDef):
                        out.write(f'- `class {n.name}:`\n')
                        for m in n.body:
                            if isinstance(m, ast.FunctionDef):
                                out.write(f'  - `def {m.name}({get_args(m)})`\n')
            except Exception as e:
                pass