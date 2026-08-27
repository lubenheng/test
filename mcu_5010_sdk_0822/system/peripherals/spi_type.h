#ifndef SPI_TYPE_H
#define SPI_TYPE_H

// #include "spi_reg.h"
// #include "core_cm0.h"
#include <stdint.h>

// SPI IP基地址（需根据实际MCU地址映射修改）
// #define SPI_BASE_ADDR 0x40000000
// #define SPI ((SPI_TypeDef *)SPI_BASE_ADDR)

// 测试状态定义
// #define TEST_PASS 0
// #define TEST_FAIL 1

// // 外设配置（根据FPGA验证环境修改）
// #define SPI_SLAVE_NUM 0       // 测试用从设备编号
// #define SPI_BAUDR_DIV 16      // 波特率分频（ssi_clk/SCKDV，需匹配FPGA时钟）
// #define SPI_FIFO_DEPTH 8      // 配置的FIFO深度（与IP参数一致）
// #define TX_FIFO_THRESHOLD 2   // 发送FIFO阈值
// #define RX_FIFO_THRESHOLD 2   // 接收FIFO阈值

// // 测试数据定义
// #define TEST_DATA_TX 0x1234   // 发送测试数据
// #define TEST_DATA_RX_EXPECT 0x4321 // 预期接收数据（主从互传时配置）

// 硬件功能宏开关（根据实际芯片配置开启）
#define ATCSPI200_SLAVE_SUPPORT         1
#define ATCSPI200_DIRECT_IO_SUPPORT     1
#define ATCSPI200_DUALSPI_SUPPORT       1
#define ATCSPI200_QUADSPI_SUPPORT       1

#define DATA_LENGTH 256

// =============================================================================
// 共用：IO模式枚举（增强代码可读性）
// =============================================================================
typedef enum {
    SPI_IO_MODE_SINGLE = 0,  // 常规单IO模式
    SPI_IO_MODE_DUAL   = 1,  // 双IO（Dual）模式
    SPI_IO_MODE_QUAD   = 2   // 四IO（Quad）模式
} SPI_IoMode_Typedef;

// =============================================================================
// 共用：传输模式枚举（仅主机完整使用，从机简化）
// =============================================================================
typedef enum {
    SPI_TRANS_MODE_WR_SYNC    = 0,  // 读写同步
    SPI_TRANS_MODE_WR_ONLY    = 1,  // 仅写
    SPI_TRANS_MODE_RD_ONLY    = 2,  // 仅读
    SPI_TRANS_MODE_WR_THEN_RD = 3,  // 写后读
    SPI_TRANS_MODE_RD_THEN_WR = 4,  // 读后写
    SPI_TRANS_MODE_WR_DUMMY_RD= 5,  // 写→虚拟→读
    SPI_TRANS_MODE_RD_DUMMY_WR= 6,  // 读→虚拟→写
    SPI_TRANS_MODE_NO_DATA    = 7,  // 无数据（需启用CmdEn/AddrEn）
    SPI_TRANS_MODE_DUMMY_WR   = 8,  // 虚拟→写
    SPI_TRANS_MODE_DUMMY_RD   = 9   // 虚拟→读
} SPI_TransMode_Typedef;


// =============================================================================
// 主机专用：基础传输格式配置
// 对应寄存器：0x10 TRANSFMT
// =============================================================================
typedef struct {
    // ---------------------- 时钟极性与相位 ----------------------
    uint8_t CPOL;        // 时钟极性：0=空闲SCLK低；1=空闲SCLK高
    uint8_t CPHA;        // 时钟相位：0=奇数边沿采样；1=偶数边沿采样

    // ---------------------- 位序与数据格式 ----------------------
    uint8_t IsLsbFirst;  // 位序：0=MSB优先；1=LSB优先
    uint8_t AddrLenByte; // 地址阶段字节长度：0=1字节；1=2字节；2=3字节；3=4字节
    uint8_t DataLenBit;  // 数据单元位宽：0~31（实际位宽=DataLenBit+1）
    uint8_t IsDataMergeEn;// 数据合并使能（仅DataLenBit=7时有效）：0=禁用；1=启用
    uint8_t IsMosiBiDirEn;// 常规模式MOSI双向使能：0=单向；1=双向（替代MOSI+MISO）

    // ---------------------- IO模式 ----------------------
    SPI_IoMode_Typedef IoMode; // 数据相位IO模式
} SPI_MasterBaseConfig_Typedef;

