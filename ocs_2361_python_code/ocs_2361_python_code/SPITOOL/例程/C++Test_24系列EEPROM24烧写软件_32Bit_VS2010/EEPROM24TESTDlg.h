// EEPROM24TESTDlg.h : header file
//

#if !defined(AFX_EEPROM24TESTDLG_H__3291D808_750F_4073_BE36_B0503087A337__INCLUDED_)
#define AFX_EEPROM24TESTDLG_H__3291D808_750F_4073_BE36_B0503087A337__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HexEditBase.h"
#include "HexEdit.h"

#define	IIC_Rate IIC_Rate_400K

#define EDIT_DISPLAY_SIZE	256

/////////////////////////////////////////////////////////////////////////////
// CEEPROM24TESTDlg dialog

class CEEPROM24TESTDlg : public CDialog
{
// Construction
public:
	void WriteFileToEeprom();
	void ReadEepromToFile();
	static DWORD WINAPI ThreadReadEepreom(LPVOID lpParmeter);
	static DWORD WINAPI ThreadWriteEeprom(LPVOID lpParmeter);
	HANDLE m_hThreadWriteEEPROM;
	HANDLE m_hThreadReadEEPROM;
	int m_USBConnectFlg;
	typedef struct {
		unsigned int pageSize;
		unsigned int pageTotal;
		unsigned int wordAddrByteNum;
	}EEPROM_PARA;
	static const EEPROM_PARA m_FWIndex[100];
	CHexEditBase m_EditHex;
	CEEPROM24TESTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEEPROM24TESTDlg)
	enum { IDD = IDD_EEPROM24TEST_DIALOG };
	CProgressCtrl	m_ProgressCtrl;
	CHexEdit	m_EditHexEEAddr;
	CComboBox	m_ComboBoxEepromTypeCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEEPROM24TESTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEEPROM24TESTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeComboEeproType();
	afx_msg void OnChangeEditPageStat();
	afx_msg void OnButtonPageRead();
	afx_msg void OnButtonPageWrite();
	afx_msg void OnButtonPageDelete();
	afx_msg void OnButtonSelectFilePath();
	afx_msg void OnButtonEepromToFile();
	afx_msg void OnButtonFileToEeprom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EEPROM24TESTDLG_H__3291D808_750F_4073_BE36_B0503087A337__INCLUDED_)
