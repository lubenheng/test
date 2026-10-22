// EEPROM24TESTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EEPROM24TEST.h"
#include "EEPROM24TESTDlg.h"
#include "USB2UARTSPIIICDLL.h"

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
// CEEPROM24TESTDlg dialog
//typedef struct {
//		unsigned int index;
//		unsigned int pageSize;
//		unsigned int pageTotal;
//		unsigned int totalByte;
// 	}EEPROM_PARA;
const CEEPROM24TESTDlg::EEPROM_PARA CEEPROM24TESTDlg::m_FWIndex[] = 
{
	{8,16,1},		//24C01
	{8,32,1},		//24C02
	{16,32,1},		//24C04
	{16,64,1},		//24C08
	{16,128,1},		//24C16		//特殊 11位地址
	{32,128,2},		//24C32
	{32,256,2},		//24C64
	{64,256,2},		//24C128
	{64,512,2},		//24C256
	{128,512,2},	//24C512
	{256,512,2},	//24C1024
};

CEEPROM24TESTDlg::CEEPROM24TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEEPROM24TESTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEEPROM24TESTDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEEPROM24TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEEPROM24TESTDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
	DDX_Control(pDX, IDC_EDIT_EEPROM_ADDR, m_EditHexEEAddr);
	DDX_Control(pDX, IDC_COMBO_EEPRO_TYPE, m_ComboBoxEepromTypeCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEEPROM24TESTDlg, CDialog)
	//{{AFX_MSG_MAP(CEEPROM24TESTDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_EEPRO_TYPE, OnSelchangeComboEeproType)
	ON_EN_CHANGE(IDC_EDIT_PAGE_STAT, OnChangeEditPageStat)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_READ, OnButtonPageRead)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_WRITE, OnButtonPageWrite)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_DELETE, OnButtonPageDelete)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE_PATH, OnButtonSelectFilePath)
	ON_BN_CLICKED(IDC_BUTTON_EEPROM_TO_FILE, OnButtonEepromToFile)
	ON_BN_CLICKED(IDC_BUTTON_FILE_TO_EEPROM, OnButtonFileToEeprom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEEPROM24TESTDlg message handlers

BOOL CEEPROM24TESTDlg::OnInitDialog()
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
	this->SetWindowText("24系列EEPROM读写软件---------------使用IIC口");
	//打开USB
	m_USBConnectFlg = OpenUsb(0);
	if(m_USBConnectFlg >= 0)
	{
		SetDlgItemText(IDC_EDIT_USB_CONNECT_STATUS,"USB已连接");
		ConfigIICParam(IIC_Rate,10000,0);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_USB_CONNECT_STATUS,"USB未连接");
	}


	CRect rect;
	rect.top = 155;
	rect.bottom = rect.top + 220;
	rect.left = 20;
	rect.right = rect.left + 475;

	m_EditHex.RegisterClass();
	m_EditHex.CreateEx(NULL,"123",ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,rect,this,IDC_EDIT_HEX);
	m_EditHex.SetShowAddress(TRUE,TRUE);

	unsigned char temp[EDIT_DISPLAY_SIZE] = {0};
	m_EditHex.SetData(temp,EDIT_DISPLAY_SIZE,TRUE);

	m_ComboBoxEepromTypeCtrl.SetCurSel(0);
	int index = m_ComboBoxEepromTypeCtrl.GetCurSel();
	SetDlgItemInt(IDC_EDIT_MEM_SIZE,m_FWIndex[index].pageSize*m_FWIndex[index].pageTotal);
	SetDlgItemInt(IDC_EDIT_PAGE_SIZE,m_FWIndex[index].pageSize);

	SetDlgItemText(IDC_EDIT_EEPROM_ADDR,"50");
	SetDlgItemText(IDC_EDIT_PAGE_STATE_ADDR,"0x00");
	SetDlgItemInt(IDC_EDIT_PAGE_STAT,0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEEPROM24TESTDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEEPROM24TESTDlg::OnPaint() 
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
HCURSOR CEEPROM24TESTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CEEPROM24TESTDlg::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CEEPROM24TESTDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CEEPROM24TESTDlg::OnSelchangeComboEeproType() 
{
	int index = m_ComboBoxEepromTypeCtrl.GetCurSel();
	
	SetDlgItemInt(IDC_EDIT_MEM_SIZE,m_FWIndex[index].pageSize*m_FWIndex[index].pageTotal);
	SetDlgItemInt(IDC_EDIT_PAGE_SIZE,m_FWIndex[index].pageSize);

	OnChangeEditPageStat();
}

void CEEPROM24TESTDlg::OnChangeEditPageStat() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	int addr;
	int index = m_ComboBoxEepromTypeCtrl.GetCurSel();
	SetDlgItemInt(IDC_EDIT_MEM_SIZE,m_FWIndex[index].pageSize*m_FWIndex[index].pageTotal);
	SetDlgItemInt(IDC_EDIT_PAGE_SIZE,m_FWIndex[index].pageSize);
	addr = m_FWIndex[index].pageSize * GetDlgItemInt(IDC_EDIT_PAGE_STAT);
	CString str;
	str.Format("0x%X",addr);
	SetDlgItemText(IDC_EDIT_PAGE_STATE_ADDR,str);
}

