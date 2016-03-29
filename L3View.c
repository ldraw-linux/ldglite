/*
 *  L3View.cpp, part of the L3 project for handling LDraw *.dat files
 *  Copyright (C) 1998  Lars C. Hassing
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*****************************************************************************
  Please do not edit this file. In stead contact Lars C. Hassing (lch@cci.dk)
  to get your changes integrated and you will receive an up-to-date version.
*****************************************************************************/

/* L3View.cpp  Traverse-DAT-tree routines for L3 program */
/*
970918 lch First version, both for TurboC 2.0 and Visual C++ 5.0
980926 lch Version released for ldlite
*/

// L3View.cpp : implementation of the CL3View class
//

#ifdef USE_OPENGL
#include "math.h"
//extern "C" {
#include "platform.h"
#endif

#ifdef USE_OPENGL
// Gotta prepare for case sensitive file systems.
#include "StdAfx.h"
#else
#include "stdafx.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "L3Def.h"

#ifdef USE_OPENGL
#include "ldliteVR.h"
#include "stub.h"
#include "ldliteVR.h"
extern void get_opengl_transforms(void);
extern LDRAW_COMMANDLINE_OPTS ldraw_commandline_opts;
static	float	v[4];

extern int Skip1Line(int IsModel, struct L3LineS *LinePtr);
extern int Init1LineCounter(void);

extern int BufA1Store(int IsModel, struct L3LineS *LinePtr);
extern int BufA1Retrieve(int IsModel, struct L3LineS *LinePtr);
int Draw1Part(int partnum, int Color);

extern int ldlite_parse_colour_meta(char *s);

extern int include_stack_ptr;
extern int hardcolor;

//************************************************************************
void MakePartBox(struct L3PartS *PartPtr,float m[4][4], vector3d bb3d[8])
{
  float          r[4];
  float          r2[4];

  r2[0]=PartPtr->BBox[0][0]; //bb[0]
  r2[1]=PartPtr->BBox[0][1];
  r2[2]=PartPtr->BBox[0][2];
  M4V3Mul(r,m,r2);
  bb3d[0].x=r[0];
  bb3d[0].y=r[1];
  bb3d[0].z=r[2];
  r2[0]=PartPtr->BBox[0][0]; //bb[1]
  r2[1]=PartPtr->BBox[1][1];
  r2[2]=PartPtr->BBox[0][2];
  M4V3Mul(r,m,r2);
  bb3d[1].x=r[0];
  bb3d[1].y=r[1];
  bb3d[1].z=r[2];
  r2[0]=PartPtr->BBox[1][0]; //bb[2]
  r2[1]=PartPtr->BBox[1][1];
  r2[2]=PartPtr->BBox[0][2];
  M4V3Mul(r,m,r2);
  bb3d[2].x=r[0];
  bb3d[2].y=r[1];
  bb3d[2].z=r[2];
  r2[0]=PartPtr->BBox[1][0]; //bb[3]
  r2[1]=PartPtr->BBox[0][1];
  r2[2]=PartPtr->BBox[0][2];
  M4V3Mul(r,m,r2);
  bb3d[3].x=r[0];
  bb3d[3].y=r[1];
  bb3d[3].z=r[2];
  r2[0]=PartPtr->BBox[0][0]; //bb[4]
  r2[1]=PartPtr->BBox[0][1];
  r2[2]=PartPtr->BBox[1][2];
  M4V3Mul(r,m,r2);
  bb3d[4].x=r[0];
  bb3d[4].y=r[1];
  bb3d[4].z=r[2];
  r2[0]=PartPtr->BBox[0][0]; //bb[5]
  r2[1]=PartPtr->BBox[1][1];
  r2[2]=PartPtr->BBox[1][2];
  M4V3Mul(r,m,r2);
  bb3d[5].x=r[0];
  bb3d[5].y=r[1];
  bb3d[5].z=r[2];
  r2[0]=PartPtr->BBox[1][0]; //bb[6]
  r2[1]=PartPtr->BBox[1][1];
  r2[2]=PartPtr->BBox[1][2];
  M4V3Mul(r,m,r2);
  bb3d[6].x=r[0];
  bb3d[6].y=r[1];
  bb3d[6].z=r[2];
  r2[0]=PartPtr->BBox[1][0]; //bb[7]
  r2[1]=PartPtr->BBox[0][1];
  r2[2]=PartPtr->BBox[1][2];
  M4V3Mul(r,m,r2);
  bb3d[7].x=r[0];
  bb3d[7].y=r[1];
  bb3d[7].z=r[2];
}

