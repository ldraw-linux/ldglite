int glutInit()
{ 
  extern int SetOffScreenRendering();
  extern int OffScreenRendering;

  printf("Warning!  Offscreen rendering only.  Use -MS switch.\n"); 
  OffScreenRendering = SetOffScreenRendering();
  return(0); 
}

int glutGetModifiers()
{ printf("glutGetModifiers\n"); return 0; }
int glutStrokeCharacter()
{ printf("glutStrokeCharacter\n"); return 0; }
int glutBitmapCharacter()
{ printf("glutBitmapCharacter\n"); return 0; }
int glutBitmapHelvetica12()
{ printf("glutBitmapHelvetica12\n"); return 0; }
int glutBitmapWidth()
{ printf("glutBitmapWidth\n"); return 0; }
int glutPostRedisplay()
{ printf("glutPostRedisplay\n"); return 0; }
int glutSwapBuffers()
{ printf("glutSwapBuffers\n"); return 0; }
int glutLayerGet()
{ printf("glutLayerGet\n"); return 0; }
int glutSetWindowTitle()
{ printf("glutSetWindowTitle\n"); return 0; }
int glutLeaveGameMode()
{ printf("glutLeaveGameMode\n"); return 0; }
int glutGetWindow()
{ printf("glutGetWindow\n"); return 0; }
int glutCreateWindow()
{ printf("glutCreateWindow\n"); return 0; }
int glutSetWindow()
{ printf("glutSetWindow\n"); return 0; }
int glutGameModeString()
{ printf("glutGameModeString\n"); return 0; }
int glutGameModeGet()
{ printf("glutGameModeGet\n"); return 0; }
int glutEnterGameMode()
{ printf("glutEnterGameMode\n"); return 0; }
int glutSetCursor()
{ printf("glutSetCursor\n"); return 0; }
int glutWarpPointer()
{ printf("glutWarpPointer\n"); return 0; }
int glutSetMenu()
{ printf("glutSetMenu\n"); return 0; }
int glutChangeToMenuEntry()
{ printf("glutChangeToMenuEntry\n"); return 0; }
int glutGet()
{ printf("glutGet\n"); return 0; }
int glutInitDisplayMode()
{ printf("glutInitDisplayMode\n"); return 0; }
int glutInitWindowSize()
{ printf("glutInitWindowSize\n"); return 0; }
int glutInitWindowPosition()
{ printf("glutInitWindowPosition\n"); return 0; }
int glutFullScreen()
{ printf("glutFullScreen\n"); return 0; }
int glutCreateMenu()
{ printf("glutCreateMenu\n"); return 0; }
int glutAddMenuEntry()
{ printf("glutAddMenuEntry\n"); return 0; }
int glutAddSubMenu()
{ printf("glutAddSubMenu\n"); return 0; }
int glutAttachMenu()
{ printf("glutAttachMenu\n"); return 0; }
int glutMainLoop()
{ printf("glutMainLoop\n"); return 0; }

int glutDisplayFunc()
{ printf("glutDisplayFunc\n"); return 0; }
int glutReshapeFunc()
{ printf("glutReshapeFunc\n"); return 0; }
int glutKeyboardFunc()
{ printf("glutKeyboardFunc\n"); return 0; }
int glutSpecialFunc()
{ printf("glutSpecialFunc\n"); return 0; }
int glutMouseFunc()
{ printf("glutMouseFunc\n"); return 0; }
int glutMotionFunc()
{ printf("glutMotionFunc\n"); return 0; }
int glutIdleFunc()
{ printf("glutIdleFunc\n"); return 0; }
