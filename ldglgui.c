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

#define TEST_MUI_GUI 1
#ifdef  TEST_MUI_GUI
#include <mui/mui.h>

extern int mui_singlebuffered;

extern GLint Width;
extern GLint Height;
extern int  filemenunum;
extern int  dirmenunum;
extern int  mainmenunum;


muiObject *b3;



/***************************************************************/
void bcallback(muiObject *obj, enum muiReturnValue r)
{
  obj = obj; r = r; /* for lint's sake */
  printf("MUI OK callback\n");
  // Gotta clean up mui and go back to whatever.
  // Must free up mui menu stuff.  
  glEnable( GL_DEPTH_TEST );
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
void maketestui(void)
{
  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
    muiNewUIList(1);	/* makes an MUI display list (number 1) */
    b3 = muiNewButton(10, 100, 70, 95);
    muiLoadButton(b3, "OK");
    muiSetCallback(b3, bcallback);
  }
  MUIstarted = 1;

}

/***************************************************************/
void mui_test()
{
  // Gotta consider using creating and destroying a subwindow for window mode.
  // For game mode I could copy the screen to the back buffer, then
  // use MUI in singlebuffered mode, then copy back to front buffer when done.

#if 0
  glDrawBuffer(GL_FRONT);  // Disable double buffer for MUI.
  mui_singlebuffered = 1;
#else
  glDrawBuffer(GL_BACK);  // Enable double buffer for MUI.
  mui_singlebuffered = 0;
#endif
  glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
  glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
  glutDetachMenu(GLUT_RIGHT_BUTTON); // Detach menu before starting MUI.

  maketestui();
  muiInit();
}
#endif

