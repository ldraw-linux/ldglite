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
// ldliteView.h : interface of the CldliteView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ldliteVIEW_H__A4E4140D_245B_11D2_A945_0080ADB46730__INCLUDED_)
#define AFX_ldliteVIEW_H__A4E4140D_245B_11D2_A945_0080ADB46730__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WM_PARSEFILE (WM_USER+100)

class CldliteView : public CView
{
protected: // create from serialization only
	CldliteView();
	DECLARE_DYNCREATE(CldliteView)

// Attributes
public:
	CldliteDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CldliteView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SpinDraw(void);
	float m_spin_rotation;
	int m_spin_color;
	void render_file();
	void render_string(char *);
	int file_being_parsed;
	virtual ~CldliteView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CldliteView)
	afx_msg void OnDestroy();
	afx_msg LRESULT OnParseFile(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMenuShading();
	afx_msg void OnDrawAll();
	afx_msg void OnDrawModel();
	afx_msg void OnDrawParts();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSetPath();
	afx_msg void OnMenuScale();
	afx_msg void OnMenuOptions();
	afx_msg void OnColor();
	afx_msg void OnClockwise();
	afx_msg void OnCounter();
	afx_msg void OnMenuPause();
	afx_msg void OnWire();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ldliteView.cpp
inline CldliteDoc* CldliteView::GetDocument()
   { return (CldliteDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ldliteVIEW_H__A4E4140D_245B_11D2_A945_0080ADB46730__INCLUDED_)
