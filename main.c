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
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h> // for polling datfile for updates.

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

#include "dirscan.h"

#ifdef USE_L3_PARSER
extern void          LoadModelPre(void);
extern int           LoadModel(const char *lpszPathName);
extern void          LoadModelPost(void);
extern void          DrawModel(void);
#endif

extern void ldlite_parse(char *filename, char *ldraw_lines);

extern char pathname[256];
extern char primitivepath[256];
extern char partspath[256];
extern char modelspath[256];
extern char datfilepath[256];

#ifdef USE_L3_PARSER
#define UNKNOWN_PARSER -1
#define LDLITE_PARSER 0
#define L3_PARSER     1

int parsername = UNKNOWN_PARSER;
#endif

char datfilename[256];
char title[256];

char buf[10240];
int use_uppercase = 0;

#define IMAGE_TYPE_PNG_RGB 1
#define IMAGE_TYPE_PNG_RGBA 2
#define IMAGE_TYPE_BMP8 3
#define IMAGE_TYPE_BMP 4

int ldraw_projection_type = 0;  // 1 = perspective, 0 = orthographic.
int ldraw_image_type = IMAGE_TYPE_BMP8;

// [Views] swiped from ldraw.ini
// Modified some views to be orthogonal.
char Back[] = "-1,0,0,0,1,0,0,0,-1";
char Left[] = "0,0,1,0,1,0,-1,0,0";
char Right[] = "0,0,-1,0,1,0,1,0,0";
char Above[] = "0,0,1,1,0,0,0,1,0";
char Beneath[] = "0,0,1,-1,0,0,0,-1,0";
char LdrawOblique[] = "1,0,1,0.5,1,-0.5,-1,0,1";
char Oblique[] = "0.707104,0,0.707104,0.353553,0.866025,-0.353553,-0.612372,0.5,0.612372";
char Front[] = "1,0,0,0,1,0,0,0,1";
//char UpsideDown[] = "-1,0,1,-0.5,-1,-0.5,1,0,0";
char UpsideDown[] = "0.707104,0,0.707104,-0.353553,-0.866025,0.353553,0.612372,-0.5,-0.612372";
//char Natural[] = "0.625,0,1.075,0.5375,1.25,-0.3125,-1.25,0,2.5";
char Natural[] = "0.5,0,0.866025,0.433013,0.866025,-0.25,-0.75,0.5,0.433013";
char *m_viewMatrix = LdrawOblique;

extern int glCurColorIndex;
extern float z_line_offset; 

extern ZIMAGE z;

GLint Width = 640;
GLint Height = 480;
int main_window = -1;

double twirl_angle = 0.0;
double twirl_increment = 10.0;

static int list_made = 0;

// Drawing modes
#define NORMAL_MODE 	0x0000
#define STUDLESS_MODE 	0x0001
#define WIREFRAME_MODE 	0x0002
#define SHADED_MODE 	0x0004
#define BBOX_MODE 	0x0008
#define INVISIBLE_MODE 	0x0010

#define USE_DOUBLE_BUFFER

int use_quads = 0;
int curstep = 0;
int cropping = 1;
int panning = 0;
int not_stepping = 0;
GLdouble pan_start_x = 0.0;
GLdouble pan_start_y = 0.0;
GLdouble pan_end_x = 0.0;
GLdouble pan_end_y = 0.0;
int pan_start_zWire;
int pan_start_F;
int pan_visible = BBOX_MODE | WIREFRAME_MODE; // Bounding Box spin mode.
int glutModifiers;

//#define USE_QUATERNION 1
#ifdef USE_QUATERNION
float qspin[4] = {0.0, 0.0, 1.0, 0.0};
#endif

int  filemenunum;
int  dirmenunum;
int  mainmenunum;
int  DateiCount    = 0;
int  FolderCount    = 0;
char DateiListe[MAX_DIR_ENTRIES][NAMELENGTH];
char FolderList[MAX_DIR_ENTRIES][NAMELENGTH];
int  minfilenum    = 0;
char progname[256];
char dirfilepath[256];
char dirpattern[256] = "*";
char filepattern[256] = "*.dat";

int drawAxis = 0;

// Camera movement variables
#define MOVE_SPEED 10.0
#define PI 3.1415927
#define PI_180 (PI/180.0)
float fCamX = 0.0;
float fCamY = 0.0;
float fCamZ = 0.0;

float fXRot = 0.0;
float fYRot = 0.0;
float fZRot = 0.0;

/***************************************************************/
void draw_string(void *font, const char* string) 
{
  while(*string)
    glutStrokeCharacter(font, *string++);
}

/***************************************************************/
void draw_string_bitmap(void *font, const char* string) 
{
  while (*string)
    glutBitmapCharacter(font, *string++);
}

/***************************************************************/
/**
 ** use glut to display a string of characters using a raster font:
 **/
/***************************************************************/
void
DoRasterString( float x, float y, char *s )
{
  char c;			/* one character to print		*/
  
  glRasterPos2f( x, y );
  for( ; ( c = *s ) != '\0'; s++ )
  {
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, c );
  }
}

/***************************************************************/
#define BYTE1(i) ((unsigned char) (i & 0x0ff))
#define BYTE2(i) ((unsigned char) ((i / 0x100) & 0x0ff))
#define BYTE3(i) ((unsigned char) ((i / 0x10000) & 0x0ff))
#define BYTE4(i) ((unsigned char) ((i / 0x1000000) & 0x0ff))

/***************************************************************/
void write_bmp(char *filename)
{
  int i, j;
  BITMAPFILEHEADER bmfh;
  LPBITMAPINFOHEADER bmh;
  char *p, c;
  FILE *fp;

  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;

  if (cropping)
  {
    xoff = max(0, z.extent_x1);
    yoff = max(0, z.extent_y1);
    width = min((z.extent_x2 - xoff), (Width - xoff));
    height = min((z.extent_y2 + 1 - yoff), (Height - yoff));
    //width = ((width + 31)/32) * 32; // round to a multiple of 32.
    width = ((width + 3)/4) * 4; // round to a multiple of 4.
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bmpsize = (%d, %d) at (%d, %d)\n", width, height, xoff, yoff);
    if ((width <= 0) || (height <= 0)) return;
  }
  
  printf("Write BMP %s\n", filename);
  if ((fp = fopen(filename,"wb+"))==NULL) {
    printf("Could not open %s\n", filename);
    return;
  }
  
  
  bmh = (LPBITMAPINFOHEADER) z.dib;
  bmh->biSize = 40;
  bmh->biWidth = width;
  bmh->biHeight = height;
  bmh->biPlanes = 1;
  //bmh->biBitCount = 16;
  bmh->biBitCount = 24;
  bmh->biSizeImage = (bmh->biWidth*bmh->biBitCount+31)/32*4
    * (bmh->biHeight>0?bmh->biHeight:-bmh->biHeight);

  p = (char *) &(bmfh.bfType);
  p[0] = 'B';
  p[1] = 'M';
  bmfh.bfReserved1 = 0;
  bmfh.bfReserved2 = 0;
  bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  bmfh.bfSize = (bmfh.bfOffBits + bmh->biSizeImage) / 4;

  bmh->biSizeImage = 0;

#if 0
  // 14 bytes
  fwrite(&bmfh.bfType, 2, 1, fp);
  fwrite(&bmfh.bfSize, 4, 1, fp);
  fwrite(&bmfh.bfReserved1, 2, 1, fp);
  fwrite(&bmfh.bfReserved2, 2, 1, fp);
  fwrite(&bmfh.bfOffBits, 4, 1, fp);
  // 40 bytes
  fwrite(bmh, sizeof(BITMAPINFOHEADER), 1, fp);
#else  
  // Copy the 54 bytes of bmfh and bmh into buf in intel byte order & packing.
  memset(buf, 0, 54);
  buf[0] = 'B';
  buf[1] = 'M';
  buf[2] = BYTE1(bmfh.bfSize);
  buf[3] = BYTE2(bmfh.bfSize);
  buf[4] = BYTE3(bmfh.bfSize);
  buf[5] = BYTE4(bmfh.bfSize);
  buf[10] = BYTE1(bmfh.bfOffBits);
  buf[11] = BYTE2(bmfh.bfOffBits);
  buf[12] = BYTE3(bmfh.bfOffBits);
  buf[13] = BYTE4(bmfh.bfOffBits);

  buf[14] = 40;
  buf[15] = 0;
  buf[16] = 0;
  buf[17] = 0;
  buf[18] = BYTE1(bmh->biWidth);
  buf[19] = BYTE2(bmh->biWidth);
  buf[20] = BYTE3(bmh->biWidth);
  buf[21] = BYTE4(bmh->biWidth);
  buf[22] = BYTE1(bmh->biHeight);
  buf[23] = BYTE2(bmh->biHeight);
  buf[24] = BYTE3(bmh->biHeight);
  buf[25] = BYTE4(bmh->biHeight);
  buf[26] = 1;
  buf[27] = 0;
  buf[28] = 24;
  buf[29] = 0;
  // The rest are all zeros.
  fwrite(buf, 54, 1, fp);
#endif

  // no pallete since we use RGB

  glReadBuffer(GL_FRONT);
  for (i = 0; i < height; i++)
  {
    glReadPixels(xoff, i+yoff, width, 1, GL_RGB, GL_UNSIGNED_BYTE, buf);
    p = buf;
    for (j = 0; j < width; j++) // RBG -> BRG
    {
      c = p[0];
      p[0] = p[2];
      p[2] = c;
      p+=3;
    }
    fwrite(buf, width*3, 1, fp);
  }
  fclose(fp);
}

#ifdef USE_PNG
/***************************************************************/
#include "png.h"

/***************************************************************/
static void png_error_fn(png_structp png_ptr, const char *err_msg)
{
	jmp_buf *j;
	j= (jmp_buf*) png_get_error_ptr(png_ptr);
	longjmp(*j, -1);
}

static void png_warning_fn(png_structp png_ptr, const char *warn_msg)
{
	return;
}

#define USE_PNG_ALPHA 1
/***************************************************************/
void write_png(char *filename)
{
  int i, j;
  char *p;

  png_structp png_ptr;
  png_infop info_ptr;
  jmp_buf jbuf;
  png_text text_ptr[1];
#ifdef USE_PNG_ALPHA
  png_color_16 background;
#endif
  FILE *fp;

  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;

  if (cropping)
  {
    xoff = max(0, z.extent_x1);
    yoff = max(0, z.extent_y1);
    width = min((z.extent_x2 - xoff), (Width - xoff));
    height = min((z.extent_y2 + 1 - yoff), (Height - yoff));
    width = ((width + 3)/4) * 4; // round to a multiple of 4.
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bmpsize = (%d, %d) at (%d, %d)\n", width, height, xoff, yoff);
    if ((width <= 0) || (height <= 0)) return;
  }
  
  if ((p = strrchr(filename, '.')) != NULL)
    *p = 0;
  strcat(filename, use_uppercase ? ".PNG" : ".png");

  printf("Write PNG %s\n", filename);
  
  // Write header stuff
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (void*)(&jbuf),
				    png_error_fn, png_warning_fn);
  if (!png_ptr) 
  {
    printf("No Memory", filename);
    return;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    printf("No Memory", filename);
    return;
  }

  if(setjmp(jbuf)) {
    // we'll get here if an error occurred in any of the following
    // png_ functions
    printf("Aborting PNG file %s", filename);
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    if(fp) fclose(fp);
    return;
  }

  if ((fp = fopen(filename,"wb+"))==NULL) {
    printf("Could not open %s\n", filename);
    return;
  }
  
  png_init_io(png_ptr, fp);

#ifdef USE_PNG_ALPHA
  // Set the default background for transparent image to white.
  // I should just store the current background color in a global and use it.
  // I should also make this a menu option/command line opt.
  // Perhaps ctl-p for the hot key.  
 // Perhaps I need to create 4 image types.  bmp24, bmp8, png, png-alpha
  // and menu/hotkey/cmdline support for cropping modifier.
  background.red = 0xffff;
  background.green = 0xffff;
  background.blue = 0xffff;

  png_set_IHDR(png_ptr, info_ptr, width, height, 8, 
	       PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_set_bKGD(png_ptr, info_ptr, &background);

#else
  png_set_IHDR(png_ptr, info_ptr, width, height, 8, 
	       PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
#endif

  // png_set_bgr(png_ptr);
  // png_set_pHYs(png_ptr, info_ptr, resolution_x, resolution_y, 1);

  png_write_info(png_ptr, info_ptr);
  
  glReadBuffer(GL_FRONT);
  // Write image rows
  //png_write_image(png_ptr, row_pointers);
  for (i = height-1; i >= 0; i--)
  {
#ifdef USE_PNG_ALPHA
    glReadPixels(xoff, i+yoff, width, 1, GL_RGBA, GL_UNSIGNED_BYTE, buf);
#else
    glReadPixels(xoff, i+yoff, width, 1, GL_RGB, GL_UNSIGNED_BYTE, buf);
#endif
    png_write_row(png_ptr, buf);
  }

  text_ptr[0].key = "Software";
  text_ptr[0].text = "LdGLite";
  text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
  png_set_text(png_ptr, info_ptr, text_ptr, 1);
  
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);
}
#endif

