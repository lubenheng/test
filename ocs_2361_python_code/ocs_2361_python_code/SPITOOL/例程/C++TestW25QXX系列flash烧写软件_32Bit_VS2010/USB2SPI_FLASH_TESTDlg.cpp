// USB2SPI_FLASH_TESTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "USB2SPI_FLASH_TEST.h"
#include "USB2SPI_FLASH_TESTDlg.h"
#include "USB2UARTSPIIICDLL.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSB2SPI_FLASH_TESTDlg dialog

CUSB2SPI_FLASH_TESTDlg::CUSB2SPI_FLASH_TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSB2SPI_FLASH_TESTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUSB2SPI_FLASH_TESTDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUSB2SPI_FLASH_TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUSB2SPI_FLASH_TESTDlg)
	DDX_Control(pDX, IDC_EDIT_FLASH_START_ADDR, m_FlashStartAddr);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
	DDX_Control(pDX, IDC_COMBO_FLASH_TYPE, m_CommFlashTypeCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUSB2SPI_FLASH_TESTDlg, CDialog)
	//{{AFX_MSG_MAP(CUSB2SPI_FLASH_TESTDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_PAGE_ADDR, OnChangeEditPageAddr)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_READ, OnButtonPageRead)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_WRITE, OnButtonPageWrite)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_ERASE, OnButtonPageErase)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE_PATH, OnButtonSelectFilePath)
	ON_BN_CLICKED(IDC_BUTTON_FLASH_TO_FILE, OnButtonFlashToFile)
	ON_BN_CLICKED(IDC_BUTTON_FILE_TO_FLASH, OnButtonFileToFlash)
	ON_BN_CLICKED(IDC_BUTTON_ERASE_FULL_CHIP, OnButtonEraseFullChip)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_ERASE64K, OnButtonPageErase64k)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSB2SPI_FLASH_TESTDlg message handlers

BOOL CUSB2SPI_FLASH_TESTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	//设置软件标题
	this->SetWindowText("串行Flash读写软件---------------使用SPI口");
	unsigned char UID[12];
	unsigned int isHsUSB;
	unsigned int FwVersion;

	//打开USB
	m_USBConnectFlg = OpenUsb(0);
	if(m_USBConnectFlg >= 0)
	{
		SetDlgItemText(IDC_EDIT_USB_CONNECT_STATUS,"USB已连接");
		isHsUSB = 0;
		GetBoardInformation (UID,&isHsUSB,&FwVersion,0);
		if(isHsUSB)
			ConfigSPIParam(SPI_RateH,SPI_MSB,SPI_SubMode_0,0);
		else
			ConfigSPIParam(SPI_RateL,SPI_MSB,SPI_SubMode_0,0);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_USB_CONNECT_STATUS,"USB未连接");
	}


	
	CRect rect;
	rect.top = 50;
	rect.bottom = rect.top + 220;
	rect.left = 20;
	rect.right = rect.left + 470;

	m_EditHex.RegisterClass();
	m_EditHex.CreateEx(NULL,"123",ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,rect,this,IDC_EDIT_HEX);
	m_EditHex.SetShowAddress(TRUE,TRUE);

	unsigned char temp[256] = {0};
	m_EditHex.SetData(temp,sizeof(temp),TRUE);

	SetDlgItemInt(IDC_EDIT_PAGE_ADDR,0);
	SetDlgItemInt(IDC_EDIT_FLASH_START_ADDR,0);

	m_CommFlashTypeCtrl.SetCurSel(0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUSB2SPI_FLASH_TESTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUSB2SPI_FLASH_TESTDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUSB2SPI_FLASH_TESTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CUSB2SPI_FLASH_TESTDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CUSB2SPI_FLASH_TESTDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CloseUsb(0);
	CDialog::OnCancel();
}

void CUSB2SPI_FLASH_TESTDlg::OnChangeEditPageAddr() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	unsigned int addr = GetDlgItemInt(IDC_EDIT_PAGE_ADDR);
	addr *= 256;
	CString str;
	str.Format("起始地址：0x%02X%02X%02X",(unsigned char)(addr>>16),(unsigned char)(addr>>8),(unsigned char)addr);
	SetDlgItemText(IDC_STATIC_ADDR,str);
}

