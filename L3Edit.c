#include "math.h"

#ifdef USE_OPENGL
// Gotta prepare for case sensitive file systems.
#include "StdAfx.h"
#else
#include "stdafx.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "L3Def.h"
//#include "math.h"
#include "string.h"

#include "platform.h"

#ifndef false
#define false 0
#endif

extern int Draw1PartPtr(struct L3LineS *LinePtr, int Color);

extern char datfilename[256];
extern float m_m[4][4];

static struct L3LineS *SelectedLinePtr = NULL;

/*****************************************************************************/
int Find1Part(int partnum)
{
    int            i = 0;
    struct L3LineS *LinePtr;

    if (partnum <= 0)
      return 0;

    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    return i;
	i++;
    }
    i--; // not found, return last piece in list.
      
    if (i < 0)
      i = 0;

    return i;
}

/*****************************************************************************/
int Select1Part(int partnum)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;

    PrevPtr = NULL;
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	PrevPtr = LinePtr;
	i++;
    }
    if (!LinePtr)
	return -1; //partnum not found

    if (PrevPtr)
    {
	PrevPtr->NextLine = LinePtr->NextLine;
    }
    else
    {
	Parts[0].FirstLine = LinePtr->NextLine;
    }
    if (SelectedLinePtr)
    {
	// Gotta free the old Selected line to avoid memleaks
	// Should also renumber lines whenever we free one.
    }
    SelectedLinePtr = LinePtr;
    return i;
}

/*****************************************************************************/
int UnSelect1Part(int partnum)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;

    if (!SelectedLinePtr)
      return -1;

    PrevPtr = NULL;
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part to insert before
	PrevPtr = LinePtr;
	i++;
    }

    if (PrevPtr)
    {
      SelectedLinePtr->NextLine = PrevPtr->NextLine;
      PrevPtr->NextLine = SelectedLinePtr;
    }
    else
    {
      SelectedLinePtr->NextLine = Parts[0].FirstLine;
      Parts[0].FirstLine = SelectedLinePtr;
    }

    SelectedLinePtr = NULL;
    return i;
}

/*****************************************************************************/
int Delete1Part(int partnum)
{
    if (!SelectedLinePtr)
      return -1;
    free(SelectedLinePtr);
    SelectedLinePtr = NULL;

    return 0;
}

/*****************************************************************************/
int Move1Part(int partnum, float m[4][4], int premult)
{
    float          m1[4][4];
    int            i = 0;
    struct L3LineS *LinePtr;
    float x, y, z;
    
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
        if (premult)
	{
	  M4M4Mul(m1,LinePtr->v,m);
	}
	else
	{
	  x = LinePtr->v[0][3];
	  y = LinePtr->v[1][3];
	  z = LinePtr->v[2][3];
	  LinePtr->v[0][3] = 0; // Rotate around origin
	  LinePtr->v[1][3] = 0;
	  LinePtr->v[2][3] = 0;
	  M4M4Mul(m1,m,LinePtr->v);
	  m1[0][3] = x;
	  m1[1][3] = y;
	  m1[2][3] = z;
	}
	//LinePtr->v = m1;
        memcpy(LinePtr->v, m1, sizeof(LinePtr->v));
	break;
    case 2:
    case 3:
    case 4:
    case 5:
    default:
	break;
    }
    
    return 1;
}

/*****************************************************************************/
int Rotate1Part(int partnum, float m[4][4])
{
    float          m1[4][4];
    int            i = 0;
    struct L3LineS *LinePtr;
    
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
	M4M4Mul(m1,LinePtr->v,m);
        memcpy(LinePtr->v, m1, sizeof(LinePtr->v));
	//LinePtr->v = m1;
	break;
    case 2:
    case 3:
    case 4:
    case 5:
    default:
	break;
    }
    
    return 1;
}

/*****************************************************************************/
int Translate1Part(int partnum, float m[4][4])
{
    float          m1[4][4];
    int            i = 0;
    struct L3LineS *LinePtr;
    
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
	M4M4Mul(m1,LinePtr->v,m);
	LinePtr->v[0][3] += m[0][3];
	LinePtr->v[1][3] += m[1][3];
	LinePtr->v[2][3] += m[2][3];
	break;
    case 2:
    case 3:
    case 4:
    case 5:
    default:
	break;
    }
    
    return 1;
}