#ifdef WINDOWS
#define USE_BMP8 1
#else
#define USE_BMP8 1
#endif

#ifdef USE_BMP8
/***************************************************************/
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define BMP_PIXELSIZE(width, height, bits) (((((width) * (bits) + 31)/32) * 4) * height)
#define BMP_HEADERSIZE (3 * 2 + 4 * 12)

/***************************************************************/
// Writes a bmp to a file
// added by LZ
static BOOL SaveBMP8(char* fileName, BYTE* colormappedbuffer, UINT width, UINT height, int colors, RGBQUAD *colormap)
{
  int datasize, cmapsize, byteswritten, row, col;
  long filesize;
  int res1, res2;
  long pixeloffset;
  int bmisize;
  long cols;
  long rows;
  WORD planes;
  long compression;
  long cmpsize;
  long xscale;
  long yscale;
  long impcolors;
  FILE *fp;
  char bm[2];
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmh;
  int i;
  int pixbuf;
  int nbits = 0;
  int offset;	// offset into our color-mapped RGB buffer
  BYTE pval;


  cmapsize = colors * 4;
  datasize = BMP_PIXELSIZE(width, height, 8);
  filesize = BMP_HEADERSIZE + cmapsize + datasize;
  res1 = res2 = 0;
  pixeloffset = BMP_HEADERSIZE + cmapsize;
  bmisize = 40;
  cols = width;
  rows = height;
  planes = 1;
  compression =0;
  cmpsize = datasize;
  xscale = 0;
  yscale = 0;
  impcolors = colors;

  fp = fopen(fileName, "wb+");
  if (fp == NULL) 
    return FALSE;
  
  bm[0]='B';
  bm[1]='M';

  // header stuff
  bmfh.bfType=*(WORD *)&bm; 
  bmfh.bfSize= filesize; 
  bmfh.bfReserved1=0; 
  bmfh.bfReserved2=0; 
  bmfh.bfOffBits=pixeloffset; 

  bmh.biSize = bmisize; 
  bmh.biWidth = cols; 
  bmh.biHeight = rows; 
  bmh.biPlanes = planes; 
  bmh.biBitCount = 8;
  bmh.biCompression = compression; 
  bmh.biSizeImage = cmpsize; 
  bmh.biXPelsPerMeter = xscale; 
  bmh.biYPelsPerMeter = yscale; 
  bmh.biClrUsed = colors;
  bmh.biClrImportant = impcolors;

#if 0	
  fwrite(&bmfh, sizeof (BITMAPFILEHEADER), 1, fp);
  fwrite(&bmh, sizeof (BITMAPINFOHEADER), 1, fp);
#else
  // Copy the 54 bytes of bmfh and bmh into buf in intel byte order & packing.
  memset(buf, 0, 54);
  buf[0] = 'B';
  buf[1] = 'M';
  buf[2] = BYTE1(bmfh.bfSize);
  buf[3] = BYTE2(bmfh.bfSize);
  buf[4] = BYTE3(bmfh.bfSize);
  buf[5] = BYTE4(bmfh.bfSize);
  buf[10] = BYTE1(bmfh.bfOffBits);
  buf[11] = BYTE2(bmfh.bfOffBits);
  buf[12] = BYTE3(bmfh.bfOffBits);
  buf[13] = BYTE4(bmfh.bfOffBits);

  buf[14] = 40;
  buf[15] = 0;
  buf[16] = 0;
  buf[17] = 0;
  buf[18] = BYTE1(bmh.biWidth);
  buf[19] = BYTE2(bmh.biWidth);
  buf[20] = BYTE3(bmh.biWidth);
  buf[21] = BYTE4(bmh.biWidth);
  buf[22] = BYTE1(bmh.biHeight);
  buf[23] = BYTE2(bmh.biHeight);
  buf[24] = BYTE3(bmh.biHeight);
  buf[25] = BYTE4(bmh.biHeight);
  buf[26] = 1;
  buf[27] = 0;
  buf[28] = 8;
  buf[29] = 0;
  buf[46] = BYTE1(bmh.biClrUsed);
  buf[47] = BYTE2(bmh.biClrUsed);
  buf[48] = BYTE3(bmh.biClrUsed);
  buf[49] = BYTE4(bmh.biClrUsed);
  buf[50] = BYTE1(bmh.biClrImportant);
  buf[51] = BYTE2(bmh.biClrImportant);
  buf[52] = BYTE3(bmh.biClrImportant);
  buf[53] = BYTE4(bmh.biClrImportant);
  // The rest are all zeros.
  fwrite(buf, 54, 1, fp);
#endif

  if (cmapsize) 
  {
    for (i = 0; i< colors; i++) 
    {
      putc(colormap[i].rgbRed, fp);
      putc(colormap[i].rgbGreen, fp);
      putc(colormap[i].rgbBlue, fp);
      putc(0, fp);	// dummy
    }
  }

  byteswritten = BMP_HEADERSIZE + cmapsize;

  for (row = 0; row< (int)height; row++) 
  {
    nbits = 0;
    for (col =0 ; col < (int)width; col++) 
    {
      offset = row * width + col; // offset into our color-mapped RGB buffer
      pval = *(colormappedbuffer + offset);
      pixbuf = (pixbuf << 8) | pval;
      nbits += 8;
      
      if (nbits > 8) 
      {
	fclose(fp);
	return FALSE;
      }
      
      if (nbits == 8) 
      {
	putc(pixbuf, fp);
	pixbuf=0;
	nbits=0;
	byteswritten++;
      }
    } // cols
    
    if (nbits > 0) 
    {
      putc(pixbuf, fp);		// write partially filled byte
      byteswritten++;
    }
    
    // DWORD align
    while ((byteswritten -pixeloffset) & 3) {
      putc(0, fp);
      byteswritten++;
    }
    
  }	//rows
  
  fclose(fp);
  return TRUE;
}

/***************************************************************/
// color reduction (Don't remember who wrote it so I can't give proper credit)
#include "quant.h"

/***************************************************************/
void write_bmp8(char *filename)
{
  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;
  BYTE* data;
  RGBQUAD RGBpal[256];
  BYTE tmpPal[3][256];
  BYTE *colormappedBuffer;
  UINT col;
  
  if (cropping)
  {
    xoff = max(0, z.extent_x1);
    yoff = max(0, z.extent_y1);
    width = min((z.extent_x2 - xoff), (Width - xoff));
    height = min((z.extent_y2 + 1 - yoff), (Height - yoff));
    //width = ((width + 31)/32) * 32; // round to a multiple of 32.
    width = ((width + 3)/4) * 4; // round to a multiple of 4.
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bmpsize = (%d, %d) at (%d, %d)\n", width, height, xoff, yoff);
    if ((width <= 0) || (height <= 0)) return;
  }
  
  colormappedBuffer = (BYTE*) malloc (width*height);
  data = (BYTE*)malloc(width*height*3);

  printf("Write BMP8 %s\n", filename);
  
  glReadBuffer(GL_FRONT);
  glReadPixels(xoff, yoff, (GLint)width, (GLint)height, GL_RGB, GL_UNSIGNED_BYTE, data);

  dl1quant(data, colormappedBuffer, width, height, 256, TRUE, tmpPal); 

  for (col = 0; col < 256; col++) 
  {
    RGBpal[col].rgbRed=tmpPal[2][col];
    RGBpal[col].rgbGreen=tmpPal[1][col];
    RGBpal[col].rgbBlue=tmpPal[0][col];
  }

  SaveBMP8(filename, colormappedBuffer, width, height, 256, &RGBpal[0]);
  
  free (colormappedBuffer);

  free(data);
}
#endif

void reshape(int width, int height);

/***************************************************************/
int platform_step_comment(char *comment_string)
{
  printf("%s\n", comment_string);

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0., 100., 0., 100. ); /* "percent units" */
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glColor4f( 0.3, 0.3, 0.3, 0.5 );		/* grey  	*/
  glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
  DoRasterString( 1.0, 1.0, comment_string );
  glEnable( GL_DEPTH_TEST ); 
  glPopMatrix();

  // Reset the projection matrix.
  reshape(Width, Height);
}

/***************************************************************/
void platform_step(int step, int level, int pause, ZIMAGE *zp)
{
  char filename[256];
  char filepath[256];
  char filenum[32];
  char *dotptr;

  if (ldraw_commandline_opts.debug_level == 1)
    printf("platform_step(%d, %d, %d)\n", step, level, pause);

  // This is probably a great place to handle begin & end display lists.
  if (step == INT_MAX) {
    // end of top-level dat file reached
    //platform_step_comment(buf)
    if (ldraw_commandline_opts.debug_level == 1)
      printf("Finished\n");
  } 
  else 	if (step >= 0) {
    //platform_step_comment(buf)
  } 
  else {
    // step == -1 means a redraw partway through a step
  }
  if ((ldraw_commandline_opts.M != 'C')||(step==INT_MAX)||(step== -1)) { 
  }
  if ((step >= 0 ) && (ldraw_commandline_opts.M == 'S')) {
    // if something has been drawn, save bitmap
    if ((step == INT_MAX) && (pause == 0)) {
      // do nothing
    } 
    else {
      // save bitmap
      concat_path(pathname, use_uppercase ? "BITMAP" : "bitmap", filepath);
      if (ldraw_commandline_opts.rotate == 1) 
      {
	sprintf(filenum,"rot_%0dd",(int)twirl_angle);
	concat_path(filepath, filenum, filename);
#ifdef WINDOWS
	mkdir(filename);
#else
	mkdir(filename,0755);
#endif
	strcpy(filepath, filename);
      }
      concat_path(filepath, datfilename, filename);
      if ((dotptr = strrchr(filename, '.')) != NULL)
	*dotptr = 0;
      if (step != INT_MAX)
      {
	sprintf(filenum,"%0d",step+1);
	strcat(filename,filenum);
      }
      strcat(filename, use_uppercase ? ".BMP" : ".bmp");

//*************************************************************************
//NOTE: Write a fn to calc zp->extents_* by checking zbuf a line at a time.
//        Find min and max x and y coords with modified zbuffer vals.
//        (What is the default zbufer val? 0? infinity?)
//        Use zp->extents in write_xxx() to limit the img to the model.
//      LDLITE calcs zp->extents as it draws the polys and lines but I dont
//        think that will work here since OpenGL does the screen transform.
//*************************************************************************

      if (ldraw_commandline_opts.debug_level == 1)
	printf("EXTENTS: (%d, %d) -> (%d, %d)\n", 
	       zp->extent_x1, zp->extent_y1, zp->extent_x2, zp->extent_y2);

#ifdef USE_PNG
      if (ldraw_image_type == IMAGE_TYPE_PNG_RGB)
	write_png(filename);
      else if (ldraw_image_type == IMAGE_TYPE_PNG_RGBA)
	write_png(filename);
      else
#endif
#ifdef USE_BMP8
      if (ldraw_image_type == IMAGE_TYPE_BMP8)
	write_bmp8(filename);
      else
#endif
	write_bmp(filename);
    }
  }
  if (pause && (ldraw_commandline_opts.M == 'P')&&(step!=INT_MAX))  {
    if (step >= 0) {
      if (ldraw_commandline_opts.debug_level == 1)
	printf("STEP %d level %d pause %d\n", step+1, level, pause);
    }
  }
}

/***************************************************************/
void platform_zDraw(ZIMAGE *zp,void *zDC)
{
  if (ldraw_commandline_opts.debug_level == 1)
    printf("zdraw\n");
}

#if defined(UNIX) || defined(MAC)
// These should really move to platform.c and platform.h
/***************************************************************/
int GetProfileInt(char *appName, char *appVar, int varDefault)
{
  // This should look up appVar in some .INI or .RC file for appName.
  // Windows uses reg key HKCU\Software\Gyugyi Cybernetics\ldlite\Settings
  // Just return the default for now.
  return (varDefault);
}

/***************************************************************/
int GetPrivateProfileInt(char *appName, char *appVar, int varDefault, char *fileName)
{
  // Just return the default for now.
  return (varDefault);
}

/***************************************************************/
int GetPrivateProfileString(char *appName, char *appVar, char *varDefault, 
			    char *retString, int strSize, char *fileName)
{
  // Just return the default for now.
  strcpy (retString, varDefault);
  return (strlen(varDefault));
}
#endif

