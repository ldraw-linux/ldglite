//***************************************************************************

  figini.c

  Parse mlcad.ini file into minifig wizard config tables using l3p structs.

//***************************************************************************

typedef unsigned long        COLORREF;

#include <stdio.h>
#include <stdlib.h>

#include "L3Def.h"
#include "math.h"

#define BUFSIZE (1024)
char buf[BUFSIZE];
char description[BUFSIZE];
static char          SubPartDatName[_MAX_PATH];

static char *PartCategories[] = 
{
  "[HATS]",
  "[HEAD]",
  "[NECK]",
  "[BODY]",
  "[BODY2]",
  "[LARM]",
  "[RARM]",
  "[LHAND]",
  "[RHAND]",
  "[LHANDA]",
  "[RHANDA]",
  "[LLEG]",
  "[RLEG]",
  "[LLEGA]",
  "[RLEGA]",
};

struct L3PartS      *CategoryPtr[15];
int                  CategorySize[15];

struct L3PartS       Parts[MAX_PARTS];
int                  nParts = 0;              /* Number of parts in Parts[]      */

//***************************************************************************

// Watch for sentinel entries:  "None"   ""   0   1 0 0 0 1 0 0 0 1  0 0 0 

//***************************************************************************
int figinit(char *inifile)
{
  FILE * fp;
  int i, j, n;
  int category = -1;
  struct L3LineS       Data;
  struct L3PartS      *PartPtr;

  // NOTE:  Should probably look in LDRAWDIR for the ini file.
  char filename[256] = "mlcad.ini"; // Default filename

  if (inifile)
    strcpy(filename, inifile);

  for (j = 0; j < 15; j++)
    CategorySize[j] = 0;

  fp = fopen(filename, "r");
  for(i = 0; ; i++)
  {
    register int len;
    char *p = &buf[0];
    
    if (!fp)
      break;
    if(! fgets(buf, BUFSIZE - 2, fp))
      break;
    if (buf[0] == ';')
    {
      // Skip comment
    }
    else if (buf[0] == '[') // else if (p = strchr(buf, '['))
    {
      for (j = 0; j < 15; j++)
      {
	if (!strncmp(buf, PartCategories[j], strlen(PartCategories[j])))
	{
	  category = j;
	  CategoryPtr[j] = &Parts[nParts];
	  break;
	}
      }
    }
    else if (category >= 0)
    {
      char *token;

      p = buf;
      token = strchr(p, '\"');
      if (!token)
	continue;
      token++;
      p = strchr(token, '\"');
      if (!p)
	continue;
      *p = 0;
      p++;
      strcpy(description, token);

      token = strchr(p, '\"');
      if (!token)
	continue;
      token++;
      p = strchr(token, '\"');
      if (!p)
	continue;
      *p = 0;
      p++;
      strcpy(SubPartDatName, token);
      p += strspn(p, " \t");

      n = sscanf(p, "%d %f %f %f %f %f %f %f %f %f %f %f %f",
                 &Data.LineType,
                 &Data.v[0][0], &Data.v[0][1], &Data.v[0][2],
                 &Data.v[1][0], &Data.v[1][1], &Data.v[1][2],
                 &Data.v[2][0], &Data.v[2][1], &Data.v[2][2],
                 &Data.v[3][0], &Data.v[3][1], &Data.v[3][2]);

      printf(" %s <%s> [%s] %d %g %g %g %g %g %g %g %g %g (%g %g %g)\n",
	     PartCategories[j], description, SubPartDatName, n,
                 Data.v[0][0], Data.v[0][1], Data.v[0][2],
                 Data.v[1][0], Data.v[1][1], Data.v[1][2],
                 Data.v[2][0], Data.v[2][1], Data.v[2][2],
                 Data.v[3][0], Data.v[3][1], Data.v[3][2]);
      if (n != 13)
	continue;

      CategorySize[category]++;

      PartPtr = &Parts[nParts++];
      memset(PartPtr, 0, sizeof(struct L3PartS));  /* Clear all flags           */

      PartPtr->DatName = strdup(SubPartDatName);
      PartPtr->FirstLine = (struct L3LineS *) malloc(sizeof(struct L3LineS));
      memcpy(PartPtr->FirstLine, &Data, sizeof(struct L3LineS));
      PartPtr->FirstLine->Comment = strdup(description);
    }
  }
  
  if (fp)
    fclose(fp);
  
}

