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
#ifndef LDLITEVR_H
#define LDLITEVR_H

#include "stdio.h"

#define LDRAW_COMPATIBLE_CENTER  // define to have origin two-thirds of the way down
								 // the screen, rather than at the center.

typedef struct vector3d_struct {
	float x;
	float y;
	float z;
} vector3d;

typedef struct matrix3d_struct {
	float a;
	float b;
	float c;
	float d;
	float e;
	float f;
	float g;
	float h;
	float i;
} matrix3d;

extern int yylineno;
extern char yyfilename[];
extern int include_stack_ptr;
extern int transform_stack_ptr;
extern int stepcount;
extern int zShading;
extern int zDetailLevel;
extern int zWire;
extern char *mpd_subfile_name;

char * strsave(char *);

#define MAX_INCLUDE_DEPTH 64
#define MAX_ZCOLOR_DEPTH  64
#define MAX_TRANSFORM_DEPTH 128

vector3d * savevec(float x, float y, float z);

matrix3d * savemat(float a, float b, float c,
		   float d, float e, float f,
		   float g, float h, float i);

extern vector3d *current_translation[MAX_INCLUDE_DEPTH];
extern matrix3d *current_transform[MAX_INCLUDE_DEPTH];
extern int current_color[MAX_INCLUDE_DEPTH];
#define TYPE_P 1
#define TYPE_PART 2
#define TYPE_MODEL 4
#define TYPE_OTHER 8
extern int current_type[MAX_INCLUDE_DEPTH];

vector3d* zpoint_lookup(char *name);
matrix3d* zmatrix_lookup(char *name);

void render_line(vector3d *p1, vector3d *p2, int c);
void render_triangle(vector3d *p1, vector3d *p2, vector3d *p3, int c);
void render_quad(vector3d *p1, vector3d *p2, vector3d *p3, vector3d *p4, int c);
void render_five(vector3d *p1, vector3d *p2, vector3d *p3, vector3d *p4, int c);

int zGetRowsize();
int zGetColsize();
void zStep(int, int);
void zPause(void);
void zWrite(char *);
void zClear(void);
void zSave(int);
int zcolor_lookup(char *name);

typedef struct zimage_struct {
	int *zbuffer;
	unsigned char *r;
	unsigned char *g;
	unsigned char *b;
	int rows;
	int cols;
	int dirty_x1;
	int dirty_x2;
	int dirty_y1;
	int dirty_y2;
	int extent_x1;
	int extent_x2;
	int extent_y1;
	int extent_y2;
	unsigned char *dib;
} ZIMAGE;

typedef struct zpoint_struct {
	int x;
	int y;
	int z;
} ZPOINT;

typedef struct zplane_struct {
	float A;
	float B;
	float C;
	float D;
} ZPLANE;

typedef struct zcolor_struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;  // alpha.  255 == solid, 0 = transparent
} ZCOLOR;

#define ZCOLOR_TABLE_DEFAULT_SIZE 64
#define ZCOLOR_TABLE_SIZE 256

typedef struct zcolor_def_table_entry_struct {
	char *name;
	int inverse_index;
	ZCOLOR primary;
	ZCOLOR dither;
} ZCOLOR_DEF_TABLE_ENTRY;

typedef struct zcolor_table_entry_struct {
	int inverse_index;
	ZCOLOR primary;
	ZCOLOR dither;
} ZCOLOR_TABLE_ENTRY;

// ldraw command line options
typedef struct {
	matrix3d A;  // transform matrix
	int B;       // background color
	int F;		// fast drawing (ignored)
	char M;     // drawing mode {P|C|S}
	vector3d O; // set x and y position (z unused)
	float S;    // scale factor
	int V_x; // video mode
	int V_y;
	int C; // default part color
    // non-LDRAW values
	int poll; // watch file for changes.
	int output; // emit LDRAW, POV or Rayshade code
	int rotate; // spin the model after rendering
	int debug_level; // 0==no messages, 1 == all
	char log_output; // 0==no logging, 1==log output to file.
	int Z; // Z distance of viewpoint, in scaled units
} LDRAW_COMMANDLINE_OPTS;

extern LDRAW_COMMANDLINE_OPTS ldraw_commandline_opts;

#define USE_QBUF_MALLOC
#ifdef USE_QBUF_MALLOC
#include "qbuf.h"
extern QBUF_ID *vector_pool;
extern QBUF_ID *matrix_pool;
extern QBUF_ID *word_pool;
#endif

#define Z_SCALE_FACTOR 16 // exagerate z axis to get more resolution.
#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#define INT_MIN 0x80000000
#endif
#ifndef max
#define max(a,b) ((a > b) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) ((a > b) ? (b) : (a))
#endif

extern char pathname[256]; // base directory for ldraw files
extern FILE *output_file;
extern char output_file_name[256];
vector3d * copyvec(vector3d *vec);
matrix3d * copymat(matrix3d *mat);

void platform_comment(char *comment_string, int level);
int cache_mpd_subfiles(char *mpd_subfile_name);
#endif