/***************************************************************/
void platform_setpath()
{
  char *env_str;

  use_uppercase = 0; // default
  env_str = platform_getenv("LDRAWDIRCASE");
  if (env_str != NULL)
  {
    if (stricmp(env_str, "UPPERCASE") == 0)
      use_uppercase = 1;
  }
  
  env_str = platform_getenv("LDRAWDIR");
  if (env_str != NULL)
  {
    strcpy(pathname, env_str);
  }
  else if (GetPrivateProfileString("LDraw","BaseDirectory","",
			  pathname,256,"ldraw.ini") == 0)
  {
#if defined(UNIX)
    sprintf(pathname, "/usr/local/ldraw");
#elif defined(MAC)
    sprintf(pathname, "Aulus:Code:Lego.CAD:LDRAW");
#elif defined(WINDOWS)
    sprintf(pathname, "c:/legos/ldraw/");
#else
#error unspecified platform in platform_getenv() definition
#endif
  }

  concat_path(pathname, use_uppercase ? "P" : "p", primitivepath);
  concat_path(pathname, use_uppercase ? "PARTS" : "parts", partspath);
  concat_path(pathname, use_uppercase ? "MODELS" : "models", modelspath);

}

/***************************************************************/
void platform_fixcase(char *path_str)
{
  int i;

  if (use_uppercase)
    for(i=0; i<strlen(path_str); i++) 
      path_str[i] = toupper(path_str[i]);
  else
    for(i=0; i<strlen(path_str); i++) 
      path_str[i] = tolower(path_str[i]);
}

/***************************************************************/
/*  Global vars for GLUT event handlers
/***************************************************************/
GLfloat zoom = 0.0;

GLboolean selection     = GL_FALSE;	/* rendering to selection buffer */

#define SELECT_BUFFER 32
GLuint select_buffer[SELECT_BUFFER];	/* selection buffer */

GLint mouse_state = -1;
GLint mouse_button = -1;

// 8 random 20% stipple patterns generated with rand()
GLubyte stips[8][4*32] = {
  {
  0x04, 0x86, 0x00, 0x10, 0x20, 0x00, 0x00, 0x30, 
  0xC2, 0x06, 0x10, 0x00, 0x10, 0x41, 0x02, 0x29, 
  0x46, 0x80, 0x00, 0xC9, 0x20, 0x11, 0x00, 0x02, 
  0x08, 0x0A, 0x80, 0x01, 0x09, 0x00, 0x22, 0x10, 
  0x02, 0x00, 0x1A, 0x48, 0x01, 0x80, 0x61, 0x00, 
  0x10, 0x09, 0x30, 0x04, 0x80, 0x01, 0x0C, 0x10, 
  0x01, 0x04, 0x51, 0x10, 0xA0, 0x40, 0x39, 0xB9, 
  0x04, 0x00, 0xAA, 0x01, 0x22, 0x04, 0x96, 0xC0, 
  0x88, 0x48, 0x04, 0x8C, 0x49, 0x01, 0x00, 0x42, 
  0x04, 0x00, 0x09, 0x54, 0x0A, 0x40, 0x64, 0x00, 
  0x4A, 0xA4, 0x19, 0x49, 0x08, 0x40, 0x08, 0x88, 
  0x04, 0x48, 0x80, 0x03, 0x30, 0x90, 0x13, 0x08, 
  0x1A, 0x00, 0x04, 0x19, 0x0A, 0x40, 0x0C, 0x09, 
  0x40, 0x0A, 0xC0, 0x8A, 0x01, 0x00, 0x60, 0x06, 
  0x00, 0x01, 0x88, 0x40, 0x20, 0xC4, 0x83, 0x99, 
  0x02, 0x10, 0x14, 0x94, 0x10, 0x10, 0x20, 0x10, 
  },
  {
  0x02, 0x40, 0x08, 0x01, 0x08, 0x00, 0x40, 0x50, 
  0x70, 0x00, 0xE5, 0x0C, 0x80, 0xCD, 0x80, 0x1A, 
  0x80, 0x41, 0x04, 0x10, 0x70, 0x03, 0x03, 0x88, 
  0x08, 0x03, 0x16, 0x00, 0x01, 0x12, 0x00, 0x80, 
  0x18, 0x00, 0x00, 0x14, 0x24, 0x08, 0x84, 0x08, 
  0x11, 0x42, 0x04, 0x44, 0x00, 0xC7, 0xD0, 0x00, 
  0x20, 0x48, 0x40, 0x89, 0x04, 0xAA, 0x09, 0x88, 
  0x00, 0x82, 0x00, 0x01, 0x18, 0x21, 0x41, 0x01, 
  0x11, 0x04, 0x04, 0x00, 0x40, 0x02, 0x69, 0x90, 
  0x4B, 0x80, 0x08, 0x00, 0x12, 0x10, 0x10, 0x01, 
  0x41, 0x22, 0x24, 0x14, 0x50, 0x0C, 0x80, 0x00, 
  0x01, 0x65, 0x16, 0x08, 0x08, 0x10, 0x02, 0x40, 
  0x0C, 0x30, 0x40, 0xC0, 0x6E, 0xE0, 0x0A, 0x23, 
  0xAC, 0xA0, 0x08, 0x80, 0x53, 0x02, 0x20, 0xC0, 
  0x40, 0x55, 0x42, 0x48, 0x05, 0x04, 0x20, 0x00, 
  0x1D, 0x06, 0x48, 0x88, 0xC8, 0x22, 0x21, 0x41, 
  },
  {
  0x00, 0x02, 0x0C, 0x43, 0x08, 0x02, 0x09, 0x02, 
  0x00, 0x00, 0x54, 0x29, 0x8A, 0x02, 0x80, 0x10, 
  0x09, 0x83, 0x24, 0x0A, 0x01, 0xC4, 0x00, 0x30, 
  0x05, 0x40, 0x88, 0x00, 0x80, 0x22, 0x8C, 0x60, 
  0x00, 0x04, 0x21, 0x32, 0x88, 0x20, 0x28, 0x48, 
  0x91, 0x00, 0x33, 0x14, 0x20, 0x89, 0x01, 0x20, 
  0x00, 0x04, 0x20, 0x00, 0x48, 0x10, 0x90, 0x60, 
  0x81, 0x0B, 0x24, 0x78, 0x86, 0x50, 0x2E, 0x08, 
  0x10, 0x29, 0x02, 0x01, 0x40, 0x42, 0x00, 0x00, 
  0xC4, 0x08, 0x80, 0x02, 0x53, 0x10, 0x05, 0x00, 
  0x90, 0x00, 0x04, 0x00, 0x20, 0x05, 0x04, 0x04, 
  0x90, 0x01, 0x02, 0x20, 0x0C, 0x02, 0x40, 0x28, 
  0x20, 0x10, 0x10, 0x20, 0x01, 0x01, 0x0B, 0x08, 
  0x40, 0x00, 0x40, 0x84, 0x00, 0xA0, 0x08, 0x21, 
  0xF2, 0x03, 0x04, 0x88, 0x20, 0x00, 0x98, 0x00, 
  0x18, 0x10, 0x08, 0x84, 0x04, 0x09, 0x08, 0x04, 
  },
  {
  0x90, 0xD0, 0x58, 0x80, 0xC8, 0xAA, 0x10, 0x80, 
  0x01, 0x05, 0x0C, 0x00, 0x08, 0x08, 0x92, 0x29, 
  0x1B, 0x21, 0x0C, 0x04, 0x08, 0x00, 0x82, 0x28, 
  0x50, 0x85, 0x02, 0x80, 0x19, 0x88, 0x08, 0xD2, 
  0x00, 0x88, 0x00, 0x40, 0x00, 0x00, 0x00, 0x24, 
  0x20, 0x40, 0x00, 0x00, 0x80, 0x50, 0x80, 0x00, 
  0x05, 0x00, 0x44, 0x48, 0xA4, 0x40, 0x80, 0x23, 
  0xC0, 0x20, 0x10, 0x00, 0xC0, 0x80, 0x00, 0x6B, 
  0x92, 0x12, 0x0D, 0x04, 0x20, 0x04, 0x80, 0x5C, 
  0x00, 0x02, 0x42, 0x08, 0xA2, 0x28, 0x04, 0x42, 
  0x10, 0x80, 0x05, 0x80, 0x0F, 0x54, 0x10, 0xC3, 
  0x00, 0x0A, 0x01, 0x12, 0x80, 0x30, 0x10, 0x20, 
  0x00, 0x80, 0x04, 0x08, 0x00, 0x00, 0x41, 0x0A, 
  0x00, 0xA0, 0x00, 0x21, 0x22, 0x88, 0x10, 0xB0, 
  0x05, 0xB9, 0x89, 0x10, 0x34, 0x30, 0x4B, 0x00, 
  0x05, 0x00, 0x10, 0x74, 0xC1, 0x01, 0x07, 0x00, 
  },
  {
  0x60, 0x14, 0x00, 0x04, 0x04, 0x48, 0x20, 0x21, 
  0xE4, 0x08, 0x42, 0x00, 0x01, 0x06, 0x03, 0x40, 
  0x5B, 0x20, 0x00, 0x04, 0x91, 0x25, 0x54, 0x43, 
  0x02, 0x01, 0x04, 0x93, 0x91, 0x81, 0x02, 0x36, 
  0x30, 0x20, 0x44, 0x08, 0x00, 0x19, 0x00, 0x00, 
  0x53, 0x80, 0x01, 0x42, 0x08, 0x01, 0x82, 0x08, 
  0xC1, 0x05, 0x01, 0x01, 0x1C, 0x44, 0x20, 0x83, 
  0x00, 0x01, 0x00, 0x08, 0x20, 0x10, 0x00, 0x28, 
  0x01, 0xC0, 0xA2, 0x00, 0x20, 0x00, 0x00, 0x80, 
  0x00, 0xC1, 0x40, 0x22, 0x00, 0x84, 0x09, 0x00, 
  0x06, 0x89, 0x33, 0x90, 0x08, 0x20, 0x00, 0x84, 
  0x60, 0x01, 0x0A, 0x34, 0x87, 0x08, 0x0A, 0x01, 
  0x1A, 0x02, 0x03, 0x14, 0x10, 0x64, 0x00, 0x4A, 
  0xC2, 0x44, 0x00, 0x04, 0x80, 0x82, 0x90, 0x30, 
  0x18, 0x02, 0x8A, 0xA1, 0x84, 0x00, 0x20, 0x08, 
  0x48, 0xD0, 0x81, 0x55, 0x01, 0xBA, 0x02, 0x40, 
  },
  {
  0x40, 0x1E, 0x00, 0x06, 0x13, 0x03, 0x45, 0x22, 
  0x50, 0x10, 0x22, 0x00, 0x12, 0x06, 0x0E, 0x01, 
  0x02, 0x08, 0xC0, 0x14, 0x34, 0x54, 0x0C, 0x0E, 
  0x80, 0x08, 0x00, 0x45, 0xA2, 0x20, 0x10, 0x98, 
  0x20, 0x7A, 0x41, 0x92, 0x48, 0x88, 0x03, 0xC1, 
  0x51, 0xB0, 0x22, 0x0A, 0x04, 0x08, 0x16, 0x02, 
  0x12, 0x00, 0x08, 0x10, 0x00, 0x80, 0x04, 0x25, 
  0x80, 0x05, 0x02, 0x00, 0x04, 0x20, 0xC1, 0x4B, 
  0x0C, 0x22, 0x00, 0x28, 0x14, 0x20, 0x44, 0x42, 
  0x20, 0x00, 0x00, 0xC4, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x22, 0x80, 0x50, 0xA4, 0x10, 0x84, 0x02, 
  0x81, 0x24, 0xC9, 0xC2, 0x50, 0x48, 0x20, 0x88, 
  0x12, 0x14, 0x82, 0x00, 0x51, 0x24, 0x05, 0x80, 
  0x02, 0xA0, 0x0C, 0x08, 0x82, 0x04, 0x20, 0x80, 
  0x10, 0x0C, 0x20, 0x12, 0x01, 0x02, 0x14, 0x00, 
  0x26, 0x12, 0x50, 0x80, 0xC4, 0x02, 0x11, 0x49, 
  },
  {
  0x00, 0xE0, 0x04, 0x18, 0x00, 0x00, 0x00, 0x82, 
  0x21, 0x04, 0x08, 0x20, 0x03, 0x02, 0x12, 0x02, 
  0x08, 0x04, 0x48, 0x00, 0x21, 0x64, 0x06, 0x08, 
  0x02, 0x30, 0x4C, 0x00, 0x01, 0x00, 0x01, 0x09, 
  0x40, 0x11, 0x04, 0x28, 0x04, 0x28, 0x70, 0x14, 
  0x28, 0x00, 0x28, 0xC1, 0x60, 0x00, 0x00, 0x51, 
  0x52, 0x00, 0xAC, 0x88, 0x24, 0x80, 0x08, 0x42, 
  0x00, 0x12, 0x40, 0x00, 0x05, 0x20, 0x04, 0x01, 
  0x00, 0xAC, 0x40, 0x01, 0x01, 0x63, 0x80, 0x2D, 
  0x18, 0x80, 0x60, 0x09, 0x0C, 0x01, 0xD2, 0x02, 
  0x18, 0x62, 0x00, 0x41, 0x91, 0x20, 0x4C, 0x20, 
  0xC2, 0x30, 0x00, 0x1C, 0x10, 0x00, 0xE8, 0x08, 
  0x88, 0x00, 0x40, 0x55, 0x00, 0xD8, 0x81, 0x86, 
  0x80, 0x28, 0x21, 0x00, 0x23, 0x08, 0x00, 0x0E, 
  0xC0, 0x02, 0x48, 0x30, 0x06, 0x08, 0x14, 0x44, 
  0x60, 0x88, 0x70, 0x0C, 0x88, 0x89, 0x84, 0x82, 
  },
  {
  0xA0, 0x56, 0x10, 0x84, 0x2A, 0x44, 0x4A, 0x84, 
  0x8E, 0x04, 0x82, 0xA0, 0x00, 0x2D, 0x90, 0x03, 
  0x02, 0x02, 0x08, 0x33, 0x8A, 0x10, 0x94, 0x16, 
  0x23, 0x04, 0x00, 0x10, 0x00, 0x11, 0x08, 0x08, 
  0x90, 0x01, 0x01, 0x8D, 0x05, 0x40, 0x02, 0x00, 
  0x21, 0xA0, 0x70, 0x02, 0x40, 0x00, 0x00, 0x80, 
  0x88, 0x2E, 0x0B, 0x44, 0x08, 0x00, 0x13, 0x44, 
  0x28, 0x22, 0x54, 0x04, 0x06, 0x08, 0x48, 0x01, 
  0x00, 0x09, 0x40, 0x30, 0xC0, 0x81, 0x80, 0x00, 
  0x20, 0x00, 0x00, 0x09, 0x82, 0x18, 0x00, 0x10, 
  0x10, 0x02, 0x14, 0x01, 0x00, 0x11, 0xC2, 0x01, 
  0x04, 0x00, 0x08, 0x88, 0x80, 0x88, 0x81, 0x8D, 
  0x80, 0x40, 0x12, 0x51, 0x08, 0x00, 0x80, 0x80, 
  0x00, 0x00, 0x80, 0x04, 0x40, 0x80, 0x5A, 0x58, 
  0x0C, 0x04, 0xB9, 0x00, 0xB0, 0x41, 0xA2, 0x24, 
  0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x48, 0x80, 
  },
};

