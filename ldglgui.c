/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/***************************************************************/
#define TEST_MUI_GUI 1
#ifdef  TEST_MUI_GUI

#include <stdio.h>
#include <GL/glut.h>

#include "platform.h"
#include "ldliteVR.h"
#ifndef WINDOWS
// This stuff gets pulled in by glut.h for windows.
#include "wstubs.h"
#else
// glut 3.7 no longer includes windows.h
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
#include <windows.h>
#endif
#endif

#include <mui/mui.h>

extern GLint Width;
extern GLint Height;
extern int  filemenunum;
extern int  dirmenunum;
extern int  mainmenunum;

extern int mui_singlebuffered;

muiObject *b1, *b2, *b3, *b4, *b5, *b6, *b7, *b8;
muiObject *b9, *b10, *b11, *b12, *b13, *b14, *b15, *b16;
muiObject *rb1, *rb2, *rb3, *rb4, *rb5, *rb6, *rb7, *rb8;
muiObject *rb9, *rb10, *rb11, *rb12, *rb13, *rb14, *rb15, *rb16;
muiObject *l1, *l2, *l3, *l4, *l5, *l6, *l7, *l8;
muiObject *t, *t1, *tl;
muiObject *pd, *hs, *vs;

int M1, M2, M3;	/* menus */

//#define THUMBHEIGHT 20
#define THUMBHEIGHT 10
#define ARROWSPACE 40

int dw = 320;
int dh = 200;
int ow = 0;
int oh = 0;

#define MAIN_UILIST	1
#define FILE_UILIST	2
#define VIEW_UILIST	3 
#define DRAW_UILIST	4
#define OPTS_UILIST	5
#define BACK_UILIST	6
#define HELP_UILIST	7

int fileOpenSave = 0;

/***************************************************************/
void muiHide(muiObject *obj, int state)
{
  muiSetVisible(obj, state);
  //muiSetEnable(obj, state);
  //muiSetActive(obj, state);
}

/***************************************************************/
#if 0
#include <mui/gizmo.h>
#else
typedef struct muicons {
    struct muicons  *next;
    muiObject	    *object;
} muiCons;

void	    muiBackgroundClear(void);
muiCons	    *muiGetListCons(int uilist);
#endif

/***************************************************************/
void muiHideAll(int uilist, int state)
{
    muiCons *mcons;

    if ((mcons = muiGetListCons(uilist)) == (muiCons *)0) return;
    muiBackgroundClear();
    while (mcons) {
      muiSetVisible(mcons->object, state);
      //muiSetEnable(mcons->object, state);
      //muiSetActive(mcons->object, state);
      mcons = mcons->next;
    }
}

/***************************************************************/
void muiMoveAll(int uilist, int x, int y)
{
    muiCons *mcons;
    muiObject *obj;

    if ((mcons = muiGetListCons(uilist)) == (muiCons *)0) return;
    muiBackgroundClear();
    while (mcons) {
      obj = mcons->object;
      obj->xmin += x;
      obj->ymin += y;
      obj->xmax += x;
      obj->ymax += y;
      mcons = mcons->next;
    }
}

/***************************************************************/
void mui_cleanup(void)
{
  printf("MUI OK callback\n");
  // Gotta clean up mui and go back to whatever.
  // Must free up mui menu stuff.  
  glEnable( GL_DEPTH_TEST );
  glScissor(0, 0, Width, Height); // x,y,width,height
  glDisable(GL_SCISSOR_TEST);
  registerGlutCallbacks();
  init();
#if 0
  dirtyWindow = 1;  // Redraw
#else
  reshape(Width, Height); // Gotta reshape since MUI changes the matrices.
#endif
  glutPostRedisplay();
}

/***************************************************************/
void bcallback(muiObject *obj, enum muiReturnValue r)
{
  int i = 0;
  void makefileui(char *);
  void makeviewui(void);
  void makebackui(void);
  void makemainui(void);

  muiHide(obj, 0);

  if (obj == b1)
  {
    i = 1;
    muiHideAll(muiGetUIList(obj), 0);
    makefileui("Open:");
  }
  else if (obj == b2)
  {
    i = 2;
    muiHideAll(muiGetUIList(obj), 0);
    makefileui("Save:");
  }
  else if (obj == b3)
  {
    i = 3;
    muiHideAll(muiGetUIList(obj), 0);
    makeviewui();
  }
  else if (obj == b4)
  {
    i = 4;
    muiSetActiveUIList(MAIN_UILIST);
    muiHideAll(muiGetUIList(obj), 1);
  }
  else if (obj == b5)
  {
    i = 5;
    muiSetActiveUIList(MAIN_UILIST);
    muiHideAll(muiGetUIList(obj), 1);
  }
  else if (obj == b6)
  {
    i = 6;
    muiHideAll(muiGetUIList(obj), 0);
    makebackui();
  }
  else if (obj == b7)
  {
    i = 7;
    muiSetActiveUIList(MAIN_UILIST);
    muiHideAll(muiGetUIList(obj), 1);
  }
  else if (obj == b8)
  {
    i = 8;
    muiSetActiveUIList(MAIN_UILIST);
    mui_cleanup();
  }
  else
  {
    muiHideAll(muiGetUIList(obj), 0);
    makemainui();
  }

  printf("Button callback %d, %d\n", i, r);
}

