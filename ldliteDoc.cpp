/*
 *  LDLITE, a program for viewing *.dat files.
 *  Copyright (C) 1998  Paul J. Gyugyi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
// ldliteDoc.cpp : implementation of the CldliteDoc class
//

#include "stdafx.h"
#include "ldlite.h"

#include "ldliteDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CldliteDoc

IMPLEMENT_DYNCREATE(CldliteDoc, CDocument)

BEGIN_MESSAGE_MAP(CldliteDoc, CDocument)
	//{{AFX_MSG_MAP(CldliteDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CldliteDoc construction/destruction

CldliteDoc::CldliteDoc()
{
	// TODO: add one-time construction code here

}

CldliteDoc::~CldliteDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CldliteDoc diagnostics

#ifdef _DEBUG
void CldliteDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CldliteDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CldliteDoc commands


BOOL CldliteDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	CldliteApp *wApp = (CldliteApp *)AfxGetApp();
	
	if (wApp->selected_filename_p == NULL) {
		AfxGetMainWnd()->MessageBox("No file loaded","Error",MB_OK);
		return FALSE;
	} else {
		AfxGetMainWnd()->MessageBox(lpszPathName,wApp->selected_filename_p,MB_OK);
		CopyFile(wApp->selected_filename_p,lpszPathName,FALSE);
		return TRUE;
	}
}
