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
// ldliteView.cpp : implementation of the CldliteView class
//

#include "stdafx.h"
#include "ldlite.h"
#include "math.h"
#include "mmsystem.h"
#include "shlobj.h"

#include "ldliteDoc.h"
#include "ldliteView.h"
#include "ScaleDialog.h"
#include "OptionsDialog.h"
extern "C" {
#include "ldliteVR.h"
extern LDRAW_COMMANDLINE_OPTS ldraw_commandline_opts;
}
#include "MainFrm.h"
#include "dibkernel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" {
	extern FILE *log_output_file;

	void ldlite_parse(char *ldraw_lines);
	void zDraw(void *);
	int zReset(long *, long *);
	int zInvalidate(void);
	void zcolor_init();
	void zFree(void);
	void platform_zDraw(ZIMAGE *,void*);
	void platform_step(int step, int level, int pause,ZIMAGE *zp);
	void platform_comment(char *, int level);
	int platform_step_comment(char *);
	void rotate_model(void);
	void znamelist_push();
	void znamelist_pop();
	void platform_getpath();
}

static int redraw_needed = 0;

/////////////////////////////////////////////////////////////////////////////
// CldliteView

IMPLEMENT_DYNCREATE(CldliteView, CView)

BEGIN_MESSAGE_MAP(CldliteView, CView)
	//{{AFX_MSG_MAP(CldliteView)
	ON_WM_DESTROY()
	ON_COMMAND(ID_MENU_SHADING, OnMenuShading)
	ON_COMMAND(ID_DRAW_ALL, OnDrawAll)
	ON_COMMAND(ID_DRAW_MODEL, OnDrawModel)
	ON_COMMAND(ID_DRAW_PARTS, OnDrawParts)
	ON_WM_TIMER()
	ON_COMMAND(ID_SET_PATH, OnSetPath)
	ON_COMMAND(ID_MENU_SCALE, OnMenuScale)
	ON_COMMAND(ID_MENU_OPTIONS, OnMenuOptions)
	ON_COMMAND(IDR_COLOR, OnColor)
	ON_COMMAND(IDR_CLOCKWISE, OnClockwise)
	ON_COMMAND(IDR_COUNTER, OnCounter)
	ON_COMMAND(IDR_PAUSE, OnMenuPause)
	ON_COMMAND(IDR_WIRE, OnWire)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	// User commands
	ON_MESSAGE(WM_PARSEFILE, OnParseFile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CldliteView construction/destruction

CldliteView::CldliteView()
{
	// TODO: add construction code here
	file_being_parsed = 0;
	platform_getpath();
	m_spin_color=0;
	m_spin_rotation=0.0;
}

CldliteView::~CldliteView()
{
}

BOOL CldliteView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}



/////////////////////////////////////////////////////////////////////////////
// CldliteView drawing


void platform_zDraw(ZIMAGE *zp, void *zDC)
{
        static HWND hwndDesktop;
        static HDC hdcDesktop;
        static HDC hdcMem;
        static HBITMAP hbm = NULL;
        static HBITMAP hbmOld=NULL;
		CDC *pDC;
		int i, j;
		pDC = (CDC *)zDC;

        // Safety check - Not all devices suport bitblt, such as printers.
        if (0 || ((pDC->IsPrinting()) || (((pDC->GetDeviceCaps (RASTERCAPS)) & RC_BITBLT) == 0))) {
				int pixel;
                for(i=0; i<zp->rows; i++) {
					for(j=0; j<zp->cols; j++) {
					        pixel = 3*(i + zp->rows*j);
							pDC->SetPixel((i),(j),
								RGB(zp->r[pixel],zp->g[pixel],zp->b[pixel]));
					}
				}
		} else {
			// if needed, update bitmap and redraw
			if (redraw_needed > 0)  {
				CWaitCursor foo;

				if (hbm && (redraw_needed==2)) {
					SelectObject(hdcMem, hbmOld);
					DeleteObject(hbm);
					DeleteDC(hdcMem);
					ReleaseDC(hwndDesktop, hdcDesktop);
					hbm = NULL;
				}
				if (!hbm) {
					hwndDesktop = GetDesktopWindow();
					hdcDesktop = GetDC(hwndDesktop);
					hdcMem = CreateCompatibleDC(hdcDesktop);
					hbm = CreateCompatibleBitmap(hdcDesktop, zp->rows, zp->cols);			
					hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
					zp->dirty_x1 = 0;
					zp->dirty_x2 = zp->rows-1;
					zp->dirty_y1 = 0;
					zp->dirty_y2 = zp->cols-1;
				}
#if 0
				for(i=zp->dirty_x1; i<zp->dirty_x2; i++) {
					for(j=zp->dirty_y1; j<zp->dirty_y2; j++) {
						SetPixel(hdcMem, i, j, RGB(zp->r[i*zp->cols+j],zp->g[i*zp->cols+j],zp->b[i*zp->cols+j]));
					}
				}
#else
				// check and see if one more line/col is being drawn than needed,
				// since the way dirty_x is defined has changed since this code
				// was written.
				i = SetDIBitsToDevice(hdcMem, zp->dirty_x1, zp->dirty_y1,
					(zp->dirty_x2-zp->dirty_x1+1),
					(zp->dirty_y2-zp->dirty_y1+1),
					zp->dirty_x1, (zp->cols-1)-zp->dirty_y2,
					0,zp->cols,
					zp->dib+40,(LPBITMAPINFO)zp->dib,DIB_RGB_COLORS);
j=GetLastError();
#endif
				zp->dirty_x1 = zp->rows-1;
				zp->dirty_x2 = 0;
				zp->dirty_y1 = zp->cols-1;
				zp->dirty_y2 = 0;
			}
			// draw the image on the screen
			if (hbm) {
				BitBlt(pDC->m_hDC,0,0,zp->rows,zp->cols,hdcMem,0,0,SRCCOPY);
			}
			redraw_needed = 0;	
		}
}

