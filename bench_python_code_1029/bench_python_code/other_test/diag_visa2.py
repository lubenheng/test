import sys
import pyvisa


def show(backend_label, rm):
    try:
        resources = rm.list_resources()
    except Exception as e:
        print(f"  [{backend_label}] list_resources() 异常: {e}")
        return
    print(f"  [{backend_label}] 枚举到 {len(resources)} 个资源:")
    for r in resources:
        print(f"      - {r}")


print("=== 默认后端 (系统已装驱动, 如 NI-VISA) ===")
try:
    rm_default = pyvisa.ResourceManager()
    print("  后端信息:", rm_default)
    show("默认后端", rm_default)
    rm_default.close()
except Exception as e:
    print("  默认后端初始化失败:", e)

print()
print("=== @py 后端 (纯 Python pyvisa-py) ===")
try:
    rm_py = pyvisa.ResourceManager('@py')
    print("  后端信息:", rm_py)
    show("@py后端", rm_py)
    rm_py.close()
except Exception as e:
    print("  @py 后端初始化失败:", e)

print()
print("诊断完成。请将以上全部输出截图/复制发回。")
input("按回车键退出...")