void CEEPROM24TESTDlg::OnButtonPageRead() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}

	unsigned char EEPROMAddr;
	unsigned int wordAddr;
	unsigned char wordAddrTemp;
	unsigned int pageSize;
	unsigned int wordAddrByteNum;
	unsigned char commBuf[1024];
	int index = m_ComboBoxEepromTypeCtrl.GetCurSel();
	pageSize = m_FWIndex[index].pageSize;

	EEPROMAddr = (unsigned char)m_EditHexEEAddr.GetValue();
	EEPROMAddr <<= 1;

	commBuf[0] = EEPROMAddr;

	wordAddr = m_FWIndex[index].pageSize * GetDlgItemInt(IDC_EDIT_PAGE_STAT);
	wordAddrByteNum = m_FWIndex[index].wordAddrByteNum;
	if(index == 4)
	{
		wordAddrTemp = wordAddr>>8;
		wordAddrTemp <<= 1;
		wordAddrTemp &= 0x0E;

		commBuf[1] = wordAddrTemp;
		commBuf[2] = wordAddr;
	}
	else if((index == 5)||(index == 6)||(index == 7)||(index == 8)||(index == 9)||(index == 10))
	{
		commBuf[1] = wordAddr>>8;
		commBuf[2] = wordAddr;
	}
	else
	{
		commBuf[1] = wordAddr;
	}
	IICSendData(TRUE,TRUE,commBuf,wordAddrByteNum+1,0);
	commBuf[0] = EEPROMAddr+1;
	IICSendData(TRUE,FALSE,commBuf,1,0);
	memset(commBuf,0xFF,sizeof(commBuf));
	IICRcvData(TRUE,commBuf,pageSize,0);
	m_EditHex.SetData(commBuf,EDIT_DISPLAY_SIZE,TRUE);
	
}

