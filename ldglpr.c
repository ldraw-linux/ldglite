 /*
 *  ldglpr.c   Module for printing pictures of ldlite .dat files
 *  Copyright (C) 2001  DMH
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

extern char buf[10240];
extern int use_uppercase;
extern int use_png_alpha;
extern ZIMAGE z;
extern GLint Width;
extern GLint Height;
extern int cropping;
extern char progname[256];

#ifdef OSMESA_OPTION
#include "GL/osmesa.h"
extern int OffScreenRendering;
extern void *OSbuffer;
extern OSMesaContext ctx;
#endif

char *pix;

/***************************************************************/
#define BYTE1(i) ((unsigned char) (i & 0x0ff))
#define BYTE2(i) ((unsigned char) ((i / 0x100) & 0x0ff))
#define BYTE3(i) ((unsigned char) ((i / 0x10000) & 0x0ff))
#define BYTE4(i) ((unsigned char) ((i / 0x1000000) & 0x0ff))

/***************************************************************/
FILE *start_bmp(char *filename, int width, int height)
{
  BITMAPINFOHEADER bmhbuf;
  BITMAPFILEHEADER bmfh;
  LPBITMAPINFOHEADER bmh;
  char *p, c;
  FILE *fp;
  char hdr[54];

  printf("Write BMP %s\n", filename);
  if ((fp = fopen(filename,"wb+"))==NULL) {
    printf("Could not open %s\n", filename);
    return(NULL);
  }
  
  //bmh = (LPBITMAPINFOHEADER) z.dib;
  bmh = (LPBITMAPINFOHEADER) &bmhbuf;
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
  // Copy the 54 bytes of bmfh and bmh into hdr in intel byte order & packing.
  memset(hdr, 0, 54);
  hdr[0] = 'B';
  hdr[1] = 'M';
  hdr[2] = BYTE1(bmfh.bfSize);
  hdr[3] = BYTE2(bmfh.bfSize);
  hdr[4] = BYTE3(bmfh.bfSize);
  hdr[5] = BYTE4(bmfh.bfSize);
  hdr[10] = BYTE1(bmfh.bfOffBits);
  hdr[11] = BYTE2(bmfh.bfOffBits);
  hdr[12] = BYTE3(bmfh.bfOffBits);
  hdr[13] = BYTE4(bmfh.bfOffBits);

  hdr[14] = 40;
  hdr[15] = 0;
  hdr[16] = 0;
  hdr[17] = 0;
  hdr[18] = BYTE1(bmh->biWidth);
  hdr[19] = BYTE2(bmh->biWidth);
  hdr[20] = BYTE3(bmh->biWidth);
  hdr[21] = BYTE4(bmh->biWidth);
  hdr[22] = BYTE1(bmh->biHeight);
  hdr[23] = BYTE2(bmh->biHeight);
  hdr[24] = BYTE3(bmh->biHeight);
  hdr[25] = BYTE4(bmh->biHeight);
  hdr[26] = 1;
  hdr[27] = 0;
  hdr[28] = 24;
  hdr[29] = 0;
  // The rest are all zeros.
  fwrite(hdr, 54, 1, fp);
#endif

  return(fp);
}

/***************************************************************/
void write_bmp(char *filename)
{
  int i, j;
  char *p, c;
  FILE *fp;

  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;
  pix = &buf[0];
  if (width > 2560)
    pix = (char*)malloc(width*3);

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

  fp = start_bmp(filename, width, height);
  if (fp == NULL)
    return;

  // no pallete since we use RGB

  glReadBuffer(GL_FRONT);
  for (i = 0; i < height; i++)
  {
#ifdef OSMESA_OPTION
    if (OffScreenRendering)
    {
      int j;
      char *b = (char *)OSbuffer;
      b += ((i+yoff)*Width +xoff) *4;
      for (j=0; j<width; j++) {
	// MESA or OSmesa bug?  ReadPixels gives GBR instead of RGB???
	pix[3*j] = b[1];
	pix[3*j+1] = b[0];
	pix[3*j+2] = b[2];
	b+=4;
      }
    }
    else
#endif
    {
      glReadPixels(xoff, i+yoff, width, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);
      p = pix;
      for (j = 0; j < width; j++) // RGB -> BGR
      {
        c = p[0];
#ifdef WINDOWS       
        p[0] = p[2];
        p[2] = c;
#else
	// MESA or OSmesa bug?  ReadPixels gives GBR instead of RGB???
        p[0] = p[1];
        p[1] = c;
#endif
        p+=3;
      }
    }
    fwrite(pix, width*3, 1, fp);
  }
  fclose(fp);

  if (width > 2560)
    free(pix);
}

