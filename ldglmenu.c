 /*
 *  LDGLITE, a program for viewing *.dat files.
 *  Copyright (C) 2002  D. Heyse
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
*/

/************************************************************************
 * ldglmenu.c - isolates the glut popup menus for toolkit independence.
 ************************************************************************/

#include "glwinkit.h"
#include "platform.h"
#include "ldliteVR.h"
#include "dirscan.h"

#ifndef TEST_MUI_GUI
#  define USE_GLUT_MENUS 1
#endif

/***************************************************************/
int  filemenunum;
int  dirmenunum;
int  mainmenunum;
int  DateiCount    = 0;
int  FolderCount    = 0;
char DateiListe[MAX_DIR_ENTRIES][NAMELENGTH];
char FolderList[MAX_DIR_ENTRIES][NAMELENGTH];
int  minfilenum    = 0;
char dirpattern[256] = "*";
char filepattern[256] = "*.ldr";
char dirfilepath[256];

void filemenu(int);

/***************************************************************/
extern char progname[256];
extern int EPS_OUTPUT_FIGURED_OUT;

extern void colormenu(int c);

/***************************************************************/
void menu(int item)
{
  if  (item == 1)
  {
    glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
    if (strcmp(filepattern, "*"))
    {
      strcpy(filepattern, "*");
      glutChangeToMenuEntry(3, "Filter - *.ldr     ", 1);
    }
    else
    {
      strcpy(filepattern, "*.ldr");
      glutChangeToMenuEntry(3, "Filter - All Files ", 1);
    }
    filemenu(15); // refresh the file list with files in new dir.
  }
  else
    keyboard((unsigned char)item, 0, 0);
}

/***************************************************************/
void dirmenu(int item)
{
  int         i, j, len1;
  const int   len3 = WORDLENGTH;
  char        myDir[WORDLENGTH+1];
  char filename[256];

  extern char modelspath[];

  if (item == 15) // Real Refresh
    minfilenum = 0;
  if (item == 0) // Nothing (seperator line)
    return;
  if (item == 13) // PgUp
  {
    minfilenum -= MAX_DIR_ENTRIES;
    if (minfilenum < 0)
      minfilenum = 0;
    item = 15;
  }
  if (item == 14) // PgDn
  {
    minfilenum += MAX_DIR_ENTRIES;
    item = 15;
  }
  if (item == 15) // Refresh
  {
    strcpy(myDir, dirfilepath);
    FolderCount = ScanFolder(myDir, dirpattern, minfilenum, FolderList);
    if ((FolderCount == 0) && (minfilenum > 0))
    { // Dont PgDn past the last file in the directory.
      if (ldraw_commandline_opts.debug_level == 1)
	printf("Rescanning from file %d", minfilenum);
      minfilenum -= MAX_DIR_ENTRIES; 
      if (minfilenum < 0)
	minfilenum = 0;
      if (ldraw_commandline_opts.debug_level == 1)
	printf("to %d\n", minfilenum);
      FolderCount = ScanFolder(myDir, dirpattern, minfilenum, FolderList);
    }
    if (ldraw_commandline_opts.debug_level == 1)
      printf ("Found %d folders starting at %d in %s\n", FolderCount,minfilenum, myDir);

    glutSetMenu(dirmenunum);
    for(j = 1; j <= MAX_DIR_ENTRIES; j++) 
      glutChangeToMenuEntry(j, basename(FolderList[j-1]), j);
    glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
  }
  else 
  {
    if (ldraw_commandline_opts.debug_level == 1)
      printf("selected dir %d = %s\n", item, FolderList[item-1]);
    if (item <= FolderCount)
    {
      strcpy(dirfilepath, FolderList[item-1]);
      strcpy(myDir, basename(dirfilepath));
      if (stricmp(myDir, ".") == 0)
	strcpy(dirfilepath, dirname(FolderList[item-1]));
      if (stricmp(myDir, "..") == 0)
      {
	// Fetch the absolute path to get to ".\.."
	if (stricmp(dirname(FolderList[item-1]), ".") == 0)
	{
	  getcwd(myDir, WORDLENGTH);
	  strcpy(dirfilepath, dirname(myDir));
	}
	else
	  strcpy(dirfilepath, dirname(dirname(FolderList[item-1])));
	if (ldraw_commandline_opts.debug_level == 1)
	  printf("Now using dir = %s\n", dirfilepath);
      }
      dirmenu(15); // refresh the folder list with folders in new dir.
      filemenu(15); // refresh the file list with files in new dir.
    }
  }
}