void CEEPROM24TESTDlg::OnButtonPageWrite() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}
	unsigned char EEPROMAddr;
	unsigned int wordAddr;
	unsigned char wordAddrTemp;
	unsigned int pageSize;
	unsigned int wordAddrByteNum;
	unsigned char commBuf[1024];
	int index = m_ComboBoxEepromTypeCtrl.GetCurSel();
	pageSize = m_FWIndex[index].pageSize;

	EEPROMAddr = (unsigned char)m_EditHexEEAddr.GetValue();
	EEPROMAddr <<= 1;

	commBuf[0] = EEPROMAddr;

	wordAddr = m_FWIndex[index].pageSize * GetDlgItemInt(IDC_EDIT_PAGE_STAT);
	wordAddrByteNum = m_FWIndex[index].wordAddrByteNum;
	if(index == 4)
	{
		wordAddrTemp = wordAddr>>8;
		wordAddrTemp <<= 1;
		wordAddrTemp &= 0x0E;

		commBuf[0] |= wordAddrTemp;
		commBuf[1] = wordAddr;
		m_EditHex.GetData(&commBuf[2],EDIT_DISPLAY_SIZE);
	}
	else if((index == 5)||(index == 6)||(index == 7)||(index == 8)||(index == 9)||(index == 10))
	{
		commBuf[1] = wordAddr>>8;
		commBuf[2] = wordAddr;
		m_EditHex.GetData(&commBuf[3],EDIT_DISPLAY_SIZE);
	}
	else
	{
		commBuf[1] = wordAddr;
		m_EditHex.GetData(&commBuf[2],EDIT_DISPLAY_SIZE);
	}
	
	IICSendData(TRUE,TRUE,commBuf,wordAddrByteNum+1+pageSize,0);

	Sleep(100);
	//写完再读出来
	commBuf[0] = EEPROMAddr;

	wordAddr = m_FWIndex[index].pageSize * GetDlgItemInt(IDC_EDIT_PAGE_STAT);
	wordAddrByteNum = m_FWIndex[index].wordAddrByteNum;
	if(index == 4)
	{
		wordAddrTemp = wordAddr>>8;
		wordAddrTemp <<= 1;
		wordAddrTemp &= 0x0E;

		commBuf[1] = wordAddrTemp;
		commBuf[2] = wordAddr;
	}
	else if((index == 5)||(index == 6)||(index == 7)||(index == 8)||(index == 9)||(index == 10))
	{
		commBuf[1] = wordAddr>>8;
		commBuf[2] = wordAddr;
	}
	else
	{
		commBuf[1] = wordAddr;
	}
	IICSendData(TRUE,TRUE,commBuf,wordAddrByteNum+1,0);

	commBuf[0] = EEPROMAddr+1;
	IICSendData(TRUE,FALSE,commBuf,1,0);
	memset(commBuf,0xFF,sizeof(commBuf));
	IICRcvData(TRUE,commBuf,pageSize,0);
	m_EditHex.SetData(commBuf,EDIT_DISPLAY_SIZE,TRUE);
}

void CEEPROM24TESTDlg::OnButtonPageDelete() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}
	unsigned char EEPROMAddr;
	unsigned int wordAddr;
	unsigned char wordAddrTemp;
	unsigned int pageSize;
	unsigned int wordAddrByteNum;
	unsigned char commBuf[1024];
	int index = m_ComboBoxEepromTypeCtrl.GetCurSel();
	pageSize = m_FWIndex[index].pageSize;

	EEPROMAddr = (unsigned char)m_EditHexEEAddr.GetValue();
	EEPROMAddr <<= 1;

	commBuf[0] = EEPROMAddr;

	wordAddr = m_FWIndex[index].pageSize * GetDlgItemInt(IDC_EDIT_PAGE_STAT);
	wordAddrByteNum = m_FWIndex[index].wordAddrByteNum;
	if(index == 4)
	{
		wordAddrTemp = wordAddr>>8;
		wordAddrTemp <<= 1;
		wordAddrTemp &= 0x0E;

		commBuf[0] |= wordAddrTemp;
		commBuf[1] = wordAddr;
		memset(&commBuf[2],0xFF,EDIT_DISPLAY_SIZE);
	}
	else if((index == 5)||(index == 6)||(index == 7)||(index == 8)||(index == 9)||(index == 10))
	{
		commBuf[1] = wordAddr>>8;
		commBuf[2] = wordAddr;
		memset(&commBuf[3],0xFF,EDIT_DISPLAY_SIZE);
	}
	else
	{
		commBuf[1] = wordAddr;
		memset(&commBuf[2],0xFF,EDIT_DISPLAY_SIZE);
	}
	
	IICSendData(TRUE,TRUE,commBuf,wordAddrByteNum+1+pageSize,0);

	//写完再读出来
	commBuf[0] = EEPROMAddr;

	wordAddr = m_FWIndex[index].pageSize * GetDlgItemInt(IDC_EDIT_PAGE_STAT);
	wordAddrByteNum = m_FWIndex[index].wordAddrByteNum;
	if(index == 4)
	{
		wordAddrTemp = wordAddr>>8;
		wordAddrTemp <<= 1;
		wordAddrTemp &= 0x0E;

		commBuf[1] = wordAddrTemp;
		commBuf[2] = wordAddr;
	}
	else if((index == 5)||(index == 6)||(index == 7)||(index == 8)||(index == 9)||(index == 10))
	{
		commBuf[1] = wordAddr>>8;
		commBuf[2] = wordAddr;
	}
	else
	{
		commBuf[1] = wordAddr;
	}
	IICSendData(TRUE,TRUE,commBuf,wordAddrByteNum+1,0);

	commBuf[0] = EEPROMAddr+1;
	IICSendData(TRUE,FALSE,commBuf,1,0);
	memset(commBuf,0xFF,sizeof(commBuf));
	IICRcvData(TRUE,commBuf,pageSize,0);
	m_EditHex.SetData(commBuf,EDIT_DISPLAY_SIZE,TRUE);
}

