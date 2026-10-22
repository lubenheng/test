import ast, os
for f in os.listdir('.'):
    if f.endswith('.py'):
        try:
            tree = ast.parse(open(f, encoding='utf-8').read())
            print(f'
--- {f} ---')
            for node in tree.body:
                if isinstance(node, ast.FunctionDef):
                    print(f'Function: {node.name}')
                elif isinstance(node, ast.ClassDef):
                    print(f'Class: {node.name}')
                    for m in node.body:
                        if isinstance(m, ast.FunctionDef):
                            print(f'  Method: {m.name}')
        except Exception as e: print(f'Error parsing {f}: {e}')