void CldliteView::OnDraw(CDC* pDC)
{
	CldliteDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	zDraw((void *)pDC);

}

void platform_step(int step, int level, int pause, ZIMAGE *zp)
{
	char buf[128];
	static int last_step=-1;
	CMainFrame *mWnd = (CMainFrame *)AfxGetMainWnd();
	CldliteApp *wApp;
	static int dirty_step = 0;

	if (mWnd == NULL) {
		// exiting before file is fully processed
		return;
	}
		
	wApp = (CldliteApp *)AfxGetApp();

	if (step == INT_MAX) {
		// end of top-level dat file reached
#ifdef SETWINDOWTEXT
		sprintf(buf,"%s: Step %d",wApp->base_filename, last_step+2);
		mWnd->SetWindowText(buf);
#else
		sprintf(buf,"Step %d",last_step+2);
		if(platform_step_comment(buf)) return;
#endif
		// if stuff was drawn since last step
		if(dirty_step || ((zp->dirty_x2 >= zp->dirty_x1) && (zp->dirty_y2 >= zp->dirty_y1))) {
			pause = 1;
		}
	} else 	if (step >= 0) {
#ifdef SETWINDOWTEXT
		sprintf(buf,"%s: Step %d",wApp->base_filename,step+1);
		mWnd->SetWindowText(buf);
#else
		sprintf(buf,"Step %d", step+1);
		if(platform_step_comment(buf)) return;
#endif
		last_step = step;
		dirty_step = 0;
	} else {
		// step == -1 means a redraw partway through a step
		dirty_step = 1;
		if (redraw_needed == 2) {
			last_step = -1;
		}
#ifdef SETWINDOWTEXT
		sprintf(buf,"%s: Step %d",wApp->base_filename,last_step+2);
		mWnd->SetWindowText(buf);
#else
		sprintf(buf,"Step %d",last_step+2);
		if (platform_step_comment(buf)) return;
#endif
	}
	// draw and wait for mouse click
	if (redraw_needed == 0) {
		redraw_needed = 1;
	}
	if ((ldraw_commandline_opts.M != 'C')||(step==INT_MAX)||(step== -1)) { 
		CRgn crgn;

		if((zp->dirty_x2 >= zp->dirty_x1) && (zp->dirty_y2 >= zp->dirty_y1)) {
			crgn.CreateRectRgn(  zp->dirty_x1,  zp->dirty_y1, 
				min((zp->dirty_x2+1),(zp->rows-1)),  min((zp->dirty_y2+1),(zp->cols-1)) );
			mWnd->GetActiveView()->InvalidateRgn(&crgn, 0);
			mWnd->GetActiveView()->UpdateWindow();
		}
	}
	if ((step >= 0 ) && ((ldraw_commandline_opts.M == 'S')||(ldraw_commandline_opts.M == 'F'))) {
		int x_size;
		int y_size;
		int x_orig;
		int y_orig;

		if (ldraw_commandline_opts.clip == 1) {
			// clip edges of image to include only pixels that have been set
			x_size = zp->extent_x2 - zp->extent_x1 + 1;
			y_size = zp->extent_y2 - zp->extent_y1 + 1;
			x_orig = zp->extent_x1;
			y_orig = zp->extent_y1;
			if ((x_size<=0)||(y_size<=0)) {
				// this seems to be what LDRAW does if nothing has been drawn yet
				x_size = 3;
				y_size = 3;
				x_orig = 0;
				y_orig = 0;
			} 
		} else {
			x_size = zp->rows;
			y_size = zp->cols;
			x_orig = 0;
			y_orig = 0;
		}
		
		if ((step == INT_MAX) && (pause == 0)) {
			// do nothing
		} else if ((level<=ldraw_commandline_opts.maxlevel) && ((ldraw_commandline_opts.M == 'S')||(step == INT_MAX))){
			if (ldraw_commandline_opts.image_filetype = 4) {
				// save bitmap
				CSize csize(x_size, y_size);
				CDib cdib(csize, 24, TRUE);
				int i,j;
				static char filename[256];
				static char basename[256];
				CPoint cpoint;
				DWORD color;
				int pixel;
				
				if (!cdib.DibLoaded()) {
					MessageBox(NULL,"dib error","error",MB_OK);
				}
				// This could be optimized quite a bit,since the source and
				// destination are both DIB structures.
				for(i=0; i<x_size; i++) {
					for (j=0; j < y_size; j++) {
						cpoint.x = i;
						cpoint.y = j;
						pixel = 3*((i+x_orig) + zp->rows*(j+y_orig));
						
						color = (0xff & zp->b[pixel]) |
							((0xff & zp->g[pixel]) <<8) |
							((0xff & zp->r[pixel]) << 16);
						cdib.SetPixel(cpoint, color);
					}
				}
				if (wApp->base_filename != NULL) {
					unsigned int i;
					strcpy(basename,wApp->base_filename);
					for(i=0; i<strlen(basename); i++) {
						basename[i] = toupper(basename[i]);
					}
					for(i=strlen(basename); i<6; i++) {
						basename[i] = '-';
					}
					if (step == INT_MAX) {
						if ((last_step+2) < 100) {
							basename[6]=0;
							sprintf(filename, "%s/bitmap/%s%02d.BMP",pathname,basename,last_step+2);
						} else if ((last_step+2) < 1000) {
							basename[5] = 0;
							sprintf(filename, "%s/bitmap/%s%03d.BMP",pathname,basename,last_step+2);
						} else if ((last_step+2) < 10000) {
							basename[4] = 0;
							sprintf(filename, "%s/bitmap/%s%04d.BMP",pathname,basename,last_step+2);
						} else if ((last_step+2) < 100000) {
							basename[3] = 0;
							sprintf(filename, "%s/bitmap/%s%05d.BMP",pathname,basename,last_step+2);
						} else if ((last_step+2) < 1000000) {
							basename[2] = 0;
							sprintf(filename, "%s/bitmap/%s%06d.BMP",pathname,basename,last_step+2);
						} else {
							sprintf(filename, "%s/bitmap/%s%d.BMP",pathname,basename,last_step+2);
						}
					} else {
						if ((step+1) < 100) {
							basename[6]=0;
							sprintf(filename, "%s/bitmap/%s%02d.BMP",pathname,basename,step+1);
						} else if ((step+1) < 1000) {
							basename[5] = 0;
							sprintf(filename, "%s/bitmap/%s%03d.BMP",pathname,basename,step+1);
						} else if ((step+1) < 10000) {
							basename[4] = 0;
							sprintf(filename, "%s/bitmap/%s%04d.BMP",pathname,basename,step+1);
						} else if ((step+1) < 100000) {
							basename[3] = 0;
							sprintf(filename, "%s/bitmap/%s%05d.BMP",pathname,basename,step+1);
						} else if ((step+1) < 1000000) {
							basename[2] = 0;
							sprintf(filename, "%s/bitmap/%s%06d.BMP",pathname,basename,step+1);
						} else {
							sprintf(filename, "%s/bitmap/%s%d.BMP",pathname,basename,step+1);
						}
						
					}
					cdib.Write(filename, TRUE);
					cdib.Empty();
				}
			} else {
				// other image formats are not yet supported
			}
		}			
	}
	if (pause && (level<=ldraw_commandline_opts.maxlevel) && (ldraw_commandline_opts.M == 'P')&&(step!=INT_MAX))  {
#ifdef SETWINDOWTEXT
		if (step >= 0) {
			sprintf(buf,"%s: Step %d -- Click on drawing to continue.",wApp->base_filename,step+1);
		} else {
			sprintf(buf,"%s: Paused -- Click on drawing to continue.",wApp->base_filename);
		}
		mWnd->SetWindowText(buf);
#else
		if (step >= 0) {
			sprintf(buf,"Click on drawing to continue.");
		} else {
			sprintf(buf,"Paused -- Click on drawing to continue.");
		}
		platform_comment(buf,0);
#endif
		{
			BOOL bWaitingForMouseClick;
			bWaitingForMouseClick = 1;
			while ( bWaitingForMouseClick ) 
			{ 
				MSG msg;
				if ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
				{ 
					if ((msg.message == WM_LBUTTONDOWN)||(msg.message == WM_KEYDOWN)) {
						bWaitingForMouseClick = 0;
#ifdef SETWINDOWTEXT
						mWnd->SetWindowText(wApp->base_filename);
#else
								platform_comment(" ",0);
#endif
					} 
					if ( !AfxGetApp()->PumpMessage( ) ) 
					{ 
						bWaitingForMouseClick = 0; 
//						::PostQuitMessage( 0 );
						// It would be nicer to call PostQuitMessage(), but 
						// that causes segfaults.
						exit(0);
						break; 
					} 
					
				} 
				// let MFC do its idle processing
				LONG lIdle = 0;
				while ( AfxGetApp()->OnIdle(lIdle++ ) );  
			}
		}
	}
}