LRESULT CUSB2SPI_FLASH_TESTDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	unsigned char UID[12];
	unsigned int isHsUSB;
	unsigned int FwVersion;
	switch(message) 
	{
	case WM_DEVICECHANGE:
		switch((UINT)(wParam))   
		{   
		case 7:		//usb插入和拔出都会检测到wParam值为7
			CloseUsb(0);
			m_USBConnectFlg = OpenUsb(0);
			if(m_USBConnectFlg >= 0)
			{
				SetDlgItemText(IDC_EDIT_USB_CONNECT_STATUS,"USB已连接");
				isHsUSB = 0;
				GetBoardInformation (UID,&isHsUSB,&FwVersion,0);
				if(isHsUSB)
					ConfigSPIParam(SPI_RateH,SPI_MSB,SPI_SubMode_0,0);
				else
					ConfigSPIParam(SPI_RateL,SPI_MSB,SPI_SubMode_0,0);
			}
			else
			{
				SetDlgItemText(IDC_EDIT_USB_CONNECT_STATUS,"USB未连接");
			}
			break;
		default:    
			break;   
		}  
		break;
		default:
			break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CUSB2SPI_FLASH_TESTDlg::OnButtonPageRead() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}

	unsigned char temp[1024];
	ZeroMemory(temp,sizeof(temp));
	unsigned int addr = GetDlgItemInt(IDC_EDIT_PAGE_ADDR);
	addr *= 256;
	temp[0] = SPIFlash_ReadData_CMD;
	temp[1] = (unsigned char)(addr>>16);
	temp[2] = (unsigned char)(addr>>8);
	temp[3] = (unsigned char)(addr>>0);
	SPISendData(0,0,temp,4,0);
	ZeroMemory(temp,sizeof(temp));
	SPIRcvData(0,1,temp,DEFAUT_PAGE_SIZE,0);
	m_EditHex.SetData(temp,DEFAUT_PAGE_SIZE);
	
}

void CUSB2SPI_FLASH_TESTDlg::OnButtonPageWrite() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}
	unsigned char temp[1024];
	ZeroMemory(temp,sizeof(temp));
	unsigned int addr = GetDlgItemInt(IDC_EDIT_PAGE_ADDR);
	addr *= 256;
	temp[0] = SPIFlash_WriteEnable_CMD;
	SPISendData(0,1,temp,1,0);

	temp[0] = SPIFlash_PageProgram_CMD;
	temp[1] = (unsigned char)(addr>>16);
	temp[2] = (unsigned char)(addr>>8);
	temp[3] = (unsigned char)(addr>>0);
	SPISendData(0,0,temp,4,0);
	ZeroMemory(temp,sizeof(temp));
	m_EditHex.GetData(temp,DEFAUT_PAGE_SIZE);
	SPISendData(0,1,temp,DEFAUT_PAGE_SIZE,0);
	
	//写完以后再读出
	temp[0] = SPIFlash_ReadData_CMD;
	temp[1] = (unsigned char)(addr>>16);
	temp[2] = (unsigned char)(addr>>8);
	temp[3] = (unsigned char)(addr>>0);
	SPISendData(0,0,temp,4,0);
	ZeroMemory(temp,sizeof(temp));
	SPIRcvData(0,1,temp,DEFAUT_PAGE_SIZE,0);
	m_EditHex.SetData(temp,DEFAUT_PAGE_SIZE);
}

void CUSB2SPI_FLASH_TESTDlg::OnButtonPageErase() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}

	unsigned char temp[1024];
	temp[0] = SPIFlash_WriteEnable_CMD;
	SPISendData(0,1,temp,1,0);


	ZeroMemory(temp,sizeof(temp));
	unsigned int addr = GetDlgItemInt(IDC_EDIT_PAGE_ADDR);
	addr *= 256;
	temp[0] = SPIFlash_SecErase_CMD;
	temp[1] = (unsigned char)(addr>>16);
	temp[2] = (unsigned char)(addr>>8);
	temp[3] = (unsigned char)(addr>>0);
	SPISendData(0,1,temp,4,0);
}

void CUSB2SPI_FLASH_TESTDlg::OnButtonSelectFilePath() 
{
	CString FilePathName;
	CFileDialog dlg(TRUE);//,"(*.H01)|*.H01","*.H01",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"数据文件(*.H01;)");///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		SetDlgItemText(IDC_EDIT_FILE_TO_FLASH_PATH,FilePathName);	
	}
}

void CUSB2SPI_FLASH_TESTDlg::OnButtonFlashToFile() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}
	m_hThreadReadFlash = CreateThread(NULL,0,ThreadReadFlash,this,0,NULL);
	CloseHandle(m_hThreadReadFlash);
}

DWORD WINAPI CUSB2SPI_FLASH_TESTDlg::ThreadReadFlash(LPVOID lpParmeter)
{
	CUSB2SPI_FLASH_TESTDlg *dlg;
	dlg = (CUSB2SPI_FLASH_TESTDlg*)lpParmeter;
	dlg->readFlashToFile();
	return TRUE;
}

