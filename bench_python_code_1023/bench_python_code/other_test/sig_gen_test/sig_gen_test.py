# -*- coding: utf-8 -*-
"""
信号发生器 (DG1022Z) 信号质量测试脚本

通过 instrument_set 统一接口控制串口、信号发生器、示波器，
自动完成 10 组 PWM 方波信号的频率/占空比测量和失真分析，
并将测试结果保存到 Excel 文件。

使用方法：
    python other_test/sig_gen_test/sig_gen_test.py

依赖：
    - openpyxl (Excel 输出)
    - 硬件：RIGOL DG1022Z + TBS 2204B (或其他示波器) + 串口控制的通路切换板
"""

import sys
import time
from pathlib import Path

# ── 将项目根目录加入 sys.path ──
_project_root = str(Path(__file__).resolve().parents[2])
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

from instrument import instrument_set as instr
import openpyxl
from openpyxl.styles import Font, Alignment, PatternFill, Border, Side
from datetime import datetime


# ═══════════════════════════════════════════════════════════════
# 配置区（根据实际硬件调整下列参数）
# ═══════════════════════════════════════════════════════════════

# 串口配置
SERIAL_BAUDRATE = 115200
SERIAL_TIMEOUT  = 2

# 示波器配置
OSC_PROBE_ATTENUATION = 10      # 探头衰减比 10X
OSC_VERTICAL_SCALE    = 2.0     # V/格
OSC_HORIZONTAL_SCALE  = 1e-3   # s/格 (1ms)

# 串口通路切换命令（根据你的继电器/GPIO 板协议修改）
CMD_RESET_PATHS   = "cmd_gpio_set_595d_test reset\r\n"
CMD_CONNECT_CH1_1 = "cmd_gpio_set_595d_test set 34 36\r\n"   # 信号发生器 CH1 → 示波器 CH1

# 示波器测量通道
OSC_MEAS_CHANNEL = "CH1"

# Excel 结果保存目录（脚本所在目录）
RESULTS_DIR = Path(__file__).resolve().parent


# ═══════════════════════════════════════════════════════════════
# 测试参数：10 组 (频率Hz, 占空比%, 幅度Vpp, 偏置V, 测试说明)
# ═══════════════════════════════════════════════════════════════

TEST_GROUPS = [
    (1000,   50, 3.3, 1.65, "基准 1kHz 方波"),
    (1000,   25, 3.3, 1.65, "低占空比 25%"),
    (1000,   75, 3.3, 1.65, "高占空比 75%"),
    (5000,   50, 3.3, 1.65, "5kHz 方波"),
    (10000,  50, 3.3, 1.65, "10kHz 方波"),
    (1000,   50, 5.0, 2.50, "高幅度 5Vpp"),
    (1000,   50, 2.0, 1.00, "低幅度 2Vpp"),
    (500,    30, 3.3, 1.65, "500Hz 30% 占空比"),
    (2000,   60, 3.3, 1.65, "2kHz 60% 占空比"),
    (100,    50, 3.3, 1.65, "100Hz 低频方波"),
]


# ═══════════════════════════════════════════════════════════════
# Excel 样式定义
# ═══════════════════════════════════════════════════════════════

_HEADER_FONT   = Font(name="微软雅黑", bold=True, size=11, color="FFFFFF")
_HEADER_FILL   = PatternFill(start_color="4472C4", end_color="4472C4", fill_type="solid")
_HEADER_ALIGN  = Alignment(horizontal="center", vertical="center", wrap_text=True)
_CELL_ALIGN   = Alignment(horizontal="center", vertical="center")
_PASS_FILL     = PatternFill(start_color="C6EFCE", end_color="C6EFCE", fill_type="solid")
_FAIL_FILL     = PatternFill(start_color="FFC7CE", end_color="FFC7CE", fill_type="solid")
_THIN_BORDER  = Border(
    left=Side(style="thin"), right=Side(style="thin"),
    top=Side(style="thin"), bottom=Side(style="thin"),
)


def _init_excel_headers(ws, headers):
    """写入表头并设置样式。"""
    for col_idx, h in enumerate(headers, 1):
        cell = ws.cell(row=1, column=col_idx, value=h)
        cell.font = _HEADER_FONT
        cell.fill = _HEADER_FILL
        cell.alignment = _HEADER_ALIGN
        cell.border = _THIN_BORDER


