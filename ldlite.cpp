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
// ldlite.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ldlite.h"
#include "math.h"

#include "ldliteDoc.h"
#include "ldliteView.h"
extern "C" {
#include "ldliteVR.h"
void platform_getpath(void);
}
#include "MainFrm.h"
#include "ldliteCommandLineInfo.h"

#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CldliteApp

BEGIN_MESSAGE_MAP(CldliteApp, CWinApp)
	//{{AFX_MSG_MAP(CldliteApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CldliteApp construction

CldliteApp::CldliteApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CldliteApp object

CldliteApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CldliteApp initialization

BOOL CldliteApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Gyugyi Cybernetics"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CldliteDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CldliteView));
	AddDocTemplate(pDocTemplate);
	RegisterShellFileTypes();

	strcpy(selected_filename, "");
	selected_filename_p = NULL;

	zShading = GetProfileInt("ldlite","shading",0);
//	zDetailLevel = TYPE_PART; // should checkmark the menu item later!
	zDetailLevel = GetProfileInt("ldlite","detail",TYPE_PART);
	zWire = GetProfileInt("ldlite","wireframe",0);

	// Parse command line for standard shell commands, DDE, file open
	// See CldliteCommandLineInfo::ParseCommandLine for LDRAW-specific parsing.
	sscanf("0,0,0,0,0,0,0,0,0","%f,%f,%f,%f,%f,%f,%f,%f,%f",
		&(ldraw_commandline_opts.A.a),
		&(ldraw_commandline_opts.A.b),
		&(ldraw_commandline_opts.A.c),
		&(ldraw_commandline_opts.A.d),
		&(ldraw_commandline_opts.A.e),
		&(ldraw_commandline_opts.A.f),
		&(ldraw_commandline_opts.A.g),
		&(ldraw_commandline_opts.A.h),
		&(ldraw_commandline_opts.A.i));
	CldliteCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if(
		(ldraw_commandline_opts.A.a == 0) &&
		(ldraw_commandline_opts.A.b == 0) &&
		(ldraw_commandline_opts.A.c == 0) &&
		(ldraw_commandline_opts.A.d == 0) &&
		(ldraw_commandline_opts.A.e == 0) &&
		(ldraw_commandline_opts.A.f == 0) &&
		(ldraw_commandline_opts.A.g == 0) &&
		(ldraw_commandline_opts.A.h == 0) &&
		(ldraw_commandline_opts.A.i == 0)
	) {
		sscanf("1,0,1,0.5,1,-0.5,-1,0,1","%f,%f,%f,%f,%f,%f,%f,%f,%f",
			&(ldraw_commandline_opts.A.a), 
			&(ldraw_commandline_opts.A.b), 
			&(ldraw_commandline_opts.A.c), 
			&(ldraw_commandline_opts.A.d), 
			&(ldraw_commandline_opts.A.e), 
			&(ldraw_commandline_opts.A.f), 
			&(ldraw_commandline_opts.A.g), 
			&(ldraw_commandline_opts.A.h), 
			&(ldraw_commandline_opts.A.i));
	}

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	{
		// set window size
		RECT client_rect;
		int newx, newy;

		newx = ldraw_commandline_opts.V_x;
		newy = ldraw_commandline_opts.V_y;

		if (newx == 0) {
			// maximize window
			m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
		} else {
			m_pMainWnd->SetWindowPos(0,0,0,
				newx,newy,
				SWP_NOMOVE | SWP_NOZORDER);
			m_pMainWnd->GetClientRect(&client_rect);
			newx += (ldraw_commandline_opts.V_x - client_rect.right);
			newy += (ldraw_commandline_opts.V_y - client_rect.bottom);
			m_pMainWnd->SetWindowPos(0,0,0,
				newx,newy,
				SWP_NOMOVE | SWP_NOZORDER);
			// The one and only window has been initialized, so show and update it.
			m_pMainWnd->ShowWindow(SW_SHOW);
		}
	}

	// Maybe need to movewindow() to reflect command line size info
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->DragAcceptFiles();
	// set menu checkmarks
	{
		CMenu *mMenu = NULL;
		
		if (mMenu= m_pMainWnd->GetMenu()) {
			if (zShading == 1) {
				mMenu->CheckMenuItem( ID_MENU_SHADING, MF_BYCOMMAND | MF_CHECKED );
			} else {
				mMenu->CheckMenuItem( ID_MENU_SHADING, MF_BYCOMMAND | MF_UNCHECKED );
			}
			switch (zDetailLevel) {
			case TYPE_P:
				mMenu->CheckMenuItem( ID_DRAW_ALL,  MF_BYCOMMAND | MF_CHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_PARTS,MF_BYCOMMAND | MF_UNCHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_MODEL,MF_BYCOMMAND | MF_UNCHECKED  );
				break;
			case TYPE_PART:
				mMenu->CheckMenuItem( ID_DRAW_ALL,  MF_BYCOMMAND | MF_UNCHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_PARTS,MF_BYCOMMAND | MF_CHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_MODEL,MF_BYCOMMAND | MF_UNCHECKED  );
				break;
			case TYPE_MODEL:
				mMenu->CheckMenuItem( ID_DRAW_ALL,  MF_BYCOMMAND | MF_UNCHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_PARTS,MF_BYCOMMAND | MF_UNCHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_MODEL,MF_BYCOMMAND | MF_CHECKED  );
				break;
			}
			if (zWire == 1) {
				mMenu->CheckMenuItem( IDR_WIRE, MF_BYCOMMAND | MF_CHECKED );
			} else {
				mMenu->CheckMenuItem( IDR_WIRE, MF_BYCOMMAND | MF_UNCHECKED );
			}
			if (ldraw_commandline_opts.M == 'P') {
				mMenu->CheckMenuItem( IDR_PAUSE, MF_BYCOMMAND | MF_CHECKED  );
			} else {
				mMenu->CheckMenuItem( IDR_PAUSE, MF_BYCOMMAND | MF_UNCHECKED  );
			}
		}
	}
	return TRUE;
}

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
		// No message handlers
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