#ifdef TESTING
//***************************************************************************
// See what this might look like with a GLUI front end.
//***************************************************************************

void main(int argc, char* argv[])
{



  figinit();




  /****************************************/
  /*   Initialize GLUT and create window  */
  /****************************************/

  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowPosition( 50, 50 );
  glutInitWindowSize( 800, 600 );
 
  main_window = glutCreateWindow( "GLUI Example 5" );
  glutDisplayFunc( myGlutDisplay );
  GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
  GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
  GLUI_Master.set_glutSpecialFunc( NULL );
  GLUI_Master.set_glutMouseFunc( myGlutMouse );
  glutMotionFunc( myGlutMotion );

  /****************************************/
  /*       Set up OpenGL lights           */
  /****************************************/

  glEnable(GL_LIGHTING);
  glEnable( GL_NORMALIZE );

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

  /****************************************/
  /*          Enable z-buferring          */
  /****************************************/

  glEnable(GL_DEPTH_TEST);

  /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/

  printf( "GLUI version: %3.2f\n", GLUI_Master.get_version() );

  /*** Create the side subwindow ***/
  glui = GLUI_Master.create_glui_subwindow( main_window, 
					    GLUI_SUBWINDOW_LEFT );

  obj_panel = glui->add_rollout( "Properties", false );

  /***** Control for object params *****/

  checkbox = 
    glui->add_checkbox_to_panel( obj_panel, "Wireframe", &wireframe, 1, 
				 control_cb );
  spinner  = glui->add_spinner_to_panel( obj_panel, "Segments:",
					 GLUI_SPINNER_INT, &segments);
  spinner->set_int_limits( 3, 60 );
  spinner->set_alignment( GLUI_ALIGN_RIGHT );

  GLUI_Spinner *scale_spinner = 
    glui->add_spinner_to_panel( obj_panel, "Scale:",
				GLUI_SPINNER_FLOAT, &scale);
  scale_spinner->set_float_limits( .2f, 4.0 );
  scale_spinner->set_alignment( GLUI_ALIGN_RIGHT );


  /******** Add some controls for lights ********/

  GLUI_Rollout *roll_lights = glui->add_rollout( "Lights", false );

  GLUI_Panel *light0 = glui->add_panel_to_panel( roll_lights, "Light 1" );
  GLUI_Panel *light1 = glui->add_panel_to_panel( roll_lights, "Light 2" );

  glui->add_checkbox_to_panel( light0, "Enabled", &light0_enabled,
			       LIGHT0_ENABLED_ID, control_cb );
  light0_spinner = 
    glui->add_spinner_to_panel( light0, "Intensity:", GLUI_SPINNER_FLOAT,
				&light0_intensity, LIGHT0_INTENSITY_ID,
				control_cb );
  light0_spinner->set_float_limits( 0.0, 1.0 );

  glui->add_checkbox_to_panel( light1, "Enabled", &light1_enabled,
			       LIGHT1_ENABLED_ID, control_cb );
  light1_spinner = 
    glui->add_spinner_to_panel( light1, "Intensity:", GLUI_SPINNER_FLOAT,
				&light1_intensity, LIGHT1_INTENSITY_ID,
				control_cb );
  light1_spinner->set_float_limits( 0.0, 1.0 );


  /*** Add another rollout ***/
  GLUI_Rollout *options = glui->add_rollout( "Options", true );
  glui->add_checkbox_to_panel( options, "Draw sphere", &show_sphere );
  glui->add_checkbox_to_panel( options, "Draw torus", &show_torus );
  glui->add_checkbox_to_panel( options, "Draw axes", &show_axes );
  glui->add_checkbox_to_panel( options, "Draw text", &show_text );


  /**** Add listbox ****/
  glui->add_statictext( "" );
  GLUI_Listbox *list = glui->add_listbox( "Text:", &curr_string );
  int i;
  for( i=0; i<4; i++ )
    list->add_item( i, string_list[i] );




  /**** Add listbox ****/
  glui->add_statictext( "" );
  GLUI_Listbox *headwear = glui->add_listbox( "HeadWear:", &curr_string );
  for( i=0; i< CategorySize[0]; i++ )
    headwear->add_item( i, CategoryPtr[0][i].FirstLine->Comment );

  /**** Add listbox ****/
  glui->add_statictext( "" );
  GLUI_Listbox *heads = glui->add_listbox( "Head:", &curr_string );
  for( i=0; i< CategorySize[1]; i++ )
    heads->add_item( i, CategoryPtr[1][i].FirstLine->Comment );

  /**** Add listbox ****/
  glui->add_statictext( "" );
  GLUI_Listbox *neckwear = glui->add_listbox( "NeckWear:", &curr_string );
  for( i=0; i< CategorySize[2]; i++ )
    neckwear->add_item( i, CategoryPtr[2][i].FirstLine->Comment );

  /**** Add listbox ****/
  glui->add_statictext( "" );
  GLUI_Listbox *torso = glui->add_listbox( "Torso:", &curr_string );
  for( i=0; i< CategorySize[3]; i++ )
    torso->add_item( i, CategoryPtr[3][i].FirstLine->Comment );





  glui->add_statictext( "" );


  /*** Disable/Enable buttons ***/
  glui->add_button( "Disable movement", DISABLE_ID, control_cb );
  glui->add_button( "Enable movement", ENABLE_ID, control_cb );
  glui->add_button( "Hide", HIDE_ID, control_cb );
  glui->add_button( "Show", SHOW_ID, control_cb );

  glui->add_statictext( "" );

  /****** A 'quit' button *****/
  glui->add_button( "Quit", 0,(GLUI_Update_CB)exit );


  /**** Link windows to GLUI, and register idle callback ******/
  
  glui->set_main_gfx_window( main_window );


#if 0
  /*** Create the bottom subwindow ***/
  glui2 = GLUI_Master.create_glui_subwindow( main_window, 
					     GLUI_SUBWINDOW_BOTTOM );
  glui2->set_main_gfx_window( main_window );

  GLUI_Rotation *view_rot = glui2->add_rotation( "Objects", view_rotate );
  view_rot->set_spin( 1.0 );
  glui2->add_column( false );
  GLUI_Rotation *sph_rot = glui2->add_rotation( "Sphere", sphere_rotate );
  sph_rot->set_spin( .98 );
  glui2->add_column( false );
  GLUI_Rotation *tor_rot = glui2->add_rotation( "Torus", torus_rotate );
  tor_rot->set_spin( .98 );
  glui2->add_column( false );
  GLUI_Rotation *lights_rot = glui2->add_rotation( "Blue Light", lights_rotation );
  lights_rot->set_spin( .82 );
  glui2->add_column( false );
  GLUI_Translation *trans_xy = 
    glui2->add_translation( "Objects XY", GLUI_TRANSLATION_XY, obj_pos );
  trans_xy->set_speed( .005 );
  glui2->add_column( false );
  GLUI_Translation *trans_x = 
    glui2->add_translation( "Objects X", GLUI_TRANSLATION_X, obj_pos );
  trans_x->set_speed( .005 );
  glui2->add_column( false );
  GLUI_Translation *trans_y = 
    glui2->add_translation( "Objects Y", GLUI_TRANSLATION_Y, &obj_pos[1] );
  trans_y->set_speed( .005 );
  glui2->add_column( false );
  GLUI_Translation *trans_z = 
    glui2->add_translation( "Objects Z", GLUI_TRANSLATION_Z, &obj_pos[2] );
  trans_z->set_speed( .005 );
#endif


  /**** We register the idle callback with GLUI, *not* with GLUT ****/
  GLUI_Master.set_glutIdleFunc( myGlutIdle );

  /**** Regular GLUT main loop ****/
  
  glutMainLoop();
}
#endif

