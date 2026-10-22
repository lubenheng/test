// EEPROM24TEST.h : main header file for the EEPROM24TEST application
//

#if !defined(AFX_EEPROM24TEST_H__A71D4B62_1117_4D79_AE3D_CA6AEC3F5479__INCLUDED_)
#define AFX_EEPROM24TEST_H__A71D4B62_1117_4D79_AE3D_CA6AEC3F5479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEEPROM24TESTApp:
// See EEPROM24TEST.cpp for the implementation of this class
//

class CEEPROM24TESTApp : public CWinApp
{
public:
	CEEPROM24TESTApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEEPROM24TESTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEEPROM24TESTApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EEPROM24TEST_H__A71D4B62_1117_4D79_AE3D_CA6AEC3F5479__INCLUDED_)
