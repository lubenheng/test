/****************************************************************************/
/** \file main.c
**
**  HWMath (32-bit 硬件除法模块) demo for ODS_5010 (Cortex-M0+)
**
**  功能：
**    1. 使能 HWMATH 外设时钟 (HWMATH_Clk_Enable)；
**    2. 32 位有符号除法 (DIV)：被除数/除数/商/余数均为 32 位有符号数，
**       覆盖 正/正、正/负、负/正、负/负 四种符号组合，与标准数学库结果比对；
**    3. 32 位有符号求余 (MOD)：同上，比较余数；
**    4. 32 位无符号开方 (SQRT)：被开方数 32 位无符号，结果为 16 位无符号；
**    5. 正余弦计算 (SIN/COS)：16 位无符号角度输入(0-65535 对应 0-360°)，
**       16 位有符号 Q15 定点输出，与标准数学库比对。
**
**  验证方法：
**    打开串口(115200-8-N-1, 与 UART0 配置一致)，上电后自动依次执行上述
**    计算并打印结果，每项比对打印 PASS / FAIL。
**    参考：Roadtest_A3_HWMath_5010_Rev0_20251118_FPGA.docx
*****************************************************************************/
#include "platform_common.h"
#include "gpio_hal.h"
#include "clock.h"
#include "clock_hal.h"       /* HWMATH_Clk_Enable() */
#include "uart_hal.h"
#include "uart_hal_cfg.h"

#include "HWMath_Reg.h"      /* HWMATH_TypeDef: 寄存器/状态位定义 */
#include "HWMath_Type.h"     /* Math_SinCos_Type */
#include "HW_Math.h"         /* HWMATH_DIV_DIV / MOD / SQRT / SINCOS 硬件计算接口 */
#include "math.h"            /* sin/cos/sqrt 标准数学库参考 */

#define HWMATH_PI               3.14159265358979323846

/* 正弦/余弦结果与标准库的允许误差(±2，Q15 定点精度) */
#define HWMATH_SINCOS_TOL       2

/* 绝对值宏 */
#define HWMATH_ABS(x)           (((x) > 0) ? (x) : -(x))


/**
 *@brief configure the gpio of UART0
 *@param[in] none
 *@return    none
 */
static void uart0_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

    /* UART0_RX, PTA1  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_1;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* UART0_TX, PTA2  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_2;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

/**
 *@brief 32 位有符号除法演示 (HWMATH_DIV_DIV)
 *       覆盖 正/正、正/负、负/正、负/负 四种符号组合及边界值，
 *       与标准数学库 '/' 结果比对。
 *@param[in] none
 *@return    none
 */
static void hwmath_div_demo(void)
{
    int32_t i;

    /* (被除数, 除数) 测试用例: 正/正, 正/负, 负/正, 负/负, INT32_MAX, INT32_MIN */
    static const int32_t div_case[][2] =
    {
        { 987654321,   123 },    /* 正 / 正 */
        {  986555,     -44 },    /* 正 / 负 (Roadtest 测试用例) */
        { -912036678,  8865 },   /* 负 / 正 */
        { -100000000, -3333 },   /* 负 / 负 */
        {  2147483647,    2 },   /* INT32_MAX  / 2 */
        { (-2147483647 - 1), 2 } /* INT32_MIN  / 2 */
    };
    uint32_t pass_cnt = 0U;

    INFO("\n===== 1. 32-bit signed DIVISION (HWMATH_DIV_DIV) =====\n");
    for (i = 0; i < (int32_t)(sizeof(div_case) / sizeof(div_case[0])); i++)
    {
        int32_t dividend = div_case[i][0];
        int32_t divisor  = div_case[i][1];
        int32_t quot_hw;      /* 硬件除法商 */
        int32_t quot_std;     /* 标准数学库商 */

        if (divisor == 0)
        {
            INFO("  [%d] divisor is 0, skip\n", (int)i);
            continue;
        }

        quot_hw  = HWMATH_DIV_DIV(dividend, divisor);
        quot_std = dividend / divisor;

        if (quot_hw == quot_std)
        {
            pass_cnt++;
            INFO("  [%d] %d / %d = %d   (HW) == %d   (Std)   PASS\n",
                 (int)i, (int)dividend, (int)divisor, (int)quot_hw, (int)quot_std);
        }
        else
        {
            INFO("  [%d] %d / %d = %d   (HW) != %d   (Std)   FAIL\n",
                 (int)i, (int)dividend, (int)divisor, (int)quot_hw, (int)quot_std);
        }
    }
    INFO("  DIV result: %u/%u PASS\n", (unsigned)pass_cnt,
         (unsigned)(sizeof(div_case) / sizeof(div_case[0])));
}