//************************************************************************
void DrawPartBox(struct L3PartS *PartPtr,int CurColor,float m[4][4],int wire)
{
  float          r[4];
  int            i, Color;
  float          r2[4];
  vector3d       bb3d[8];

#ifdef SIXTEEN_EDGE_COLORS
  if (0 <= CurColor  &&  CurColor <= 15)
    Color = edge_color(CurColor);
  else
    Color = 0;
#else
    Color = edge_color(CurColor);
#endif
  
  Color = CurColor;
#if 0	    
  printf("IsModel = %d  FromPARTS = %d  color = %d\n",
	 IsModel, PartPtr->FromPARTS, Color);
  printf("BBox = (%0.2f,%0.2f,%0.2f) (%0.2f,%0.2f,%0.2f)\n",
	 PartPtr->BBox[0][0],PartPtr->BBox[0][1],PartPtr->BBox[0][2],
	 PartPtr->BBox[1][0],PartPtr->BBox[1][1],PartPtr->BBox[1][2]);
#endif
  MakePartBox(PartPtr, m, bb3d);
  if (wire) // Draw wireframe box.
  {
    render_line(&bb3d[0],&bb3d[1],Color);
    render_line(&bb3d[1],&bb3d[2],Color);
    render_line(&bb3d[2],&bb3d[3],Color);
    render_line(&bb3d[3],&bb3d[0],Color);
    render_line(&bb3d[4],&bb3d[5],Color);
    render_line(&bb3d[5],&bb3d[6],Color);
    render_line(&bb3d[6],&bb3d[7],Color);
    render_line(&bb3d[7],&bb3d[4],Color);
    render_line(&bb3d[0],&bb3d[4],Color);
    render_line(&bb3d[1],&bb3d[5],Color);
    render_line(&bb3d[2],&bb3d[6],Color);
    render_line(&bb3d[3],&bb3d[7],Color);
  }
  else // Lets try a solid box.
  {
    render_quad(&bb3d[0],&bb3d[1],&bb3d[2],&bb3d[3],Color);
    render_quad(&bb3d[4],&bb3d[5],&bb3d[6],&bb3d[7],Color);
    
    render_quad(&bb3d[0],&bb3d[4],&bb3d[5],&bb3d[1],Color);
    render_quad(&bb3d[1],&bb3d[5],&bb3d[6],&bb3d[2],Color);
    render_quad(&bb3d[2],&bb3d[6],&bb3d[7],&bb3d[3],Color);
    render_quad(&bb3d[3],&bb3d[7],&bb3d[4],&bb3d[0],Color);
  }
}

//************************************************************************
void DrawPartLine(struct L3PartS *PartPtr,int CurColor,float m[4][4])
{
  float          r[4];
  int            i, Color;
  float          r2[4];
  vector3d       bb3d[8];

#ifdef SIXTEEN_EDGE_COLORS
  if (0 <= CurColor  &&  CurColor <= 15)
    Color = edge_color(CurColor);
  else
    Color = 0;
#else
    Color = edge_color(CurColor);
#endif
  
  //Color = CurColor;

  r2[0]=(PartPtr->BBox[0][0] + PartPtr->BBox[1][0]) / 2.0;
  r2[1]=(PartPtr->BBox[0][1]);
  r2[2]=(PartPtr->BBox[0][2] + PartPtr->BBox[1][2]) / 2.0;
  M4V3Mul(r,m,r2);
  bb3d[0].x=r[0];
  bb3d[0].y=r[1];
  bb3d[0].z=r[2];

  r2[1]=PartPtr->BBox[1][1];
  M4V3Mul(r,m,r2);
  bb3d[1].x=r[0];
  bb3d[1].y=r[1];
  bb3d[1].z=r[2];

  render_line(&bb3d[0],&bb3d[1],Color);
}
#endif