// =============================================================================
// 主机专用：传输控制配置
// 对应寄存器：0x20 TRANSCTRL、0x24 CMD、0x28 ADDR
// =============================================================================
typedef struct {
    // ---------------------- 传输阶段使能 ----------------------
    uint8_t IsCmdEn;     // 命令阶段使能：0=禁用；1=启用
    uint8_t IsAddrEn;    // 地址阶段使能：0=禁用；1=启用
    uint8_t AddrFmtSameAsData; // 地址阶段格式：0=常规单IO；1=与数据阶段一致

    // ---------------------- 传输模式与计数 ----------------------
    SPI_TransMode_Typedef TransMode; // 传输模式
    uint8_t IsTokenEn;    // 读传输附加令牌使能：0=禁用；1=启用
    uint8_t TokenValue;   // 令牌值：0=0x00；1=0x69
    uint8_t DummyCnt;     // 虚拟周期计数：0~3（实际周期=DummyCnt+1）
    uint16_t WrTranCnt;   // 写数据计数：0~511（实际数=WrTranCnt+1）
    uint16_t RdTranCnt;   // 读数据计数：0~511（实际数=RdTranCnt+1）

    // ---------------------- 命令与地址 ----------------------
    uint8_t CmdCode;      // SPI命令值（写此寄存器触发传输）
    uint32_t TransAddr;   // SPI传输地址
} SPI_MasterTransConfig_Typedef;

// =============================================================================
// 主机专用：FIFO与DMA配置
// 对应寄存器：0x30 CTRL
// =============================================================================
typedef struct {
    uint8_t TxThres;      // TX FIFO阈值：0~31
    uint8_t RxThres;      // RX FIFO阈值：0~31
    uint8_t IsTxDmaEn;    // TX DMA使能：0=禁用；1=启用
    uint8_t IsRxDmaEn;    // RX DMA使能：0=禁用；1=启用
} SPI_MasterFifoDmaConfig_Typedef;

// =============================================================================
// 主机专用：接口时序配置
// 对应寄存器：0x40 TIMING
// =============================================================================
typedef struct {
    uint8_t Cs2Sclk;       // CS到SCLK间隔：0~3（实际=(SCLK周期/2)*(Cs2Sclk+1)）
    uint8_t CsHoldTime;    // CS保持高电平时间：0~15（实际=(SCLK周期/2)*(CsHoldTime+1)）
    uint32_t SCLK_SPEED;      // SCLK速率，计算：SCLK period = (SCLK_DIV + 1) × 2 × system clock period
} SPI_MasterTimingConfig_Typedef;

// =============================================================================
// 主机专用：存储器映射访问配置
// 对应寄存器：0x50 MEMCTRL
// =============================================================================
typedef struct {
    uint8_t MemReadCmd;    // 内存映射读命令格式：0x0~0xF（参考数据手册Table17）
} SPI_MasterMemMapConfig_Typedef;

// =============================================================================
// 主机专用：直接IO控制配置（可选）
// 对应寄存器：0x14 DIRECTIO
// =============================================================================
#ifdef ATCSPI200_DIRECT_IO_SUPPORT
typedef struct {
    uint8_t IsDirectIOEn;  // 直接IO总使能：0=禁用；1=启用
    uint8_t CsOe;          // CS输出使能：0=禁用；1=启用
    uint8_t SclkOe;        // SCLK输出使能：0=禁用；1=启用
    uint8_t MosiOe;        // MOSI输出使能：0=禁用；1=启用
    uint8_t MisoOe;        // MISO输出使能：0=禁用；1=启用
    uint8_t WpOe;          // WP输出使能：0=禁用；1=启用
    uint8_t HoldOe;        // HOLD输出使能：0=禁用；1=启用
    uint8_t CsOut;         // CS输出值：0=低；1=高
    uint8_t SclkOut;       // SCLK输出值：0=低；1=高
    uint8_t MosiOut;       // MOSI输出值：0=低；1=高
    uint8_t MisoOut;       // MISO输出值：0=低；1=高
    uint8_t WpOut;         // WP输出值：0=低；1=高
    uint8_t HoldOut;       // HOLD输出值：0=低；1=高
} SPI_MasterDirectIOConfig_Typedef;
#endif

// =============================================================================
// 主机总配置管理结构体
// =============================================================================
typedef struct {
    SPI_MasterBaseConfig_Typedef      baseConfig;      // 基础传输格式
    SPI_MasterTransConfig_Typedef     transConfig;     // 传输控制
    SPI_MasterFifoDmaConfig_Typedef   fifoDmaConfig;   // FIFO与DMA
    SPI_MasterTimingConfig_Typedef    timingConfig;    // 接口时序
    SPI_MasterMemMapConfig_Typedef    memMapConfig;    // 存储器映射
#ifdef ATCSPI200_DIRECT_IO_SUPPORT
    SPI_MasterDirectIOConfig_Typedef  directIoConfig;  // 直接IO（可选）
#endif
} SPI_MasterHandleTypeDef;