/*****************************************************************************/
int Color1Part(int partnum, int Color)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
	LinePtr->Color = Color;
	break;
    case 2:
    case 3:
    case 4:
    case 5:
    default:
	break;
    }
    
    return 1;
}

/*****************************************************************************/
int Find1PartMatrix(int partnum, float m[4][4])
{
    int            i = 0;
    struct L3LineS *LinePtr;
    
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
        memcpy(m, LinePtr->v, sizeof(LinePtr->v));
	//printf("Piece (%0.2f, %0.2f, %0.2f)\n", m[0][3],m[1][3],m[2][3]);
	M4M4Mul(m,m_m,LinePtr->v); // Adjust center point of part by view matrix.
	//printf("View  (%0.2f, %0.2f, %0.2f)\n", m[0][3],m[1][3],m[2][3]);
	return 1;
    case 2:
    case 3:
    case 4:
    case 5:
    default:
	break;
    }
    
    return 0;
}

/*****************************************************************************/
int Print1LinePtr(struct L3LineS *LinePtr, int i, char *s, FILE *f)
{
    if (!LinePtr)
    {
      if (i < 0)
      {
      if (s)
	sprintf(s,"--START--\n");
      if (f)
	fprintf(f,"--START--\n");
      }
      else
      {
      if (s)
	sprintf(s,"--END--\n");
      if (f)
	fprintf(f,"--END--\n");
      }
      return 0; //partnum not found
    }
    
    switch (LinePtr->LineType)
    {
    case 0:
      if (s)
	sprintf(s,"%d %s\n", LinePtr->LineType, LinePtr->Comment);
      if (f)
	fprintf(f,"%d %s\n", LinePtr->LineType, LinePtr->Comment);
      break;
    case 1:
      if (s)
	sprintf(s,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g %s\n",
		LinePtr->LineType, LinePtr->Color,
		LinePtr->v[0][3],LinePtr->v[1][3],LinePtr->v[2][3],
		LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2],
		LinePtr->PartPtr->DatName);
      if (f)
	fprintf(f,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g %s\n",
		LinePtr->LineType, LinePtr->Color,
		LinePtr->v[0][3],LinePtr->v[1][3],LinePtr->v[2][3],
		LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2],
		LinePtr->PartPtr->DatName);
      break;
    case 2:
      if (s)
	sprintf(s,"%d %d %g %g %g %g %g %g\n",
		LinePtr->LineType, LinePtr->Color,
		LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2]);
      if (f)
	fprintf(f,"%d %d %g %g %g %g %g %g\n",
		LinePtr->LineType, LinePtr->Color,
		LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2]);
      break;
    case 3:
      if (s)
	sprintf(s,"%d %d %g %g %g %g %g %g %g %g %g\n",
		LinePtr->LineType, LinePtr->Color,
		LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2]);
      if (f)
	fprintf(f,"%d %d %g %g %g %g %g %g %g %g %g\n",
		LinePtr->LineType, LinePtr->Color,
		LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2]);
      break;
    case 4:
      if (s)
	sprintf(s,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		LinePtr->LineType, LinePtr->Color,
		LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2],
		LinePtr->v[3][0],LinePtr->v[3][1],LinePtr->v[3][2]);
      if (f)
	fprintf(f,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		LinePtr->LineType, LinePtr->Color,
		LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2],
		LinePtr->v[3][0],LinePtr->v[3][1],LinePtr->v[3][2]);
      break;
    case 5:
      if (s)
	sprintf(s,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		LinePtr->LineType, LinePtr->Color,
		LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2],
		LinePtr->v[3][0],LinePtr->v[3][1],LinePtr->v[3][2]);
      if (f)
	fprintf(f,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		LinePtr->LineType, LinePtr->Color,
		LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2],
		LinePtr->v[3][0],LinePtr->v[3][1],LinePtr->v[3][2]);
      break;
    default:
      if (s)
	sprintf(s,"\n");
      if (f)
	fprintf(f,"\n");
      break;
    }

    return 1;
}