void CUSB2SPI_FLASH_TESTDlg::readFlashToFile()
{
	/*
	unsigned int maxAddr = 0;
	unsigned int addr = 0;
	double x, y;
	unsigned char temp[1024];
	x = m_CommFlashTypeCtrl.GetCurSel();
	y = pow(2,x);
	maxAddr = DEFAUT_PAGE_SIZE * (unsigned int)y;
	maxAddr *= 256;
	CString FilePathName;
	CFileDialog dlg(FALSE,NULL,"文件名.bin");
	if(dlg.DoModal() == IDOK)
	{
		m_ProgressCtrl.SetRange32(0,maxAddr);
		m_ProgressCtrl.SetPos(0);
		FilePathName=dlg.GetPathName();
		CFile file;
		file.Open(FilePathName,CFile::modeCreate|CFile::modeWrite);
		for(addr=0;addr<maxAddr;addr+=DEFAUT_PAGE_SIZE)
		{
			ZeroMemory(temp,sizeof(temp));
			temp[0] = SPIFlash_ReadData_CMD;
			temp[1] = (unsigned char)(addr>>16);
			temp[2] = (unsigned char)(addr>>8);
			temp[3] = (unsigned char)(addr>>0);
			SPISendData(0,0,temp,4);
			ZeroMemory(temp,sizeof(temp));
			SPIRcvData(0,1,temp,DEFAUT_PAGE_SIZE);
			m_EditHex.SetData(temp,DEFAUT_PAGE_SIZE);
			file.Write(temp,DEFAUT_PAGE_SIZE);
			m_ProgressCtrl.SetPos(addr);
		}
 		file.Close();
		m_ProgressCtrl.SetPos(maxAddr);
		MessageBox("读取完成");
	}
	*/
	unsigned int maxAddr = 0;
	unsigned int addr = 0;
	double x, y;
	unsigned char temp[1024];
	x = m_CommFlashTypeCtrl.GetCurSel();
	y = pow(2,x);
	maxAddr = DEFAUT_PAGE_SIZE * (unsigned int)y;
	maxAddr *= 256;
	CString FilePathName;
	CFileDialog dlg(FALSE,NULL,"文件名.bin");
	if(dlg.DoModal() == IDOK)
	{
		m_ProgressCtrl.SetRange32(0,maxAddr);
		m_ProgressCtrl.SetPos(0);
		FilePathName=dlg.GetPathName();
		CFile file;
		file.Open(FilePathName,CFile::modeCreate|CFile::modeWrite);
		for(addr=0;addr<maxAddr;addr+=DEFAUT_PAGE_SIZE)
		{
			ZeroMemory(temp,sizeof(temp));
			temp[0] = SPIFlash_ReadData_CMD;
			temp[1] = (unsigned char)(addr>>16);
			temp[2] = (unsigned char)(addr>>8);
			temp[3] = (unsigned char)(addr>>0);
			SPISendData(0,0,temp,4,0);
			ZeroMemory(temp,sizeof(temp));
			SPIRcvData(0,1,temp,DEFAUT_PAGE_SIZE,0);
			m_EditHex.SetData(temp,DEFAUT_PAGE_SIZE);
			file.Write(temp,DEFAUT_PAGE_SIZE);
			m_ProgressCtrl.SetPos(addr);
		}
 		file.Close();
		m_ProgressCtrl.SetPos(maxAddr);
		MessageBox("读取完成");
	}
}

void CUSB2SPI_FLASH_TESTDlg::OnButtonFileToFlash() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}
	CString FilePathName;
	GetDlgItemText(IDC_EDIT_FILE_TO_FLASH_PATH,FilePathName);
	if(FilePathName.IsEmpty())
	{
		MessageBox("请选择文件");
		return;
	}
	m_hThreadWriteFlash = CreateThread(NULL,0,ThreadWriteFlash,this,0,NULL);
	CloseHandle(m_hThreadWriteFlash);
}

DWORD WINAPI CUSB2SPI_FLASH_TESTDlg::ThreadWriteFlash(LPVOID lpParmeter)
{
	CUSB2SPI_FLASH_TESTDlg *dlg;
	dlg = (CUSB2SPI_FLASH_TESTDlg*)lpParmeter;
	dlg->WriteFlashFromFile();
	return TRUE;
}