/**
 *@brief 32 位有符号求余演示 (HWMATH_DIV_MOD)
 *       覆盖 正/正、正/负、负/正、负/负 四种符号组合，与标准数学库 '%' 结果比对。
 *@param[in] none
 *@return    none
 */
static void hwmath_mod_demo(void)
{
    int32_t i;

    static const int32_t mod_case[][2] =
    {
        {  123456789,  1000 },    /* 正 / 正 */
        {  986555,     -44 },     /* 正 / 负 (Roadtest 测试用例) */
        { -912036678,  8865 },    /* 负 / 正 */
        { -100000000, -3333 },    /* 负 / 负 */
        {  2147483647,    7 },    /* INT32_MAX  % 7 */
        { (-2147483647 - 1), 7 }  /* INT32_MIN  % 7 */
    };
    uint32_t pass_cnt = 0U;

    INFO("\n===== 2. 32-bit signed MODULO (HWMATH_DIV_MOD) =====\n");
    for (i = 0; i < (int32_t)(sizeof(mod_case) / sizeof(mod_case[0])); i++)
    {
        int32_t dividend = mod_case[i][0];
        int32_t divisor  = mod_case[i][1];
        int32_t rem_hw;       /* 硬件余数 */
        int32_t rem_std;      /* 标准数学库余数 */

        if (divisor == 0)
        {
            INFO("  [%d] divisor is 0, skip\n", (int)i);
            continue;
        }

        rem_hw  = HWMATH_DIV_MOD(dividend, divisor);
        rem_std = dividend % divisor;

        if (rem_hw == rem_std)
        {
            pass_cnt++;
            INFO("  [%d] %d %% %d = %d   (HW) == %d   (Std)   PASS\n",
                 (int)i, (int)dividend, (int)divisor, (int)rem_hw, (int)rem_std);
        }
        else
        {
            INFO("  [%d] %d %% %d = %d   (HW) != %d   (Std)   FAIL\n",
                 (int)i, (int)dividend, (int)divisor, (int)rem_hw, (int)rem_std);
        }
    }
    INFO("  MOD result: %u/%u PASS\n", (unsigned)pass_cnt,
         (unsigned)(sizeof(mod_case) / sizeof(mod_case[0])));
}

/**
 *@brief 32 位无符号开方演示 (HWMATH_SQRT)
 *       被开方数 32 位无符号，开方结果为 16 位无符号，与标准数学库 sqrt() 比对。
 *@param[in] none
 *@return    none
 */
static void hwmath_sqrt_demo(void)
{
    static const uint32_t sqrt_case[] =
    {
        0U,             /* 0 -> 0 */
        65536U,         /* 65536 -> 256 */
        2524158208U,    /* Roadtest 测试用例 */
        0xFFFFFFFFU     /* 2^32-1 -> 65535 */
    };
    uint32_t i;
    uint32_t pass_cnt = 0U;

    INFO("\n===== 3. 32-bit unsigned SQUARE ROOT (HWMATH_SQRT) =====\n");
    for (i = 0U; i < (uint32_t)(sizeof(sqrt_case) / sizeof(sqrt_case[0])); i++)
    {
        uint32_t radicand = sqrt_case[i];
        uint32_t sqrt_hw  = HWMATH_SQRT(radicand);
        uint32_t sqrt_std = (uint32_t)sqrt((double)radicand);

        if (sqrt_hw == sqrt_std)
        {
            pass_cnt++;
            INFO("  [%u] sqrt(%u) = %u   (HW) == %u   (Std)   PASS\n",
                 (unsigned)i, (unsigned)radicand, (unsigned)sqrt_hw, (unsigned)sqrt_std);
        }
        else
        {
            INFO("  [%u] sqrt(%u) = %u   (HW) != %u   (Std)   FAIL\n",
                 (unsigned)i, (unsigned)radicand, (unsigned)sqrt_hw, (unsigned)sqrt_std);
        }
    }
    INFO("  SQRT result: %u/%u PASS\n", (unsigned)pass_cnt,
         (unsigned)(sizeof(sqrt_case) / sizeof(sqrt_case[0])));
}