/***************************************************************/
void filemenu(int item)
{
  int         i, j, len1;
  const int   len3 = WORDLENGTH;
  char        myDir[WORDLENGTH+1];
  char filename[256];

  extern char modelspath[];

  if (item == 15) // Real Refresh
    minfilenum = 0;
  if (item == 0) // Nothing (seperator line)
    return;
  if (item == 13) // PgUp
  {
    minfilenum -= MAX_DIR_ENTRIES;
    if (minfilenum < 0)
      minfilenum = 0;
    item = 15;
  }
  if (item == 14) // PgDn
  {
    minfilenum += MAX_DIR_ENTRIES;
    item = 15;
  }
  if (item == 15) // Refresh
  {
    strcpy(myDir, dirfilepath);
    DateiCount = ScanDirectory(myDir, filepattern, minfilenum, DateiListe);
    if ((DateiCount == 0) && (minfilenum > 0))
    { // Dont PgDn past the last file in the directory.
      if (ldraw_commandline_opts.debug_level == 1)
	printf("Rescanning from file %d", minfilenum);
      minfilenum -= MAX_DIR_ENTRIES; 
      if (minfilenum < 0)
	minfilenum = 0;
      if (ldraw_commandline_opts.debug_level == 1)
	printf("to %d\n", minfilenum);
      DateiCount = ScanDirectory(myDir, filepattern, minfilenum, DateiListe);
    }
    if (ldraw_commandline_opts.debug_level == 1)
      printf ("Found %d files starting at %d in %s\n", DateiCount,minfilenum, myDir);

    glutSetMenu(filemenunum);
    for(j = 1; j <= MAX_DIR_ENTRIES; j++) 
      glutChangeToMenuEntry(j, basename(DateiListe[j-1]), j);
    glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
  }
  else 
  {
    if (ldraw_commandline_opts.debug_level == 1)
      printf("selected file %d = %s\n", item, DateiListe[item-1]);
    if (item <= DateiCount)
      loadnewdatfile(dirname(DateiListe[item-1]),basename(DateiListe[item-1]));
  }
}

