#if !defined(AFX_SCALEDIALOG_H__40F57770_A153_11D2_AA1D_0080ADB46730__INCLUDED_)
#define AFX_SCALEDIALOG_H__40F57770_A153_11D2_AA1D_0080ADB46730__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScaleDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScaleDialog dialog

class CScaleDialog : public CDialog
{
// Construction
public:
	CScaleDialog(CWnd* pParent = NULL);   // standard constructor

	BOOL m_changed;

// Dialog Data
	//{{AFX_DATA(CScaleDialog)
	enum { IDD = IDD_SCALE_DIALOG };
	CComboBox	m_viewpoint_list;
	float	m_scale;
	CString	m_viewpoint;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScaleDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScaleDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposScalespin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposScalespin2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEdit1();
	afx_msg void OnSelchangeCombo1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCALEDIALOG_H__40F57770_A153_11D2_AA1D_0080ADB46730__INCLUDED_)