LRESULT CEEPROM24TESTDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
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
				ConfigIICParam(IIC_Rate,10000,0);
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

void CEEPROM24TESTDlg::OnButtonSelectFilePath() 
{
	CString FilePathName;
	CFileDialog dlg(TRUE);//,"(*.H01)|*.H01","*.H01",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"数据文件(*.H01;)");///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		SetDlgItemText(IDC_EDIT_FILE_TO_EEPROM_PATH,FilePathName);	
	}
}

void CEEPROM24TESTDlg::OnButtonEepromToFile() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}
	m_hThreadReadEEPROM = CreateThread(NULL,0,ThreadReadEepreom,this,0,NULL);
	CloseHandle(m_hThreadReadEEPROM);
}

void CEEPROM24TESTDlg::OnButtonFileToEeprom() 
{
	if(m_USBConnectFlg < 0)
	{
		MessageBox("USB未连接");
		return;
	}
	CString FilePathName;
	GetDlgItemText(IDC_EDIT_FILE_TO_EEPROM_PATH,FilePathName);
	if(FilePathName.IsEmpty())
	{
		MessageBox("请选择文件");
		return;
	}
	m_hThreadWriteEEPROM = CreateThread(NULL,0,ThreadWriteEeprom,this,0,NULL);
	CloseHandle(m_hThreadWriteEEPROM);
}

DWORD WINAPI CEEPROM24TESTDlg::ThreadWriteEeprom(LPVOID lpParmeter)
{
	CEEPROM24TESTDlg *dlg;
	dlg = (CEEPROM24TESTDlg*)lpParmeter;
	dlg->WriteFileToEeprom();
	return TRUE;
}

DWORD WINAPI CEEPROM24TESTDlg::ThreadReadEepreom(LPVOID lpParmeter)
{
	CEEPROM24TESTDlg *dlg;
	dlg = (CEEPROM24TESTDlg*)lpParmeter;
	dlg->ReadEepromToFile();
	return TRUE;
}

void CEEPROM24TESTDlg::ReadEepromToFile()
{
	unsigned char EEPROMAddr;
	unsigned int wordAddr;
	unsigned char wordAddrTemp;
	unsigned int pageSize;
	unsigned int wordAddrByteNum;
	unsigned char commBuf[1024];
	unsigned int i;
	int index = m_ComboBoxEepromTypeCtrl.GetCurSel();
	pageSize = m_FWIndex[index].pageSize;

	EEPROMAddr = (unsigned char)m_EditHexEEAddr.GetValue();
	EEPROMAddr <<= 1;

	commBuf[0] = EEPROMAddr;

	CString FilePathName;
	CFileDialog dlg(FALSE,NULL,"文件名.bin");
	if(dlg.DoModal() == IDOK)
	{
		m_ProgressCtrl.SetRange32(0,m_FWIndex[index].pageTotal);
		m_ProgressCtrl.SetPos(0);
		FilePathName=dlg.GetPathName();
		CFile file;
		file.Open(FilePathName,CFile::modeCreate|CFile::modeWrite);
		for(i=0;i<m_FWIndex[index].pageTotal;i++)
		{
			wordAddr = m_FWIndex[index].pageSize * i;
			wordAddrByteNum = m_FWIndex[index].wordAddrByteNum;
			if(index == 4)
			{
				wordAddrTemp = wordAddr>>8;
				wordAddrTemp <<= 1;
				wordAddrTemp &= 0x0E;

				commBuf[1] = wordAddrTemp;
				commBuf[2] = wordAddr;
			}
			else if((index == 5)||(index == 6)||(index == 7)||(index == 8)||(index == 9)||(index == 10))
			{
				commBuf[1] = wordAddr>>8;
				commBuf[2] = wordAddr;
			}
			else
			{
				commBuf[1] = wordAddr;
			}
			IICSendData(TRUE,TRUE,commBuf,wordAddrByteNum+1,0);

			commBuf[0] = EEPROMAddr+1;
			IICSendData(TRUE,FALSE,commBuf,1,0);
			memset(commBuf,0xFF,sizeof(commBuf));
			IICRcvData(TRUE,commBuf,pageSize,0);
			m_EditHex.SetData(commBuf,EDIT_DISPLAY_SIZE,TRUE);
			file.Write(commBuf,pageSize);
			m_ProgressCtrl.SetPos(i);
			
		}
		file.Close();
		m_ProgressCtrl.SetPos(m_FWIndex[index].pageTotal);
		MessageBox("读取完成");
	}
	
}

