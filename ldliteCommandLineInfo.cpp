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
*/// ldliteCommandLineInfo.cpp: implementation of the CldliteCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

//#include "afxwin.h"
#include "ldlite.h"
#include "ldliteCommandLineInfo.h"
extern "C" {
#include "ldliteVR.h"
}
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CldliteCommandLineInfo::CldliteCommandLineInfo()
{
	m_nShellCommand = FileNew;
	ldraw_commandline_opts.A.a=1.0;
	ldraw_commandline_opts.A.b=0.0;
	ldraw_commandline_opts.A.c=1.0;
	ldraw_commandline_opts.A.d=0.5;
	ldraw_commandline_opts.A.e=1.0;
	ldraw_commandline_opts.A.f=-0.5;
	ldraw_commandline_opts.A.g=-1.0;
	ldraw_commandline_opts.A.h=0.0;
	ldraw_commandline_opts.A.i=1.0;
	ldraw_commandline_opts.B=15;
	ldraw_commandline_opts.C=7;
	ldraw_commandline_opts.F=0;
	ldraw_commandline_opts.M= 'P';
	ldraw_commandline_opts.O.x=0.0;
	ldraw_commandline_opts.O.y=0.0;
	ldraw_commandline_opts.O.z=0.0;
	ldraw_commandline_opts.S=1.0;
	ldraw_commandline_opts.V_x=0;
	ldraw_commandline_opts.V_y=0;
	ldraw_commandline_opts.poll=0;
	ldraw_commandline_opts.output=0;
	ldraw_commandline_opts.rotate=0;
	ldraw_commandline_opts.debug_level=0;
	ldraw_commandline_opts.log_output=0;
	ldraw_commandline_opts.Z=INT_MIN;
}

CldliteCommandLineInfo::~CldliteCommandLineInfo()
{

}

void CldliteCommandLineInfo::ParseParam(const TCHAR * pszParam, BOOL bFlag, BOOL bLast)
{
	char type;
	int mode;

	if (bFlag == FALSE) {
		// must be a filename
		m_strFileName = pszParam;
		m_nShellCommand = FileOpen;
	} else {
		switch(pszParam[0]) {
		case 'A':
		case 'a':
			sscanf(pszParam,"%c%f,%f,%f,%f,%f,%f,%f,%f,%f",
				&type,
				&(ldraw_commandline_opts.A.a),
				&(ldraw_commandline_opts.A.b),
				&(ldraw_commandline_opts.A.c),
				&(ldraw_commandline_opts.A.d),
				&(ldraw_commandline_opts.A.e),
				&(ldraw_commandline_opts.A.f),
				&(ldraw_commandline_opts.A.g),
				&(ldraw_commandline_opts.A.h),
				&(ldraw_commandline_opts.A.i));
			break;
		case 'B':
		case 'b':
			sscanf(pszParam,"%c%d",&type,&(ldraw_commandline_opts.B));
			break;
		case 'C':
		case 'c':
			sscanf(pszParam,"%c%d",&type,&(ldraw_commandline_opts.C));
			break;
		case 'F':
		case 'f':
			{
				char c;
				sscanf(pszParam,"%c%c",&type,&c);
				c = toupper(c);
				switch (c) {
				case 'S':
					ldraw_commandline_opts.F = 1;
					break;
				case 'W':
					ldraw_commandline_opts.F = 2;
					zWire = 1;
					break;
				}
			}
			break;
			break;
		case 'G':
		case 'g':
			ldraw_commandline_opts.debug_level = 1;
			break;
		case 'L':
		case 'l':
			ldraw_commandline_opts.log_output = 1;
			break;
		case 'M':
		case 'm':
			sscanf(pszParam,"%c%c",&type,&(ldraw_commandline_opts.M));
			ldraw_commandline_opts.M = toupper(ldraw_commandline_opts.M);
			break;
		case 'O':
		case 'o':
			{
				float dx, dy;
				sscanf(pszParam,"%c%f,%f",&type, &dx, &dy);
				ldraw_commandline_opts.O.x = dx;
				ldraw_commandline_opts.O.y = dy;
				ldraw_commandline_opts.O.z = 0.0;
			}
			break;
		case 'P':
		case 'p':
			ldraw_commandline_opts.poll = 1;
			break;
		case 'R':
		case 'r':
			sscanf(pszParam,"%c%s",&type, &output_file_name);
			ldraw_commandline_opts.output=1;
			break;
		case 'S':
		case 's':
			sscanf(pszParam,"%c%g",&type,&(ldraw_commandline_opts.S));
			break;
		case 'T':
		case 't':
			ldraw_commandline_opts.rotate = 1;
			break;
		case 'V':
		case 'v':
			sscanf(pszParam,"%c%d",&type, &mode);
			switch(mode) {
			case 0:
				ldraw_commandline_opts.V_x=320;
				ldraw_commandline_opts.V_y=200;
				break;
			case 1:
				ldraw_commandline_opts.V_x=640;
				ldraw_commandline_opts.V_y=200;
				break;
			case 2:
				ldraw_commandline_opts.V_x=640;
				ldraw_commandline_opts.V_y=350;
				break;
			case 3:
				ldraw_commandline_opts.V_x=640;
				ldraw_commandline_opts.V_y=480;
				break;
			case 4:
				ldraw_commandline_opts.V_x=800;
				ldraw_commandline_opts.V_y=600;
				break;
			case 5:
				ldraw_commandline_opts.V_x=1024;
				ldraw_commandline_opts.V_y=768;
				break;
			case 6:
				ldraw_commandline_opts.V_x=1280;
				ldraw_commandline_opts.V_y=1024;
				break;
			}
			break;
		case 'Z':
		case 'z':
			{
				double g;
				sscanf(pszParam,"%c%g",&type,&g);
				ldraw_commandline_opts.Z = (int) (Z_SCALE_FACTOR * g + 0.5);
			}
			break;
		}
	}
}