void platform_comment(char *comment_string, int level)
{
	if (log_output_file != 0) {
		if ((level==0) || (ldraw_commandline_opts.debug_level>0)) {
			fprintf(log_output_file,"%s\n",comment_string);
		}
	}
#if 1
	if (level == 0) {
		if (ldraw_commandline_opts.debug_level>0){
			TRACE(comment_string);TRACE("\n");
		}
		CMainFrame *fWnd = (CMainFrame *)AfxGetMainWnd();
		if (fWnd) {
			strcpy(&(fWnd ->Type0Message[0]),comment_string);
			// let MFC do its idle processing to update status bar
			LONG lIdle = 0;
			while ( AfxGetApp()->OnIdle(lIdle++ ) );
		}
	}
#endif
}

int platform_step_comment(char *comment_string)
{
	MSG msg;
	CMainFrame *fWnd = (CMainFrame *)AfxGetMainWnd();

	// Check for Quit messages.
	if (fWnd) {
		strcpy(&(fWnd ->StepMessage[0]),comment_string);
		while ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
		{ 
			if ( !AfxGetApp()->PumpMessage( ) ) 
			{ 
//				::PostQuitMessage( 0 ); 
						// It would be nicer to call PostQuitMessage(), but 
						// that causes segfaults.
						exit(0);
				return 1; 
			} else {
				// let MFC do its idle processing
				LONG lIdle = 0;
				while ( AfxGetApp()->OnIdle(lIdle++ ) );  
			}			
		} 
	}
	return 0;
}