/***************************************************************/
/*  GLUT event handlers
/***************************************************************/
void init(void)
{
  int i;

  // Set the light way up and behind us.  Will this make it too dim?
  // NOTE: The LDRAW polys are not CCW compliant so the normals are random
  // LdLite uses 2 opposing lights to avoid the problem with normals?
  // I attempted this below but it does not seem to work for OpenGL.
  // Hmmm, perhaps LdLite just took the fabs() of normals instead.
  // If I calculate normals then I could do that too.

  // x, y, z, dist divisor.  (divisor = 0 for light at infinite distance)
    GLfloat lightposition0[] = { -1000.0, 1000.0, 1000.0, 0.0 };
    GLfloat lightposition1[] = { 1000.0, -1000.0, -1000.0, 0.0 };
    GLfloat lightcolor0[] =  { 1.0, 1.0, 1.0, 1.0 }; // White light
    GLfloat lightcolor1[] =  { 0.75, 0.75, 0.75, 1.0 }; // bright light
    GLfloat lightcolor2[] =  { 0.5, 0.5, 0.5, 1.0 }; // Half light
    GLfloat lightcolor3[] =  { 0.25, 0.25, 0.25, 1.0 }; // dim light


    // Ambient and diffusion properties for front and back faces.
    // Full ambient and diffusion for R, G, B, alpha ???
    GLfloat full_mat[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat half_mat[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat no_shininess[] = { 1.0 };
    GLfloat lo_shininess[] = { 5.0 };
    GLfloat hi_shininess[] = { 100.0 };

    // glSelectBuffer(SELECT_BUFFER, select_buffer);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    if (1) //(zShading)
    {

      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_POSITION, lightposition0);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcolor2);
      //glLightfv(GL_LIGHT0, GL_AMBIENT, lightcolor1);
#if 0
      glEnable(GL_LIGHT1);
      glLightfv(GL_LIGHT1, GL_POSITION, lightposition1);
      glLightfv(GL_LIGHT1, GL_DIFFUSE, lightcolor3);
      glLightfv(GL_LIGHT1, GL_AMBIENT, lightcolor1);
#endif
      glEnable(GL_LIGHTING);

      // reverse the normals for back face polys and light them
      // Unfortunately this still doesnt seem the same as front faces.

      // Bright ambient light for the whole scene.
      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightcolor1);
#if 1
      //glFrontFace(GL_CCW);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
#else
      glFrontFace(GL_CW);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
#endif

#if 1 
      /* 
      OpenGL's color material feature provides a less expensive way to change
      material parameters. With color material enabled, material colors track
      the current color. This means that instead of using the relatively
      expensive glMaterialfv routine, you can use the glColor3f routine. 
      */
      glEnable(GL_COLOR_MATERIAL);
      glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, full_mat);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, no_mat);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, no_shininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);
#else
      glDisable(GL_COLOR_MATERIAL);
#endif
    }
    if (zShading)
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);

    // glEnable(GL_CULL_FACE);
    // Disable backface culling since the LDRAW parts arent BFC compliant.
    glDisable(GL_CULL_FACE);

    //glClearColor(0.0, 0.0, 0.0, 0.0);
    // Set the background to white like ldlite.
    glClearColor(1.0, 1.0, 1.0, 0.0);

    // Make lines a wider than a pixel so they are not hidden by surfaces.
    // (Not needed because PolygonOffset works so much better)
    //glLineWidth(1.25);

    // Nudge back the zbuffer values of surfaces so the hilited edges show.
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

#if 0
    // Make command lists for the eight stipple patterns.
#define STIP_OFFSET 100
    for (i = 0; i < 8; i++)
    {
      glNewList(i+STIP_OFFSET, GL_COMPILE);
      glPolygonStipple(&stips[i][0]);
      glEndList();
    }
#endif

#if 0
    // NOTE:  I could achieve L3Lab speeds if I delay rendering studs till last
    // then do occlusion tests on the bounding boxes before rendering.

    // disable updates to color and depth buffer (optional)
    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
    // enable occlusion test
    glEnable(GL_OCCLUSION_TEST_HP);
    // render bounding geometry
    // gl rendering calls
    // disable occlusion test
    glDisable(GL_OCCLUSION_TEST_HP);
    // enable updates to color and depth buffer
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    // read occlusion test result
    glGetBooleanv(GL_OCCLUSION_TEST_RESULT_HP, &result);
    // if (result) render internal geometry
    // else don't render
#endif
}

/***************************************************************/
void reshape(int width, int height)
{
    GLdouble left, right, top, bottom, aspect;

    Width = width;
    Height = height;

    aspect = ((GLdouble)width/(GLdouble)height);
    left = (GLdouble)-width / 2.0;
    right = left + (GLdouble)width;
    bottom = (GLdouble)-height / 2.0;
    top = bottom + (GLdouble)height;
    
    //    int x, y;
    //    GLdouble pan_x, pan_y, pan_z;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // frustrum = clipping space(left, right, bottom, top, near, far)
    //glFrustum(-3.0, 3.0, -3.0, 3.0, 64, 256);
 
#define WIDE_ANGLE_VIEW 1
    if (ldraw_projection_type)
    {
      // fov, aspect, near, far
      // try to get better resolution in depth buffer.  Move near, far.
#if WIDE_ANGLE_VIEW
      gluPerspective(45.0, aspect, 1.0, 2000.0);
#else
      gluPerspective(20.0, aspect, 100.0, 4000.0);
#endif
      //glDepthRange(0.0, 1.0); // I do NOT understand this fn.

    }
    else
    {
      // left, right, bottom, top, near, far
#if WIDE_ANGLE_VIEW
      glOrtho(left, right, bottom, top, 1.0, 2000.0);
#else
      glOrtho(left, right, bottom, top, 100.0, 4000.0);
#endif
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (ldraw_projection_type)
    {
      // Perhaps this belongs in the ModelView matrix.
      // Supposedly only GluPerspective, glGrustrum, glOrtho* belong in
      // the Projection matrix.
#if 0
      glDepthRange(0.0, 1.0); // I do NOT understand this fn.
#endif
    }

#if 0    
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, view);

    // Hmmm, do these convert things to screen coords and vice versa?
    // Convert coords from screen(x,y,0) to world(x,y,z)?
    gluProject((GLdouble)x, (GLdouble)y, 0.0,
	       model, proj, view,
	       &pan_x, &pan_y, &pan_z);
    // Convert coords from world(x,y,z) to screen(x,y,z)?
    gluUnProject((GLdouble)x, (GLdouble)y, pan_z,
		 model, proj, view,
		 &pan_x, &pan_y, &pan_z);
#endif

    
    //glTranslatef(0.0, 0.0, -200.0 + zoom);
}

#define USE_F00_CAMERA 1
#ifdef USE_F00_CAMERA
  extern void resetCamera();
  extern void applyCamera();
  extern void specialFunc( int key, int x, int y );
#endif

/***************************************************************/
void display(void)
{
  int rc;
  int client_rect_right;
  int client_rect_bottom;
  int res;

  if (panning)
    glDrawBuffer(GL_BACK);  // Enable double buffer for spin mode.
  else
    glDrawBuffer(GL_FRONT);  // Effectively disable double buffer.

  if (zShading)
    glEnable(GL_LIGHTING);
  else
    glDisable(GL_LIGHTING);

  glCurColorIndex = -1;

  client_rect_right = Width;
  client_rect_bottom = Height;

  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#ifdef IGNORE_DIRTY
  // I cannot really do this until I can also redraw the whole image 
  // (up to the current step) on demand when window gets dirty.
  if (ldraw_commandline_opts.M == 'P')
  {
    // Non-continuous output stop after each step.
    if (curstep == 0) // (Or dirty)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  else
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(1.0, 1.0, 1.0); // White.

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

#ifdef USE_QUATERNION
  // NOTE:  This does NOT work since I'm doing the spin transform BEFORE
  // the gluLookAt() transform, so the model moves around the camera 
  // rather than the camera orbiting the model.
  if (qspin[3] == 0.0)
  {
    glLoadIdentity();
  }
#else
  glLoadIdentity();
#endif

#ifdef USE_F00_CAMERA
  applyCamera();
#endif

#define ORBIT_THE_CAMERA_ABOUT_THE_MODEL 1

#ifdef ORBIT_THE_MODEL_ABOUT_THE_CAMERA
  // Do camera translation/rotation BEFORE GluLookAt()
  glRotatef(fXRot, 1.0f, 0.0f, 0.0f);
  glRotatef(360 - fYRot, 0.0f, 1.0f, 0.0f);
  glTranslatef(fCamX, fCamY, fCamZ);        
  // Draw everything else below after gluLookAt()
#endif

  // ldlite_parse seems to offset x,y coords by half the window size.

  // Hmmm, my up vector is straight up.  This may NOT be perpendicular 
  // to my view vector if it looks down at an angle toward (0,100,0).

  // from, toward, upvector
#if WIDE_ANGLE_VIEW
  // Height/6 moves the origin from halfway to 2/3 of the way down the screen.
  // Original LdLite uses zGetRowsize() and zGetColsize() to do this.
  // I stubbed them to return 0 for OpenGL since its origin is the window
  // center, not the corner.  See LDRAW_COMPATIBLE_CENTER in LdliteVR_main.c.
  gluLookAt(0.0, Height/6.0, 1000.0, 0.0, Height/6.0, 0.0, 0.0, 1.0, 0.0);
#else
  gluLookAt(0.0, Height/6.0, 2000.0, 0.0, Height/6.0, 0.0, 0.0, 1.0, 0.0);
#endif

#ifdef ORBIT_THE_CAMERA_ABOUT_THE_MODEL
  // Do camera translation/rotation AFTER the GluLookAt camera transform.
  glRotatef(fXRot, 1.0f, 0.0f, 0.0f);
  glRotatef(-fYRot, 0.0f, 1.0f, 0.0f);
  glTranslatef(fCamX, fCamY, fCamZ);        
  // Draw everything else below
#endif

#ifdef USE_GL_TWIRL
  gluLookAt(400.0, 600.0, 1000.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  // This does not work right because the ldlite default model matrix
  // has already tilted the model so it twirls tilted if I do this.
  // This WOULD work if I used glulookAt() to set the view angle and
  // substituted an identity matrix for the ldlite model matrix.
  if (twirl_angle != 0.0)
    glRotatef((float)-twirl_angle, 0.0, 1.0, 0.0);
#endif
#ifdef USE_QUATERNION
  printf("spin(%0.2f, %0.2f, %0.2f, %0.2f)\n", 
	 qspin[3], qspin[0], qspin[1], qspin[2]);
  glRotatef(qspin[3], qspin[0], qspin[1], qspin[2]);
#endif

  if (drawAxis)
  {
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0); // r
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1000.0, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0); // g
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 500.0, 0.0);
    glColor3f(0.0, 0.0, 1.0); // b
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 400.0);
    glEnd();
  }

  glColor3f(1.0, 1.0, 1.0); // White.

  //glEdgeFlag(GL_FALSE); // Do not draw poly edges?
  glShadeModel(GL_FLAT); // not GL_SMOOTH
  //glShadeModel(GL_SMOOTH); // not GL_SMOOTH

  //glPolygonMode(GL_FRONT,GL_FILL);
  //glPolygonMode(GL_BACK,GL_FILL);
  //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

  zcolor_init();

  stepcount = 0;

  rc = zReset(&(client_rect_right),&(client_rect_bottom));
  if (rc != 0) {
    printf("Out of Memory, exiting");
    exit(-1);
  }

