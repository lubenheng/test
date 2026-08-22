import pandas as pd

def load_excel_data(file_path="test_cases.xlsx"):
    """加载Excel数据, 返回数据表"""
    return pd.read_excel(file_path)

def get_params_by_test_name(test_name, df):
    """
    根据 测试项名称 匹配参数
    :param test_name: 测试项名称（如"定时器测试"）
    :param df: Excel数据表
    :return: 字典（包含所有参数）
    """
    row = df[df["测试项名称"] == test_name]
    if row.empty:
        raise ValueError(f"未找到测试项：{test_name}")
    
    return {
        "reg_name": row["寄存器名(唯一)"].values[0],
        "target_val": int(row["目标值"].values[0]),
        "min_val": int(row["最小值"].values[0]),
        "max_val": int(row["最大值"].values[0]),
        "unit": row["单位"].values[0]
    }

def get_32bit_reg_address(reg_name, df):
    """
    完全适配Excel寄存器表：A列=寄存器，B列=地址
    根据寄存器名，获取32位十六进制寄存器地址（整数类型，可直接用于J-Link写寄存器）
    """
    # ---------------------- 步骤1：校验Excel列名（和你的表头100%匹配） ----------------------
    required_cols = ["寄存器", "地址"]
    for col in required_cols:
        if col not in df.columns:
            raise ValueError(
                f"❌ Excel中未找到列名：{col}\n"
                f"你的Excel实际列名：{list(df.columns)}\n"
                "请确保Excel表头为：A列=「寄存器」，B列=「地址」！"
            )

    # ---------------------- 步骤2：匹配寄存器（处理空格，避免匹配失败） ----------------------
    # 清洗寄存器名：去掉前后空格，防止Excel里的空格导致匹配不上
    df["寄存器_清洗"] = df["寄存器"].astype(str).str.strip()
    reg_name_clean = str(reg_name).strip()
    
    # 按寄存器名筛选行
    row = df[df["寄存器_清洗"] == reg_name_clean]
    if row.empty:
        # 提示用户Excel中所有已有的寄存器名，方便排查拼写
        existing_regs = df["寄存器_清洗"].dropna().unique()
        raise ValueError(
            f"❌ 未找到寄存器：{reg_name_clean}\n"
            f"Excel中已有的寄存器名：{list(existing_regs)}\n"
            "请检查寄存器名拼写是否完全一致！"
        )

    # ---------------------- 步骤3：清洗地址数据（自动处理0x前缀） ----------------------
    # 提取B列地址，去掉空格、空值
    hex_addr = str(row["地址"].values[0]).strip()
    # 处理空值/NaN
    if not hex_addr or hex_addr.lower() in ["nan", "none", ""]:
        raise ValueError(f"❌ 寄存器 {reg_name_clean} 的地址为空，请检查Excel！")

    # 自动补0x前缀：不管地址是「0x4000141C」还是「4000141C」，都能正常转换
    if not hex_addr.startswith(("0x", "0X")):
        hex_addr = f"0x{hex_addr}"

    # ---------------------- 步骤4：转换为32位整数（符合芯片寄存器要求） ----------------------
    try:
        # 16进制转10进制整数，强制转为32位无符号整数（处理溢出）
        reg_addr = int(hex_addr, 16)
        reg_addr = reg_addr & 0xFFFFFFFF  # 确保是32位，符合芯片地址规范
        return reg_addr
    except ValueError as e:
        raise ValueError(
            f"❌ 寄存器 {reg_name_clean} 的地址格式错误！\n"
            f"Excel中的地址：{hex_addr}\n"
            f"错误原因：{str(e)}\n"
            "请确保地址是有效十六进制（如0x4000141C、4000141C）！"
        )