void CUSB2SPI_FLASH_TESTDlg::WriteFlashFromFile()
{
	/*
	int i;
	unsigned int maxAddr = 0;
	unsigned int addr = 0;
	double x, y;
	unsigned char temp[1024];
	unsigned char temp1[1024];
	unsigned char temp2[1024];
	int writeBytes;
	x = m_CommFlashTypeCtrl.GetCurSel();
	y = pow(2,x);
	maxAddr = DEFAUT_PAGE_SIZE * (unsigned int)y;
	maxAddr *= 256;
	CString FilePathName;
	GetDlgItemText(IDC_EDIT_FILE_TO_FLASH_PATH,FilePathName);

	m_ProgressCtrl.SetRange32(0,maxAddr);
	m_ProgressCtrl.SetPos(0);
	CFile file;
	file.Open(FilePathName,CFile::modeRead);
	for(addr=0;addr<maxAddr;addr+=DEFAUT_PAGE_SIZE)
	{
		temp[0] = SPIFlash_WriteEnable_CMD;
		SPISendData(0,1,temp,1);

		temp[0] = SPIFlash_PageProgram_CMD;
		temp[1] = (unsigned char)(addr>>16);
		temp[2] = (unsigned char)(addr>>8);
		temp[3] = (unsigned char)(addr>>0);
		SPISendData(0,0,temp,4);
		ZeroMemory(temp,sizeof(temp));
		writeBytes = file.Read(temp,DEFAUT_PAGE_SIZE);
		if(writeBytes<0)
		{
			file.Close();
			//m_ProgressCtrl.SetPos(maxAddr);
			MessageBox("写入失败");
			return;
		}
		SPISendData(0,1,temp,writeBytes);

		temp2[0] = SPIFlash_ReadSR_CMD;
		SPISendData(0,0,temp2,1);
		do
		{
			SPIRcvData(0,0,temp2,10);
		}while(temp2[0]&0x01 == 1);
		SPIRcvData(0,1,temp2,1);
		
		//写完以后再读出
		temp1[0] = SPIFlash_ReadData_CMD;
		temp1[1] = (unsigned char)(addr>>16);
		temp1[2] = (unsigned char)(addr>>8);
		temp1[3] = (unsigned char)(addr>>0);
		SPISendData(0,0,temp1,4);
		ZeroMemory(temp1,sizeof(temp1));
		SPIRcvData(0,1,temp1,writeBytes);
		m_EditHex.SetData(temp1,DEFAUT_PAGE_SIZE);

		for(i=0;i<writeBytes;i++)
		{
			if(temp[i] != temp1[i])
			{
				file.Close();
				MessageBox("写入失败");
				return;
			}
		}
		m_ProgressCtrl.SetPos(addr);
		if(writeBytes<(int)DEFAUT_PAGE_SIZE)
		{
			file.Close();
			m_ProgressCtrl.SetPos(maxAddr);
			MessageBox("写入完成");
			return;
		}
		
	}
 	file.Close();
	m_ProgressCtrl.SetPos(maxAddr);
	MessageBox("写入完成");
	*/

	unsigned int maxAddr = 0;
	unsigned int addr = 0;
	unsigned int addrTemp = 0;
	unsigned int startAddr = 0;
	unsigned int eraseTimes = 0;
	double x, y;
	unsigned char temp[1024];
	unsigned char temp2[1024];
	int writeBytes;
	x = m_CommFlashTypeCtrl.GetCurSel();
	y = pow(2,x);
	maxAddr = DEFAUT_PAGE_SIZE * (unsigned int)y;
	maxAddr *= 256;
	CString FilePathName;
	GetDlgItemText(IDC_EDIT_FILE_TO_FLASH_PATH,FilePathName);

	m_ProgressCtrl.SetRange32(0,maxAddr);
	m_ProgressCtrl.SetPos(0);
	CFile file;
	file.Open(FilePathName,CFile::modeRead);
	eraseTimes = 0;
	startAddr = (unsigned int)m_FlashStartAddr.GetValue();
	for(addr=startAddr;addr<maxAddr;)//addr+=DEFAUT_PAGE_SIZE)
	{
		//if(addr%4096 == 0)
		if((eraseTimes%4)==0)
		{
			if(((CButton*)(GetDlgItem( IDC_CHECK_BEFOR_WRITE_ERZESE )))->GetCheck())
			{
				temp[0] = SPIFlash_WriteEnable_CMD;
				SPISendData(0,1,temp,1,0);

				temp[0] = SPIFlash_SecErase_CMD;
				temp[1] = (unsigned char)(addr>>16);
				temp[2] = (unsigned char)(addr>>8);
				temp[3] = (unsigned char)(addr>>0);
				SPISendData(0,1,temp,4,0);

				temp2[0] = SPIFlash_ReadSR_CMD;
				SPISendData(0,0,temp2,1,0);
				do
				{
					SPIRcvData(0,0,temp2,10,0);
				}while((temp2[0]&0x01) == 1);
				SPIRcvData(0,1,temp2,1,0);
			}
		}
		eraseTimes++;

		if(addr == startAddr)
		{
			//写使能
			temp[0] = SPIFlash_WriteEnable_CMD;
			SPISendData(0,1,temp,1,0);
			//写数据
			temp[0] = SPIFlash_PageProgram_CMD;
			temp[1] = (unsigned char)(addr>>16);
			temp[2] = (unsigned char)(addr>>8);
			temp[3] = (unsigned char)(addr>>0);
			SPISendData(0,0,temp,4,0);
			ZeroMemory(temp,sizeof(temp));
			writeBytes = file.Read(temp,DEFAUT_PAGE_SIZE-(startAddr%DEFAUT_PAGE_SIZE));
			if(writeBytes<0)
			{
				file.Close();
				MessageBox("写入失败");
				return;
			}
			SPISendData(0,1,temp,writeBytes,0);

			temp2[0] = SPIFlash_ReadSR_CMD;
			SPISendData(0,0,temp2,1,0);
			do
			{
				SPIRcvData(0,0,temp2,10,0);
			}while((temp2[0]&0x01) == 1);
			SPIRcvData(0,1,temp2,1,0);
			m_ProgressCtrl.SetPos(addr);
 			m_EditHex.SetData(temp,DEFAUT_PAGE_SIZE);
		}
		else
		{
			//写使能
			temp[0] = SPIFlash_WriteEnable_CMD;
			SPISendData(0,1,temp,1,0);
			//写数据
			temp[0] = SPIFlash_PageProgram_CMD;
			temp[1] = (unsigned char)(addr>>16);
			temp[2] = (unsigned char)(addr>>8);
			temp[3] = (unsigned char)(addr>>0);
			SPISendData(0,0,temp,4,0);
			ZeroMemory(temp,sizeof(temp));
			writeBytes = file.Read(temp,DEFAUT_PAGE_SIZE);
			if(writeBytes<0)
			{
				file.Close();
				MessageBox("写入失败");
				return;
			}
			
			SPISendData(0,1,temp,writeBytes,0);

			temp2[0] = SPIFlash_ReadSR_CMD;
			SPISendData(0,0,temp2,1,0);
			do
			{
				SPIRcvData(0,0,temp2,10,0);
			}while((temp2[0]&0x01) == 1);
			SPIRcvData(0,1,temp2,1,0);
			m_ProgressCtrl.SetPos(addr);
			if(writeBytes > 0)
 				m_EditHex.SetData(temp,DEFAUT_PAGE_SIZE);

			if(writeBytes == 0)
			{
				file.Close();
				m_ProgressCtrl.SetPos(maxAddr);
				MessageBox("写入完成");
				return;
			}
		}

		addr += writeBytes;
			
	}
 	file.Close();
	m_ProgressCtrl.SetPos(maxAddr);
	MessageBox("写入完成");

}