def _write_result_row(ws, row_idx, r):
    """将一条测试结果写入 Excel 一行，带条件颜色。"""
    values = [
        r["group"],
        r["description"],
        r["set_freq_hz"],
        r["set_duty_pct"],
        r["set_amp_vpp"],
        r["set_offset_v"],
        r["meas_freq_hz"] if r["meas_freq_hz"] is not None else "N/A",
        f"{r['meas_period_us']:.3f}" if r["meas_period_us"] is not None else "N/A",
        f"{r['meas_duty_pct']:.3f}" if r["meas_duty_pct"] is not None else "N/A",
        f"{r['freq_error_pct']:+.3f}" if r["freq_error_pct"] is not None else "N/A",
        f"{r['duty_error_pct']:+.2f}" if r["duty_error_pct"] is not None else "N/A",
    ]

    # 判断通过：频率误差 < 1% 且占空比偏差 < 5%
    passed = (
        r["freq_error_pct"] is not None and abs(r["freq_error_pct"]) < 1.0
        and r["duty_error_pct"] is not None and abs(r["duty_error_pct"]) < 5.0
    )
    values.append("PASS" if passed else "FAIL")

    fill = _PASS_FILL if passed else _FAIL_FILL

    for col_idx, v in enumerate(values, 1):
        cell = ws.cell(row=row_idx, column=col_idx, value=v)
        cell.alignment = _CELL_ALIGN
        cell.border = _THIN_BORDER
        cell.fill = fill


def save_to_excel(results, elapsed_sec):
    """
    将测试结果保存到 Excel 文件。

    参数：
        results: list[dict] - 测试结果列表
        elapsed_sec: float - 测试总耗时（秒）
    返回：
        Path - 保存的文件路径
    """
    # 确保目录存在
    RESULTS_DIR.mkdir(parents=True, exist_ok=True)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filepath = RESULTS_DIR / f"sigen_test_report_{timestamp}.xlsx"

    wb = openpyxl.Workbook()

    # ── Sheet 1: 详细数据 ──
    ws1 = wb.active
    ws1.title = "测试数据"

    headers = [
        "组号", "说明", "设定频率(Hz)", "设定占空比(%)",
        "设定幅度(Vpp)", "设定偏置(V)", "实测频率(Hz)",
        "实测周期(us)", "实测占空比(%)", "频率误差(%)",
        "占空比偏差(%)", "测试结果",
    ]
    _init_excel_headers(ws1, headers)

    for i, r in enumerate(results):
        _write_result_row(ws1, i + 2, r)

    # 列宽
    col_widths = [6, 18, 12, 14, 12, 12, 14, 12, 14, 12, 12, 10]
    for idx, w in enumerate(col_widths, 1):
        ws1.column_dimensions[openpyxl.utils.get_column_letter(idx)].width = w

    # ── Sheet 2: 统计汇总 ──
    ws2 = wb.create_sheet("汇总")
    passed = sum(1 for r in results if r.get("freq_error_pct") is not None
                 and abs(r["freq_error_pct"]) < 1.0
                 and r.get("duty_error_pct") is not None
                 and abs(r["duty_error_pct"]) < 5.0)
    total = len(results)

    summary_data = [
        ("测试时间", datetime.now().strftime("%Y-%m-%d %H:%M:%S")),
        ("测试总组数", total),
        ("通过组数", passed),
        ("失败组数", total - passed),
        ("通过率", f"{passed / total * 100:.1f}%" if total else "N/A"),
        ("测试总耗时", f"{elapsed_sec:.1f} 秒 ({elapsed_sec / 60:.1f} 分钟)"),
        ("", ""),
        ("判定标准", "频率误差 < 1% 且 占空比偏差 < 5%"),
    ]

    for row_idx, (key, val) in enumerate(summary_data, 1):
        ck = ws2.cell(row=row_idx, column=1, value=key)
        cv = ws2.cell(row=row_idx, column=2, value=val)
        ck.font = Font(bold=True)
        ck.alignment = _CELL_ALIGN
        cv.alignment = _CELL_ALIGN
        ck.border = _THIN_BORDER
        cv.border = _THIN_BORDER

    ws2.column_dimensions["A"].width = 16
    ws2.column_dimensions["B"].width = 36

    wb.save(str(filepath))
    return filepath


# ═══════════════════════════════════════════════════════════════
# 测试主流程
# ═══════════════════════════════════════════════════════════════

