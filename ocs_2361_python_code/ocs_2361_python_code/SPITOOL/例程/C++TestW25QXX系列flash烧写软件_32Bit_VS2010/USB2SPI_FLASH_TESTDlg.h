// USB2SPI_FLASH_TESTDlg.h : header file
//

#if !defined(AFX_USB2SPI_FLASH_TESTDLG_H__08359F32_D0E7_4626_BCAC_D85B745BE02D__INCLUDED_)
#define AFX_USB2SPI_FLASH_TESTDLG_H__08359F32_D0E7_4626_BCAC_D85B745BE02D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HexEditBase.h"
#include "HexEdit.h"

#define DEFAUT_PAGE_SIZE			256

#define W25_05_TOTAL_PAGE_NUM		256
#define W25_10_TOTAL_PAGE_NUM		512
#define W25_20_TOTAL_PAGE_NUM		1024
#define W25_40_TOTAL_PAGE_NUM		2048
#define W25_80_TOTAL_PAGE_NUM		4096
#define W25_16_TOTAL_PAGE_NUM		8192
#define W25_32_TOTAL_PAGE_NUM		16384
#define W25_64_TOTAL_PAGE_NUM		32768		
#define W25_128_TOTAL_PAGE_NUM		65536
#define W25_256_TOTAL_PAGE_NUM		131072
#define W25_512_TOTAL_PAGE_NUM		131072*2

#define    SPIFLASH_CMD_LENGTH        0x03   
#define    SPIFLASH_WRITE_BUSYBIT     0X01 //状态寄存器Busy位（状态寄存器的0位为 BUSY位）   
#define    SPIFlash_ReadData_CMD      0x03 //读数据  
#define    SPIFlash_WriteEnable_CMD   0x06  //写使能 会使“状态寄存器”WEL为置位。在执行“擦除”前，需要置位WEL  
#define    SPIFlash_WriteDisable_CMD  0x04  //写使能关闭  
#define    SPIFlash_PageProgram_CMD   0x02   //页编程指令 （页256字节）  
#define    SPIFlash_WriteSR_CMD       0x01   //写状态寄存器  
#define    SPIFlash_ReadSR_CMD        0x05 //读状态寄存器 当CS低位，05H送入引脚，在CLK上升沿数据被芯片采集，芯片会把“状态寄存器”的值从DO输出，数据在CLK下降沿输出，高位在前.  
#define    SPIFlash_SecErase_CMD      0x20  //扇区擦除指令 （一扇区 4K）  
#define    SPIFlash_BlockErase_CMD    0xD8   //块擦除指令  将一块区（64K）全部变成1，即FFH  
#define    SPIFlash_ChipeErase_CMD    0xC7   //芯片擦除指令  
#define    SPIFLASH_ReadChipId_CMD    0x90   //读芯片ID  

////SPI速率
//#define SPI_Rate_281K						0
//#define SPI_Rate_562K						1
//#define SPI_Rate_1_125M						2
//#define SPI_Rate_2_25M						3
//#define SPI_Rate_4_5M						4
//#define SPI_Rate_9M							5
//#define SPI_Rate_18M						6
//#define SPI_Rate_36M						7

#define		SPI_RateL					SPI_Rate_9M
#define		SPI_RateH					SPI_Rate_36M

/////////////////////////////////////////////////////////////////////////////
// CUSB2SPI_FLASH_TESTDlg dialog

class CUSB2SPI_FLASH_TESTDlg : public CDialog
{
// Construction
public:
	void WriteFlashFromFile();
	static DWORD WINAPI ThreadWriteFlash(LPVOID lpParmeter);
	void readFlashToFile();
	static DWORD WINAPI ThreadReadFlash(LPVOID lpParmeter);
	HANDLE m_hThreadWriteFlash;
	HANDLE m_hThreadReadFlash;
	int m_USBConnectFlg;
	CHexEditBase m_EditHex;
	CUSB2SPI_FLASH_TESTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUSB2SPI_FLASH_TESTDlg)
	enum { IDD = IDD_USB2SPI_FLASH_TEST_DIALOG };
	CHexEdit	m_FlashStartAddr;
	CProgressCtrl	m_ProgressCtrl;
	CComboBox	m_CommFlashTypeCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSB2SPI_FLASH_TESTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CUSB2SPI_FLASH_TESTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeEditPageAddr();
	afx_msg void OnButtonPageRead();
	afx_msg void OnButtonPageWrite();
	afx_msg void OnButtonPageErase();
	afx_msg void OnButtonSelectFilePath();
	afx_msg void OnButtonFlashToFile();
	afx_msg void OnButtonFileToFlash();
	afx_msg void OnButtonEraseFullChip();
	afx_msg void OnButtonPageErase64k();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USB2SPI_FLASH_TESTDLG_H__08359F32_D0E7_4626_BCAC_D85B745BE02D__INCLUDED_)
