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
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ldlite.h"
#include "ldliteVR.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TYPE0, OnUpdateType0)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_STEP, OnUpdateStep)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
		ID_INDICATOR_STEP,
		ID_INDICATOR_TYPE0
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bAutoMenuEnable = 0;
	strcpy(Type0Message,"LDLITE");
	strcpy(StepMessage," ");
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
#if 1
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
#endif

//	ShowWindow(SW_SHOWMAXIMIZED);
   	SetWindowPos(0,0,0,640,480,SWP_NOMOVE | SWP_NOZORDER);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFrameWnd::PreCreateWindow(cs);
}

void CMainFrame::OnUpdateType0(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString strType0;
	if (Type0Message != NULL)
	{
		strType0.Format("%s",Type0Message);
		pCmdUI->SetText(strType0);
	}
}

void CMainFrame::OnUpdateStep(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString strStep;
	if (StepMessage != NULL)
	{
		strStep.Format("%s",StepMessage);
		pCmdUI->SetText(strStep);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
// This routine can be used to lock the window to a particular size.
void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
#if 0
//	CFrameWnd::OnGetMinMaxInfo(lpMMI);
    lpMMI->ptMaxSize.x = g_max_zimage_width;
    lpMMI->ptMaxSize.y = g_max_zimage_height;
    lpMMI->ptMaxPosition.x = g_max_zimage_width;
    lpMMI->ptMaxPosition.y = g_max_zimage_height;
    lpMMI->ptMinTrackSize.x = g_max_zimage_width;
    lpMMI->ptMinTrackSize.y = g_max_zimage_height;
    lpMMI->ptMaxTrackSize.x = g_max_zimage_width;
    lpMMI->ptMaxTrackSize.y = g_max_zimage_height;
#endif
}