void CUSB2SPI_FLASH_TESTDlg::OnButtonEraseFullChip() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}
	unsigned char temp[1024];
	ZeroMemory(temp,sizeof(temp));
	temp[0] = SPIFlash_WriteEnable_CMD;
	SPISendData(0,1,temp,1,0);

	temp[0] = SPIFlash_ChipeErase_CMD;
	SPISendData(0,1,temp,1,0);

	temp[0] = SPIFlash_ReadSR_CMD;
	SPISendData(0,0,temp,1,0);

	m_ProgressCtrl.SetRange32(0,5000);
	m_ProgressCtrl.SetPos(0);
	
	int i = 0;

	do
	{
		SPIRcvData(0,0,temp,10,0);
		Sleep(100);
		m_ProgressCtrl.SetPos(i++);
		
	}while((temp[0]&0x01) == 1);
	SPIRcvData(0,1,temp,1,0);

	m_ProgressCtrl.SetPos(5000);
	
	MessageBox("整片擦除完成");


}

void CUSB2SPI_FLASH_TESTDlg::OnButtonPageErase64k() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}

	unsigned char temp[1024];
	temp[0] = SPIFlash_WriteEnable_CMD;
	SPISendData(0,1,temp,1,0);


	ZeroMemory(temp,sizeof(temp));
	unsigned int addr = GetDlgItemInt(IDC_EDIT_PAGE_ADDR);
	addr *= 256;
	temp[0] = SPIFlash_BlockErase_CMD;
	temp[1] = (unsigned char)(addr>>16);
	temp[2] = (unsigned char)(addr>>8);
	temp[3] = (unsigned char)(addr>>0);
	SPISendData(0,1,temp,4,0);
}