#ifdef USE_OPENGL_OCCLUSION
//************************************************************************
#include <GL/glut.h>
int Occluded(struct L3PartS *PartPtr, int Color, float m1[4][4])
{
  GLfloat feedbackBuffer[1024];
  GLint returned = 0;
  int size = 1024;

  // Nutz!!  Depth test is performed AFTER feedback is generated
  // so this does NOT work.  Need HP_OCCLUSION_TEST extension...
  glFeedbackBuffer(size, GL_3D_COLOR, feedbackBuffer);
  (void) glRenderMode(GL_FEEDBACK);

  DrawPartBox(PartPtr, Color, m1, 0);
  glFlush();

  returned = glRenderMode(GL_RENDER);

  if (returned > 0)
    return 0;
  else 
    return 1;
}

//************************************************************************
#endif

static void DrawPart(int IsModel, struct L3PartS *PartPtr, int CurColor, float m[4][4])
{
	float          r[4], m1[4][4];
	int            i, Color;
	struct L3LineS *LinePtr;
	vector3d       v3d[4];

#ifdef USE_OPENGL
	float mm[4][4];
	float det = 0;

	if ((ldraw_commandline_opts.F & TYPE_F_STUDLINE_MODE) != 0)
	  if (PartPtr->IsStud)
	  {
	    DrawPartLine(PartPtr, CurColor, m);
	    //DrawPartBox(PartPtr, CurColor, m, 1);
	    return;
	  }

	// Draw only bounding boxes of top level parts if in fast spin mode.
	if (ldraw_commandline_opts.F & TYPE_F_BBOX_MODE) 
	  if (PartPtr->FromPARTS) // (!IsModel)
	  {
	    if (ldraw_commandline_opts.F & TYPE_F_NO_POLYGONS) 
	      DrawPartBox(PartPtr, CurColor, m, 1);
	    else
	      DrawPartBox(PartPtr, CurColor, m, 0);
	    return;
	  }	

	  if (PartPtr->IsStud)
	    det = M3Det(m); // Check the determinant of m to fix mirrored studs.
#endif

	for (LinePtr = PartPtr->FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
	{
#ifdef USE_OPENGL
                char *s;

	        if (Skip1Line(IsModel,LinePtr))
		  continue;
#endif
		hardcolor = 0; // Assume color 16 or 24.
		switch (LinePtr->Color)
		{
		case 16:
			Color = CurColor;
			break;
		case 24:
#ifdef SIXTEEN_EDGE_COLORS
			if (0 <= CurColor  &&  CurColor <= 15)
				Color = edge_color(CurColor);
			else
				Color = 0;
#else
			Color = edge_color(CurColor);
#endif
			break;
		default:
			Color = LinePtr->Color;
#if 0
			if ((LinePtr->LineType == 3) || 
			    (LinePtr->LineType == 4))
			  // Hardcoded color = printed.  Blend me!
			  hardcolor = 1; 
#else
			if (LinePtr->LineType != 1)
			  // Hardcoded color = printed.  Blend me!
			  hardcolor = 1; 
#endif
			break;
		}
		switch (LinePtr->LineType)
		{
		case 0:
#ifdef USE_OPENGL
		        // Skip whitespace
		        for (s = LinePtr->Comment; *s != 0; s++)
		        {
			  if ((*s != ' ') && (*s != '\t'))
			    break;
			}
			if (strnicmp(s,"STEP",4) == 0)
			{
			  // if (include_stack_ptr <= ldraw_commandline_opts.output_depth )
			  {
			    zStep(stepcount,1);
			    stepcount++;
			  }
			}
			else if (strncmp(s,"CLEAR",5) == 0)
			{
			  // if (include_stack_ptr <= ldraw_commandline_opts.output_depth )
			  {
			    zClear();
			  }
			}
			// Experiment with MLCAD extensions
			// Based on info provided on lugnet.
			else if (strncmp(s,"BUFEXCHG A STORE",16) == 0)
			{
			  int k;

			  if (IsModel)
			  {
			    k = BufA1Store(IsModel,LinePtr);
			    printf("BUFEXCHG A STORE %d\n", k);
			  }
			}
			else if (strncmp(s,"BUFEXCHG A RETRIEVE",19) == 0)
			{
			  int j,n, opts, dcp, cp;
			  extern int curpiece;
			  extern int DrawToCurPiece;
			  void DrawModel(void);
			   
			  if (IsModel)
			  {
			    n = BufA1Retrieve(IsModel,LinePtr);
			    printf("BUFEXCHG A RETRIEVE %d\n", n);
			    // clear and redraw model up to saved point.
#if 0		    
			    opts = ldraw_commandline_opts.M;
			    ldraw_commandline_opts.M = 'C';
			    dcp = DrawToCurPiece;
			    DrawToCurPiece = 1;
			    cp = curpiece;
			    zClear();
			    curpiece = n;
			    Init1LineCounter();
			    BufA1Store(IsModel,LinePtr);
			    DrawModel();
			    //for(j = 0; j < n; j++)
			    //  Draw1Part(j, -1);
			    DrawToCurPiece = dcp;
			    curpiece = cp;
			    ldraw_commandline_opts.M = opts;
#endif
			  }
			}
			else if (strncmp(s,"GHOST",5) == 0)
			{
			  if (IsModel)
			  {
			    // Parse the rest of the line as a .DAT line.
			  }
			}
			if (ldraw_commandline_opts.M != 'C')
			{
			  // Non-continuous output stop after each step.
			}

			// Parse global color change meta-commands
			// Should?? be done by ReadMetaLine() in L3Input.cpp			
			// Should SAVE old colors, restore after the for loop.
			// To save space, build a linked list of saved colors.
			// Do NOT resave a color if its already saved.
			if ((strncmp(s,"COLOR",5) == 0) ||
			    (strncmp(s,"COLOUR",6) == 0))
			{
			  if (ldraw_commandline_opts.debug_level == 1)
			    printf("%s\n", s);
			  //0 COLOR 4 red 0 196 0 38 255 196 0 38 255
			  char colorstr[256];
			  char name[256];
			  int n, inverse_index;

			  n = sscanf(s, "%s %d %s %d %f %f %f %f %f %f %f %f",
				     colorstr, &i, name, &inverse_index,
				     &m1[0][0], &m1[0][1], &m1[0][2], &m1[0][3],
				     &m1[1][0], &m1[1][1], &m1[1][2], &m1[1][3]);
			  if (n != 12)
			  {
			    if (ldraw_commandline_opts.debug_level == 1)
			      printf("Illegal COLOR syntax %d\n",n);
			    break;
			  }
			  zcolor_modify(i,name,inverse_index,
					(int)m1[0][0], (int)m1[0][1], (int)m1[0][2], (int)m1[0][3],
					(int)m1[1][0], (int)m1[1][1], (int)m1[1][2], (int)m1[1][3]);
			}

			// Intercept the ldconfig.ldr !COLOUR meta command.
			if (ldlite_parse_colour_meta(s))
			  break;
#else
			if (strncmp(LinePtr->Comment,"STEP",4) == 0)
			{
				// STEP encountered, you may set some flags to enable/disable e.g. drawing
				// (Note that I have not tested this, but this is the way it is supposed to work :-)
			}
#endif
			break;
		case 1:
#ifdef USE_OPENGL
    // NOTE:  I could achieve L3Lab speeds if I delay rendering studs till last
    // then do occlusion tests on the bounding boxes before rendering.
    // Unfortunately GL_OCCLUSION_TEST_HP is an extension (SUN, HP, ???)
    // 
    // Other ideas include substituting GLU cylinder primitives for studs.  
		        if (LinePtr->PartPtr->IsStud)
			{
                          if ((ldraw_commandline_opts.F & TYPE_F_STUDLESS_MODE) != 0)
                            break;
#ifdef USE_OPENGL_OCCLUSION
                          if ((ldraw_commandline_opts.F & TYPE_F_STUDONLY_MODE) != 0)
			  {
			    M4M4Mul(m1,m,LinePtr->v);
			    if (Occluded(LinePtr->PartPtr, Color, m1))
			      break;
			  }
#endif
			}	    
#endif
			M4M4Mul(m1,m,LinePtr->v);
#ifdef USE_OPENGL
			// Negative determinant means its a mirrored stud.
			if (det < 0)
			{
			  // For now, we only support Paul Easters logo.dat texture.
			  // For display speed, we really should precalculate an IsLogo flag
			  // and use that here rather than IsStud.
			  if  (LinePtr->PartPtr && LinePtr->PartPtr->DatName &&
			       !stricmp(LinePtr->PartPtr->DatName, "logo.dat"))
			  {
			    float mirror[4][4] = {
			      {1.0,0.0,0.0,0.0},
			      {0.0,1.0,0.0,0.0},
			      {0.0,0.0,-1.0,0.0},
			      {0.0,0.0,0.0,1.0}
			    };
			    M4M4Mul(mm,m1,mirror);
			    memcpy(m1,mm,sizeof(m1));
			  }
			}
			  
			// implement nesting level counter.
			include_stack_ptr++;
			DrawPart(0,LinePtr->PartPtr,Color,m1);
			include_stack_ptr--;	
			// Do zStep() after the model, not toplevel parts.
#else
			DrawPart(0,LinePtr->PartPtr,Color,m1);
			if (IsModel) zStep(0,0);
#endif
			break;
		case 2:
#ifdef USE_OPENGL_OCCLUSION
		        if ((ldraw_commandline_opts.F & TYPE_F_STUDONLY_MODE) &&
			    !(PartPtr->IsStud))
                            break;
#endif
			for (i=0; i<LinePtr->LineType; i++)
			{
				M4V3Mul(r,m,LinePtr->v[i]);
				v3d[i].x=r[0];
				v3d[i].y=r[1];
				v3d[i].z=r[2];
			}
			render_line(&v3d[0],&v3d[1],Color);
			break;
		case 3:
#ifdef USE_OPENGL_OCCLUSION
		        if ((ldraw_commandline_opts.F & TYPE_F_STUDONLY_MODE) &&
			    !(PartPtr->IsStud))
                            break;
#endif
			for (i=0; i<LinePtr->LineType; i++)
			{
				M4V3Mul(r,m,LinePtr->v[i]);
				v3d[i].x=r[0];
				v3d[i].y=r[1];
				v3d[i].z=r[2];
			}
#ifdef USE_OPENGL
			// Try to render solid Primitives visibly when in XOR wire mode.
			if (ldraw_commandline_opts.F & TYPE_F_XOR_PRIMITIVE)
			{
			    render_line(&v3d[0],&v3d[1],Color);
			    render_line(&v3d[1],&v3d[2],Color);
			    render_line(&v3d[2],&v3d[0],Color);
			    break;
			}
#endif
			render_triangle(&v3d[0],&v3d[1],&v3d[2],Color);
			break;
		case 4:
#ifdef USE_OPENGL_OCCLUSION
		        if ((ldraw_commandline_opts.F & TYPE_F_STUDONLY_MODE) &&
			    !(PartPtr->IsStud))
                            break;
#endif
			for (i=0; i<LinePtr->LineType; i++)
			{
				M4V3Mul(r,m,LinePtr->v[i]);
				v3d[i].x=r[0];
				v3d[i].y=r[1];
				v3d[i].z=r[2];
			}
#ifdef USE_OPENGL
			// Try to render solid Primitives visibly when in XOR wire mode.
			if (ldraw_commandline_opts.F & TYPE_F_XOR_PRIMITIVE)
			{
			    render_line(&v3d[0],&v3d[1],Color);
			    render_line(&v3d[1],&v3d[2],Color);
			    render_line(&v3d[2],&v3d[3],Color);
			    render_line(&v3d[3],&v3d[0],Color);
			    break;
			}
#endif
			render_quad(&v3d[0],&v3d[1],&v3d[2],&v3d[3],Color);
			break;
		case 5:
#ifdef USE_OPENGL_OCCLUSION
		        if ((ldraw_commandline_opts.F & TYPE_F_STUDONLY_MODE) &&
			    !(PartPtr->IsStud))
                            break;
#endif
			for (i=0; i<4; i++)
			{
				M4V3Mul(r,m,LinePtr->v[i]);
				v3d[i].x=r[0];
				v3d[i].y=r[1];
				v3d[i].z=r[2];
			}
			render_five(&v3d[0],&v3d[1],&v3d[2],&v3d[3],Color);
			break;
		}
	}
	if (((zDetailLevel == TYPE_PART) && (PartPtr->FromPARTS)) ||
	    ((zDetailLevel == TYPE_P) && (PartPtr->FromP)))
	  zStep(-1, 0);
}


#ifdef USE_OPENGL
float	m_m[4][4];
#else
static	float	m_m[4][4];
#endif

int SetViewMatrix(char *Values) 
{
	float	m[4][4];
	int r;
	memset(m,0,sizeof(m));
	//double: r=sscanf(Values,"%lg %lg %lg %lg %lg %lg %lg %lg %lg",
	r=sscanf(Values,"%g %g %g %g %g %g %g %g %g",
		&m[0][0], &m[0][1], &m[0][2],
		&m[1][0], &m[1][1], &m[1][2],
		&m[2][0], &m[2][1], &m[2][2]);
	if (r != 9) return 0;
	m[3][3] = 1.0;
	m[0][3] = ldraw_commandline_opts.O.x + (zGetRowsize()/2);
	m[1][3] = ldraw_commandline_opts.O.y + (2*zGetColsize()/3);
	m[2][3] = ldraw_commandline_opts.O.z;
	memcpy(m_m,m,sizeof(m));
	return 1;
}


#ifdef USE_OPENGL
void InitViewMatrix(void)
{
	static char matrix_string[256];

	// put the center at the origin for opengl.
	get_opengl_transforms();

	sprintf(matrix_string,"%0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f",
		(ldraw_commandline_opts.S * ldraw_commandline_opts.A.a),
		(ldraw_commandline_opts.S * ldraw_commandline_opts.A.b),
		(ldraw_commandline_opts.S * ldraw_commandline_opts.A.c),
		(ldraw_commandline_opts.S * ldraw_commandline_opts.A.d),
		(ldraw_commandline_opts.S * ldraw_commandline_opts.A.e),
		(ldraw_commandline_opts.S * ldraw_commandline_opts.A.f),
		(ldraw_commandline_opts.S * ldraw_commandline_opts.A.g),
		(ldraw_commandline_opts.S * ldraw_commandline_opts.A.h),
		(ldraw_commandline_opts.S * ldraw_commandline_opts.A.i));

#ifdef USE_GL_TWIRL
	sprintf(matrix_string,"1.00 0.00 0.00 0.00 1.00 0.00 0.00 0.00 1.00");
#endif
	if (ldraw_commandline_opts.debug_level == 1)
	  printf("SetViewMatrix(%s);\n", matrix_string);
	SetViewMatrix(matrix_string);
}

int Draw1PartPtr(struct L3LineS *LinePtr, int Color)
{
	float          m1[4][4];
	int CurColor = ldraw_commandline_opts.C;
	int SaveColor;

	int            i;
	float          r[4];
	vector3d       v3d[4];

	if (!LinePtr)
	  return 0;

	InitViewMatrix();
	include_stack_ptr = 1; // Start nesting level pointer at 1.

	SaveColor = LinePtr->Color;
	if (Color < 0) 
	    Color = LinePtr->Color;
	else
	    LinePtr->Color = Color;
	switch (Color)
	{
	case 16:
	    Color = CurColor;
	    break;
	case 24:
#ifdef SIXTEEN_EDGE_COLORS
	    if (0 <= CurColor  &&  CurColor <= 15)
		Color = edge_color(CurColor);
	    else
		Color = 0;
#else
	    Color = edge_color(CurColor);
#endif
	    break;
	default:
	    break;
	}
	
	switch (LinePtr->LineType)
	{
	case 0:
	    break;
	case 1:
	    M4M4Mul(m1,m_m,LinePtr->v);
	    if ((ldraw_commandline_opts.F & TYPE_F_XOR_MODE) && (LinePtr->PartPtr->FromP))
	    {
		// This is a primitive and may render invisibly in TYPE_F_XOR_MODE
		// if it contains no edges.
		ldraw_commandline_opts.F |= TYPE_F_XOR_PRIMITIVE;
		DrawPart(0,LinePtr->PartPtr,Color,m1);
		ldraw_commandline_opts.F &= ~(TYPE_F_XOR_PRIMITIVE);
		break;
	    }
	    DrawPart(0,LinePtr->PartPtr,Color,m1);
	    break;
	case 2:
	    for (i=0; i<LinePtr->LineType; i++)
	    {
		M4V3Mul(r,m_m,LinePtr->v[i]);
		v3d[i].x=r[0];
		v3d[i].y=r[1];
		v3d[i].z=r[2];
	    }
	    render_line(&v3d[0],&v3d[1],Color);
	    break;
	case 3:
	    for (i=0; i<LinePtr->LineType; i++)
	    {
		M4V3Mul(r,m_m,LinePtr->v[i]);
		v3d[i].x=r[0];
		v3d[i].y=r[1];
		v3d[i].z=r[2];
	    }
	    //if (zWire)
	    if ((ldraw_commandline_opts.F & TYPE_F_NO_POLYGONS) ||
		(ldraw_commandline_opts.F & TYPE_F_XOR_MODE))
	    {
		// This would render invisibly in TYPE_F_XOR_MODE 
		// since it contains no edges.
		render_line(&v3d[0],&v3d[1],Color);
		render_line(&v3d[1],&v3d[2],Color);
		render_line(&v3d[2],&v3d[0],Color);
		break;
	    }
	    render_triangle(&v3d[0],&v3d[1],&v3d[2],Color);
	    break;
	case 4:
	    for (i=0; i<LinePtr->LineType; i++)
	    {
		M4V3Mul(r,m_m,LinePtr->v[i]);
		v3d[i].x=r[0];
		v3d[i].y=r[1];
		v3d[i].z=r[2];
	    }
	    //if (zWire)
	    if ((ldraw_commandline_opts.F & TYPE_F_NO_POLYGONS) ||
		(ldraw_commandline_opts.F & TYPE_F_XOR_MODE))
	    {
		// This would render invisibly in TYPE_F_XOR_MODE
		// since it contains no edges.
		render_line(&v3d[0],&v3d[1],Color);
		render_line(&v3d[1],&v3d[2],Color);
		render_line(&v3d[2],&v3d[3],Color);
		render_line(&v3d[3],&v3d[0],Color);
		break;
	    }
	    render_quad(&v3d[0],&v3d[1],&v3d[2],&v3d[3],Color);
	    break;
	case 5:
	    for (i=0; i<4; i++)
	    {
		M4V3Mul(r,m_m,LinePtr->v[i]);
		v3d[i].x=r[0];
		v3d[i].y=r[1];
		v3d[i].z=r[2];
	    }
	    if (ldraw_commandline_opts.F & TYPE_F_XOR_MODE)
	    {
		render_line(&v3d[0],&v3d[1],Color);
		render_line(&v3d[0],&v3d[2],Color);
		render_line(&v3d[1],&v3d[3],Color);
		break;
	    }
	    render_five(&v3d[0],&v3d[1],&v3d[2],&v3d[3],Color);
	    break;
	default:
	    break;
	}
	
	LinePtr->Color = SaveColor;
	return 1;
}

int Draw1Part(int partnum, int Color)
{
	float          m1[4][4];
	int            i;
	struct L3LineS *LinePtr;
	int SaveColor;

	int CurColor = ldraw_commandline_opts.C;

	InitViewMatrix();

	i = 0;
	for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
	{
	    if (i == partnum)
	    {
		//ldraw_commandline_opts.F |= TYPE_F_XOR_MODE;
		Draw1PartPtr(LinePtr, Color);
		//ldraw_commandline_opts.F &= ~(TYPE_F_XOR_MODE);
		return 1;
	    }
	    i++;
	}
	return 0; //partnum not found
}

/***************************************************************/
struct L3PartS *LoadRC()
{
  extern struct L3PartS *FindPart(int Internal, char *DatName);
  extern int LoadPart(struct L3PartS * PartPtr, int IsModel, char *ReferencingDatfile);

  struct L3PartS *PartPtr;

  PartPtr = FindPart(0, "ldconfig.ldr");
  if (PartPtr)
    if (LoadPart(PartPtr, false, NULL) != 2)
      return(PartPtr);

  PartPtr = FindPart(0, "ldliterc.dat");
  if (!PartPtr)
    return NULL; //partnum not found
  //if (LoadPart(PartPtr, false, Parts[0].DatName) == 2)
  if (LoadPart(PartPtr, false, NULL) == 2)
  {
    return NULL;
  }

  return(PartPtr);
}

/***************************************************************/
#endif

void DrawModel(void)
{
	if (!nParts) return;
#ifdef USE_OPENGL
	InitViewMatrix();

	if (ldraw_commandline_opts.output > 0) {
	  if ((output_file = fopen(output_file_name,"w+"))==NULL) {
	    ldraw_commandline_opts.output = 0;
	  }
	}

	Init1LineCounter();

	include_stack_ptr = 0; // Start nesting level pointer at 0.
#ifdef USE_OPENGL
	if (1) 
	{
	  struct L3PartS *RCPartPtr = LoadRC();
	  
	  if (RCPartPtr)
	    DrawPart(1,RCPartPtr, ldraw_commandline_opts.C, m_m);
	}
#endif
	DrawPart(1,&Parts[0], ldraw_commandline_opts.C, m_m);

	//if (ldraw_commandline_opts.output != 1) zStep(INT_MAX, 0);
	// if (include_stack_ptr <= ldraw_commandline_opts.output_depth )
	include_stack_ptr = 0;
	zStep(stepcount,0);

	if (output_file != NULL) {
	  fclose(output_file);
	  output_file = NULL;
	}
	if (ldraw_commandline_opts.debug_level == 1)
	  printf("Done\n");
#else
	SetViewMatrix("1 0 1  .5 1 -.5  -1 0 1");
	DrawPart(1,&Parts[0], 7, m_m);
#endif

}



#ifdef NOT_YET
void CL3View::OnToolbarFur() // Front Upper Right
{
	SetViewMatrix("1 0 1  .5 1 -.5  -1 0 1");
}

void CL3View::OnToolbarFront() 
{
	SetViewMatrix("1 0 0  0 1 0  0 0 1");
}

void CL3View::OnToolbarRight() 
{
	SetViewMatrix("0 0 -1  0 1 0  1 0 0");
}

void CL3View::OnToolbarBack() 
{
	SetViewMatrix("-1 0 0  0 1 0  0 0 -1");
}

void CL3View::OnToolbarLeft() 
{
	SetViewMatrix("0 0 1  0 1 0  -1 0 0");
}

void CL3View::OnToolbarOver() 
{
	SetViewMatrix("0 0 1  1 0 0  0 1 0");
}

void CL3View::OnToolbarUnder() 
{
	SetViewMatrix("0 0 1  -1 0 0  0 -1 0");
}
#endif

#ifdef USE_OPENGL
//}
#endif /* __cplusplus */