/***************************************************************/
void initializeMenus(void)
{
#ifdef USE_GLUT_MENUS
  int opts, view, colors, helpmenunum;

#ifndef AGL
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
// Rats, no menus in game mode.  Perhaps GLUI or PUI look good again.
  if (ldraw_commandline_opts.V_x >= -1)
#endif
#endif
  {
  view = glutCreateMenu(menu);
  glutAddMenuEntry("Ldraw Oblique   ", '0');
  glutAddMenuEntry("Back            ", '1');
  glutAddMenuEntry("Left            ", '2');
  glutAddMenuEntry("Right           ", '3');
  glutAddMenuEntry("Above           ", '4');
  glutAddMenuEntry("Beneath         ", '5');
  glutAddMenuEntry("Oblique         ", '6');
  glutAddMenuEntry("Front           ", '7');
  glutAddMenuEntry("UpsideDown      ", '8');
  glutAddMenuEntry("Natural         ", '9');

  opts = glutCreateMenu(menu);
  glutAddMenuEntry("OrthoGraphic    ", 'j');
  glutAddMenuEntry("Perspective     ", 'J');
  glutAddMenuEntry("                ", '\0');
  glutAddMenuEntry("Shading         ", 'h');
  glutAddMenuEntry("Linemode        ", 'l');
  glutAddMenuEntry("Normal          ", 'n');
  glutAddMenuEntry("Studs           ", 'f');
  glutAddMenuEntry("Quality Lines   ", 'q');
  glutAddMenuEntry("Visible spin    ", 'v');
  glutAddMenuEntry("Step-Continuous ", 's');
  glutAddMenuEntry("Polling         ", 'g');
#ifdef USE_L3_PARSER
  glutAddMenuEntry("Parser          ", 'r');
#endif
  glutAddMenuEntry("                ", '\0');
  glutAddMenuEntry("Zoom out        ", 2);
  glutAddMenuEntry("Zoom in         ", 3);
  glutAddMenuEntry("Scale dn        ", '-');
  glutAddMenuEntry("Scale up        ", '+');

  colors = glutCreateMenu(colormenu);
  glutAddMenuEntry("Black              ", 0);
  glutAddMenuEntry("Blue               ", 1);
  glutAddMenuEntry("Green              ", 2);
  glutAddMenuEntry("Dk Cyan            ", 3);
  glutAddMenuEntry("Red                ", 4);
  glutAddMenuEntry("Purple             ", 5);
  glutAddMenuEntry("Brown              ", 6);
  glutAddMenuEntry("Lt Gray            ", 7);
  glutAddMenuEntry("Dk Gray            ", 8);
  glutAddMenuEntry("Lt Blue            ", 9);
  glutAddMenuEntry("Lt Green           ", 10);
  glutAddMenuEntry("Lt Cyan            ", 11);
  glutAddMenuEntry("Lt Red             ", 12);
  glutAddMenuEntry("Pink               ", 13);
  glutAddMenuEntry("Yellow             ", 14);
  glutAddMenuEntry("White              ", 15);

  dirmenunum = glutCreateMenu(dirmenu);
  glutAddMenuEntry("                   ", 1);
  glutAddMenuEntry("                   ", 2);
  glutAddMenuEntry("                   ", 3);
  glutAddMenuEntry("                   ", 4);
  glutAddMenuEntry("                   ", 5);
  glutAddMenuEntry("                   ", 6);
  glutAddMenuEntry("                   ", 7);
  glutAddMenuEntry("                   ", 8);
  glutAddMenuEntry("                   ", 9);
  glutAddMenuEntry("                   ", 10);
  glutAddMenuEntry("                   ", 0);
  glutAddMenuEntry("Page Up            ", 13);
  glutAddMenuEntry("Page Dn            ", 14);

  filemenunum = glutCreateMenu(filemenu);
  glutAddMenuEntry("                   ", 1);
  glutAddMenuEntry("                   ", 2);
  glutAddMenuEntry("                   ", 3);
  glutAddMenuEntry("                   ", 4);
  glutAddMenuEntry("                   ", 5);
  glutAddMenuEntry("                   ", 6);
  glutAddMenuEntry("                   ", 7);
  glutAddMenuEntry("                   ", 8);
  glutAddMenuEntry("                   ", 9);
  glutAddMenuEntry("                   ", 10);
  glutAddMenuEntry("                   ", 0);
  glutAddMenuEntry("Page Up            ", 13);
  glutAddMenuEntry("Page Dn            ", 14);

  helpmenunum = glutCreateMenu(menu);
  glutAddMenuEntry(progname             , '\0');
  glutAddMenuEntry("Version 0.9.7      ", '\0');

  mainmenunum = glutCreateMenu(menu);
  glutAddSubMenu(  "File               ", filemenunum);
  glutAddSubMenu(  "Folder             ", dirmenunum);
  glutAddMenuEntry("Filter - All Files ", 1);
  glutAddMenuEntry("                   ", '\0');
  glutAddSubMenu(  "View               ", view);
  glutAddSubMenu(  "Options            ", opts);
  glutAddSubMenu(  "BackGround Color   ", colors);
  glutAddMenuEntry("                   ", '\0');
  glutAddMenuEntry("Bitmap             ", 'B');
  if (EPS_OUTPUT_FIGURED_OUT)
  {
  glutAddMenuEntry("EPS file (sorted)  ", 4);
  glutAddMenuEntry("EPS file (UNsorted)", 5);
  glutAddMenuEntry("EPS debug          ", 6);
  }
  glutAddMenuEntry("                   ", '\0');
  glutAddSubMenu(  "Help               ", helpmenunum);
  glutAddMenuEntry("Quit               ", '\033');
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  view = glutCreateMenu(menu);
  glutAddMenuEntry("Ldraw Oblique   ", '0');

  // Read in the current directories dat filenames.
  dirmenu(15);
  filemenu(15);
  }
#endif /* TEST_MUI_GUI */

}