#if 0
/***************************************************************/
void muiPrintObject(muiCons *mcons)
{
  muiObject *obj = mcons->object;

  switch (mcons->object->type) {
  case MUI_BUTTON:
    printf("MUI_BUTTON(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_TEXTBOX:
    printf("MUI_TEXTBOX(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_VSLIDER:
    printf("MUI_VSLIDER(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_HSLIDER:
    printf("MUI_HSLIDER(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_TEXTLIST:
    printf("MUI_TEXTLIST(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_RADIOBUTTON:
    printf("MUI_RADIOBUTTON(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_TINYRADIOBUTTON:
    printf("MUI_TINYRADIOBUTTON(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_PULLDOWN:
    printf("MUI_PULLDOWN(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_LABEL:
    printf("MUI_LABEL(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_BOLDLABEL:
    printf("MUI_BOLDLABEL(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  default:
    printf("MUI_UNKNOWN(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  }
}
#endif

/***************************************************************/
static void nonmuicallback(int x, int y)
{
  int i;
  muiCons *mcons;

  extern int muiInObject(muiObject *obj, int x, int y);

  printf("nonMUI callback %d, %d\n", x, y);

#if 0
  //ActiveCons = muiGetListCons(ActiveUIList);
  //obj = muiFastHitInList(ActiveCons, x, y);
  i = muiGetActiveUIList();
  for (mcons = muiGetListCons(i); mcons; mcons = mcons->next)
  {
    muiPrintObject(mcons);
    if (muiInObject(mcons->object, x, y))
      break;
  }

  if (mcons)
    printf("Should have found  %p\n",obj);
  else
    printf("Found nothing\n");
#endif

}

/***************************************************************/
void makemainui(void)
{
  static muiObject *l1, *l2;
  int xmin, ymin, xmax, ymax;

  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
    muiNewUIList(MAIN_UILIST);	/* makes a MUI display list (number 1) */
    muiNewUIList(FILE_UILIST);
    muiNewUIList(VIEW_UILIST);
    muiNewUIList(DRAW_UILIST);
    muiNewUIList(OPTS_UILIST);
    muiNewUIList(BACK_UILIST);
    muiNewUIList(HELP_UILIST);
    muiSetActiveUIList(MAIN_UILIST);

    b1 = muiNewButton(ow+0, ow+80, oh+dh-25, oh+dh);
    muiLoadButton(b1, "Open");
    muiSetCallback(b1, bcallback);

    b2 = muiNewButton(ow+0, ow+80, oh+dh-50, oh+dh-26);
    muiLoadButton(b2, "Save");
    muiSetCallback(b2, bcallback);

    b3 = muiNewButton(ow+0, ow+80, oh+dh-75, oh+dh-51);
    muiLoadButton(b3, "View");
    muiSetCallback(b3, bcallback);

    b4 = muiNewButton(ow+0, ow+80, oh+dh-100, oh+dh-76);
    muiLoadButton(b4, "Drawing");
    muiSetCallback(b4, bcallback);

    b5 = muiNewButton(ow+0, ow+80, oh+dh-125, oh+dh-101);
    muiLoadButton(b5, "Options");
    muiSetCallback(b5, bcallback);

    b6 = muiNewButton(ow+0, ow+80, oh+dh-150, oh+dh-126);
    muiLoadButton(b6, "Background");
    muiSetCallback(b6, bcallback);

    b7 = muiNewButton(ow+0, ow+80, oh+dh-175, oh+dh-151);
    muiLoadButton(b7, "Help");
    muiSetCallback(b7, bcallback);

    b8 = muiNewButton(ow+0, ow+80, oh+dh-200, oh+dh-176);
    muiLoadButton(b8, "Quit");
    muiSetCallback(b8, bcallback);

#if 0
    l1 = muiNewBoldLabel(ow+60+dw/2-35, oh+dh/2+18, "LdGLite");
#else
    l1 = muiNewLabel(ow+60+dw/2-35, oh+dh/2+18, "LdGLite");
#endif
    l2 = muiNewLabel(ow+60+dw/2-55, oh+dh/2-18, "Version 0.9.6");

    muiSetNonMUIcallback(nonmuicallback);
  }
  else
  {
    muiSetActiveUIList(MAIN_UILIST);
    // Use l4 since its global and exclusive to file menu.
    muiHideAll(muiGetUIList(l1), 1);

    muiAttachUIList(MAIN_UILIST);
  }
  MUIstarted = 1;

  //muiAttachUIList(MAIN_UILIST);
}

/***************************************************************/
/***************************************************************/
void readbutton(muiObject *obj, enum muiReturnValue r)
{
  int i = 0;

  if (obj == rb1)
    i = 1;
  else if (obj == rb2)
    i = 2;
  else if (obj == rb3)
    i = 3;

  printf("radio callback %d, %d\n", i, r);
}

/***************************************************************/
void makeviewui(void)
{
  static muiObject *b3, *b4;
  int xmin, ymin, xmax, ymax;

  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
#if 0
    muiNewUIList(VIEW_UILIST);
#else
    muiSetActiveUIList(VIEW_UILIST);
#endif

    rb1 = muiNewTinyRadioButton(ow+2, oh+dh-20);
    rb2 = muiNewTinyRadioButton(ow+2, oh+dh-40);
    rb3 = muiNewTinyRadioButton(ow+2, oh+dh-60);
    rb4 = muiNewTinyRadioButton(ow+2, oh+dh-80);
    rb5 = muiNewTinyRadioButton(ow+2, oh+dh-100);
    rb6 = muiNewTinyRadioButton(ow+2, oh+dh-120);
    rb7 = muiNewTinyRadioButton(ow+2, oh+dh-140);
    rb8 = muiNewTinyRadioButton(ow+2, oh+dh-160);
    rb9 = muiNewTinyRadioButton(ow+2, oh+dh-180);
    rb10 = muiNewTinyRadioButton(ow+2, oh+dh-200);
    muiLoadButton(rb1, "Ldraw Oblique");
    muiLoadButton(rb2, "Back");
    muiLoadButton(rb3, "Left");
    muiLoadButton(rb4, "Right");
    muiLoadButton(rb5, "Above");
    muiLoadButton(rb6, "Beneath");
    muiLoadButton(rb7, "Oblique");
    muiLoadButton(rb8, "Front");
    muiLoadButton(rb9, "UpsideDown");
    muiLoadButton(rb10, "Natural");
    muiLinkButtons(rb1, rb2);
    muiLinkButtons(rb2, rb3);
    muiLinkButtons(rb3, rb4);
    muiLinkButtons(rb4, rb5);
    muiLinkButtons(rb5, rb6);
    muiLinkButtons(rb6, rb7);
    muiLinkButtons(rb7, rb8);
    muiLinkButtons(rb8, rb9);
    muiLinkButtons(rb9, rb10);
    muiSetCallback(rb1, readbutton);
    muiSetCallback(rb2, readbutton);
    muiSetCallback(rb3, readbutton);
    muiSetCallback(rb4, readbutton);
    muiSetCallback(rb5, readbutton);
    muiSetCallback(rb6, readbutton);
    muiSetCallback(rb7, readbutton);
    muiSetCallback(rb8, readbutton);
    muiSetCallback(rb9, readbutton);
    muiSetCallback(rb10, readbutton);
    muiClearRadio(rb1);

    b3 = muiNewButton(ow+dw-60, ow+dw-34, oh+2, oh+27);
    muiLoadButton(b3, "Ok");
    muiSetCallback(b3, bcallback);

    b4 = muiNewButton(ow+dw-30, ow+dw-4, oh+2, oh+27);
    muiLoadButton(b4, "X");
    muiSetCallback(b4, bcallback);

#if 0
    rb11 = muiNewRadioButton(ow+2, oh+dh-100);
    rb12 = muiNewRadioButton(ow+2, oh+dh-125);
    rb13 = muiNewRadioButton(ow+2, oh+dh-150);
    muiLoadButton(rb11, "Radio1");
    muiLoadButton(rb12, "Radio2");
    muiLoadButton(rb13, "Radio3");
    muiLinkButtons(rb11, rb12);
    muiLinkButtons(rb12, rb13);
    muiSetCallback(rb11, readbutton);
    muiSetCallback(rb12, readbutton);
    muiSetCallback(rb13, readbutton);
    muiClearRadio(rb11);
#endif
  }
  else
  {
    muiSetActiveUIList(VIEW_UILIST);
    muiHideAll(muiGetUIList(b3), 1);
  }
  MUIstarted = 1;

  muiAttachUIList(VIEW_UILIST);
}

/***************************************************************/
void makebackui(void)
{
  static muiObject *b3, *b4;
  int xmin, ymin, xmax, ymax;

  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
#if 0
    muiNewUIList(BACK_UILIST);
#else
    muiSetActiveUIList(BACK_UILIST);
#endif

    rb1 = muiNewTinyRadioButton(ow+2, oh+dh-20);
    rb2 = muiNewTinyRadioButton(ow+2, oh+dh-40);
    rb3 = muiNewTinyRadioButton(ow+2, oh+dh-60);
    rb4 = muiNewTinyRadioButton(ow+2, oh+dh-80);
    rb5 = muiNewTinyRadioButton(ow+2, oh+dh-100);
    rb6 = muiNewTinyRadioButton(ow+2, oh+dh-120);
    rb7 = muiNewTinyRadioButton(ow+2, oh+dh-140);
    rb8 = muiNewTinyRadioButton(ow+2, oh+dh-160);
    rb9 = muiNewTinyRadioButton(ow+dw/2, oh+dh-20);
    rb10 = muiNewTinyRadioButton(ow+dw/2, oh+dh-40);
    rb11 = muiNewTinyRadioButton(ow+dw/2, oh+dh-60);
    rb12 = muiNewTinyRadioButton(ow+dw/2, oh+dh-80);
    rb13 = muiNewTinyRadioButton(ow+dw/2, oh+dh-100);
    rb14 = muiNewTinyRadioButton(ow+dw/2, oh+dh-120);
    rb15 = muiNewTinyRadioButton(ow+dw/2, oh+dh-140);
    rb16 = muiNewTinyRadioButton(ow+dw/2, oh+dh-160);
    muiLoadButton(rb1, "Black");
    muiLoadButton(rb2, "Blue");
    muiLoadButton(rb3, "Green");
    muiLoadButton(rb4, "Dk Cyan");
    muiLoadButton(rb5, "Red");
    muiLoadButton(rb6, "Purple");
    muiLoadButton(rb7, "Brown");
    muiLoadButton(rb8, "Lt Gray");
    muiLoadButton(rb9, "Dk Gray");
    muiLoadButton(rb10, "Lt Blue");
    muiLoadButton(rb11, "Lt Green");
    muiLoadButton(rb12, "Lt Cyan");
    muiLoadButton(rb13, "Lt Red");
    muiLoadButton(rb14, "Pink");
    muiLoadButton(rb15, "Yellow");
    muiLoadButton(rb16, "White");
    muiLinkButtons(rb1, rb2);
    muiLinkButtons(rb2, rb3);
    muiLinkButtons(rb3, rb4);
    muiLinkButtons(rb4, rb5);
    muiLinkButtons(rb5, rb6);
    muiLinkButtons(rb6, rb7);
    muiLinkButtons(rb7, rb8);
    muiLinkButtons(rb8, rb9);
    muiLinkButtons(rb9, rb10);
    muiLinkButtons(rb10, rb11);
    muiLinkButtons(rb11, rb12);
    muiLinkButtons(rb12, rb13);
    muiLinkButtons(rb13, rb14);
    muiLinkButtons(rb14, rb15);
    muiLinkButtons(rb15, rb16);
    muiSetCallback(rb1, readbutton);
    muiSetCallback(rb2, readbutton);
    muiSetCallback(rb3, readbutton);
    muiSetCallback(rb4, readbutton);
    muiSetCallback(rb5, readbutton);
    muiSetCallback(rb6, readbutton);
    muiSetCallback(rb7, readbutton);
    muiSetCallback(rb8, readbutton);
    muiSetCallback(rb9, readbutton);
    muiSetCallback(rb10, readbutton);
    muiSetCallback(rb11, readbutton);
    muiSetCallback(rb12, readbutton);
    muiSetCallback(rb13, readbutton);
    muiSetCallback(rb14, readbutton);
    muiSetCallback(rb15, readbutton);
    muiSetCallback(rb16, readbutton);
    muiClearRadio(rb1);

    b3 = muiNewButton(ow+dw-60, ow+dw-34, oh+2, oh+27);
    muiLoadButton(b3, "Ok");
    muiSetCallback(b3, bcallback);

    b4 = muiNewButton(ow+dw-30, ow+dw-4, oh+2, oh+27);
    muiLoadButton(b4, "X");
    muiSetCallback(b4, bcallback);
  }
  else
  {
    muiSetActiveUIList(BACK_UILIST);
    muiHideAll(muiGetUIList(b3), 1);
  }
  MUIstarted = 1;

  muiAttachUIList(BACK_UILIST);
}

/***************************************************************/
/***************************************************************/
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#ifndef MAXNAMLEN
#define MAXNAMLEN 256
#endif

char	**filelist = NULL;
char	err[80];
char	*dot = ".";
char	*dotdot = "..";
char	directory[300], originaldir[300];
struct stat	d, dd;
struct dirent	*dir;

DIR	*file;
int	off;

int selectedfile = -1;

/***************************************************************/
void errormsg(char *s)
{
    fprintf(stderr, "%s\n", s);
}

/***************************************************************/
void prname(void)
{
	directory[0] = '/';
	if (off == 0)
		off = 1;
	directory[off] = 0;
}

/***************************************************************/
int dirlevels(char *s)
{
    int levels;

    for (levels = 0; *s; s++)
	if (*s == '/')
	    levels++;
    return(levels);
}

/***************************************************************/
int cat(void)
{
	register i, j;
	char *name = directory + 1;	/* I love C */

	i = -1;
	while (dir->d_name[++i] != 0) 
	if ((off+i+2) > MAXNAMLEN - 1) {
		prname();
		return 1;
	}
	for(j=off+1; j>=0; --j)
		name[j+i+1] = name[j];
	off=i+off+1;
	name[i] = '/';
	for(--i; i>=0; --i)
		name[i] = dir->d_name[i];
	return 0;
}

/***************************************************************/
/* get the current working directory (the following routines are from pwd.c) */
int pwd(void)
{
#ifndef UNIX
  getcwd(directory, MAXNAMLEN);
#else
#ifdef AGL
  getcwd(directory, MAXNAMLEN);
#else
  for(off = 0;;) {
    if(stat(dot, &d) < 0) {
      fprintf(stderr, "pwd: cannot stat .!\n");
      return(2);
    }
    if ((file = opendir(dotdot)) == NULL) {
      fprintf(stderr,"pwd: cannot open ..\n");
      return(2);
    }
    if(fstat(file->dd_fd, &dd) < 0) {
      fprintf(stderr, "pwd: cannot stat ..!\n");
      return(2);
      
    }
    if(stat(dotdot, &dd) < 0) {
      fprintf(stderr, "pwd: cannot stat ..!\n");
      return(2);
    }
    if(chdir(dotdot) < 0) {
      fprintf(stderr, "pwd: cannot chdir to ..\n");
      return(2);
    }
    if(d.st_dev == dd.st_dev) {
      if(d.st_ino == dd.st_ino) {
	prname();
	chdir(directory);
	return(0);
      }
      do
	if ((dir = readdir(file)) == NULL) {
	  fprintf(stderr, "pwd: read error in ..\n");
	  return(2);
	}
      while (dir->d_ino != d.st_ino);
    }
    else do {
      if((dir = readdir(file)) == NULL) {
	fprintf(stderr, "pwd: read error in ..\n");
	return(2);
      }
      stat(dir->d_name, &dd);
    } while(dd.st_ino != d.st_ino || dd.st_dev != d.st_dev);
    (void)closedir(file);
    if (cat()) {
      chdir(directory);
      return(0);
    }
  }
#endif
#endif
      return(0);
}

/***************************************************************/
void freels(void)
{
    char **p;

    p = filelist;
    if (!p)
      return;
    while (*p != 0) {
	free(*p);
	*p = 0;
	p++;
    }
}

/***************************************************************/
int mystrcmp(char **s1, char **s2)
{
    return strcmp(*s1,*s2);
}

/***************************************************************/
void ls(void)
{
    DIR			*dirp;
    int			i = 0;
    int			len;
    int                 maxfile = 0;
    struct dirent	*dir;
    struct stat		statbuf;

    if ((dirp = opendir(directory)) == NULL) {
	errormsg("bad directory\n");
	return;
    }
    freels();

    if (filelist)
      free(filelist);
    while ((dir = readdir(dirp)) != NULL)
      maxfile++;
    filelist = (char **) calloc(sizeof(char *), maxfile+1);
    rewinddir(dirp);

    chdir(directory);
    while ((dir = readdir(dirp)) != NULL) {
	if (dir->d_name[0] == '.')
	    continue;
	/*f = open(dir->d_name, O_RDONLY);
	if (!f) 
	    continue;
	if (!okfiletype(getfiletype(f)))
	    continue;
	close(f);*/
	stat(dir->d_name,&statbuf);
	len = strlen(dir->d_name) + 1 + (statbuf.st_mode & S_IFDIR? 1 : 0);
	filelist[i] = (char *)malloc(len);
	strcpy(filelist[i], dir->d_name);
	if (statbuf.st_mode & S_IFDIR) {
	    filelist[i][len-2] = '/'; filelist[i][len-1] = 0;
	}
	i++;
	if (i >= maxfile)
	{
	  // UH OH!  Too many files in directory.
	  break;
	}
    }
    filelist[i] = 0;
    qsort(&filelist[0], i, sizeof (char *), 
	  (int (*)(const void *, const void *))mystrcmp);
    closedir(dirp);
}

/***************************************************************/
// see if its a directory
static int isDir(char *filename)
{
  struct stat st;

  if (stat(filename, &st))
    return 0;

  if (S_ISDIR(st.st_mode))
    return 1;

  return 0;
}

/***************************************************************/
/***************************************************************/
int cd(char *s)
{
  printf("cd %s\n", s);
    if(chdir(s) < 0) {
	fprintf(stderr,"cannot open %s\n",s);
	return -1;
    }
    pwd();
    ls();
    muiSetTLStrings(tl, filelist);
    muiChangeLabel(l4, directory);
    selectedfile = 0;
    muiSetVSValue(vs, 1.0);
    return 0;
}

/***************************************************************/
void writeoutputfile(char *dir, char *file)
{
  extern void loadnewdatfile(char *datpath, char *datfile);
  extern void saveasdatfile(char *datpath, char *datfile);

  printf("D:%s\n", dir);
  if (file)
    printf("F:%s\n", file);
  
  if (dir && file)
  {
    //Got a dir and file.  Do something with it.
    if (fileOpenSave)
      saveasdatfile(dir, file);
    else
      loadnewdatfile(dir, file);
  }
  
  mui_cleanup();
}

/***************************************************************/
void	controltltop(muiObject *obj, enum muiReturnValue value)
{
    float sliderval;

    if ((value != MUI_SLIDER_RETURN) && (value != MUI_SLIDER_THUMB)) return;
    sliderval = muiGetVSVal(obj);
    muiSetTLTop(tl, sliderval);
}

/***************************************************************/
void	handlefileselection(muiObject *obj, enum muiReturnValue value)
{
    char *fname;
    int len;

    if (value == MUI_TEXTLIST_RETURN_CONFIRM) {
	selectedfile = muiGetTLSelectedItem(obj);
	fname = filelist[selectedfile];
	printf("Selected file %s\n", fname);
	len = strlen(fname);
	if (fname[len-1] == '/') {
	    fname[len-1] = 0;
	    cd(fname);
	    return;
	} else {
	    writeoutputfile(directory, fname);
	    return; //exit(0);
	}
    }
    if (value != MUI_TEXTLIST_RETURN) return;
    selectedfile = muiGetTLSelectedItem(obj);
    muiSetVSValue(vs, 1.0);
}

/***************************************************************/
void handleaccept(muiObject *obj, enum muiReturnValue value)
{
    char *fname;
    int len;

    if (value != MUI_BUTTON_PRESS) return;
    fname = muiGetTBString(t1);
    if (!fname || (strlen(fname) == 0))
    {
      if (selectedfile == -1) return;
      fname = filelist[selectedfile];
    }
    len = strlen(fname);
    if (fname[len-1] == '/') {
	fname[len-1] = 0;
	cd(fname);
	return;
    }
    else if (isDir(fname)) {
	cd(fname);
	return;
    } else {
	writeoutputfile(directory, fname);
	return; //exit(0);
    }
    obj = 0;	/* for lint's sake */
}

/***************************************************************/
void handleoriginal(muiObject *obj, enum muiReturnValue value)
{
    if (value != MUI_BUTTON_PRESS) return;
    cd(originaldir);
    obj = 0;	/* for lint's sake */
}

/***************************************************************/
void handleupdir(muiObject *obj, enum muiReturnValue value)
{
    if (value != MUI_BUTTON_PRESS) return;
    cd("..");
    obj = 0;	/* for lint's sake */
}

/***************************************************************/
void handlecancel(muiObject *obj, enum muiReturnValue value)
{
    if (value != MUI_BUTTON_PRESS) return;
    writeoutputfile(directory, 0);
    return; //exit(0);
    obj = 0;	/* for lint's sake */
}

/***************************************************************/
void handletextbox(muiObject *obj, enum muiReturnValue value)
{
    char *s, *slash;

    if (value != MUI_TEXTBOX_RETURN) return;
    s = muiGetTBString(obj);
    if (0 == cd(s)) {
	muiClearTBString(obj);
	return;
    }
    /* hack up the path, if any */
    slash = strrchr(s, '/');
    if (slash == 0) {
	slash = s-1;	/* to make filename == slash+1 */
    } else {
	if (*s == '/') { /* absolute path */
	    strncpy(directory, s, slash-s);
	    directory[slash-s] = 0;
	} else {
	    strcat(directory, "/");
	    strncat(directory, s, slash-s);
	}
    }
     /* now filename == slash+1 */
    writeoutputfile(directory, slash+1);
    return; //exit(0);
}

/***************************************************************/
void makefileui(char *s)
{
  static muiObject *l1, *b1, *b2, *b3, *b4;
  int xmin, ymin, xmax, ymax;

  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
#if 0
    muiNewUIList(FILE_UILIST);
#else
    muiSetActiveUIList(FILE_UILIST);
#endif
    l4 = muiNewLabel(ow+4, oh+dh-18, "./");

    b1 = muiNewButton(ow+dw-60, ow+dw-34, oh+dh-24, oh+dh-2);
    muiLoadButton(b1, "<-");
    muiSetCallback(b1, handleoriginal);

    b2 = muiNewButton(ow+dw-30, ow+dw-4, oh+dh-24, oh+dh-2);
    muiLoadButton(b2, "..");
    muiSetCallback(b2, handleupdir);

    tl = muiNewTextList(ow+4, oh+30, ow+dw-25, 8);
    muiGetObjectSize(tl, &xmin, &ymin, &xmax, &ymax);
    vs = muiNewVSlider(xmax+1, ymin+2, ymax-1, 0, THUMBHEIGHT);
    muiSetVSValue(vs, 1.0);
    //muiSetVSArrowDelta(vs, (ymax-ymin-10-THUMBHEIGHT-ARROWSPACE)/((sizeof strs)/(sizeof (char *))-9));
    muiSetVSArrowDelta(vs, 1);

    //l2 = muiNewBoldLabel(ow+4, oh+8, "File:");
    l1 = muiNewLabel(ow+4, oh+8, "File:");

    t1 = muiNewTextbox(ow+40, ow+dw-64, oh+0);
    muiSetActive(t1, 1);
    muiSetCallback(t1, handletextbox);

    b3 = muiNewButton(ow+dw-60, ow+dw-34, oh+2, oh+27);
    muiLoadButton(b3, "Ok");
    muiSetCallback(b3, handleaccept);

    b4 = muiNewButton(ow+dw-30, ow+dw-4, oh+2, oh+27);
    muiLoadButton(b4, "X");
    muiSetCallback(b4, handlecancel);

    muiSetCallback(vs, controltltop);
    muiSetCallback(tl, handlefileselection);
  }
  else
  {
    muiSetActiveUIList(FILE_UILIST);
    muiHideAll(muiGetUIList(l1), 1);
    muiClearTBString(t1);
  }
  MUIstarted = 1;

  muiAttachUIList(FILE_UILIST);

  muiChangeLabel(l1, s);
  fileOpenSave = strcmp(s, "Open:");

  muiClearTBString(t1);  // Clear out the text box

  strcpy(directory, ".");
  cd(directory);
  strcpy(originaldir, directory);
}

/***************************************************************/
// Glut MUI callbacks.  mui_Reshape must be fixed to not be static.
/***************************************************************/
extern void mui_keyboard(unsigned char c, int x, int y);
extern void mui_mouse(int b, int s, int x, int y);
extern void mui_Reshape(int width, int height);
extern void mui_glutmotion(int x, int y);
extern void mui_glutpassivemotion(int x, int y);
extern void mui_drawgeom(void);
extern void mui_menufunc(int state);

/***************************************************************/
void unMUI_keyboard(unsigned char key, int x, int y)
{
  if (key == 27)
  {
    mui_cleanup();
    return;
  }

  mui_keyboard(key, x, y);
}

/***************************************************************/
void unMUI_fnkeys(int key, int x, int y)
{
  float sliderval;

  if (!vs)
    return;
  if (!muiGetVisible(vs))
    return;

  sliderval = muiGetVSVal(vs);

  printf("sliderval = %g\n", sliderval);

  switch(key) {
  case GLUT_KEY_PAGE_UP:
  case GLUT_KEY_UP:
    muiSetVSValue(vs, sliderval + 1.0);
  case GLUT_KEY_PAGE_DOWN:
  case GLUT_KEY_DOWN:
    muiSetVSValue(vs, sliderval - 1.0);
  case GLUT_KEY_RIGHT:
  case GLUT_KEY_LEFT:
    break;
  }  
}

/***************************************************************/
void unMUI_viewport()
{
  static int MUIstarted = 0;

  // Cannot move the viewport once the widgets are created.
  // Unless we want to move all the widget positions.
  if (MUIstarted == 0)
  {
    ow = (Width-dw)/2;
    oh = (Height-dh)/2;
  }
  else
  {
    int dx = ow;        // Save old offset (x, y)
    int dy = oh;
    
    ow = (Width-dw)/2;
    oh = (Height-dh)/2; // Calculate new offset (x, y)

    dx = ow - dx;       // Find difference in offset (dx, dy)
    dy = oh - dy;

    muiMoveAll(MAIN_UILIST, dx, dy);
    muiMoveAll(FILE_UILIST, dx, dy);
    muiMoveAll(VIEW_UILIST, dx, dy);
    muiMoveAll(DRAW_UILIST, dx, dy);
    muiMoveAll(OPTS_UILIST, dx, dy);
    muiMoveAll(BACK_UILIST, dx, dy);
    muiMoveAll(HELP_UILIST, dx, dy);
  }
  MUIstarted = 1;

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D(0, Width, 0, Height);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glRasterPos2i(0, 0);

  glColor3ub(0, 0, 0);
  glColor4ub(0, 0, 0, 0);
  glBegin(GL_QUADS);
  glVertex2i(ow-1, oh-1);
  glVertex2i(ow+dw+1, oh-1);
  glVertex2i(ow+dw+1, oh+dh+1);
  glVertex2i(ow-1, oh+dh+1);
  glEnd();

  glEnable(GL_SCISSOR_TEST);
  glScissor(ow, oh, dw, dh); // x,y,width,height
  printf("glScissor(%d, %d, %d, %d)\n",ow, oh, dw, dh); // x,y,width,height
}

/***************************************************************/
void unMUI_Reshape(int width, int height)
{
  Width = width;
  Height = height;

  mui_Reshape(width, height); // Sets glViewport();

  // MUI does not know about the SCISSOR 
  // so we need to clear the window then adjust the SCISSOR.
  glDisable(GL_SCISSOR_TEST);
  //muiBackgroundClear();
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  unMUI_viewport();

  glEnable(GL_SCISSOR_TEST);
}

/***************************************************************/
void mui_test()
{
  // Gotta consider using creating and destroying a subwindow for window mode.
  // For game mode I could copy the screen to the back buffer, then
  // use MUI in singlebuffered mode, then copy back to front buffer when done.

  glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
  glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
  glutDetachMenu(GLUT_RIGHT_BUTTON); // Detach menu before starting MUI.

#ifdef SINGLE_BUFFER_MUI
  // Single buffered MUI is flickery.  You see the redraws, back to front.
  glDrawBuffer(GL_FRONT);  // Disable double buffer for MUI.
  mui_singlebuffered = 1;
#else
  glDrawBuffer(GL_BACK);  // Enable double buffer for MUI.
  mui_singlebuffered = 0;

  // Copy the front buffer to the back buffer before enabling scissor.
  // This way MUI will draw on top of the model already in the back buffer.
  // Dont copying if MUI will take over the whole screen, copying is VERY slow.
  glReadBuffer(GL_FRONT); // set pixel source
  glDrawBuffer(GL_BACK); // set pixel destination
  glDisable( GL_DEPTH_TEST ); // Speed up copying
  glDisable(GL_LIGHTING);     // Speed up copying
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D(0, Width, 0, Height);
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glRasterPos2i(0, 0);
  glDepthMask(GL_FALSE); // disable updates to depth buffer
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //enable color buffer updates
  glCopyPixels(0, 0, Width, Height, GL_COLOR);
  glPopMatrix();
  glDepthMask(GL_TRUE); // enable updates to depth buffer
  glDepthFunc(GL_LESS);
  glEnable(GL_LIGHTING);
#endif

  // Setup nice rendering context for MUI.
  glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
  glDisable(GL_LIGHTING);
  // glPolygonMode(GL_FRONT, GL_FILL); 
  // glPolygonMode(GL_BACK, GL_LINE);
  glDisable(GL_STENCIL_TEST);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //enable color buffer updates

  //glViewport(0, 0, Width, Height-40);
  // Scissor is probably easier than viewport
  // Watch out.  Textlist widget uses Scissor in push/popviewport()
  // So we MUST go full window for any gui with a textlist widget.
  unMUI_viewport();

  makemainui();
  muiInit();

  // ---------------------------------------------------------------------
  // After muiInit() we can insert shim fns to intercept events before MUI.
  // ---------------------------------------------------------------------
  // glutKeyboardFunc(mui_keyboard);
  // glutMouseFunc(mui_mouse);
  // glutReshapeFunc(mui_Reshape);
  // glutMotionFunc(mui_glutmotion);
  // glutPassiveMotionFunc(mui_glutpassivemotion);
  // glutDisplayFunc(mui_drawgeom);
  // glutMenuStateFunc(mui_menufunc);
  glutKeyboardFunc(unMUI_keyboard); // Shim ESC key handler before MUI.
  glutReshapeFunc(unMUI_Reshape);
  glutSpecialFunc(unMUI_fnkeys);
  // ---------------------------------------------------------------------

  glutPostRedisplay(); // Otherwise MUI waits for mouse movement to display.
}
#endif