#ifdef USE_L3_PARSER
  if (parsername == L3_PARSER)
  {
  if (!list_made)
  {
    // LoadModel frees Parts[0] and Lights etc.
    LoadModelPre();
    // L3Time_t ttt = L3GetTime();
    res = LoadModel(datfilename);
    // ttt = L3GetTime() - ttt;
    LoadModelPost();
    // printf("LoadModel: %d",ttt);
    list_made = 1;
  }
  if (ldraw_commandline_opts.debug_level == 1)
    printf("DrawModel %s\n", datfilename);
#ifdef ONE_BIG_DISPLAY_LIST
  // Displays in 1/3 less time, but does not allow other views
  // except when using opengl to move camera.  (arrow keys)
  // Type 5 lines should be excluded from display list (or drawn afterwards).
  if (list_made < 2)
  {
    glNewList(1, GL_COMPILE);
    DrawModel();
    glEndList();
    list_made = 2;
  }
  glCallList(1);
#else
  DrawModel();
#endif
  }
  else
#endif

  {
#ifdef ONE_BIG_DISPLAY_LIST
  // Nice speedup: Displays in 1/3 time, but does not allow other views.
  if (!list_made)
  {
    glNewList(1, GL_COMPILE);
    znamelist_push();
    ldlite_parse(datfilename, buf);
    znamelist_pop();
    glEndList();
    list_made = 1;
  }
  glCallList(1);
#else
  mpd_subfile_name = NULL; // potential memory leak
  znamelist_push();
  ldlite_parse(datfilename, buf);
  znamelist_pop();
  if (mpd_subfile_name != NULL) 
  {
    // set file name to first subfile
    if (ldraw_commandline_opts.debug_level == 1)
      printf("Draw MPD %s\n", mpd_subfile_name);

    zcolor_init();

    rc = zReset(&(client_rect_right),&(client_rect_bottom));
    if (rc != 0) {
      printf("Out of Memory, exiting");
      exit(-1);
    }

    znamelist_push();
    // Do NOT call ldlite_parse directly on mpd_subfile_name.  
    // It does not check the file cache for the filename passed to it.
    //**********************************************************************
    // NOTE:  Unfortunately this method calls yy_delete_buffer() which
    // sets yy_current_buffer to NULL, causing a crash on the next redisplay.
    // This is because yy_current_buffer only gets initialized the first time
    // yylex() is called due to the static variable yy_init getting set to 0.
    // So how does the REAL ldlite handle this problem?
    // Perhaps this is why ldliteView.cpp ALWAYS uses the NULL filename way.
    // FIXED??  See my "weak attempt to fix" this in ldlite_parse().
    //**********************************************************************
    //ldlite_parse(mpd_subfile_name,NULL);
    sprintf(buf,"1 16 0 0 0 1 0 0 0 1 0 0 0 1 %s\n", mpd_subfile_name);
    ldlite_parse(NULL,buf);
    znamelist_pop();
  }
#endif
  }

#if 0
  glColor3f(1.0, 1.0, 1.0); // White.
  glutWireCube(20.0);
#endif

#if 0
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0., 100., 0., 100. ); /* "percent units" */
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
  glColor3f( 0.3, 0.3, 0.3 );		/* grey  	*/
  DoRasterString( 5., 60., "My Volume = 345.6" );
#endif


#if 0
  // Try out some text strings so I can add a console someday.
  glColor3f(0.0, 0.5, 1.0); // Greenish blue
  DoRasterString( -50.0, -100.0, "LdGLite" );
  DoRasterString( 0.0, 0.0, "ldglite" );
  /*
GLUT_BITMAP_9_BY_15 
GLUT_BITMAP_8_BY_13 
GLUT_BITMAP_TIMES_ROMAN_10 
GLUT_BITMAP_TIMES_ROMAN_24 
GLUT_BITMAP_HELVETICA_10 
GLUT_BITMAP_HELVETICA_12 
GLUT_BITMAP_HELVETICA_18
  */
  glColor3f(0.0, 0.0, 0.0); // black
  glPushMatrix();
  glTranslatef(-Width/2, -100, 0);
  glScalef(0.1f, 0.1f, 0.1f);
  draw_string(GLUT_STROKE_ROMAN, "Hello!");
  glTranslatef(-Width/2, -60, 0);
  draw_string(GLUT_STROKE_MONO_ROMAN, "Hello!");
  glPopMatrix();
  gluOrtho2D(0, Width, 0, Height);
  glRasterPos2f(0, 0);
  draw_string_bitmap(GLUT_BITMAP_HELVETICA_12, "HELLO!");
  //rx+=glutBitmapWidth(GLUT_BITMAP_9_BY_15,'H'); 
  //rx+=glutBitmapLength(GLUT_BITMAP_HELVETICA_12, "HELLO!");
  //int glutStrokeWidth(void *font, int character);
  //int glutStrokeLength(void *font, const unsigned char *string);
#endif

  // Do not increment step right after (or during) panning.
  if (not_stepping)
    not_stepping = 0;
  else if (!panning)
  {
    if (ldraw_commandline_opts.M == 'P')
    {
      sprintf(buf,"Step %d of %d.  ",curstep+1, stepcount+1);
      // Non-continuous output stop after each step.
      if (stepcount == curstep)
      {
	strcat(buf, "Finished.");
	curstep = 0; // Reset to first step
      }
      else 
      {
	strcat(buf, "Click on drawing to continue.");
	curstep++; // Move on to next step
      }
      platform_step_comment(buf);
    }
  }

  glFlush();

  glPopMatrix();
  
  //  if (!selection)
  //    glutSwapBuffers();

#ifdef USE_DOUBLE_BUFFER
  if (panning)
    glutSwapBuffers();
#endif

  // If we just want the output files then quit when idle.
  if ((ldraw_commandline_opts.output == 1) ||
       (ldraw_commandline_opts.M == 'S')) 
  {
    // Do NOT quit just yet if we need to twirl.
    if (ldraw_commandline_opts.rotate != 1) 
      exit(0); //quit the program
  }

}

/***************************************************************/
void initCamera(void)
{
  fCamX = 0.0;
  fCamY = 0.0;
  fCamZ = 0.0;
  fXRot = 0.0;
  fYRot = 0.0;
  fZRot = 0.0;

#ifdef USE_F00_CAMERA
  resetCamera();
#endif
}

void rotate_about(float x, float y, float z, float degrees);

/***************************************************************/
void parse_view(char *viewMatrix)
{
      sscanf(viewMatrix,"%f,%f,%f,%f,%f,%f,%f,%f,%f",
	     &(ldraw_commandline_opts.A.a),
	     &(ldraw_commandline_opts.A.b),
	     &(ldraw_commandline_opts.A.c),
	     &(ldraw_commandline_opts.A.d),
	     &(ldraw_commandline_opts.A.e),
	     &(ldraw_commandline_opts.A.f),
	     &(ldraw_commandline_opts.A.g),
	     &(ldraw_commandline_opts.A.h),
	     &(ldraw_commandline_opts.A.i));
}

/***************************************************************/
void fnkeys(int key, int x, int y)
{
  /*
    This will rotate you around x and y and translate you along x, y, and z. 
    It shouldn't take much to add rotation around the z axis.
  */

  // Add modifier check so I can:
  //   spin about the 3 axis with 
  //   slide along 3 axis with shifted keys
  //   orbit about the center of the viewpoint with ctrl modified keys?
  //
  // glutGetModifiers() return masks.
  //   GLUT_ACTIVE_SHIFT
  //   GLUT_ACTIVE_CTRL
  //   GLUT_ACTIVE_ALT

  // The PG_UP, PG_DN keys seem to zoom in and out (only in perspective mode)
  // You can NOT zoom in or out in orthographic mode, only scale.
  switch(key) {
#ifdef USE_F00_CAMERA
  case GLUT_KEY_PAGE_UP:
  case GLUT_KEY_PAGE_DOWN:
  case GLUT_KEY_RIGHT:
  case GLUT_KEY_LEFT:
  case GLUT_KEY_UP:
  case GLUT_KEY_DOWN:
    specialFunc( key, x, y );
    break;
#else
  case GLUT_KEY_PAGE_UP:
    fCamX += (float)sin(fYRot*PI_180) * MOVE_SPEED * 20;
    fCamZ += (float)cos(fYRot*PI_180) * MOVE_SPEED * 20;
    fCamY += (float)sin(fXRot*PI_180) * MOVE_SPEED * 20;
    break;
  case GLUT_KEY_PAGE_DOWN:
    fCamX -= (float)sin(fYRot*PI_180) * MOVE_SPEED * 20;
    fCamZ -= (float)cos(fYRot*PI_180) * MOVE_SPEED * 20;
    fCamY -= (float)sin(fXRot*PI_180) * MOVE_SPEED * 20;
    break;
  case GLUT_KEY_RIGHT:
    fYRot -= MOVE_SPEED / 2;
    if(fYRot < -360.0)			
      fYRot += 360.0;
    break;
  case GLUT_KEY_LEFT:
    fYRot += MOVE_SPEED / 2;
    if(fYRot > 360.0)
      fYRot -= 360.0;	
    break;
  case GLUT_KEY_UP:
    fXRot -= MOVE_SPEED / 2;
    if(fXRot < -360.0)
      fXRot += 360.0;
    break;
  case GLUT_KEY_DOWN:
    fXRot += MOVE_SPEED / 2;
    if(fXRot > 360.0)
      fXRot -= 360.0;
    break;
#endif
  case GLUT_KEY_HOME:
    rotate_about(0.0, 1.0, 0.0, 45.0 );
    break;
  case GLUT_KEY_END:
    rotate_about(1.0, 0.0, 0.0, 45.0 );
    break;
  case GLUT_KEY_F1:
    rotate_about(1.0, 0.0, 0.0, 5.0 );
    break;
  case GLUT_KEY_F2:
    rotate_about(0.0, 1.0, 0.0, 5.0 );
    break;
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
  case GLUT_KEY_F9:
    glutLeaveGameMode();
    if (ldraw_commandline_opts.debug_level == 1)
      printf("Window %d -> %d\n", glutGetWindow(), main_window);
    if ( glutGetWindow() != main_window )
    {
      if (main_window == -1)
        main_window = glutCreateWindow(title);
      else
        glutSetWindow(main_window); 
    }
    registerGlutCallbacks();
    init();
    break;
  case GLUT_KEY_F10:
    //glutGameModeString("800x600:16@60") ;
    //glutGameModeString("800x600:16") ;
    glutGameModeString("640x480") ;
    // if the width is different to -1
    if (glutGameModeGet(GLUT_GAME_MODE_WIDTH) != -1)
    {
      glutEnterGameMode();		// enter full screen mode
      registerGlutCallbacks();
      init();
    }
    break;
#endif
  default:
    return;
  }

  if (ldraw_commandline_opts.debug_level == 1)
  {
    printf("Cam = (%.2f, %.2f, %.2f)\n", fCamX, fCamY, fCamZ);
    printf("Rot = (%.2f, %.2f, %.2f)\n", fXRot, fYRot, fZRot);
  }

  // The LdrawOblique matrix is a projection matrix.
  // Substitute Oblique view matrix so we can rotate it.
  if (m_viewMatrix == LdrawOblique)
  {
    m_viewMatrix = Oblique;
    parse_view(m_viewMatrix);
  }

  glutPostRedisplay();

  /*
    Then, all you have to do is this
    at the beginning of your display function to implement your camera.
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();	
    // Do camera translation/rotation
    glRotatef(fXRot, 1.0f, 0.0f, 0.0f);
    glRotatef(360 - fYRot, 0.0f, 1.0f, 0.0f);
    glTranslatef(fCamX, fCamY, fCamZ);        
    // Draw everything else below
  */
}