// App command to run the dialog
void CldliteApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CldliteApp commands

CDocument* CldliteApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	CDocument *selected_document = NULL;
	POSITION pos;
	int found_it=0;

	strcpy(selected_filename, lpszFileName);
	selected_filename_p = selected_filename;
	TRY
	{
		CFile f( selected_filename, CFile::modeRead );
		found_it=1;
		strcpy(base_filename, f.GetFileName());
		// remove trailing .dat if it is there.
		int len;
		len = strlen(base_filename);
		if (len > 4) {
			if(base_filename[len-4] == '.') {
				base_filename[len-4] = 0;
			}
		}
	}
	CATCH( CFileException, e )
	{
#ifdef _DEBUG
        afxDump << "File could not be opened " << e->m_cause << "\n";
#endif
	}
	END_CATCH
		
	// get directory of file
	if (found_it == 1) {
		TRY {
			CString newDirectory( lpszFileName );
			_chdir( newDirectory.Left( newDirectory.ReverseFind('\\') ) );
		} CATCH( CFileException, e ) {
#ifdef _DEBUG
			afxDump << "Could not change to directory " << e->m_cause << "\n";
#endif
		} END_CATCH
	}

	platform_getpath();
	
	pos = GetFirstDocTemplatePosition();
	if (found_it == 0) {
		selected_document = GetNextDocTemplate( pos )->OpenDocumentFile(NULL);
		// assume no path is given
		strcpy(base_filename, selected_filename);
		// remove trailing .dat if it is there.
		int len;
		len = strlen(base_filename);
		if (len > 4) {
			if(base_filename[len-4] == '.') {
				base_filename[len-4] = 0;
			}
		}
	} else {
		selected_document = CWinApp::OpenDocumentFile(selected_filename);
	}
	pos = selected_document->GetFirstViewPosition();
	while (pos != NULL) {
		CldliteView* pFirstView = (CldliteView *)selected_document->GetNextView( pos );
		pFirstView->PostMessage(WM_PARSEFILE);
	}
	return selected_document;
}