void CEEPROM24TESTDlg::WriteFileToEeprom()
{
	unsigned char EEPROMAddr;
	unsigned int wordAddr;
	unsigned char wordAddrTemp;
	unsigned int pageSize;
	unsigned int wordAddrByteNum;
	unsigned char commBuf[1024];
	unsigned int i;
	unsigned int writeBytes;
	int index = m_ComboBoxEepromTypeCtrl.GetCurSel();
	pageSize = m_FWIndex[index].pageSize;

	EEPROMAddr = (unsigned char)m_EditHexEEAddr.GetValue();
	EEPROMAddr <<= 1;

	commBuf[0] = EEPROMAddr;
	CString FilePathName;
	GetDlgItemText(IDC_EDIT_FILE_TO_EEPROM_PATH,FilePathName);

	m_ProgressCtrl.SetRange32(0,m_FWIndex[index].pageTotal);
	m_ProgressCtrl.SetPos(0);
	CFile file;
	file.Open(FilePathName,CFile::modeRead);
	for(i=0;i<m_FWIndex[index].pageTotal;i++)
	{
		
		wordAddr = m_FWIndex[index].pageSize * i;
		wordAddrByteNum = m_FWIndex[index].wordAddrByteNum;
		if(index == 4)
		{
			wordAddrTemp = wordAddr>>8;
			wordAddrTemp <<= 1;
			wordAddrTemp &= 0x0E;

			commBuf[0] |= wordAddrTemp;
			commBuf[1] = wordAddr;
			writeBytes = file.Read(&commBuf[2],m_FWIndex[index].pageSize);
			if(writeBytes<0)
			{
				file.Close();
				MessageBox("写入失败");
				return;
			}
			m_EditHex.SetData(&commBuf[2],EDIT_DISPLAY_SIZE,TRUE);
		}
		else if((index == 5)||(index == 6)||(index == 7)||(index == 8)||(index == 9)||(index == 10))
		{
			commBuf[1] = wordAddr>>8;
			commBuf[2] = wordAddr;
			writeBytes = file.Read(&commBuf[3],m_FWIndex[index].pageSize);
			if(writeBytes<0)
			{
				file.Close();
				MessageBox("写入失败");
				return;
			}
			m_EditHex.SetData(&commBuf[3],EDIT_DISPLAY_SIZE,TRUE);
		}
		else
		{
			commBuf[1] = wordAddr;
			writeBytes = file.Read(&commBuf[2],m_FWIndex[index].pageSize);
			if(writeBytes<0)
			{
				file.Close();
				MessageBox("写入失败");
				return;
			}
			m_EditHex.SetData(&commBuf[2],EDIT_DISPLAY_SIZE,TRUE);
		}
		
		IICSendData(TRUE,TRUE,commBuf,wordAddrByteNum+1+writeBytes,0);
		Sleep(50);
		m_ProgressCtrl.SetPos(i);

		if(writeBytes != m_FWIndex[index].pageSize)
		{
			file.Close();
			MessageBox("写入完成");
			return;
		}
	}
	file.Close();
	MessageBox("写入完成");
}