/***************************************************************/
void keyboard(unsigned char key, int x, int y)
{
  int newview = 0;
  char c;
  
  glutModifiers = glutGetModifiers(); // Glut doesn't like this in motion() fn.

    switch(key) {
    case 'z':
        ldraw_commandline_opts.S *= 0.9;
	//reshape(Width, Height);
	break;
    case 'Z':
        ldraw_commandline_opts.S *= (1.0 / 0.9);
	//reshape(Width, Height);
	break;
    case 's':
        ldraw_commandline_opts.S *= 0.5;
	//reshape(Width, Height);
	break;
    case 'S':
        ldraw_commandline_opts.S *= (1.0 / 0.5);
	//reshape(Width, Height);
	break;
    case '0':
      m_viewMatrix = LdrawOblique;
      newview = 1;
      break;
    case '1':
      m_viewMatrix = Back;
      newview = 1;
      break;
    case '2':
      m_viewMatrix = Left;
      newview = 1;
      break;
    case '3':
      m_viewMatrix = Right;
      newview = 1;
      break;
    case '4':
      m_viewMatrix = Above;
      newview = 1;
      break;
    case '5':
      m_viewMatrix = Beneath;
      newview = 1;
      break;
    case '6':
      m_viewMatrix = Oblique;
      newview = 1;
      break;
    case '7':
      m_viewMatrix = Front;
      newview = 1;
      break;
    case '8':
      m_viewMatrix = UpsideDown;
      newview = 1;
      break;
    case '9':
      m_viewMatrix = Natural;
      newview = 1;
      break;
    case 'j':
      ldraw_projection_type = 0;
      reshape(Width, Height);
      break;
    case 'J':
      ldraw_projection_type = 1;
      reshape(Width, Height);
      break;
    case 'f':
      ldraw_commandline_opts.F ^= STUDLESS_MODE;
      reshape(Width, Height);
      break;
    case 'n':
      zWire = 0;
      zShading = 0;
      ldraw_commandline_opts.F &= !(WIREFRAME_MODE);
      ldraw_commandline_opts.F &= !(SHADED_MODE);
      reshape(Width, Height);
      break;
    case 'h':
      zWire = 0;
      zShading = 1;
      ldraw_commandline_opts.F &= !(WIREFRAME_MODE);
      ldraw_commandline_opts.F |= SHADED_MODE;
      reshape(Width, Height);
      break;
    case 'w':
      zWire = 1;
      zShading = 0;
      ldraw_commandline_opts.F |= WIREFRAME_MODE;
      ldraw_commandline_opts.F &= !(SHADED_MODE);
      reshape(Width, Height);
      break;
    case 'P':
    case 'p':
      if (key == 'P') 
      {
	ldraw_image_type = IMAGE_TYPE_PNG_RGB;
	if (glutModifiers & GLUT_ACTIVE_CTRL)
	  ldraw_image_type = IMAGE_TYPE_PNG_RGBA;
      }
      else 
      {
	ldraw_image_type = IMAGE_TYPE_BMP8;
	if (glutModifiers & GLUT_ACTIVE_CTRL)
	  ldraw_image_type = IMAGE_TYPE_BMP;
      }
      c = ldraw_commandline_opts.M;
      ldraw_commandline_opts.M = 'S';
      platform_step(INT_MAX, 0, -1, NULL);
      ldraw_commandline_opts.M = c;
      return;
    case 'v':
      if (pan_visible == (BBOX_MODE | WIREFRAME_MODE) )
	pan_visible = (WIREFRAME_MODE | STUDLESS_MODE);
      else if (pan_visible == (WIREFRAME_MODE | STUDLESS_MODE) )
	pan_visible = INVISIBLE_MODE;
      else if (pan_visible == (INVISIBLE_MODE) )
	pan_visible = (BBOX_MODE | SHADED_MODE);
      else if (pan_visible == (BBOX_MODE | SHADED_MODE) )
	pan_visible = (STUDLESS_MODE | SHADED_MODE);
      else if (pan_visible == (STUDLESS_MODE | SHADED_MODE) )
	pan_visible = (BBOX_MODE | WIREFRAME_MODE);
      else 
	pan_visible = (BBOX_MODE | WIREFRAME_MODE);
      return;
    case 'c':
      // NOTE: I could toggle the menu strings but that would require
      // setting them right initially, and making sure its not fullscreen.
      // Right now than means checking  if (ldraw_commandline_opts.V_x >= -1)
      //glutSetMenu(opts); // Reset the current menu to the main menu.
      if (ldraw_commandline_opts.M == 'C')
      {	
	ldraw_commandline_opts.M = 'P';
	//glutChangeToMenuEntry(3, "Continuous         ", 'c');
      }
      else
      {	
	ldraw_commandline_opts.M = 'C'; //Switch to continuous output.
	//glutChangeToMenuEntry(3, "Pause              ", 'c');
      }
      curstep = 0; // Reset to first step
      return;
    case 'g':
      ldraw_commandline_opts.poll ^= 1;
      return;
#ifdef USE_L3_PARSER
    case 'l':
      if (parsername == LDLITE_PARSER)
	parsername = L3_PARSER;
      else
	parsername = LDLITE_PARSER;
      list_made = 0; // Gotta reparse the file.
      break;
#endif
    case 27:
	exit(0);
	break;
    case '\r':
	break;
    default:
	return;
    }
    if (newview)
    {
      parse_view(m_viewMatrix);
      initCamera(); // Reset the camera position for any stock views.
    }

    glutPostRedisplay();
}

/***************************************************************/
void rotate_the_model(double y_angle, double x_angle)
{
  // modify command line option A matrix
  matrix3d xm_rot;
  matrix3d ym_rot;
  vector3d v_dummy;
  matrix3d *m_temp;
  vector3d *v_temp;
  
  y_angle *= (3.1415927/180.0);
  x_angle *= (3.1415927/180.0);
  
  v_dummy.x = 0;
  v_dummy.y = 0;
  v_dummy.z = 0;

  xm_rot.a = (float)cos(y_angle);
  xm_rot.b = 0;
  xm_rot.c = (float)sin(y_angle);
  xm_rot.d = 0;
  xm_rot.e = 1;
  xm_rot.f = 0;
  xm_rot.g = (float)(-1.0*sin(y_angle));
  xm_rot.h = 0;
  xm_rot.i = (float)cos(y_angle);

  ym_rot.a = 1;
  ym_rot.b = 0;
  ym_rot.c = 0;
  ym_rot.d = 0;
  ym_rot.e = (float)cos(x_angle);
  ym_rot.f = (float)(-1.0*sin(x_angle));
  ym_rot.g = 0;
  ym_rot.h = (float)sin(x_angle);
  ym_rot.i = (float)cos(x_angle);
  transform_multiply(&v_dummy,&ym_rot,
		     &v_dummy,&xm_rot,
		     &v_temp, &m_temp);
  xm_rot = *m_temp;

  free(m_temp);
  free(v_temp);

  transform_multiply(&v_dummy,&xm_rot,
		     &v_dummy,&(ldraw_commandline_opts.A),
		     &v_temp, &m_temp);
  ldraw_commandline_opts.A = *m_temp;

  free(m_temp);
  free(v_temp);
}

/***************************************************************/
// Rotate by angle degrees about an arbitrary vector.
// Code borrowed from push_rotation() and push_transform() in y.tab.c
// This works but transform_multiply() from ldliteVR_main.c appears buggy.
void rotate_about(float x, float y, float z, float degrees)
{
  // convert axis - degrees into rotation matrix
  matrix3d *m;
  matrix3d newm;
  matrix3d *oldm; 
  vector3d t = {0.0, 0.0, 0.0};
  double a,b,c,s, veclen_inv, sin2a;

  // Don't bother with bad axis or 0 rotations.
  if (degrees == 0.0) 
    return;
  if ((x == 0.0) && (y == 0.0) && (z == 0.0))
    return;

  // convert axis and degrees into a quaternion
  veclen_inv = 1.0/sqrt(x*x + y*y + z*z);
  sin2a = sin((3.1415927*degrees)/360.0);
  a = sin2a * x * veclen_inv;
  b = sin2a * y * veclen_inv;
  c = sin2a * z * veclen_inv;
  s = cos((3.1415927*degrees)/360.0);

  // convert quaternion into a rotation matrix.
  m = (matrix3d *)malloc(sizeof(matrix3d));

  m->a = (float)(1 - 2*b*b-2*c*c);
  m->b = (float)(2*a*b - 2*s*c);
  m->c = (float)(2*a*c + 2*s*b);
  m->d = (float)(2*a*b+2*s*c);
  m->e = (float)(1 - 2*a*a - 2*c*c);
  m->f = (float)(2*b*c - 2*s*a);
  m->g = (float)(2*a*c - 2*s*b);
  m->h = (float)(2*b*c + 2*s*a);
  m->i = (float)(1 - 2*a*a - 2*b*b);

#if 0
  // This applies the new rotation to the model before the view transform.
  // Which feels wrong.  (not like orbiting the camera about the model)
  oldm = &(ldraw_commandline_opts.A);
#else
  // This seems to apply the new rotation after the view transform,
  // but also seems to warp the image.  Why???
  // Actually It only seems to happen if I start with the oblique, natural,
  // or upside-down transforms.  What's so strange about those transforms.
  // Do they actually shear the model in a way we don't see until we rotate?
  //
  // Hmmm this is the same problem transform_multiply() suffers from...
  // That's why transform_multiply works for twirl but not for mouse spin.
  // Twirl angle is applied before the view transform, but spin is after.
  // So what is up with those 3 view matrices, and is this the L3Lab problem.
  oldm = m;
  m = &(ldraw_commandline_opts.A);
#endif

  newm.a = oldm->a * m->a + oldm->b * m->d + oldm->c * m->g;
  newm.b = oldm->a * m->b + oldm->b * m->e + oldm->c * m->h;
  newm.c = oldm->a * m->c + oldm->b * m->f + oldm->c * m->i;
  newm.d = oldm->d * m->a + oldm->e * m->d + oldm->f * m->g;
  newm.e = oldm->d * m->b + oldm->e * m->e + oldm->f * m->h;
  newm.f = oldm->d * m->c + oldm->e * m->f + oldm->f * m->i;
  newm.g = oldm->g * m->a + oldm->h * m->d + oldm->i * m->g;
  newm.h = oldm->g * m->b + oldm->h * m->e + oldm->i * m->h;
  newm.i = oldm->g * m->c + oldm->h * m->f + oldm->i * m->i;

#if 0
  fprintf(stdout,"Transform old:\n");
  print_transform(&t, &(ldraw_commandline_opts.A));
  fprintf(stdout,"Transform current:\n");
  print_transform(&t, &newm);
#endif

  ldraw_commandline_opts.A = newm;
}

/***************************************************************/
// Rotate by angle degrees about an arbitrary vector.
void old_rotate_about(float x, float y, float z, float angle)
{
  extern  void normalize(float v[3]);

  // modify command line option A matrix
  matrix3d m_rot;
  vector3d v_dummy;
  matrix3d *m_temp;
  vector3d *v_temp;
  float c, s;
  float v[3];
  
  // Don't bother with bad axis or 0 rotations.
  if (angle == 0.0) 
    return;
  if ((x == 0.0) && (y == 0.0) && (z == 0.0))
    return;

  v[0] = x;
  v[1] = y;
  v[2] = z;

  normalize(v);
  x = v[0];
  y = v[1];
  z = v[2];

#if USE_QUATERNION
  qspin[0] = -v[0];
  qspin[1] = v[1];
  qspin[2] = v[2];
  qspin[3] = angle;
#else
  angle *= (3.1415927/180.0);
  c = cos(angle);
  s = sin(angle);

  v_dummy.x = 0;
  v_dummy.y = 0;
  v_dummy.z = 0;

  m_rot.a = (float)(x*x*(1-c)+c);
  m_rot.b = (float)(y*x*(1-c)+(z*s));
  m_rot.c = (float)(x*z*(1-c)-(y*s));
  m_rot.d = (float)(x*y*(1-c)-(z*s));
  m_rot.e = (float)(y*y*(1-c)+c);
  m_rot.f = (float)(y*z*(1-c)+(x*s));
  m_rot.g = (float)(x*z*(1-c)+(y*s));
  m_rot.h = (float)(y*z*(1-c)-(x*s));
  m_rot.i = (float)(z*z*(1-c)+c);

  transform_multiply(&v_dummy,&(ldraw_commandline_opts.A),
		     &v_dummy,&m_rot,
		     &v_temp, &m_temp);
  ldraw_commandline_opts.A = *m_temp;

  free(m_temp);
  free(v_temp);
#endif
}