def run_test():
    """
    完整测试流程：
      1. 初始化串口、信号发生器、示波器
      2. 对 10 组参数逐组测量
      3. 计算频率误差和占空比偏差
      4. 保存结果到 Excel
    """
    from instrument import instrument_set as instr

    results = []
    start_time = time.time()

    # ── 1. 初始化串口 ──
    print("=" * 60)
    print("Step 1/4: 初始化串口…")
    if not instr.serial_auto_init(baudrate=SERIAL_BAUDRATE):
        ports = instr.serial_list_ports()
        if not ports:
            print("  错误: 未找到任何串口，退出测试")
            return
        if not instr.serial_init(ports[0], baudrate=SERIAL_BAUDRATE,
                                 timeout=SERIAL_TIMEOUT):
            print(f"  错误: 串口 {ports[0]} 初始化失败")
            return

    # ── 2. 初始化信号发生器 ──
    print("Step 2/4: 初始化信号发生器…")
    if not instr.sig_init():
        print("  错误: 信号发生器初始化失败")
        instr.serial_close()
        return

    # ── 3. 初始化示波器 ──
    print("Step 3/4: 初始化示波器…")
    if not instr.osc_init(
        probe_attenuation=OSC_PROBE_ATTENUATION,
        vertical_scale=OSC_VERTICAL_SCALE,
        horizontal_scale=OSC_HORIZONTAL_SCALE,
    ):
        print("  错误: 示波器初始化失败")
        instr.sig_close()
        instr.serial_close()
        return

    try:
        # ── 复位通路 + 切换到测量路径 ──
        print("Step 4/4: 开始 10 组信号质量测试…\n")
        print("  切换通路: 信号发生器 CH1 → 示波器 CH1")
        instr.serial_send_and_receive(CMD_CONNECT_CH1_1)
        time.sleep(1)

        # ── 逐组测试 ──
        for i, (freq, duty, amp, offset, desc) in enumerate(TEST_GROUPS, 1):
            print(f"  [{i}/{len(TEST_GROUPS)}] {desc}")

            # 配置信号发生器
            instr.sig_set_pwm(
                frequency=freq, duty_cycle=duty,
                amplitude=amp, offset=offset, channel=1,
            )
            time.sleep(0.2)

            # 打开输出
            instr.sig_output_on(1)
            time.sleep(0.8)

            # 示波器 AutoSet + 测量
            instr.osc_auto_set()
            time.sleep(0.5)
            freq_meas, period_us, duty_meas = instr.osc_measure_frequency(
                OSC_MEAS_CHANNEL
            )

            # 关闭输出
            instr.sig_output_off(1)
            time.sleep(0.1)

            # 记录结果
            result = {
                "group": i,
                "description": desc,
                "set_freq_hz": freq,
                "set_duty_pct": duty,
                "set_amp_vpp": amp,
                "set_offset_v": offset,
                "meas_freq_hz": freq_meas,
                "meas_period_us": period_us,
                "meas_duty_pct": duty_meas,
            }
            if freq_meas and freq_meas > 0:
                result["freq_error_pct"] = (freq_meas - freq) / freq * 100
            else:
                result["freq_error_pct"] = None
            if duty_meas is not None:
                result["duty_error_pct"] = duty_meas - duty
            else:
                result["duty_error_pct"] = None

            results.append(result)

            # 打印
            freq_str = f"{freq_meas:.3f} Hz" if freq_meas else "N/A"
            duty_str = f"{duty_meas:.3f}%" if duty_meas is not None else "N/A"
            fe_str = f"{result['freq_error_pct']:+.3f}%" if result['freq_error_pct'] is not None else "N/A"
            de_str = f"{result['duty_error_pct']:+.2f}%" if result['duty_error_pct'] is not None else "N/A"
            print(f"    设定: {freq}Hz / {duty}% / {amp}Vpp")
            print(f"    实测: {freq_str} / 占空比 {duty_str}")
            print(f"    误差: 频率 {fe_str} , 占空比 {de_str}")
            print()

        # ── 保存到 Excel ──
        elapsed = time.time() - start_time
        out_path = save_to_excel(results, elapsed)
        print(f" 测试报告已保存: {out_path}")

        # ── 打印汇总 ──
        passed = sum(1 for r in results
                     if r["freq_error_pct"] is not None
                     and abs(r["freq_error_pct"]) < 1.0
                     and r["duty_error_pct"] is not None
                     and abs(r["duty_error_pct"]) < 5.0)
        print(f"\n 测试完成! 通过 {passed}/{len(results)}, "
              f"耗时 {elapsed:.1f}s")

    except KeyboardInterrupt:
        print("\n  测试被用户中断")
    except Exception as e:
        print(f"\n  测试异常: {e}")
        import traceback
        traceback.print_exc()
    finally:
        # ── 清理 ──
        print("\n清理资源…")
        instr.serial_send_and_receive(CMD_RESET_PATHS)
        time.sleep(0.3)
        instr.sig_output_off(1)
        instr.sig_close()
        instr.osc_close()
        instr.serial_close()
        print("资源已释放")


if __name__ == "__main__":
    run_test()
