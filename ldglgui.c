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
void maketestui(void)
{
  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
    muiNewUIList(1);	/* makes an MUI display list (number 1) */
    b3 = muiNewButton(10, 100, Height-30, (Height-30)+25);
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

  // Copy the front buffer to the back buffer befor enabling scissor.
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

  //glViewport(0, 0, Width, Height-40);
  // Scissor is probably easier than viewport
  // Watch out.  Textlist widget uses Scissor in push/popviewport()
  // So we MUST go full window for any gui with a textlist widget.
  glEnable(GL_SCISSOR_TEST);
  glScissor(0, Height-100, Width, 100); // x,y,width,height

  maketestui();
  muiInit();
}
#endif

