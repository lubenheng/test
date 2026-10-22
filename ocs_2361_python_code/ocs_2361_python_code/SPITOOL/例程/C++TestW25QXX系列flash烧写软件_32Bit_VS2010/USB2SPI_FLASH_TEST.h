// USB2SPI_FLASH_TEST.h : main header file for the USB2SPI_FLASH_TEST application
//

#if !defined(AFX_USB2SPI_FLASH_TEST_H__17A38005_877A_41EF_B21C_59EC97BDD9A6__INCLUDED_)
#define AFX_USB2SPI_FLASH_TEST_H__17A38005_877A_41EF_B21C_59EC97BDD9A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CUSB2SPI_FLASH_TESTApp:
// See USB2SPI_FLASH_TEST.cpp for the implementation of this class
//

class CUSB2SPI_FLASH_TESTApp : public CWinApp
{
public:
	CUSB2SPI_FLASH_TESTApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSB2SPI_FLASH_TESTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CUSB2SPI_FLASH_TESTApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USB2SPI_FLASH_TEST_H__17A38005_877A_41EF_B21C_59EC97BDD9A6__INCLUDED_)