/***************************************************************/
void
mouse(int button, int state, int x, int y)
{
  GLdouble model[4*4];
  GLdouble proj[4*4];
  GLint view[4];
  GLdouble pan_x, pan_y, pan_z, x_angle, y_angle, angle, depth;

  glutModifiers = glutGetModifiers(); // Glut doesn't like this in motion() fn.

  mouse_state = state;
  mouse_button = button;
  
  if (button != GLUT_LEFT_BUTTON) {
    return;
  }

  // Only count mouse button down GLUT_UP
  if (state == GLUT_DOWN)
  {
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, view);
    gluUnProject((GLdouble)x, (GLdouble)y, 1.0,
		 model, proj, view,
		 &pan_x, &pan_y, &pan_z);
    pan_y = -pan_y;

    pan_start_x = pan_end_x = pan_x;
    pan_start_y = pan_end_y = pan_y;

    if (ldraw_commandline_opts.debug_level == 1)
      printf("pdn(%d, %d), -> (%0.2f, %0.2f, %0.2f)\n", x, y, pan_x, pan_y, pan_z);
    panning = 0;
    //glutSetCursor(GLUT_CURSOR_INHERIT);
    //glutWarpPointer(Width/2, Height/2);
    return;
  }
  else if (panning)
  {
    // Restore wireframe and stud draw modes.
    zWire = pan_start_zWire;
    ldraw_commandline_opts.F = pan_start_F;

    // The LdrawOblique matrix is a projection matrix.
    // Substitute Oblique view matrix so we can rotate it.
    if (m_viewMatrix == LdrawOblique)
    {
      m_viewMatrix = Oblique;
      parse_view(m_viewMatrix);
    }
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, view);
    gluUnProject((GLdouble)x, (GLdouble)y, 1.0,
		 model, proj, view,
		 &pan_x, &pan_y, &pan_z);
    pan_y = -pan_y;

    if (ldraw_commandline_opts.debug_level == 1)
      printf("pup(%d, %d), -> (%0.2f, %0.2f, %0.2f)\n", x, y, pan_x, pan_y, pan_z);
    // NOTE:  This would be a great place to rotate or pan the model.

    //glRotated(pan_x-pan_start_x, 1.0, 0.0, 0.0);
    //glRotated(pan_y-pan_start_y, 0.0, 1.0, 0.0);

    // Convert from world coords across screen plane to angle thru origin.
#if WIDE_ANGLE_VIEW
    depth = 1000.0;
#else
    depth = 500.0;
#endif

#if 0
    pan_z = pan_x - pan_start_x;
    y_angle = atan2(pan_z, depth);
    y_angle *= -1.0;
    y_angle *= (180.0/3.1415927);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("rotating(%0.2f - %0.2f = %0.2f) by angle %0.2f\n", pan_x, pan_start_x, pan_z, y_angle);

    pan_z = pan_y - pan_start_y;
    x_angle = atan2(pan_z, depth);
    x_angle *= -1.0;
    x_angle *= (180.0/3.1415927);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("rotating(%0.2f - %0.2f = %0.2f) by angle %0.2f\n", pan_y, pan_start_y, pan_z, x_angle);
    //rotate_the_model(0.0, x_angle);
    //rotate_the_model(y_angle, 0.0 );
    rotate_the_model(y_angle, x_angle );
#else
    pan_x -= pan_start_x;
    pan_y -= pan_start_y;
    pan_y = -pan_y; //Beats me why.
    pan_z = max(fabs(pan_x), fabs(pan_y));
    angle = atan2(pan_z, depth);
    //angle *= -1.0;
    angle *= 5.0;
    angle *= (180.0/3.1415927);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("rotating about(%0.2f, %0.2f) by angle %0.2f\n", pan_y, -pan_x, angle);
    rotate_about(pan_y, -pan_x, 0.0, angle );
#endif

    panning = 0;
    not_stepping = 1;  // Do not increment step counter on post pan redraw.
    glutSetCursor(GLUT_CURSOR_INHERIT);
    glutWarpPointer(Width/2, Height/2);
  }

  glutPostRedisplay();
}

/***************************************************************/
void
motion(int x, int y)
{
  GLdouble model[4*4];
  GLdouble proj[4*4];
  GLint view[4];
  GLdouble pan_x, pan_y, pan_z, x_angle, y_angle, angle, depth;
  GLdouble p_x, p_y;
  //int glutModifiers;  // Glut doesn't like this here!

  if (mouse_state == GLUT_DOWN && mouse_button == GLUT_LEFT_BUTTON) {
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, view);
    gluUnProject((GLdouble)x, (GLdouble)y, 1.0,
		 model, proj, view,
		 &pan_x, &pan_y, &pan_z);
    pan_y = -pan_y;

    if (ldraw_commandline_opts.debug_level == 1)
      printf("pan(%d, %d), -> (%0.2f, %0.2f, %0.2f)\n", x, y, pan_x, pan_y, pan_z);
    // NOTE:  This would be a nice place to rotate or pan the model.
    //        (If only the redisplay were more realtime)
    
    if (panning == 0)
    {
      // Add some hysteresis before we start panning.
      // NOTE:  This should really be done in screen coords, 
      // not world coords that have gone thru gluUnProject.
      // In fact the whole spinning thing is messed up when done 
      // in perspective mode (too fast) because I think I use the 
      // rear plane of the viewing volume for the z coord.
      // Should I use -1.0 for the z coord to get the front plane?
      if((fabs(pan_x -pan_start_x) < 4) && (fabs(pan_y -pan_start_y) < 4))
      {
	if (ldraw_commandline_opts.debug_level == 1)
	  printf("hysteresis = %0.2f, %0.2f\n", 
		 fabs(pan_x -pan_start_x), fabs(pan_y -pan_start_y));
	return;
      }
      // Save draw modes, then switch to wireframe and turn off studs.
      pan_start_zWire = zWire;

      pan_start_F = ldraw_commandline_opts.F;
      ldraw_commandline_opts.F = pan_visible; 
      zWire = (ldraw_commandline_opts.F & WIREFRAME_MODE);
    }
    glutSetCursor(GLUT_CURSOR_NONE);
    panning = 1;

    // Check for shift or ctrl mouse drag changes on the fly.
    //glutModifiers = glutGetModifiers(); // Glut doesn't like this here!
    if (glutModifiers & GLUT_ACTIVE_CTRL)
	ldraw_commandline_opts.F = (STUDLESS_MODE | SHADED_MODE);
    else if (glutModifiers & GLUT_ACTIVE_SHIFT)
	ldraw_commandline_opts.F = (BBOX_MODE | SHADED_MODE);
    else 
      ldraw_commandline_opts.F = pan_visible; 
    zWire = (ldraw_commandline_opts.F & WIREFRAME_MODE);

    if (ldraw_commandline_opts.F & INVISIBLE_MODE) // Draw the rubberband line
    {
      //printf("MOTION(%0.2f, %0.2f, %0.2f)\n", pan_x, -pan_y, pan_z);
      glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
      glEnable( GL_COLOR_LOGIC_OP ); 
      //glEnable( GL_LOGIC_OP_MODE ); 
      glLogicOp(GL_XOR);
      glColor3f(1.0, 1.0, 1.0); // white
      glBegin(GL_LINES);
      glVertex3f(pan_start_x, pan_start_y, pan_z);
      glVertex3f(pan_end_x, pan_end_y, pan_z);
      glEnd();
      glBegin(GL_LINES);
      glVertex3f(pan_start_x, pan_start_y, pan_z);
      glVertex3f(pan_x, pan_y, pan_z);
      glEnd();
      glLogicOp(GL_COPY);
      glEnable( GL_DEPTH_TEST ); 
      glDisable( GL_COLOR_LOGIC_OP ); 
      //glDisable( GL_LOGIC_OP_MODE ); 
      glFlush();

      pan_end_x = pan_x;
      pan_end_y = pan_y;
    }
    else    // Lets rotate the wireframe.
    {
      p_x = pan_x; //Save next pan_start coords.
      p_y = pan_y;

      // The LdrawOblique matrix is a projection matrix.
      // Substitute Oblique view matrix so we can rotate it.
      if (m_viewMatrix == LdrawOblique)
      {
	m_viewMatrix = Oblique;
	parse_view(m_viewMatrix);
      }

      // Convert from world coords across screen plane to angle thru origin.
#if WIDE_ANGLE_VIEW
      depth = 1000.0;
#else
      depth = 500.0;
#endif

      pan_x -= pan_start_x;
      pan_y -= pan_start_y;
      pan_y = -pan_y; //Beats me why.
      pan_z = max(fabs(pan_x), fabs(pan_y));
      angle = atan2(pan_z, depth);
      //angle *= -1.0;
      angle *= 5.0;
      angle *= (180.0/3.1415927);
      if (ldraw_commandline_opts.debug_level == 1)
	printf("ROTATING about(%0.2f, %0.2f) by angle %0.2f\n", pan_y, -pan_x, angle);
      rotate_about(pan_y, -pan_x, 0.0, angle );
      
      pan_start_x = p_x; //Set next pan_start coords.
      pan_start_y = p_y;

      glutPostRedisplay();
    }
  }

}

void filemenu(int);

/***************************************************************/
void menu(int item)
{
  if  (item == 1)
  {
    glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
    if (strcmp(filepattern, "*"))
    {
      strcpy(filepattern, "*");
      glutChangeToMenuEntry(3, "Filter - *.dat     ", 1);
    }
    else
    {
      strcpy(filepattern, "*.dat");
      glutChangeToMenuEntry(3, "Filter - All Files ", 1);
    }
    filemenu(15); // refresh the file list with files in new dir.
  }
  else
    keyboard((unsigned char)item, 0, 0);
}

/***************************************************************/
void colormenu(int c)
{
  ZCOLOR zc, zs;

  extern ZCOLOR_DEF_TABLE_ENTRY zcolor_table_default[];

  ldraw_commandline_opts.B = c;

#ifdef USE_L3_PARSER
  if (parsername == L3_PARSER)
    translate_color(c,&zc,&zs);
  else
#endif
  {
  // NOTE:  I dont understand the colortable stack used in translate_color()
  // so use the default color_table to avoid bad colors (bad stack ptr?).
  zc.r = zcolor_table_default[c].primary.r;
  zc.b = zcolor_table_default[c].primary.b;
  zc.g = zcolor_table_default[c].primary.g;
  }
  glClearColor(((float)zc.r)/255.0,((float)zc.g)/255.0,((float)zc.b)/255.0,0.0);
  if (ldraw_commandline_opts.debug_level == 1)
    printf("clearcolor %d = (%d, %d, %d)\n", c, zc.r, zc.g, zc.b);
  glutPostRedisplay();
}

/***************************************************************/
void dirmenu(int item)
{
  int         i, j, len1;
  const int   len3 = WORDLENGTH;
  char        myDir[len3+1];
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
  char        myDir[len3+1];
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
    {
      strcpy(datfilename, basename(DateiListe[item-1]));
      strcpy(datfilepath, dirname(DateiListe[item-1]));
      strcpy(dirfilepath, dirname(DateiListe[item-1]));

      // Change the title of the window to show the new dat filename.
      concat_path(datfilepath, datfilename, filename);
      if (filename[0] == '.') // I hate the ./filename thing.
	sprintf(title, "%s - %s", progname, datfilename);
      else
	sprintf(title, "%s - %s", progname, filename);
      glutSetWindowTitle(title);

#ifdef USE_L3_PARSER
      if (parsername == L3_PARSER)
        list_made = 0; // Gotta reparse the file.
#endif
      glutPostRedisplay();
    }
  }
}

