// OptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ldlite.h"
#include "OptionsDialog.h"
extern "C" {
#include "ldliteVR.h"
extern LDRAW_COMMANDLINE_OPTS ldraw_commandline_opts;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char *options_color_list[] = {
	"00: Black",
	"01: Blue",
	"02: Green",
	"03: Dark Cyan",
	"04: Red",
	"05: Purple",
	"06: Brown",
	"07: Light Gray",
	"08: Dark Gray",
	"09: Light Blue",
	"10: Light Green",
	"11: Light Cyan",
	"12: Light Red",
	"13: Pink",
	"14: Yellow",
	"15: White"
};

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog


COptionsDialog::COptionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDialog)
	m_viewMatrix = _T("");
	m_drawingMode = -1;
	m_pollChanges = FALSE;
	m_rotateAfter = FALSE;
	m_distance = 0;
	m_debug = FALSE;
	m_log = FALSE;
	m_videoX = 0;
	m_videoY = 0;
	m_scale = 0.0f;
	m_offsetX = 0.0f;
	m_offsetY = 0.0f;
	//}}AFX_DATA_INIT
}


void COptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDialog)
	DDX_Control(pDX, IDC_PREDEFINED, m_predefined_list);
	DDX_Control(pDX, IDC_PIECECOLOR_LIST, m_pieceColor_List);
	DDX_Control(pDX, IDC_BGCOLOR_LIST, m_bgColor_List);
	DDX_Text(pDX, IDC_View_Edit, m_viewMatrix);
	DDX_Radio(pDX, IDC_STEP_RADIO, m_drawingMode);
	DDX_Check(pDX, IDC_POLL_CHECK, m_pollChanges);
	DDX_Check(pDX, IDC_ROTATE_CHECK, m_rotateAfter);
	DDX_Text(pDX, IDC_DISTANCE_EDIT, m_distance);
	DDX_Check(pDX, IDC_DEBUG_CHECK, m_debug);
	DDX_Check(pDX, IDC_LOG_CHECK, m_log);
	DDX_Text(pDX, IDC_VIDEOX_EDIT, m_videoX);
	DDX_Text(pDX, IDC_VIDEOY_EDIT, m_videoY);
	DDX_Text(pDX, IDC_SCALE_EDIT, m_scale);
	DDX_Text(pDX, IDC_OFFSETX_EDIT, m_offsetX);
	DDX_Text(pDX, IDC_OFFSETY_EDIT, m_offsetY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
	//{{AFX_MSG_MAP(COptionsDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DISTANCE_SMALLDELTA, OnDeltaposDistanceSmalldelta)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DISTANCE_BIGDELTA, OnDeltaposDistanceBigdelta)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SCALE_SMALLDELTA, OnDeltaposScaleSmalldelta)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SCALE_BIGDELTA, OnDeltaposScaleBigdelta)
	ON_BN_CLICKED(IDC_CONT_RADIO, OnChanged)
	ON_CBN_SELCHANGE(IDC_BGCOLOR_LIST, OnSelchangeBgcolorList)
	ON_CBN_SELCHANGE(IDC_PIECECOLOR_LIST, OnSelchangePiececolorList)
	ON_BN_CLICKED(IDC_DEBUG_CHECK, OnChanged)
	ON_EN_CHANGE(IDC_DISTANCE_EDIT, OnChanged)
	ON_BN_CLICKED(IDC_LOG_CHECK, OnChanged)
	ON_EN_CHANGE(IDC_OFFSETX_EDIT, OnChanged)
	ON_EN_CHANGE(IDC_OFFSETY_EDIT, OnChanged)
	ON_BN_CLICKED(IDC_POLL_CHECK, OnChanged)
	ON_BN_CLICKED(IDC_ROTATE_CHECK, OnChanged)
	ON_BN_CLICKED(IDC_SAVE_RADIO, OnChanged)
	ON_EN_CHANGE(IDC_SCALE_EDIT, OnChanged)
	ON_BN_CLICKED(IDC_STEP_RADIO, OnChanged)
	ON_EN_CHANGE(IDC_VIDEOX_EDIT, OnChanged)
	ON_EN_CHANGE(IDC_VIDEOY_EDIT, OnChanged)
	ON_EN_CHANGE(IDC_View_Edit, OnChanged)
	ON_CBN_SELCHANGE(IDC_PREDEFINED, OnSelchangePredefined)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog message handlers

BOOL COptionsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	static char matrix_string[256];
	sprintf(matrix_string,"%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f",
		(ldraw_commandline_opts.A.a),
		(ldraw_commandline_opts.A.b),
		(ldraw_commandline_opts.A.c),
		(ldraw_commandline_opts.A.d),
		(ldraw_commandline_opts.A.e),
		(ldraw_commandline_opts.A.f),
		(ldraw_commandline_opts.A.g),
		(ldraw_commandline_opts.A.h),
		(ldraw_commandline_opts.A.i));
	m_viewMatrix = matrix_string;
	m_debug = (BOOL) ldraw_commandline_opts.debug_level;
	m_log = (BOOL) ldraw_commandline_opts.log_output;
	m_pollChanges = (BOOL) ldraw_commandline_opts.poll;
	m_scale = ldraw_commandline_opts.S;
	m_rotateAfter = (BOOL) ldraw_commandline_opts.rotate;
	m_videoX = ldraw_commandline_opts.V_x;
	m_videoY = ldraw_commandline_opts.V_y;
	for( int c=0; c<16; c++ ) {
		m_bgColor_List.AddString(options_color_list[c]);
		m_pieceColor_List.AddString(options_color_list[c]);
	}
	m_bgColor_List.SetCurSel( ldraw_commandline_opts.B );
	m_pieceColor_List.SetCurSel( ldraw_commandline_opts.C );
	switch( ldraw_commandline_opts.M ) {
	case 'P':
	default:
		m_drawingMode = 0;
		break;
	case 'C':
		m_drawingMode = 1;
		break;
	case 'S':
		m_drawingMode = 2;
		break;
	}
	m_distance = ldraw_commandline_opts.Z;
	m_offsetX = ldraw_commandline_opts.O.x;
	m_offsetY = ldraw_commandline_opts.O.y;

	m_bgColor_List.GetLBText(m_bgColor_List.GetCurSel(), m_bgColor);
	m_pieceColor_List.GetLBText(m_pieceColor_List.GetCurSel(), m_pieceColor);
	m_changed = FALSE;

	static char bigbuffer[4096]; 
	char *p;

	GetPrivateProfileString("Views",NULL,"no views defined",bigbuffer,4095,"ldraw.ini");
	p = bigbuffer;
	while(*p != 0) {
		m_predefined_list.AddString(p);
		p += strlen(p)+1;
	}
	m_predefined_list.AddString("Current");
	m_predefined_list.SelectString(-1,"Current");
	

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDialog::OnDeltaposDistanceSmalldelta(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_distance += -(pNMUpDown->iDelta) * 1;
	if( m_scale < 0 ) {
		m_scale = 0;
	}

	OnChanged();

	UpdateData(FALSE);
	
	*pResult = 0;
}

void COptionsDialog::OnDeltaposDistanceBigdelta(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_distance += -(pNMUpDown->iDelta) * 5;
	if( m_scale < 0 ) {
		m_scale = 0;
	}

	OnChanged();

	UpdateData(FALSE);
	
	*pResult = 0;
}

void COptionsDialog::OnDeltaposScaleSmalldelta(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_scale += -(pNMUpDown->iDelta) * (float)0.1;
	if( m_scale < 0.1 ) {
		m_scale = (float)0.1;
	}

	OnChanged();

	UpdateData(FALSE);
	
	*pResult = 0;
}

void COptionsDialog::OnDeltaposScaleBigdelta(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_scale += -(pNMUpDown->iDelta) * 1;
	if( m_scale < 0.1 ) {
		m_scale = (float)0.1;
	}

	OnChanged();

	UpdateData(FALSE);
	
	*pResult = 0;
}

void COptionsDialog::OnChanged() 
{
	// TODO: Add your control notification handler code here

	m_changed = TRUE;

}

void COptionsDialog::OnSelchangeBgcolorList() 
{
	// TODO: Add your control notification handler code here

	m_bgColor_List.GetLBText(m_bgColor_List.GetCurSel(), m_bgColor);
	OnChanged();
}

void COptionsDialog::OnSelchangePiececolorList() 
{
	// TODO: Add your control notification handler code here
	
	m_pieceColor_List.GetLBText(m_pieceColor_List.GetCurSel(), m_pieceColor);
	OnChanged();
}

void COptionsDialog::OnSelchangePredefined() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	static char matrix_string[256];
	CString view_name;
	m_predefined_list.GetLBText( m_predefined_list.GetCurSel(), view_name);
	if( strcmp(view_name,"Current") ) {
		GetPrivateProfileString( "Views", view_name, "1,0,0,0,1,0,0,0,1", matrix_string, 256, "ldraw.ini" );
	} else {
		sprintf(matrix_string,"%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f",
			(ldraw_commandline_opts.A.a),
			(ldraw_commandline_opts.A.b),
			(ldraw_commandline_opts.A.c),
			(ldraw_commandline_opts.A.d),
			(ldraw_commandline_opts.A.e),
			(ldraw_commandline_opts.A.f),
			(ldraw_commandline_opts.A.g),
			(ldraw_commandline_opts.A.h),
			(ldraw_commandline_opts.A.i));
	}
	m_viewMatrix = matrix_string;
	
	UpdateData(FALSE);

	OnChanged();
}