/***************************************************************/
FILE *start_ppm(char *filename, int width, int height)
{
  char *p;
  FILE *fp;

  if ((p = strrchr(filename, '.')) != NULL)
    *p = 0;
  strcat(filename, use_uppercase ? ".PPM" : ".ppm");
  
  printf("Write PPM %s\n", filename);
  
  fp = fopen(filename, "wb");  // open in binary mode (to use unix \n chars)
  if (!fp) {
    printf("Couldn't open image file: %s\n", filename);
    return(NULL);
  }
  fprintf(fp,"P6\n");
  fprintf(fp,"# ppm-file created by %s\n", progname);
  fprintf(fp,"%i %i\n", width, height);
  fprintf(fp,"255\n"); // need unix \n here for some ppm interpreters.
  fclose(fp);
  fp = fopen(filename, "ab");  /* now append binary data */
  if (!fp) {
    printf("Couldn't append to image file: %s\n", filename);
    return(NULL);
  }

  return (fp);
}

/***************************************************************/
void write_ppm(char *filename)
{
  int i, j;

  FILE *fp;
  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;
  pix = &buf[0];
  if (width > 2560)
    pix = (char*)malloc(width*3);

  if (cropping)
  {
    xoff = max(0, z.extent_x1);
    yoff = max(0, z.extent_y1);
    width = min((z.extent_x2 - xoff), (Width - xoff));
    height = min((z.extent_y2 + 1 - yoff), (Height - yoff));
    //width = ((width + 3)/4) * 4; // round to a multiple of 4.
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bmpsize = (%d, %d) at (%d, %d)\n", width, height, xoff, yoff);
    if ((width <= 0) || (height <= 0)) return;
  }

  fp = start_ppm(filename, width, height);
  if (fp == NULL)
    return;

  glReadBuffer(GL_FRONT);
  // Write image rows
  //png_write_image(png_ptr, row_pointers);
  for (i = height-1; i >= 0; i--)
  {
#ifdef OSMESA_OPTION
    if (OffScreenRendering)
    {
      int j;
      char *b = (char *)OSbuffer;
      b += ((i+yoff)*Width +xoff) *4;
      for (j=0; j<width; j++) {
	pix[3*j] = b[0];
	pix[3*j+1] = b[1];
	pix[3*j+2] = b[2];
	b+=4;
      }
    }
    else
#endif
    glReadPixels(xoff, i+yoff, width, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);
    fwrite(pix, width*3, 1, fp);
  }

  fclose(fp);

  if (width > 2560)
    free(pix);
}

/***************************************************************/
void
write_targa(char *filename, const GLubyte *buffer, int width, int height)
{
  char *p;
  FILE *f;

  if ((p = strrchr(filename, '.')) != NULL)
    *p = 0;
  strcat(filename, use_uppercase ? ".TGA" : ".tga");

  f = fopen( filename, "w" );
  if (f) {
    int i, x, y;
    const GLubyte *ptr = buffer;
    printf ("Write TGA %s\n", filename);
    fputc (0x00, f);	/* ID Length, 0 => No ID	*/
    fputc (0x00, f);	/* Color Map Type, 0 => No color map included	*/
    fputc (0x02, f);	/* Image Type, 2 => Uncompressed, True-color Image */
    fputc (0x00, f);	/* Next five bytes are about the color map entries */
    fputc (0x00, f);	/* 2 bytes Index, 2 bytes length, 1 byte size */
    fputc (0x00, f);
    fputc (0x00, f);
    fputc (0x00, f);
    fputc (0x00, f);	/* X-origin of Image	*/
    fputc (0x00, f);
    fputc (0x00, f);	/* Y-origin of Image	*/
    fputc (0x00, f);
    fputc (Width & 0xff, f);      /* Image Width	*/
    fputc ((Width>>8) & 0xff, f);
    fputc (Height & 0xff, f);     /* Image Height	*/
    fputc ((Height>>8) & 0xff, f);
    fputc (0x18, f);		/* Pixel Depth, 0x18 => 24 Bits	*/
    fputc (0x20, f);		/* Image Descriptor	*/
    fclose(f);
    f = fopen( filename, "ab" );  /* reopen in binary append mode */
    for (y=height-1; y>=0; y--) {
      for (x=0; x<width; x++) {
	i = (y*width + x) * 4;
	fputc(ptr[i+2], f); /* write blue */
	fputc(ptr[i+1], f); /* write green */
	fputc(ptr[i], f);   /* write red */
      }
    }
    fclose(f);
  }
  else
    printf ("Failed to write tga %s\n", filename);
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

/***************************************************************/
FILE *start_png(char *filename, int width, int height,   
		png_structp *png_pp, png_infop *info_pp)
{
  char *p;

  png_structp png_ptr;
  png_infop info_ptr;
  jmp_buf jbuf;
  png_color_16 background;
  FILE *fp;

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
    return(NULL);
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    printf("No Memory", filename);
    return(NULL);
  }

  if(setjmp(jbuf)) {
    // we'll get here if an error occurred in any of the following
    // png_ functions
    printf("Aborting PNG file %s", filename);
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    if(fp) fclose(fp);
    return(NULL);
  }

  if ((fp = fopen(filename,"wb+"))==NULL) {
    printf("Could not open %s\n", filename);
    return(NULL);
  }
  
  png_init_io(png_ptr, fp);

  if ( use_png_alpha )
  {
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
  }
  else
  {
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, 
		 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  }

  // png_set_bgr(png_ptr);
  // png_set_pHYs(png_ptr, info_ptr, resolution_x, resolution_y, 1);

  png_write_info(png_ptr, info_ptr);
  
  *png_pp = png_ptr;
  *info_pp = info_ptr;
  return(fp);
}

