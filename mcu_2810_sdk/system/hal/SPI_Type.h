#ifndef _SPI_HAL_H
#define _SPI_HAL_H
#include "stdint.h"
 
 
#define SPI_BASE         (0x40001000UL)
#define SPI              ((spi_reg_t *)SPI_BASE)

// ========================== CR0 (0x00) 掩码 ==========================
#define SPI_CR0_RX_EMPTY_Pos      (0U)
#define SPI_CR0_RX_EMPTY_Msk      (1U << SPI_CR0_RX_EMPTY_Pos)

#define SPI_CR0_SR_EMPTY_Pos      (1U)
#define SPI_CR0_SR_EMPTY_Msk      (1U << SPI_CR0_SR_EMPTY_Pos)

#define SPI_CR0_NSSIN_Pos         (2U)
#define SPI_CR0_NSSIN_Msk         (1U << SPI_CR0_NSSIN_Pos)

#define SPI_CR0_SLVSEL_Pos        (3U)
#define SPI_CR0_SLVSEL_Msk        (1U << SPI_CR0_SLVSEL_Pos)

#define SPI_CR0_CPOL_Pos          (4U)
#define SPI_CR0_CPOL_Msk          (1U << SPI_CR0_CPOL_Pos)

#define SPI_CR0_CPHA_Pos          (5U)
#define SPI_CR0_CPHA_Msk          (1U << SPI_CR0_CPHA_Pos)

#define SPI_CR0_SPIMST_Pos        (6U)
#define SPI_CR0_SPIMST_Msk        (1U << SPI_CR0_SPIMST_Pos)

#define SPI_CR0_SPIBSY_Pos        (7U)
#define SPI_CR0_SPIBSY_Msk        (1U << SPI_CR0_SPIBSY_Pos)

// ========================== CR1 (0x01) 掩码 ==========================
#define SPI_CR1_SPIEN_Pos         (0U)
#define SPI_CR1_SPIEN_Msk         (1U << SPI_CR1_SPIEN_Pos)

#define SPI_CR1_TX_EMPTY_Pos      (1U)
#define SPI_CR1_TX_EMPTY_Msk      (1U << SPI_CR1_TX_EMPTY_Pos)

#define SPI_CR1_NSSMOD_Pos        (2U)
#define SPI_CR1_NSSMOD_Msk        (3U << SPI_CR1_NSSMOD_Pos)  // bit2~3

#define SPI_CR1_UNREAD_Pos        (4U)
#define SPI_CR1_UNREAD_Msk        (1U << SPI_CR1_UNREAD_Pos)

#define SPI_CR1_MST_OFF_Pos       (5U)
#define SPI_CR1_MST_OFF_Msk       (1U << SPI_CR1_MST_OFF_Pos)

#define SPI_CR1_WR_CONFLICT_Pos   (6U)
#define SPI_CR1_WR_CONFLICT_Msk   (1U << SPI_CR1_WR_CONFLICT_Pos)

#define SPI_CR1_SPIIF_Pos         (7U)
#define SPI_CR1_SPIIF_Msk         (1U << SPI_CR1_SPIIF_Pos)

#define SPI_CR1_INT_DETAIL_EN_Pos (8U)
#define SPI_CR1_INT_DETAIL_EN_Msk (0xFU << SPI_CR1_INT_DETAIL_EN_Pos)  // bit8~11

#define SPI_CR1_INT_EN_Pos        (12U)
#define SPI_CR1_INT_EN_Msk        (0xFU << SPI_CR1_INT_EN_Pos)        // bit12~15

// ========================== 中断细分使能掩码 ==========================
#define SPI_INT_DETAIL_UNREAD_EN_Pos    (8U)
#define SPI_INT_DETAIL_UNREAD_EN_Msk    (1U << SPI_INT_DETAIL_UNREAD_EN_Pos)

#define SPI_INT_DETAIL_MST_OFF_EN_Pos  (9U)
#define SPI_INT_DETAIL_MST_OFF_EN_Msk  (1U << SPI_INT_DETAIL_MST_OFF_EN_Pos)

#define SPI_INT_DETAIL_WR_CONFLICT_EN_Pos (10U)
#define SPI_INT_DETAIL_WR_CONFLICT_EN_Msk (1U << SPI_INT_DETAIL_WR_CONFLICT_EN_Pos)

#define SPI_INT_DETAIL_SPIIF_EN_Pos     (11U)
#define SPI_INT_DETAIL_SPIIF_EN_Msk     (1U << SPI_INT_DETAIL_SPIIF_EN_Pos)

// ========================== 中断线使能掩码 ==========================
#define SPI_INT_EN_UNREAD_Pos           (12U)
#define SPI_INT_EN_UNREAD_Msk           (1U << SPI_INT_EN_UNREAD_Pos)

#define SPI_INT_EN_MST_OFF_Pos          (13U)
#define SPI_INT_EN_MST_OFF_Msk          (1U << SPI_INT_EN_MST_OFF_Pos)

#define SPI_INT_EN_WR_CONFLICT_Pos      (14U)
#define SPI_INT_EN_WR_CONFLICT_Msk      (1U << SPI_INT_EN_WR_CONFLICT_Pos)

#define SPI_INT_EN_SPIIF_Pos            (15U)
#define SPI_INT_EN_SPIIF_Msk            (1U << SPI_INT_EN_SPIIF_Pos)

// ========================== SPI_CLK / SPI_DR ==========================
#define SPI_SPI_CLK_Msk                 (0xFFU)
#define SPI_SPI_DR_Msk                  (0xFFU)



#endif 