/***************************************** myGlutIdle() ***********/
void myGlutIdle( void )
{
    char filename[256];
    static int init=0;
    static time_t last_file_time;
    struct stat datstat;
    int ret;

  /* According to the GLUT specification, the current window is
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
  if (ldraw_commandline_opts.V_x >= -1)
#endif
  if ( glutGetWindow() != main_window )
    glutSetWindow(main_window);

  if (ldraw_commandline_opts.rotate == 1) 
  {
    // rotate model
    twirl_angle += twirl_increment;
    if (twirl_angle >= 360.0)
      {
	// All done rotating
	ldraw_commandline_opts.rotate = 0;
	twirl_angle = 0.0;
	// If we just want the output files then quit when idle.
	if ((ldraw_commandline_opts.output == 1) ||
	    (ldraw_commandline_opts.M == 'S')) 
	  exit(0); //quit the program
	else
	  return;
      }
    //stepcount = 0;
    // This looks funny (not orthogonal) 
    // Perhaps cumulative round off errors in the modeling matrix?
#ifdef USE_GL_TWIRL
    // This is done with the GL model matrix.
#else
    rotate_model();
#endif
    glutPostRedisplay();
    return;
  }
  else if (ldraw_commandline_opts.poll == 1) 
  {
    // Gotta handle polling.
    // Check to see if timestamp on filename has changed.
    concat_path(datfilepath, datfilename, filename);
    ret = stat(datfilename, &datstat);
    if (!init) 
    {
      init = 1;
      printf("Polling %s = %d\n", filename, datstat.st_mtime);
    }
    else if (datstat.st_mtime != last_file_time)
    {
      printf("Reloading %s = %d\n", filename, last_file_time);
#ifdef USE_L3_PARSER
      if (parsername == L3_PARSER)
	list_made = 0; // Gotta reparse the file.
#endif
      glutPostRedisplay();
    }
    last_file_time = datstat.st_mtime;
  }

#if 0
  // NOTE:  This does NOT seem to wait till the display() fn is finished.
  // If we just want the output files then quit when idle.
  if ((ldraw_commandline_opts.output == 1) ||
      (ldraw_commandline_opts.M == 'S')) 
  {
    // Draw the last step.
    if (ldraw_commandline_opts.M == 'S')
      platform_step(INT_MAX, 0, -1, NULL);
    // quit the program
    exit(0);
  }
#endif
}

/***************************************************************/
// Stolen from the constructor in ldliteCommandLineInfo.cpp
void CldliteCommandLineInfo()
{
#if 0
  ldraw_commandline_opts.A.a=1.0;
  ldraw_commandline_opts.A.b=0.0;
  ldraw_commandline_opts.A.c=1.0;
  ldraw_commandline_opts.A.d=0.5;
  ldraw_commandline_opts.A.e=1.0;
  ldraw_commandline_opts.A.f=-0.5;
  ldraw_commandline_opts.A.g=-1.0;
  ldraw_commandline_opts.A.h=0.0;
  ldraw_commandline_opts.A.i=1.0;
#else
  parse_view(m_viewMatrix);
#endif
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

/***************************************************************/
// Mostly stolen from ParseParam() in ldliteCommandLineInfo.cpp
void ParseParams(int *argc, char **argv)
{
  char *pszParam;
  int i;

  char type;
  int mode;


  // Initialize datfilepath to none so we can take commands from stdin.
#if defined(UNIX)
  strcpy(datfilepath, "./");
  strcpy(datfilename, "./");
  
#elif defined(MAC)
  strcpy(datfilepath, "");
  strcpy(datfilename, "");
  
#elif defined(WINDOWS)
  strcpy(datfilepath, "");
  strcpy(datfilename, "");
  
#else
#error unspecified platform in ParseParams() definition
#endif
  
  strcpy(dirfilepath, datfilepath);

  for (i = 1; i < *argc; i++)
  {
    pszParam = argv[i];
    if (pszParam[0] != '-') 
    {
      // It must be a filename.  Save it for parsing.
      strcpy(datfilename, basename(argv[i]));
      strcpy(datfilepath, dirname(argv[i]));
      strcpy(dirfilepath, datfilepath);
#if 0
      chdir(datfilepath); // problem with chdir to dir with spaces in win32.
#endif
      // we used to put a trailing / on datfilepath
    }
    else
    {
      pszParam++; // skip over the dash char.

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
      case 'E':
      case 'e':
	sscanf(pszParam,"%c%f",&type,&z_line_offset);
	if (z_line_offset > 1.0)
	  z_line_offset = 1.0;
	break;
      case 'F':
      case 'f':
	{
	  char c;
	  sscanf(pszParam,"%c%c",&type,&c);
	  c = toupper(c);
	  switch (c) {
	  case 'H':
	    zShading = 1; // mnemonic = sHading? =Hi quality?
	    ldraw_commandline_opts.F |= SHADED_MODE;
	    break;
	  case 'S':
	    ldraw_commandline_opts.F |= STUDLESS_MODE;
	    break;
	  case 'W':
	    ldraw_commandline_opts.F |= WIREFRAME_MODE;
	    zWire = 1;
	    break;
	  case 'N':
	    zShading = 0; // mnemonic = normal (no shading)
	    zWire = 0;
	    ldraw_commandline_opts.F &= !(SHADED_MODE);
	    ldraw_commandline_opts.F &= !(WIREFRAME_MODE);
	    break;
	  }
	}
	break;
	break;
      case 'G':
      case 'g':
	ldraw_commandline_opts.debug_level = 1;
	break;
      case 'i':
      case 'I':
	sscanf(pszParam,"%c%d",&type,&ldraw_image_type);
	// Turn off cropping (image size = window size) if negative.
	if (ldraw_image_type < 0)
	{
	  ldraw_image_type *= -1;
	  cropping = 0;
	}
	break;
      case 'J':
      case 'j':
	ldraw_projection_type = 1;
	break;
      case 'L':
      case 'l':
	// -l3 forces l3 parser, -ld forces ldlite parser, -l sets logging.
	if (pszParam[1] == '3')
	  parsername = L3_PARSER;
	else if ((pszParam[1] == 'd') || (pszParam[1] == 'D'))
	  parsername = LDLITE_PARSER;
	else 
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
	ldraw_commandline_opts.poll= 1;
	break;
      case 'R':
      case 'r':
	sscanf(pszParam,"%c%s",&type, &output_file_name);
	ldraw_commandline_opts.output=1;
	printf("Save (%s)\n", output_file_name);
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
	case -2:
	  ldraw_commandline_opts.V_x=-2;
	  ldraw_commandline_opts.V_y=-2;
	  break;
	case -1:
	  ldraw_commandline_opts.V_x=-1;
	  ldraw_commandline_opts.V_y=-1;
	  break;
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
      case 'X':
      case 'x':
	drawAxis = 1;
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
}

/***************************************************************/
int InitInstance()
{
  // Probably should use basename(argv[0]) instead of "ldlite"
  // Big switch from ldlite.  Turn shading is on by default.
  zShading = GetProfileInt("ldlite","shading",1);
  //	zDetailLevel = TYPE_PART; // should checkmark the menu item later!
  zDetailLevel = GetProfileInt("ldlite","detail",TYPE_PART);
  zWire = GetProfileInt("ldlite","wireframe",0);
}

/***************************************************************/
// Called when user wants to open a new file
int setfilename(const char *newfile)
{
  strcpy(datfilename, basename(newfile));
  strcpy(datfilepath, dirname(newfile));
  strcpy(dirfilepath, datfilepath);
}


/***************************************************************/
int registerGlutCallbacks()
{
    // I think I have to reregister all of these again for gamemode.
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(fnkeys);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(myGlutIdle);
}

/***************************************************************/
int 
main(int argc, char **argv)
{
  char filename[256];
  int opts, view, colors, helpmenunum;
  int exitcode;
  char *str;
  
  platform_startup(&argc, &argv);
	
  // glutInit moved first, so that GL can have as many args as it can recognize
  // and because glutInit is what actually fills argv on the Mac.
  glutInit(&argc, argv);

  InitInstance();
  platform_setpath();
  CldliteCommandLineInfo();
  ParseParams(&argc, argv);

  Width = ldraw_commandline_opts.V_x;
  Height = ldraw_commandline_opts.V_y;

  if ((Width <= 0) || (Height <= 0))
  {
    if ((Width < 0) && (Height < 0))
    {
      //glutFullScreen();  // This shows no window decorations.
      Width = glutGet(GLUT_SCREEN_WIDTH);
      Height = ldraw_commandline_opts.V_y = glutGet(GLUT_SCREEN_HEIGHT);
    }   
    else
    {
      // get the screen size and subtract a fudge factor for window borders
      Width = ldraw_commandline_opts.V_x = glutGet(GLUT_SCREEN_WIDTH) - 8;
      Height = ldraw_commandline_opts.V_y = glutGet(GLUT_SCREEN_HEIGHT) - 32;
    }

    if ((Width <= 0) || (Height <= 0))
    {
      Width = ldraw_commandline_opts.V_x = 1280;
      Height = ldraw_commandline_opts.V_y = 1024;
    }
  }

#if 0
  sprintf(output_file_name, "%s.log", datfilename);
  output_file = fopen(output_file_name,"w+");
#endif

  strcpy(progname, basename(argv[0]));
  concat_path(datfilepath, datfilename, filename);
  if (filename[0] == '.') // I hate the ./filename thing.
    sprintf(title, "%s - %s", progname, datfilename);
  else
    sprintf(title, "%s - %s", progname, filename);

#ifdef USE_L3_PARSER
  // If the parser type is not specified on the commandline 
  // then set it here based on the program name.  
  if (parsername == UNKNOWN_PARSER)
  {
    parsername = LDLITE_PARSER; // default to ldlite parser.
    if ((stricmp("l3glite", progname) == 0) ||
	(stricmp("l3glite.exe", progname) == 0))
    {
      parsername = L3_PARSER;

      // The L3 parser fixes bowtie quads once during parsing.
      // and (I hope) turns concave quads into 2 triangles?
      // So I don't need my slow 3 triangle hack.
      use_quads = 1;
    }
  }
#endif

  //glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
#ifdef USE_DOUBLE_BUFFER
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glDrawBuffer(GL_FRONT_AND_BACK);  // Effectively disable double buffer.
#else
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
#endif

#if (GLUT_XLIB_IMPLEMENTATION >= 13)
  if (ldraw_commandline_opts.V_x < -1)
  {
    //glutGameModeString("800x600:16@60") ;
    glutGameModeString("640x480@60") ;
    // if the width is different to -1
    if (glutGameModeGet(GLUT_GAME_MODE_WIDTH) != -1)
    {
      glutEnterGameMode();		// enter full screen mode
    }
  }
  else
#endif
  {
    glutInitWindowSize(Width, Height);
    glutInitWindowPosition(0, 0);

    main_window = glutCreateWindow(title);

    if (ldraw_commandline_opts.V_x < 0)
    {
      ldraw_commandline_opts.V_x = Width;
      glutFullScreen();  // This shows no window decorations.
    }   
  }

  str = (char *) glGetString(GL_VERSION);
  printf("GL_VERSION = %s\n", str);

  str = (char *) glGetString(GL_EXTENSIONS);
  printf("GL_EXTENSIONS = %s\n", str);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(fnkeys);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutIdleFunc(myGlutIdle);

#if (GLUT_XLIB_IMPLEMENTATION >= 13)
// Rats, no menus in game mode.  Perhaps GLUI or PUI look good again.
  if (ldraw_commandline_opts.V_x >= -1)
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
  glutAddMenuEntry("Wireframe       ", 'w');
  glutAddMenuEntry("Normal          ", 'n');
  glutAddMenuEntry("Studs           ", 'f');
  glutAddMenuEntry("Visible spin    ", 'v');
  glutAddMenuEntry("Continuous      ", 'c');
  glutAddMenuEntry("Polling         ", 'g');
#ifdef USE_L3_PARSER
  glutAddMenuEntry("Parser          ", 'l');
#endif
  glutAddMenuEntry("                ", '\0');
  glutAddMenuEntry("Zoom out        ", 'z');
  glutAddMenuEntry("Zoom in         ", 'Z');
  glutAddMenuEntry("Scale dn        ", 's');
  glutAddMenuEntry("Scale up        ", 'S');

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
  glutAddMenuEntry("Version 0.7.2      ", '\0');

  mainmenunum = glutCreateMenu(menu);
  glutAddSubMenu(  "File               ", filemenunum);
  glutAddSubMenu(  "Folder             ", dirmenunum);
  glutAddMenuEntry("Filter - All Files ", 1);
  glutAddMenuEntry("                   ", '\0');
  glutAddSubMenu(  "View               ", view);
  glutAddSubMenu(  "Options            ", opts);
  glutAddSubMenu(  "BackGround Color   ", colors);
  glutAddMenuEntry("                   ", '\0');
  glutAddMenuEntry("Picture            ", 'P');
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

  initCamera();
  init();

  glutMainLoop();

  exitcode = 0; // no error
  
  return exitcode;
}