/***************************************************************/
void write_png(char *filename)
{
  int i, j;

  png_structp png_ptr;
  png_infop info_ptr;
  png_text text_ptr[1];
  FILE *fp;

  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;
  pix = &buf[0];
  if (width > 2560)
  {
    if (use_png_alpha)
      pix = (char*)malloc(width*3);
    else
      pix = (char*)malloc(width*4);
  }

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
  
  fp = start_png(filename, width, height, &png_ptr, &info_ptr);
  if (fp == NULL)
    return;

  glReadBuffer(GL_FRONT);
  // Write image rows
  //png_write_image(png_ptr, row_pointers);
  for (i = height-1; i >= 0; i--)
  {
#ifdef OSMESA_OPTION
    if (OffScreenRendering)
    {
      int j;
      char *b = (char *)OSbuffer;
      b += ((i+yoff)*Width +xoff) *4;
      for (j=0; j<width; j++) {
	if (use_png_alpha){
	  pix[4*j] = b[0];
	  pix[4*j+1] = b[1];
	  pix[4*j+2] = b[2];
	  pix[4*j+3] = b[3];
	}
	else {
	  pix[3*j] = b[0];
	  pix[3*j+1] = b[1];
	  pix[3*j+2] = b[2];
	}
	b+=4;
      }
    }
    else
#endif
    if (use_png_alpha)
      glReadPixels(xoff, i+yoff, width, 1, GL_RGBA, GL_UNSIGNED_BYTE, pix);
    else
      glReadPixels(xoff, i+yoff, width, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);

    png_write_row(png_ptr, pix);
  }

  text_ptr[0].key = "Software";
  text_ptr[0].text = "LdGLite";
  text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
  png_set_text(png_ptr, info_ptr, text_ptr, 1);
  
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);

  if (width > 2560)
    free(pix);
}
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
  char hdr[54];

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
  // Copy the 54 bytes of bmfh and bmh into hdr in intel byte order & packing.
  memset(hdr, 0, 54);
  hdr[0] = 'B';
  hdr[1] = 'M';
  hdr[2] = BYTE1(bmfh.bfSize);
  hdr[3] = BYTE2(bmfh.bfSize);
  hdr[4] = BYTE3(bmfh.bfSize);
  hdr[5] = BYTE4(bmfh.bfSize);
  hdr[10] = BYTE1(bmfh.bfOffBits);
  hdr[11] = BYTE2(bmfh.bfOffBits);
  hdr[12] = BYTE3(bmfh.bfOffBits);
  hdr[13] = BYTE4(bmfh.bfOffBits);

  hdr[14] = 40;
  hdr[15] = 0;
  hdr[16] = 0;
  hdr[17] = 0;
  hdr[18] = BYTE1(bmh.biWidth);
  hdr[19] = BYTE2(bmh.biWidth);
  hdr[20] = BYTE3(bmh.biWidth);
  hdr[21] = BYTE4(bmh.biWidth);
  hdr[22] = BYTE1(bmh.biHeight);
  hdr[23] = BYTE2(bmh.biHeight);
  hdr[24] = BYTE3(bmh.biHeight);
  hdr[25] = BYTE4(bmh.biHeight);
  hdr[26] = 1;
  hdr[27] = 0;
  hdr[28] = 8;
  hdr[29] = 0;
  hdr[46] = BYTE1(bmh.biClrUsed);
  hdr[47] = BYTE2(bmh.biClrUsed);
  hdr[48] = BYTE3(bmh.biClrUsed);
  hdr[49] = BYTE4(bmh.biClrUsed);
  hdr[50] = BYTE1(bmh.biClrImportant);
  hdr[51] = BYTE2(bmh.biClrImportant);
  hdr[52] = BYTE3(bmh.biClrImportant);
  hdr[53] = BYTE4(bmh.biClrImportant);
  // The rest are all zeros.
  fwrite(hdr, 54, 1, fp);
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
#ifdef OSMESA_OPTION
  if (OffScreenRendering)
  {
    int i;
    pix = data;
    for (i = 0; i < height; i++)
    {
      int j;
      char *b = (char *)OSbuffer;
      b += ((i+yoff)*Width +xoff) *4;
      for (j=0; j<width; j++) {
	pix[3*j] = b[0];
	pix[3*j+1] = b[1];
	pix[3*j+2] = b[2];
	b+=4;
      }
      pix += width*3;
    }
  }
  else
#endif
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