/*****************************************************************************/
int Print3Parts(int partnum, char *s1, char *s2, char *s3)
{
    int            i = 0;
    struct L3LineS *LinePtr = NULL;
    struct L3LineS *PrevPtr = NULL;
    struct L3LineS *NextPtr = NULL;
    
    if (SelectedLinePtr)
    {
      for (PrevPtr = Parts[0].FirstLine; PrevPtr; PrevPtr = PrevPtr->NextLine)
      {
	if (i == (partnum-1))
	{
	  break;
	}
	i++;
      }
      LinePtr = SelectedLinePtr;
      i = partnum;
    }
    else 
      for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	{
	  break;
	}
	i++;
	PrevPtr = LinePtr;
    }

    if (!LinePtr)
      i = partnum;
    else 
      NextPtr = LinePtr->NextLine;

    Print1LinePtr(PrevPtr, i-1, s1, NULL);
    Print1LinePtr(LinePtr, i, s2, NULL);
    Print1LinePtr(NextPtr, i+1, s3, NULL);
    
    return i;
}

/*****************************************************************************/
int Print1Part(int partnum, FILE *f)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    
    if (SelectedLinePtr)
    {
	LinePtr = SelectedLinePtr;
	i = partnum;
    }
    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }

    if (!LinePtr)
      i = partnum;
    i = Print1LinePtr(LinePtr, i, NULL, f);
    
    return i;
}

/*****************************************************************************/
int Add1Part(int partnum)
{
    float m[4][4] = {
      {1.0,0.0,0.0,0.0},
      {0.0,1.0,0.0,0.0},
      {0.0,0.0,1.0,0.0},
      {0.0,0.0,0.0,1.0}
    };

    int            i = 0;
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;
    struct L3PartS *PartPtr;
    char *SubPartDatName;
    int Color;

    if (SelectedLinePtr)
	UnSelect1Part(SelectedLinePtr->LineNo);

    PrevPtr = NULL;
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	PrevPtr = LinePtr;
	if (i == partnum)
	{
	    i++;
	    break; // found the part to insert after.
	}
	i++;
    }

    if (!PrevPtr)
    {
	// I have to make a Parts[0] for the new nameless model.
	// Unless its an old model where we deleted all the pieces.
	PartPtr = &Parts[0];
	if (nParts == 0)
	    nParts = 1;                         /* First time a model is loaded    */
	if (!strlen(datfilename) || (!strcmp(datfilename, " ")))
	    strcpy(datfilename, "model.dat");
	PartPtr->DatName = Strdup(datfilename);
	if (!PartPtr->DatName)
	    return 0;
	PartPtr->FirstLine = NULL;
	nColors = 0;
	/* Be sure to register default part color */
	Colors[nColors++].Color = L3Pov.DefaultPartColorNumber;

	SubPartDatName = Strdup("3001.dat");
	Color = 0;
    }
    else if (!(PrevPtr->PartPtr) || 
	     !(PrevPtr->PartPtr->DatName) ||
	     !(strlen(PrevPtr->PartPtr->DatName)))
    {
	// No previous part, so just use the default.
	SubPartDatName = Strdup("3001.dat");
	Color = 0;
    }
    else
    {
	SubPartDatName = PrevPtr->PartPtr->DatName;
	Color = PrevPtr->Color;
    }


    /* Watch out for uppercase and (back)slashes in filename... */
    FixDatName(SubPartDatName);
    PartPtr = FindPart(0, SubPartDatName);
    if (!PartPtr)
	return 0; //partnum not found
    if (LoadPart(PartPtr, false, Parts[0].DatName) == 2)
    {
	return 0;
    }

    LinePtr = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
    if (!LinePtr)
	return 0;

    LinePtr->LineType = 1;
    LinePtr->Color = Color;
#ifndef __TURBOC__
    LinePtr->LineNo = i;
#endif
    if (PrevPtr)
    {
#ifdef REUSE_ORIENTATION
        LinePtr->v = PrevPtr->v;
#else
        memcpy(LinePtr->v, m, sizeof(LinePtr->v));
	//LinePtr->v = m;
        // Just copy the position, but not the orientation.
	LinePtr->v[0][3] += PrevPtr->v[0][3];
	LinePtr->v[1][3] += PrevPtr->v[1][3];
	LinePtr->v[2][3] += PrevPtr->v[2][3];
#endif
    }
    else
        memcpy(LinePtr->v, m, sizeof(LinePtr->v));
        //LinePtr->v = m;
    LinePtr->PartPtr = PartPtr;
    if (PrevPtr)
    {
	LinePtr->NextLine = PrevPtr->NextLine;
	PrevPtr->NextLine = LinePtr;
    }
    else
    {
	Parts[0].FirstLine = LinePtr;
    }

    return LinePtr->LineNo;
}