void platform_setpath()
{
	BROWSEINFO bi;
	LPITEMIDLIST lpiil;
	char display_name[MAX_PATH];
	
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = display_name;
	bi.lpszTitle = "Please select the base directory for LDRAW files and press OK.";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	lpiil = SHBrowseForFolder(&bi);
	if (lpiil != NULL) {
	  SHGetPathFromIDList(lpiil,pathname);
	  WritePrivateProfileString("LDraw","BaseDirectory",pathname,
		"ldraw.ini"); 
	}
}

void platform_getpath()
{
	static char ldrawdir[256];
	
	GetPrivateProfileString("LDraw","BaseDirectory","",
		ldrawdir,256,"ldraw.ini"); 
	if (ldrawdir[0] != 0) {
		int len = strlen(ldrawdir);
		if ((ldrawdir[len] == '\\') ||
			(ldrawdir[len] == '/')) {
			ldrawdir[len] = 0;
		}
		strcpy(pathname,ldrawdir); // pathname is a global
	} else {  
		MessageBox(NULL,"Path to LDRAW files not found. \nPlease click OK and in the next dialog box\nchoose the base directory for LDRAW files.","Warning",MB_OK);
		platform_setpath();
	}
}
/////////////////////////////////////////////////////////////////////////////
// CldliteView printing