// =============================================================================
// 从机专用：基础传输格式配置（必须与主机一致）
// 对应寄存器：0x10 TRANSFMT
// =============================================================================
typedef struct {
    // ---------------------- 时钟极性与相位（核心：必须与主机一致） ----------------------
    uint8_t CPOL;        // 时钟极性：0=空闲SCLK低；1=空闲SCLK高
    uint8_t CPHA;        // 时钟相位：0=奇数边沿采样；1=偶数边沿采样

    // ---------------------- 位序与数据格式 ----------------------
    uint8_t IsLsbFirst;  // 位序：0=MSB优先；1=LSB优先
    uint8_t DataLenBit;  // 数据单元位宽：0~31（实际位宽=DataLenBit+1，推荐7=8bit）
    uint8_t IsDataMergeEn;// 数据合并使能（仅DataLenBit=7时有效）：0=禁用；1=启用
    uint8_t IsMosiBiDirEn;// 常规模式MOSI双向使能：0=单向；1=双向

    // ---------------------- IO模式（必须与主机一致） ----------------------
    SPI_IoMode_Typedef IoMode; // 数据相位IO模式
} SPI_SlaveBaseConfig_Typedef;

// =============================================================================
// 从机专用：简化传输控制配置（仅保留IO模式，其余由主机决定）
// 对应寄存器：0x20 TRANSCTRL（仅DualQuad位域有效）
// =============================================================================
typedef struct {
    // 从机模式下，传输阶段、计数、命令地址等均由主机控制，无需配置
    // 仅保留IO模式的冗余配置（已在BaseConfig中定义，此处可省略，或保留用于运行时切换）
    SPI_IoMode_Typedef IoMode; // 冗余：IO模式（建议统一使用BaseConfig中的配置）
} SPI_SlaveTransConfig_Typedef;

// =============================================================================
// 从机专用：FIFO与DMA配置
// 对应寄存器：0x30 CTRL
// =============================================================================
typedef struct {
    uint8_t TxThres;      // TX FIFO阈值：0~31（低于该值触发中断/DMA）
    uint8_t RxThres;      // RX FIFO阈值：0~31（高于该值触发中断/DMA）
    uint8_t IsTxDmaEn;    // TX DMA使能：0=禁用；1=启用
    uint8_t IsRxDmaEn;    // RX DMA使能：0=禁用；1=启用
} SPI_SlaveFifoDmaConfig_Typedef;

// =============================================================================
// 从机专用：从机模式核心配置
// 对应寄存器：0x60 SLVST
// =============================================================================
#ifdef ATCSPI200_SLAVE_SUPPORT
typedef struct {
    uint16_t UserStatus;   // 用户自定义状态：0x0000~0xFFFF（主机可通过读状态命令获取）
    uint8_t IsReady;       // 就绪标志：0=未就绪；1=就绪（事务结束后硬件自动清0）
} SPI_SlaveCoreConfig_Typedef;
#endif

// =============================================================================
// 从机专用：直接IO控制配置（不推荐从机使用，避免干扰总线）
// 对应寄存器：0x14 DIRECTIO
// =============================================================================
#ifdef ATCSPI200_DIRECT_IO_SUPPORT
typedef struct {
    uint8_t IsDirectIOEn;  // 直接IO总使能：从机模式推荐设为0（禁用）
    // 其余字段同主机直接IO配置，但从机模式下不建议使用
} SPI_SlaveDirectIOConfig_Typedef;
#endif

// =============================================================================
// 从机总配置管理结构体
// =============================================================================
typedef struct {
    SPI_SlaveBaseConfig_Typedef      baseConfig;      // 基础传输格式（必须与主机一致）
    SPI_SlaveTransConfig_Typedef     transConfig;     // 简化传输控制（仅冗余IO模式）
    SPI_SlaveFifoDmaConfig_Typedef   fifoDmaConfig;   // FIFO与DMA
#ifdef ATCSPI200_SLAVE_SUPPORT
    SPI_SlaveCoreConfig_Typedef      coreConfig;      // 从机核心状态
#endif
#ifdef ATCSPI200_DIRECT_IO_SUPPORT
    SPI_SlaveDirectIOConfig_Typedef  directIoConfig;  // 直接IO（不推荐使用）
#endif
} SPI_SlaveHandleTypeDef;

typedef struct{
    SPI_MasterHandleTypeDef spi_master_config;
    SPI_SlaveHandleTypeDef spi_slave_config;
}SPI_HandleTypeDef;
typedef struct{
    uint8_t *data;
    uint8_t data_len;
}transmit_data;

typedef struct{
    uint8_t *data_current;
    uint8_t data_len;
    uint8_t data_index;
}transmit_data_index;


#define INT_RX_FIFO_OR      0x01
#define INT_TX_FIFO_UR      0x02
#define INT_RX_FIFO_TH      0x04
#define INT_TX_FIFO_TH      0x08
#define INT_TRANS_END       0x10
#define INT_SLV_CMD         0x20











#endif