/*****************************************************************************/
int Swap1Part(int partnum, char *SubPartDatName)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    struct L3PartS *PartPtr;

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum) // found the part
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
	/* Watch out for uppercase and (back)slashes in filename... */
	FixDatName(SubPartDatName);
	PartPtr = FindPart(0, SubPartDatName);
	if (PartPtr)
	{
	    if (LoadPart(PartPtr, false, Parts[0].DatName) == 2)
	    {
		return 0;
	    }
	    LinePtr->PartPtr = PartPtr;
	    return 1;
	}
	break;
    case 2:
    case 3:
    case 4:
    case 5:
    default:
	break;
    }
    
    return 0;
}

/*****************************************************************************/
int DrawCurPart(int Color)
{
    return Draw1PartPtr(SelectedLinePtr, Color);
}

/*****************************************************************************/
int Print1Model(char *filename)
{
    FILE *f;
    struct L3LineS *LinePtr;
    
    printf("Write DAT %s\n", filename);
    f = fopen(filename, "w+");
    if (!f)
    {
	printf("Could not open %s\n", filename);
	return(0);
    }
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	switch (LinePtr->LineType)
	{
	case 0:
	    fprintf(f,"%d %s\n", LinePtr->LineType, LinePtr->Comment);
	    break;
	case 1:
	    fprintf(f,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g %s\n",
		    LinePtr->LineType, LinePtr->Color,
		    LinePtr->v[0][3],LinePtr->v[1][3],LinePtr->v[2][3],
		    LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		    LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		    LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2],
		    LinePtr->PartPtr->DatName);
	    break;
	case 2:
	    fprintf(f,"%d %d %g %g %g %g %g %g\n",
		    LinePtr->LineType, LinePtr->Color,
		    LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		    LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2]);
	    break;
	case 3:
	    fprintf(f,"%d %d %g %g %g %g %g %g %g %g %g\n",
		    LinePtr->LineType, LinePtr->Color,
		    LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		    LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		    LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2]);
	    break;
	case 4:
	    fprintf(f,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		    LinePtr->LineType, LinePtr->Color,
		    LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		    LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		    LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2],
		    LinePtr->v[3][0],LinePtr->v[3][1],LinePtr->v[3][2]);
	    break;
	case 5:
	    fprintf(f,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		    LinePtr->LineType, LinePtr->Color,
		    LinePtr->v[0][0],LinePtr->v[0][1],LinePtr->v[0][2],
		    LinePtr->v[1][0],LinePtr->v[1][1],LinePtr->v[1][2],
		    LinePtr->v[2][0],LinePtr->v[2][1],LinePtr->v[2][2],
		    LinePtr->v[3][0],LinePtr->v[3][1],LinePtr->v[3][2]);
	    break;
	default:
	    fprintf(f,"\n");
	    break;
	}
    }
    fclose(f);
    return 0;
}

/*****************************************************************************/
int Comment1Part(int partnum, char *Comment)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    int            Len;

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;
    else
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part
	i++;
    }

    if (!LinePtr)
	return 0; //partnum not found
    
    if (LinePtr->PartPtr)
    {
      //free(LinePtr->PartPtr);
      LinePtr->PartPtr = NULL;
    }

    LinePtr->LineType = 0;
    // SaveLine(&LinePtrPtr, LinePtr, comment);
    Len = strlen(Comment) + 1;
    if (Len > sizeof(LinePtr->v))
    {
      LinePtr->Comment = Strdup(Comment);
      if (!LinePtr->Comment)
      {
	free(LinePtr);
	return (1);
      }
    }
    else
    {
      /* Reuse the 64 bytes of float v[4][4] */
      LinePtr->Comment = (char *) LinePtr->v;
      strcpy(LinePtr->Comment, Comment);
    }

    return 1;
}

/*****************************************************************************/
extern int editing;
extern int curpiece;
extern int movingpiece;
// Note: should I reset these next 2 to defaults when exiting edit mode?
extern int StartLineNo;
extern int DrawToCurPiece;

static int LineNoCounter = -1;
/*****************************************************************************/
int Init1LineCounter()
{
  LineNoCounter = 0;
}

