#if !defined(AFX_OPTIONSDIALOG_H__A7841F83_BA3C_11D2_97F6_0008C72996EB__INCLUDED_)
#define AFX_OPTIONSDIALOG_H__A7841F83_BA3C_11D2_97F6_0008C72996EB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OptionsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog

class COptionsDialog : public CDialog
{
// Construction
public:
	CString m_pieceColor;
	CString m_bgColor;
	BOOL m_changed;
	COptionsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDialog)
	enum { IDD = IDD_OPTIONS_DIALOG };
	CComboBox	m_predefined_list;
	CComboBox	m_pieceColor_List;
	CComboBox	m_bgColor_List;
	CString	m_viewMatrix;
	int		m_drawingMode;
	BOOL	m_pollChanges;
	BOOL	m_rotateAfter;
	int		m_distance;
	BOOL	m_debug;
	BOOL	m_log;
	int		m_videoX;
	int		m_videoY;
	float	m_scale;
	float	m_offsetX;
	float	m_offsetY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposDistanceSmalldelta(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposDistanceBigdelta(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposScaleSmalldelta(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposScaleBigdelta(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChanged();
	afx_msg void OnSelchangeBgcolorList();
	afx_msg void OnSelchangePiececolorList();
	afx_msg void OnSelchangePredefined();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDIALOG_H__A7841F83_BA3C_11D2_97F6_0008C72996EB__INCLUDED_)