BOOL CldliteView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CldliteView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CldliteView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CldliteView diagnostics

#ifdef _DEBUG
void CldliteView::AssertValid() const
{
	CView::AssertValid();
}

void CldliteView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CldliteDoc* CldliteView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CldliteDoc)));
	return (CldliteDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CldliteView message handlers

void CldliteView::OnDestroy() 
{
	CView::OnDestroy();
	
	zFree();	
}

void CldliteView::render_file()
{
	static char filename[128];
	int   start, finish;
	double   elapsed_time;
	RECT client_rect;
	static char buf[256];

	elapsed_time = 0.0;
	{
		int rc;
		CldliteApp *wApp;
		CWnd *mWnd;
		CMenu *mMenu;

		wApp = (CldliteApp *)AfxGetApp();
		if (wApp) {
			mWnd = AfxGetMainWnd();
			if (mWnd) {
				mMenu= mWnd->GetMenu();
			}
		} else {
			return;
		}

		// exit if no file has yet been loaded.
		if (wApp->selected_filename_p == NULL) {
			return;
		}

		
		mMenu->EnableMenuItem( ID_FILE_OPEN, MF_BYCOMMAND | MF_GRAYED );
		wApp->DoWaitCursor(1);
		strcpy(filename, wApp->selected_filename_p);
		start = ::timeGetTime();
		zcolor_init();
		if (ldraw_commandline_opts.V_x==0) {
			// determine window size in case it has been resized
			GetClientRect(&client_rect);
			// round up to multiple of four.
			client_rect.right = 4*((client_rect.right+3) / 4);
			client_rect.bottom = 4*((client_rect.bottom+3) / 4);
			rc = zReset(&(client_rect.right),&(client_rect.bottom));
		} else {
			rc = zReset((long *)&(ldraw_commandline_opts.V_x),
				(long *)&(ldraw_commandline_opts.V_y));
		}
		if (rc != 0) {
			MessageBox("Out of Memory, exiting","Fatal Error",MB_OK);
			exit(-1);
		}
		redraw_needed = 2;
	}
	znamelist_push();
	{
		static char buf[16*1024];
		FILE *fp;
		int bytes;
		
		bytes = 0;
		fp = fopen("ldliterc.dat","rb");
		if (fp != NULL) {
			bytes += fread(buf,1,15*1024,fp);
			fclose(fp);
		}
		sprintf(&(buf[bytes]),"\n1 16 0 0 0 1 0 0 0 1 0 0 0 1 \"%s\"\n",
			filename);
		ldlite_parse(buf);
	}

	znamelist_pop();
	finish = ::timeGetTime();
	elapsed_time = (finish-start)*0.001;
	{
		CldliteApp *wApp;
		CWnd *mWnd;
		CMenu *mMenu;

		wApp = (CldliteApp *)AfxGetApp();
		if (wApp) {
			mWnd = AfxGetMainWnd();
			if (mWnd) {
				mMenu= mWnd->GetMenu();
			}
		}

		wApp->DoWaitCursor(-1);

		if (mWnd != NULL) {
			if (ldraw_commandline_opts.output != 1) {
				zStep(INT_MAX, 0);
			}
			wApp = (CldliteApp *)AfxGetApp();
			if (wApp) {
				mWnd = AfxGetMainWnd();
				if (mWnd) {
#ifdef SETWINDOWTEXT
					sprintf(buf,"%s: Finished - %f seconds",wApp->base_filename,elapsed_time);
					mWnd->SetWindowText(buf);
#else
					sprintf(buf,"Finished - %f seconds",elapsed_time);
					platform_comment(buf,0);
#endif
					mMenu= mWnd->GetMenu();
					if (mMenu) {
						mMenu->EnableMenuItem( ID_FILE_OPEN, MF_BYCOMMAND | MF_ENABLED );
					}
					// start timer if we will be rotating or polling
					if ((ldraw_commandline_opts.poll == 1) ||
					(ldraw_commandline_opts.rotate == 1)){
						SetTimer(1, 500, NULL);
					}
					InvalidateRect(NULL,FALSE);
				} 
			}
		}
	}
	return;	
	
}

void CldliteView::render_string(char *ldraw_lines)
{
	RECT client_rect;
	static char buf[256];
	int rc;
	
	// determine window size
	GetClientRect(&client_rect);
	// round up to multiple of four.
	client_rect.right = 4*((client_rect.right+3) / 4);
	client_rect.bottom = 4*((client_rect.bottom+3) / 4);
	zcolor_init();
	rc = zReset(&(client_rect.right),&(client_rect.bottom));
	if (rc != 0) {
		MessageBox("Out of Memory, exiting","Fatal Error",MB_OK);
		exit(-1);
	}
	redraw_needed = 2;
	znamelist_push();
	ldlite_parse(ldraw_lines);
	znamelist_pop();
	zStep(INT_MAX, 0);
	return;	
	
}

LRESULT CldliteView::OnParseFile(WPARAM wParam, LPARAM lParam)
{
	if (file_being_parsed == 0) {
		file_being_parsed = 1;
		mpd_subfile_name = NULL; // potential memory leak
		render_file();
		if (mpd_subfile_name != NULL) {
			// set file name to first subfile
			CldliteApp *wApp;
			
			wApp = (CldliteApp *)AfxGetApp();
			if (wApp) {
				wApp->selected_filename_p = mpd_subfile_name;
				render_file();
			}
		}
		if ((ldraw_commandline_opts.output == 1) ||
			(ldraw_commandline_opts.M == 'S')||(ldraw_commandline_opts.M == 'F')) {
			// quit the program
			PostQuitMessage(0);
		}
		file_being_parsed=0;
	} else {
		Beep(0x05FF,128);
	}
	return 0;
}

void CldliteView::OnMenuShading() 
{
	CWinApp *wApp = NULL;
	CWnd *mWnd = NULL;
	CMenu *mMenu = NULL;
		
	if (wApp = AfxGetApp()) {
		if (mWnd = AfxGetMainWnd()) {
			if (mMenu= mWnd->GetMenu()) {
				if (zShading != 0) {
					mMenu->CheckMenuItem( ID_MENU_SHADING, MF_BYCOMMAND | MF_UNCHECKED  );
					zShading = 0;
				} else {
					mMenu->CheckMenuItem( ID_MENU_SHADING, MF_BYCOMMAND | MF_CHECKED  );
					zShading = 1;
				}
				wApp->WriteProfileInt("ldlite","shading",zShading);
			}
		}
	}
}

void CldliteView::OnDrawAll() 
{
	CWinApp *wApp = NULL;
	CWnd *mWnd = NULL;
	CMenu *mMenu = NULL;
		
	if (wApp = AfxGetApp()) {
		if (mWnd = AfxGetMainWnd()) {
			if (mMenu= mWnd->GetMenu()) {
				zDetailLevel = TYPE_P;
				mMenu->CheckMenuItem( ID_DRAW_ALL,  MF_BYCOMMAND | MF_CHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_PARTS, MF_BYCOMMAND | MF_UNCHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_MODEL,MF_BYCOMMAND | MF_UNCHECKED  );
				wApp->WriteProfileInt("ldlite","detail",zDetailLevel);
			}
		}
	}
	
}

void CldliteView::OnDrawModel() 
{
	CWinApp *wApp = NULL;
	CWnd *mWnd = NULL;
	CMenu *mMenu = NULL;
		
	if (wApp = AfxGetApp()) {
		if (mWnd = AfxGetMainWnd()) {
			if (mMenu= mWnd->GetMenu()) {
				zDetailLevel = TYPE_MODEL;
				mMenu->CheckMenuItem( ID_DRAW_ALL,  MF_BYCOMMAND | MF_UNCHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_PARTS,MF_BYCOMMAND | MF_UNCHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_MODEL,MF_BYCOMMAND | MF_CHECKED  );
				wApp->WriteProfileInt("ldlite","detail",zDetailLevel);
			}
		}
	}
	
}

void CldliteView::OnDrawParts() 
{
	CWinApp *wApp = NULL;
	CWnd *mWnd = NULL;
	CMenu *mMenu = NULL;
		
	if (wApp = AfxGetApp()) {
		if (mWnd = AfxGetMainWnd()) {
			if (mMenu= mWnd->GetMenu()) {
				zDetailLevel = TYPE_PART;
				mMenu->CheckMenuItem( ID_DRAW_ALL,  MF_BYCOMMAND | MF_UNCHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_PARTS,MF_BYCOMMAND | MF_CHECKED  );
				mMenu->CheckMenuItem( ID_DRAW_MODEL,MF_BYCOMMAND | MF_UNCHECKED  );
				wApp->WriteProfileInt("ldlite","detail",zDetailLevel);
			}
		}
	}
	
}

void CldliteView::OnTimer(UINT nIDEvent) 
{
	CldliteApp *wApp = (CldliteApp *)AfxGetApp();
		
	// Only gets called if -p (for polling) 
	// or -t (for turning/rotate) was specified on command line.
	if (wApp->selected_filename_p == NULL) {
		return;
	}
	if (file_being_parsed != 0) {
		return;
	}
	if (ldraw_commandline_opts.rotate == 1) {
		// rotate model
		rotate_model();
		PostMessage(WM_PARSEFILE,0,0);
	} else {
		// polling
		// Check to see if timestamp on filename has changed.
		CFileFind finder;
		FILETIME file_time;
		static int init=0;
		static FILETIME last_file_time;
		
		BOOL bWorking = finder.FindFile(wApp->selected_filename_p);
		if (bWorking)
		{
			bWorking = finder.FindNextFile();
			finder.GetLastWriteTime( &file_time );
			if (!init) {
				last_file_time = file_time;
				init = 1;
			} else {
				// compare file times
				if ((file_time.dwLowDateTime != last_file_time.dwLowDateTime) ||
					(file_time.dwHighDateTime != last_file_time.dwHighDateTime)) {
					// If so, send WM_PARSEFILE message.
					last_file_time = file_time;
					PostMessage(WM_PARSEFILE,0,0);
				}
			}
		}
	}
}

void CldliteView::OnSetPath() 
{
	platform_setpath();	
}

void CldliteView::OnMenuScale() 
{
	CScaleDialog csd;
	static char last_viewpoint[256];
	static int init=0;
	static char matrix_string[256];

	if(!init) {
		init = 1;
		last_viewpoint[0] = 0;
	}

	csd.m_scale = ldraw_commandline_opts.S;
	csd.m_viewpoint = last_viewpoint;
	if (csd.DoModal() == IDOK && csd.m_changed) {
		ldraw_commandline_opts.S = csd.m_scale;
		strcpy(last_viewpoint, csd.m_viewpoint);
		if( strcmp("Current", csd.m_viewpoint) ) {
			GetPrivateProfileString("Views",csd.m_viewpoint,
				"1,0,0,0,1,0,0,0,1",matrix_string,256,"ldraw.ini");
		}
		sscanf(matrix_string,"%f,%f,%f,%f,%f,%f,%f,%f,%f",
			&(ldraw_commandline_opts.A.a),
			&(ldraw_commandline_opts.A.b),
			&(ldraw_commandline_opts.A.c),
			&(ldraw_commandline_opts.A.d),
			&(ldraw_commandline_opts.A.e),
			&(ldraw_commandline_opts.A.f),
			&(ldraw_commandline_opts.A.g),
			&(ldraw_commandline_opts.A.h),
			&(ldraw_commandline_opts.A.i));
		zInvalidate();
		InvalidateRect(NULL,FALSE);
		PostMessage(WM_PARSEFILE,0,0);
	}
}

void CldliteView::OnMenuOptions() 
{
	// TODO: Add your command handler code here
	COptionsDialog cod;
	if( cod.DoModal() == IDOK && cod.m_changed==TRUE ) {
		sscanf(cod.m_viewMatrix,"%f,%f,%f,%f,%f,%f,%f,%f,%f",
			&(ldraw_commandline_opts.A.a),
			&(ldraw_commandline_opts.A.b),
			&(ldraw_commandline_opts.A.c),
			&(ldraw_commandline_opts.A.d),
			&(ldraw_commandline_opts.A.e),
			&(ldraw_commandline_opts.A.f),
			&(ldraw_commandline_opts.A.g),
			&(ldraw_commandline_opts.A.h),
			&(ldraw_commandline_opts.A.i));
		ldraw_commandline_opts.debug_level = (int) cod.m_debug;
		ldraw_commandline_opts.log_output = (int) cod.m_log;
		ldraw_commandline_opts.O.x = cod.m_offsetX;
		ldraw_commandline_opts.O.y = cod.m_offsetY;
		ldraw_commandline_opts.poll = (int) cod.m_pollChanges;
		ldraw_commandline_opts.S = cod.m_scale;
		ldraw_commandline_opts.rotate = (int) cod.m_rotateAfter;
		ldraw_commandline_opts.V_x = cod.m_videoX;
		ldraw_commandline_opts.V_y = cod.m_videoY;
		ldraw_commandline_opts.Z = cod.m_distance;
		ldraw_commandline_opts.B = atoi( cod.m_bgColor.Left(cod.m_bgColor.Find(':')) );
		ldraw_commandline_opts.C = atoi( cod.m_pieceColor.Left(cod.m_pieceColor.Find(':')) );
		switch(cod.m_drawingMode) {
		case 0:
			ldraw_commandline_opts.M = 'P';
			break;
		case 1:
			ldraw_commandline_opts.M = 'C';
			break;
		case 2:
			ldraw_commandline_opts.M = 'S';
			break;
		}
		zInvalidate();
		InvalidateRect(NULL,FALSE);
		PostMessage( WM_PARSEFILE, 0, 0 );
	}
	
}

void CldliteView::OnColor() 
{
	m_spin_color++;
	SpinDraw();
}

void CldliteView::OnClockwise() 
{
	int i;
	int   start, finish;
	double   elapsed_time;
	static char buf[256];

	elapsed_time = 0.0;
	start = ::timeGetTime();
	for(i=0; i<=180; i+=10) {
		m_spin_rotation=(float)(1.0*i);
		SpinDraw();
	}
	finish = ::timeGetTime();
	elapsed_time = (finish-start)*0.001;
	{
		CldliteApp *wApp;
		CWnd *mWnd;

		wApp = (CldliteApp *)AfxGetApp();
		if (wApp) {
			mWnd = AfxGetMainWnd();
			if (mWnd) {
#ifdef SETWINDOWTEXT
				sprintf(buf,"Finished - %f seconds",elapsed_time);
				mWnd->SetWindowText(buf);
#endif
				InvalidateRect(NULL,FALSE);
			} 
		}
	}
}

void CldliteView::OnCounter() 
{
	m_spin_rotation-=10.0;
	SpinDraw();
}

void CldliteView::SpinDraw()
{
	static char buf[4096];
	static char filename[MAX_PATH];
	char save_mode;
	CldliteApp *wApp;

	wApp = (CldliteApp *)AfxGetApp();
	if (wApp->selected_filename_p == NULL) {
		strcpy(filename,"3001.dat");
	} else {
		strcpy(filename,wApp->selected_filename_p);
	}

	save_mode=ldraw_commandline_opts.M;
	ldraw_commandline_opts.M='C';
//	sprintf(buf,"0 ROTATE %f 0 1 0 \n1 %d 0 0 0 1 0 0 0 1 0 0 0 1 979.dat\n0 ROTATE END\n0 STEP\n",m_spin_rotation,m_spin_color);
	sprintf(buf,"0 ROTATE %f 0 1 0 \n1 %d 0 0 0 1 0 0 0 1 0 0 0 1 \"%s\"\n0 ROTATE END\n0 STEP\n",
		m_spin_rotation,m_spin_color, filename);
	render_string(buf);
	ldraw_commandline_opts.M=save_mode;
}

void CldliteView::OnMenuPause() 
{
	CWinApp *wApp = NULL;
	CWnd *mWnd = NULL;
	CMenu *mMenu = NULL;
		
	if (wApp = AfxGetApp()) {
		if (mWnd = AfxGetMainWnd()) {
			if (mMenu= mWnd->GetMenu()) {
				if (ldraw_commandline_opts.M != 'P') {
					mMenu->CheckMenuItem( IDR_PAUSE, MF_BYCOMMAND | MF_CHECKED  );
					ldraw_commandline_opts.M = 'P';
				} else {
					mMenu->CheckMenuItem( IDR_PAUSE, MF_BYCOMMAND | MF_UNCHECKED  );
					ldraw_commandline_opts.M = 'C';
				}
//				wApp->WriteProfileInt("ldlite","pause",ldraw_commandline_opts.M);
			}
		}
	}
	
}

void CldliteView::OnWire() 
{
	CWinApp *wApp = NULL;
	CWnd *mWnd = NULL;
	CMenu *mMenu = NULL;
		
	if (wApp = AfxGetApp()) {
		if (mWnd = AfxGetMainWnd()) {
			if (mMenu= mWnd->GetMenu()) {
				if (zWire != 0) {
					mMenu->CheckMenuItem( IDR_WIRE, MF_BYCOMMAND | MF_UNCHECKED  );
					zWire = 0;
				} else {
					mMenu->CheckMenuItem( IDR_WIRE, MF_BYCOMMAND | MF_CHECKED  );
					zWire = 1;
				}
				wApp->WriteProfileInt("ldlite","wireframe",zWire);
			}
		}
	}
	
}