/**
 *@brief 正余弦计算演示 (HWMATH_SINCOS_Cal)
 *       16 位无符号角度输入 0-65535 对应 0-360°，16 位有符号 Q15 定点输出，
 *       与标准数学库 sin()/cos() 缩放比对。
 *@param[in] none
 *@return    none
 */
static void hwmath_sincos_demo(void)
{
    /* 16 位无符号角度输入, 0-65535 对应 0-360° (函数形参为 int16_t,
     * 仅需保证写入寄存器的位模式正确, 因此显式 (int16_t) 截断) */
    static const uint16_t angle_case[] =
    {
        0U,     /* 0°     -> sin=0,    cos=+32767 */
        8192U,  /* 45°    */
        16364U, /* 约90°  (Roadtest 测试用例) */
        32768U, /* 180°   -> sin=0,    cos=-32767 */
        49152U, /* 270°   */
        65535U  /* ~360°  */
    };
    uint32_t i;
    uint32_t pass_cnt = 0U;

    INFO("\n===== 4. SINCOS calculation (HWMATH_SINCOS_Cal) =====\n");
    for (i = 0U; i < (uint32_t)(sizeof(angle_case) / sizeof(angle_case[0])); i++)
    {
        uint16_t angle = angle_case[i];
        Math_SinCos_Type sincos_hw;
        int32_t sin_hw, cos_hw;
        int32_t sin_std, cos_std;
        bool sin_pass, cos_pass;

        HWMATH_SINCOS_Cal((int16_t)angle, &sincos_hw);
        sin_hw = sincos_hw.Sin;
        cos_hw = sincos_hw.Cos;

        /* 标准库参考: Q15 定点, 即 32767 * sin/cos(角度)。
         * 角度寄存器为 16 位, 一个整圆按 65536 分度 (angle/65536*360°),
         * 故此处除以 65536.0 (不可用 65535.0, 否则 65535 会被映射成整 360°)。 */
        sin_std = (int32_t)(32767.0 * sin((double)angle * 2.0 * HWMATH_PI / 65536.0));
        cos_std = (int32_t)(32767.0 * cos((double)angle * 2.0 * HWMATH_PI / 65536.0));

        sin_pass = (HWMATH_ABS(sin_std - sin_hw) <= HWMATH_SINCOS_TOL);
        cos_pass = (HWMATH_ABS(cos_std - cos_hw) <= HWMATH_SINCOS_TOL);
        if (sin_pass && cos_pass)
        {
            pass_cnt++;
        }

        INFO("  [%u] angle=%5d: sin=%6d(HW) vs %6d(Std) %s, "
             "cos=%6d(HW) vs %6d(Std) %s\n",
             (unsigned)i, (int)angle,
             (int)sin_hw, (int)sin_std, sin_pass ? "PASS" : "FAIL",
             (int)cos_hw, (int)cos_std, cos_pass ? "PASS" : "FAIL");
    }
    INFO("  SINCOS result: %u/%u PASS\n", (unsigned)pass_cnt,
         (unsigned)(sizeof(angle_case) / sizeof(angle_case[0])));
}


int main(void)
{
    delay_1us(200000);

    flash_clk_enable();
    gpio_clk_enable();
    uart_clk_enable();

    /* ---- 使能 HWMATH 外设时钟 ---- */
    HWMATH_Clk_Enable();

    /* ---- UART0 控制台初始化 ---- */
    uart0_gpio_init();
    (void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
    uart_transmit_enable(UART_CONSOLE_INSTANCE, true);

    INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);
    INFO("HWMath demo: 32-bit hardware division module (ODS_5010)\n");

    /* 读取硬件版本寄存器，确认模块可访问 */
    INFO("HWMATH VERID Word = 0x%08x\n", (unsigned)HWMATH->VERID_STR.Word);

    __enable_irq();

    /* ---- 依次执行 HWMath 功能演示 ---- */
    hwmath_div_demo();       /* 1. 32 位有符号除法 */
    hwmath_mod_demo();       /* 2. 32 位有符号求余 */
    hwmath_sqrt_demo();      /* 3. 32 位无符号开方 */
    hwmath_sincos_demo();    /* 4. 正余弦计算 */

    INFO("\nHWMath demo finished, check UART output above.\n");

    while (1)
    {
        /* 演示结束，循环等待 (可在此打断点观察变量) */
    }
}
