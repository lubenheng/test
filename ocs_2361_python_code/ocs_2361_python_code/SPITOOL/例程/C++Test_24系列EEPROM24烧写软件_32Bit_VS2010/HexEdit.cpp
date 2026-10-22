// HexEdit.cpp : implementation file
//

#include "stdafx.h"
#include "HexEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHexEdit
BEGIN_MESSAGE_MAP(CHexEdit, CEdit)
	//{{AFX_MSG_MAP(CHexEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHexEdit message handlers

CHexEdit::CHexEdit()
{
	bInit = FALSE;
	m_currentLength = 0;
	m_maxLength = 2;
	m_case = CHEXEDIT_CASE_BOTH;
	m_value = 0;
	memset(m_valueStr, 0, CHEXEDIT_MAX_LENGTH + 1);
}

CHexEdit::~CHexEdit()
{
}


UINT64 CHexEdit::GetValue()
{
	GetWindowText(m_valueStr, CHEXEDIT_MAX_LENGTH + 1);
	m_currentLength = strlen(m_valueStr);
/*
	if (m_currentLength <= 8)
		m_value = strtoul(m_valueStr, NULL, 16);
	else
	{
		UINT64	multiplier = 1;
		char	*hexTable = "0123456789abcdef", *p;
		int		i, j;

		m_value = 0;
		strlwr(m_valueStr);
		for (i = m_currentLength - 1; i >= 0; i--)
		{
			p = hexTable;
			j = 0;
			while (*p++ != m_valueStr[i]) j++;
			m_value += (j * multiplier);
			multiplier *= 16;
		}
	}
*/
	char	*p = m_valueStr;
	m_value = 0;
	for (DWORD i = 0; i < m_currentLength; i++)
	{
		m_value *= 16;
		if (*p >= '0' && *p <= '9')
			m_value += (*p - '0');
		else if (*p >= 'a' && *p <= 'f')
			m_value += (10 + *p - 'a');
		else if (*p >= 'A' && *p <= 'F')
			m_value += (10 + *p - 'A');
		p++;
	}
	return(m_value);
}


int CHexEdit::GetString(char *buf)
{
	if (m_currentLength == 0)
		return(-1);
	else
	{
		GetWindowText(buf, CHEXEDIT_MAX_LENGTH + 1);
		return(0);
	}
}


int CHexEdit::GetCurrentLength()
{
	return(m_currentLength);
}


int CHexEdit::GetMaxLength()
{
	return(m_maxLength);
}


int CHexEdit::SetMaxLength(int lengthValue)
{
	if (lengthValue <= 0 || lengthValue > CHEXEDIT_MAX_LENGTH)
		return(-1);
	else
	{
		m_maxLength = lengthValue;
		LimitText(m_maxLength);
		return(lengthValue);
	}
}


int CHexEdit::GetCase()
{
	return(m_case);
}


int CHexEdit::SetCase(int caseValue)
{
	switch (caseValue)
	{
		case CHEXEDIT_CASE_BOTH:
			m_case = caseValue;
			break;
		case CHEXEDIT_CASE_UPPER:
			m_case = caseValue;
			if (m_currentLength > 0)
			{
				GetWindowText(m_valueStr, CHEXEDIT_MAX_LENGTH + 1);
				char *p = m_valueStr;
				while (*p != 0)
				{
					if (*p >= 'a' && *p <= 'z')
						*p -= 32;
					p++;
				}
				SetWindowText(m_valueStr);
			}
			break;
		case CHEXEDIT_CASE_LOWER:
			m_case = caseValue;
			if (m_currentLength > 0)
			{
				GetWindowText(m_valueStr, CHEXEDIT_MAX_LENGTH + 1);
				char *p = m_valueStr;
				while (*p != 0)
				{
					if (*p >= 'A' && *p <= 'Z')
						*p += 32;
					p++;
				}
				SetWindowText(m_valueStr);
			}
			break;
		default:
			return(-1);
			break;
	}

	return(m_case);
}


void CHexEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	if (nChar >= '0' && nChar <= '9' && m_currentLength < m_maxLength)
	if (nChar >= '0' && nChar <= '9')
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		GetWindowText(m_valueStr, CHEXEDIT_MAX_LENGTH + 1);
		m_currentLength = strlen(m_valueStr);
	}
//	else if (m_case != CHEXEDIT_CASE_UPPER && nChar >= 'a' && nChar <= 'f' && m_currentLength < m_maxLength)
	else if (m_case != CHEXEDIT_CASE_UPPER && nChar >= 'a' && nChar <= 'f')
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		GetWindowText(m_valueStr, CHEXEDIT_MAX_LENGTH + 1);
		m_currentLength = strlen(m_valueStr);
//		m_currentLength++;
//		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
//	else if (m_case != CHEXEDIT_CASE_LOWER && nChar >= 'A' && nChar <= 'F' && m_currentLength < m_maxLength)
	else if (m_case != CHEXEDIT_CASE_LOWER && nChar >= 'A' && nChar <= 'F')
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		GetWindowText(m_valueStr, CHEXEDIT_MAX_LENGTH + 1);
		m_currentLength = strlen(m_valueStr);
//		m_currentLength++;
//		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
//	else if (nChar == VK_BACK && m_currentLength > 0)
	else if (nChar == VK_BACK)
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		GetWindowText(m_valueStr, CHEXEDIT_MAX_LENGTH + 1);
		m_currentLength = strlen(m_valueStr);
//		m_currentLength--;
//		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}


BOOL CHexEdit::PreTranslateMessage(MSG* pMsg) 
{
//	if (pMsg->message == 0x118 && !bInit)
	if (m_hWnd != 0 && !bInit)
	{
		bInit = TRUE;
		LimitText(m_maxLength);
	}

	return CEdit::PreTranslateMessage(pMsg);
}
