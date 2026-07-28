"""
测试结果记录工具
- 自动捕获print输出，保存到Save_Item/item_results/对应测试项工作表
- 记录pass/fail结果到"测试一览表"的B列
- 记录测试数据到"测试一览表"的C列
"""

import os
import sys
import shutil
import openpyxl

# 基于脚本所在目录构造保存路径
_script_dir = os.path.dirname(os.path.abspath(__file__))
SAVE_DIR = os.path.join(_script_dir, "..", "Save_Item", "item_results")
EXCEL_PATH = os.path.join(SAVE_DIR, "item_results.xlsx")
_TEMPLATE_PATH = os.path.join(_script_dir, "..", "Save_Results", "item_results.xlsx")


def _init_workbook():
    """确保保存目录和Excel文件存在，若不存在则从模板复制"""
    os.makedirs(SAVE_DIR, exist_ok=True)
    if not os.path.exists(EXCEL_PATH) and os.path.exists(_TEMPLATE_PATH):
        shutil.copy2(_TEMPLATE_PATH, EXCEL_PATH)


class TestContext:
    """
    测试上下文管理器，自动捕获print输出并记录结果到Excel。

    用法:
        with TestContext() as ctx:
            print("测试过程信息")  # 自动捕获
            ctx.record_result("UVLO_RSTH", "pass", 3.2)
    """

    def __init__(self):
        self.wb = None
        self._buffer = []
        self._original_stdout = None
        self._excel_path = EXCEL_PATH

    def __enter__(self):
        _init_workbook()
        try:
            self.wb = openpyxl.load_workbook(self._excel_path)
        except FileNotFoundError:
            print(f"错误: 未找到Excel模板文件 {self._excel_path}")
            self.wb = openpyxl.Workbook()
        self._buffer = []
        self._original_stdout = sys.stdout
        sys.stdout = self
        return self

    def write(self, text):
        """拦截sys.stdout.write，同时输出到控制台和缓冲区"""
        if self._original_stdout:
            self._original_stdout.write(text)
            self._original_stdout.flush()
        self._buffer.append(text)

    def flush(self):
        if self._original_stdout:
            self._original_stdout.flush()

    def _flush_buffer_to_sheet(self, test_item_name):
        """将缓冲区内容写入对应测试项工作表"""
        if not self._buffer:
            return

        text = ''.join(self._buffer)
        self._buffer = []

        if not text.strip():
            return

        if test_item_name in self.wb.sheetnames:
            ws = self.wb[test_item_name]
        else:
            ws = self.wb.create_sheet(title=test_item_name)

        for line in text.rstrip('\n').split('\n'):
            ws.cell(row=ws.max_row + 1, column=1).value = line

    def record_result(self, test_item_name, result, actual_value=None):
        """
        记录测试结果。
        1. 将当前缓冲区中的print输出保存到对应测试项工作表
        2. 在"测试一览表"的B列写入pass/fail，C列写入实测值

        Args:
            test_item_name: 测试项名称（对应工作表名和"测试一览表"A列中的名称）
            result: "pass" 或 "fail"
            actual_value: 实测数据（可选），将写入C列
        """
        # 1. 先保存当前打印输出到对应工作表
        self._flush_buffer_to_sheet(test_item_name)

        if self.wb is None:
            return

        # 2. 在"测试一览表"（第一个sheet）中查找对应的测试项行
        ws_overview = self.wb[self.wb.sheetnames[0]]

        found = False
        for row in range(1, ws_overview.max_row + 1):
            # 先在A列匹配测试项名称
            cell_val = ws_overview.cell(row=row, column=1).value
            if cell_val and str(cell_val).strip() == test_item_name:
                ws_overview.cell(row=row, column=2).value = result
                if actual_value is not None:
                    ws_overview.cell(row=row, column=3).value = str(actual_value)
                found = True
                break

        if not found:
            # 未在A列找到，尝试在H列（测试工况）匹配
            for row in range(1, ws_overview.max_row + 1):
                cell_val = ws_overview.cell(row=row, column=8).value
                if cell_val and str(cell_val).strip() == test_item_name:
                    ws_overview.cell(row=row, column=2).value = result
                    if actual_value is not None:
                        ws_overview.cell(row=row, column=3).value = str(actual_value)
                    break

        # 保存Excel
        try:
            self.wb.save(self._excel_path)
        except Exception as e:
            if self._original_stdout:
                self._original_stdout.write(f"\n保存测试结果失败: {e}\n")

    def __exit__(self, exc_type, exc_val, exc_tb):
        """恢复stdout，保存剩余缓冲内容"""
        sys.stdout = self._original_stdout if self._original_stdout else sys.stdout

        # 如果有未关联测试项的残留输出，保存到工作簿
        if self._buffer and self.wb:
            remaining = ''.join(self._buffer).strip()
            if remaining:
                try:
                    self.wb.save(self._excel_path)
                except Exception:
                    pass

        self._buffer = []
        return False
