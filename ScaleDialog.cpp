// ScaleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ldlite.h"
#include "ScaleDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScaleDialog dialog


CScaleDialog::CScaleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CScaleDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScaleDialog)
	m_scale = 0.0f;
	m_viewpoint = _T("");
	//}}AFX_DATA_INIT
}


void CScaleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScaleDialog)
	DDX_Control(pDX, IDC_COMBO1, m_viewpoint_list);
	DDX_Text(pDX, IDC_EDIT1, m_scale);
	DDV_MinMaxFloat(pDX, m_scale, 0.f, 1.e+017f);
	DDX_CBString(pDX, IDC_COMBO1, m_viewpoint);
	DDV_MaxChars(pDX, m_viewpoint, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScaleDialog, CDialog)
	//{{AFX_MSG_MAP(CScaleDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SCALESPIN1, OnDeltaposScalespin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SCALESPIN2, OnDeltaposScalespin2)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScaleDialog message handlers

BOOL CScaleDialog::OnInitDialog() 
{
	static char bigbuffer[4096]; 
	char *p;

	m_changed = FALSE;

	CDialog::OnInitDialog();
	GetPrivateProfileString("Views",NULL,"no views defined",bigbuffer,4095,"ldraw.ini");
	p = bigbuffer;
	while(*p != 0) {
		m_viewpoint_list.AddString(p);
		p += strlen(p)+1;
	}
	m_viewpoint_list.AddString("Current");
	m_viewpoint = "Current";
	m_viewpoint_list.SelectString(-1,m_viewpoint);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScaleDialog::OnDeltaposScalespin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_scale += -(pNMUpDown->iDelta) * (float)0.1;
	if( m_scale < 0.1 ) {
		m_scale = (float)0.1;
	}
	UpdateData(FALSE);

	m_changed = TRUE;
	
	*pResult = 0;
}

void CScaleDialog::OnDeltaposScalespin2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_scale += -(pNMUpDown->iDelta) * 1;
	if( m_scale < 0.1 ) {
		m_scale = (float)0.1;
	}
	UpdateData(FALSE);

	m_changed = TRUE;
	
	*pResult = 0;
}

void CScaleDialog::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
	m_changed = TRUE;

}

void CScaleDialog::OnSelchangeCombo1() 
{
	// TODO: Add your control notification handler code here
	
	m_changed = TRUE;

}
