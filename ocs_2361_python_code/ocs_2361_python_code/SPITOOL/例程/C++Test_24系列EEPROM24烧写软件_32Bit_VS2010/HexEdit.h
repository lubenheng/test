#if !defined(AFX_HEXEDIT_H__9B3E2602_BBC1_11D3_BC5C_00A0CC3A0C54__INCLUDED_)
#define AFX_HEXEDIT_H__9B3E2602_BBC1_11D3_BC5C_00A0CC3A0C54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HexEdit.h : header file
//
#define	CHEXEDIT_CASE_BOTH			1
#define	CHEXEDIT_CASE_UPPER			2
#define	CHEXEDIT_CASE_LOWER			4
#define CHEXEDIT_MAX_LENGTH			16

/////////////////////////////////////////////////////////////////////////////
// CHexEdit window

class CHexEdit : public CEdit
{
// Construction
public:
	CHexEdit();

// Operations
public:
	int			GetString(char *);
	UINT64		GetValue();
	int			GetCurrentLength();
	int			GetMaxLength();
	int			SetMaxLength(int lengthValue);
	int			GetCase();
	int			SetCase(int caseValue);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHexEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHexEdit();

	// Generated message map functions
protected:
	BOOL		bInit;
	DWORD		m_currentLength;
	DWORD		m_maxLength;
	DWORD		m_case;
	UINT64		m_value;
	char		m_valueStr[CHEXEDIT_MAX_LENGTH + 1];

	//{{AFX_MSG(CHexEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEXEDIT_H__9B3E2602_BBC1_11D3_BC5C_00A0CC3A0C54__INCLUDED_)