/*****************************************************************************/
int Skip1Line(int IsModel, struct L3LineS *LinePtr)
{
  if (!editing)
    return 0;

  if (!IsModel)
    return 0;
  
  LineNoCounter++;

  // This needs work.  I should count the lines myself.
  // Make Init1LineCounter() fn and make a static counter in this module.
  // Call Init1LineCounter() in DrawModel() in L3View.cpp.
  // Use the static counter instead of LinePtr->LineNo.
  if (LineNoCounter <= StartLineNo)
  {
    //printf("Skipping %d < %d\n", LinePtr->LineNo, StartLineNo);
    return 1;
  }

  if (!DrawToCurPiece)
    return 0;

  // This needs work.  
  // I need to redraw up to the current piece whenever I pick "goto piece".
  if (LineNoCounter > curpiece)
  {
    //printf("Skipping %d > %d\n", LinePtr->LineNo, curpiece);
    return 1;
  }

  return 0;
}

/*****************************************************************************/
int Switch1Part(int partnum)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;
    struct L3LineS *NextPtr;

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    PrevPtr = NULL;
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part
	PrevPtr = LinePtr;
	i++;
    }

    if (!LinePtr)
	return 0; //partnum not found

    if (!LinePtr->NextLine)
	return 0; // next part not found
    
    NextPtr = LinePtr->NextLine;
    if (!PrevPtr)
      Parts[0].FirstLine = NextPtr;
    else
      PrevPtr->NextLine = NextPtr;
    LinePtr->NextLine = NextPtr->NextLine;
    NextPtr->NextLine = LinePtr;

    return 1;
}

/*****************************************************************************/
// This can be used to free any parts temporarily loaded for parts.lst
// Just save nParts, load & show a part, then FreeSomeParts(oldnParts, nParts)
/*****************************************************************************/
extern void FreePart(struct L3PartS * PartPtr);

/*****************************************************************************/
void FreeSomeParts(firstpart, lastpart)
{
   register int         i;
   for (i = firstpart; i < lastpart; i++)
      FreePart(&Parts[i]);
   //nParts = firstpart;
}

/*****************************************************************************/
#include <GL/Glut.h>

extern GLdouble model_mat[4*4];
extern GLdouble proj_mat[4*4];
extern GLint view_mat[4];

extern GLint Width;
extern GLint Height;

typedef struct vector3d_struct {
	float x;
	float y;
	float z;
} vector3d;

/*****************************************************************************/
void GetPartBox(struct L3LineS *LinePtr, int sc[4])
{
  float          r[4];
  int            i;
  float          r2[4];
  vector3d       bb3d[8];
  float m[4][4];
  struct L3PartS *PartPtr = LinePtr->PartPtr;

  GLdouble s0x, s0y, s0z;
  GLdouble s1x, s1y, s1z;
  GLdouble s2x, s2y, s2z;

  M4M4Mul(m,m_m,LinePtr->v); // Adjust center point of part by view matrix.

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

  // Gotta convert to screen coords first for opengl.
  s2x = s1y = 0.0;
  s0x = Width;
  s0y = Height;

  for (i = 0; i < 8; i++)
  {
    gluProject((GLdouble)bb3d[i].x, (GLdouble)-bb3d[i].y, (GLdouble)-bb3d[i].z,
	       model_mat, proj_mat, view_mat,
	       &s1x, &s1y, &s1z);

    if (s1x < s0x)
      s0x = s1x;
    if (s1y < s0y)
      s0y = s1y;

    if (s1x > s2x)
      s2x = s1x;
    if (s1y > s2y)
      s2y = s1y;
  }
  // FUDGE FACTOR.
  s0x -= 8;
  s0y -= 8;
  s2x += 8;
  s2y += 8;

  if (s0x < 0.0) s0x = 0.0;
  if (s0y < 0.0) s0y = 0.0;
  if (s2x > Width) s2x = Width;
  if (s2y > Height) s2y = Height;

  sc[0] = (int)s0x;
  sc[1] = (int)s0y;
  sc[2] = (int)(s2x - s0x);
  sc[3] = (int)(s2y - s0y);

  if (sc[2] <= 0) sc[2] = 1;
  if (sc[3] <= 0) sc[3] = 1;
}

/*****************************************************************************/
int Get1PartBox(int partnum, int sc[4])
{
    int            i = 0;
    struct L3LineS *LinePtr;

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else 
      for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part
	i++;
    }

    if (!LinePtr)
	return 0; //partnum not found

    GetPartBox(LinePtr, sc);

    return 1;
}

