#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#include "stdio.h"
#include "stdlib.h"
#include "ldliteVR.h"
#include "math.h"
#define NO_COMMENTS
#undef NO_LINES

char yyfilename[256];			/* Input filename */
char *ldraw_model=NULL;
int tmp_i;
void yyerror ( char * );
int stepcount=0;
char *mpd_subfile_name;

vector3d *current_translation[MAX_INCLUDE_DEPTH];
matrix3d *current_transform[MAX_INCLUDE_DEPTH];
int current_color[MAX_INCLUDE_DEPTH];
int current_type[MAX_INCLUDE_DEPTH];
int push_transform(vector3d *t,matrix3d *m);
void znamelist_push();
int print_transform(vector3d *t,matrix3d *m);
int is_current_file_not_cached(void);
void platform_comment(char *, int level);
int edge_color(int);
matrix3d * savemat(float a, float b, float c,
		   float d, float e, float f,
		   float g, float h, float i);

typedef union {
	char *c;
	int i;
	double d;
	vector3d *v;
	matrix3d *m;
} YYSTYPE;
#define tIDENT 257
#define tGARBAGE 258
#define tINT 259
#define tFLOAT 260
#define tZERO 261
#define tONE 262
#define tTWO 263
#define tTHREE 264
#define tFOUR 265
#define tFIVE 266
#define tSTEP 267
#define tPAUSE 268
#define tWRITE 269
#define tCLEAR 270
#define tSAVE 271
#define tEOL 272
#define tEOF 273
#define tROTATE 274
#define tTRANSLATE 275
#define tEND 276
#define tSCALE 277
#define tTRANSFORM 278
#define tCOLOR 279
#define tALIAS 280
#define tPOINT 281
#define tMATRIX 282
#define tFILE 283
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    8,    8,    9,    9,    9,    9,    9,    9,    9,
    9,    9,    9,    9,    9,    9,    9,    9,    9,    9,
    9,    9,    9,    9,    9,    9,    2,    2,    2,    2,
    2,    2,    2,    2,   10,   10,   11,   11,   12,   12,
   13,   13,   28,   28,   29,   29,   30,   30,   31,   31,
   32,   32,   14,   14,   20,   20,   21,   21,   22,   22,
   23,   23,   24,   24,   25,   25,   26,   26,   27,   27,
    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
    4,    4,    4,    4,    4,    4,    4,    3,    3,   34,
   34,   33,   33,    5,    5,    7,    7,    6,    6,    1,
   15,   15,   16,   16,   17,   17,   18,   18,   19,   19,
};
short yylen[] = {                                         2,
    1,    0,    2,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    3,    3,    3,    3,    1,    1,    1,
    1,    1,    1,    1,    2,    2,    1,    1,    1,    1,
    1,    1,    1,    1,    4,    4,    4,    4,    4,    4,
    4,    4,   14,   14,    5,    5,    5,    5,    5,    5,
    4,    4,    4,    4,    4,    4,    4,    4,    5,    5,
    4,    4,    4,    4,    4,    4,    5,    5,    4,    4,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    0,    2,    3,
    3,    1,    1,    1,    1,    9,    1,    3,    1,    1,
    6,    6,    5,    5,    6,    6,    7,    7,    7,    7,
};
short yydefred[] = {                                      2,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   92,
   93,    3,    4,    5,    6,    7,    8,    9,   10,   11,
   12,   13,    2,    2,    2,    2,   18,   19,   20,   21,
   22,   23,   24,   25,   26,   88,   88,   88,   88,   88,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   95,   33,   34,   27,   28,   29,   30,   31,   32,   94,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   99,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   71,   72,   74,   75,   76,
   77,   78,   90,   91,   80,   79,   81,   84,   85,   82,
   86,   83,   87,   73,   89,    0,    0,    0,    0,    0,
    0,   14,    0,   15,    0,   16,    0,   17,   35,   36,
   37,   38,   39,   40,   41,   42,   53,   54,    0,    0,
   55,   56,   63,   64,   97,    0,    0,    0,    0,    0,
    0,   51,   52,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   59,   60,   98,    0,   67,   68,    0,   45,
   46,   47,   48,   49,   50,  100,    0,  103,  104,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  101,  102,
  105,  106,    0,    0,   57,   58,   61,   62,   65,   66,
   69,   70,    0,    0,  107,  108,  109,  110,    0,    0,
    0,    0,    0,    0,    0,    0,   96,    0,    0,   43,
   44,
};
short yydgoto[] = {                                       1,
  167,   77,   50,  105,   61,   78,  137,    2,   12,   13,
   14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
  112,   24,  114,   25,  116,   26,  118,   27,   28,   29,
   30,   31,   32,   33,
};
short yysindex[] = {                                      0,
    0,  -98, -245,  150,  245,  245,  245,  245,  245,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -205,  255, -205,  255, -205, -227, -222, -220, -218,  -53,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  255,  255,  255,  255,  255,  -80,  106,  119,  132,  -27,
    1,   27,   53,   79,  255,    0, -205, -239, -226,  265,
 -209,  245,  255,  265, -199,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  265,  255,  255,  255,  255,
  167,    0,  184,    0,  201,    0,  218,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -182, -205,
    0,    0,    0,    0,    0, -205, -180, -205, -152, -150,
 -144,    0,    0, -195, -135,  255,  255,  255, -177, -149,
  147, -157,    0,    0,    0, -205,    0,    0, -205,    0,
    0,    0,    0,    0,    0,    0, -131,    0,    0, -129,
  255,  255, -125, -123, -120, -117, -205, -205,    0,    0,
    0,    0, -111, -103,    0,    0,    0,    0,    0,    0,
    0,    0, -205, -205,    0,    0,    0,    0, -205, -205,
 -205, -205, -205, -205, -205, -205,    0, -205,  -95,    0,
    0,
};
short yyrindex[] = {                                      0,
    0,   64,    0, -247,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -247,    0, -247,    0, -247,    0, -247,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,
};
short yygindex[] = {                                      0,
    0,   -5,  -16,    0,   -1,  -12,  -75,   18,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,
};
#define YYTABLESIZE 531
short yytable[] = {                                      60,
   60,   60,   60,   60,   62,   63,   64,   65,  141,   88,
   88,   88,   88,   88,   88,   88,   88,   88,   88,   70,
   71,   72,   73,   74,   88,   88,   34,   35,   88,   82,
  144,   80,  131,  132,   83,   75,   84,   79,   85,   81,
   66,   67,   68,   69,  104,  133,  134,  138,  106,  107,
  108,  109,  110,   52,   53,   54,   55,   56,   57,   58,
   59,  166,  129,    1,  104,  104,  104,  104,  104,    0,
  140,  130,  142,  143,  136,    0,   60,    0,  136,   76,
  139,   52,   53,   54,   55,   56,   57,   58,   59,  153,
  154,  157,  158,    0,  145,  146,  147,  148,  173,   76,
  136,   52,   53,   54,   55,   56,   57,   58,   59,   52,
   53,   54,   55,   56,   57,   58,   59,    0,  176,  160,
  161,  162,  163,    0,  155,    0,  174,  164,  165,    0,
  156,    0,  159,  170,  171,  172,  168,  169,    0,   80,
  179,  180,  181,  182,   75,   79,  185,  186,  187,  188,
  177,  189,  190,  178,  191,  192,    0,    3,  183,  184,
  195,  196,    4,    5,    6,    7,    8,    9,  197,  198,
    0,  193,  194,   10,   11,    3,  210,  211,    0,    0,
  111,    5,    6,    7,    8,    9,    0,  199,  200,    0,
    0,   10,   11,  201,  202,  203,  204,  205,  206,  207,
  208,    0,  209,   86,   87,   52,   53,   54,   55,   56,
   57,   58,   59,   88,   89,   90,   91,   92,   93,   94,
   95,   96,   97,   98,   99,  100,  101,  102,  103,   86,
   87,   52,   53,   54,   55,   56,   57,   58,   59,   88,
   89,   90,   91,   92,  119,  120,   95,   96,   97,   98,
   99,  100,  101,  102,  103,    0,    0,   86,   87,   52,
   53,   54,   55,   56,   57,   58,   59,   88,   89,   90,
   91,   92,  121,  122,   95,   96,   97,   98,   99,  100,
  101,  102,  103,   86,   87,   52,   53,   54,   55,   56,
   57,   58,   59,   88,   89,   90,   91,   92,  123,  124,
   95,   96,   97,   98,   99,  100,  101,  102,  103,   86,
   87,   52,   53,   54,   55,   56,   57,   58,   59,   88,
   89,   90,   91,   92,  125,  126,   95,   96,   97,   98,
   99,  100,  101,  102,  103,   86,   87,   52,   53,   54,
   55,   56,   57,   58,   59,   88,   89,   90,   91,   92,
  127,  128,   95,   96,   97,   98,   99,  100,  101,  102,
  103,    3,    0,    0,    0,    0,  113,    5,    6,    7,
    8,    9,    0,    0,    3,    0,    0,   10,   11,  115,
    5,    6,    7,    8,    9,    0,    0,    3,    0,    0,
   10,   11,  117,    5,    6,    7,    8,    9,    0,    0,
    0,    0,    0,   10,   11,   52,   53,   54,   55,   56,
   57,   58,   59,    0,    0,    0,   36,   37,   38,   39,
   40,    0,  175,   41,   42,    0,   43,   44,   45,   46,
   47,   48,   49,   36,   37,   38,   39,   40,    0,    0,
   41,  149,    0,   43,   44,   45,   46,   47,   48,   49,
   36,   37,   38,   39,   40,    0,    0,  150,   42,    0,
   43,   44,   45,   46,   47,   48,   49,   36,   37,   38,
   39,   40,    0,    0,   41,   42,    0,  151,   44,   45,
   46,   47,   48,   49,   36,   37,   38,   39,   40,    0,
    0,   41,   42,    0,   43,  152,   45,   46,   47,   48,
   49,   51,    0,   52,   53,   54,   55,   56,   57,   58,
   59,   76,    0,   52,   53,   54,   55,   56,   57,   58,
   59,  135,    0,   52,   53,   54,   55,   56,   57,   58,
   59,
};
short yycheck[] = {                                       5,
    6,    7,    8,    9,    6,    7,    8,    9,   84,  257,
  258,  259,  260,  261,  262,  263,  264,  265,  266,   36,
   37,   38,   39,   40,  272,  273,  272,  273,  276,  257,
  106,   44,  272,  273,  257,   41,  257,   43,  257,   45,
   23,   24,   25,   26,   50,  272,  273,  257,   61,   62,
   63,   64,   65,  259,  260,  261,  262,  263,  264,  265,
  266,  257,   75,    0,   70,   71,   72,   73,   74,   -1,
   83,   77,  272,  273,   80,   -1,   82,   -1,   84,  257,
   82,  259,  260,  261,  262,  263,  264,  265,  266,  272,
  273,  272,  273,   -1,  107,  108,  109,  110,  276,  257,
  106,  259,  260,  261,  262,  263,  264,  265,  266,  259,
  260,  261,  262,  263,  264,  265,  266,   -1,  276,  272,
  273,  272,  273,   -1,  130,   -1,  276,  272,  273,   -1,
  136,   -1,  138,  146,  147,  148,  272,  273,   -1,  152,
  272,  273,  272,  273,  150,  151,  272,  273,  272,  273,
  156,  272,  273,  159,  272,  273,   -1,  256,  171,  172,
  272,  273,  261,  262,  263,  264,  265,  266,  272,  273,
   -1,  177,  178,  272,  273,  256,  272,  273,   -1,   -1,
  261,  262,  263,  264,  265,  266,   -1,  193,  194,   -1,
   -1,  272,  273,  199,  200,  201,  202,  203,  204,  205,
  206,   -1,  208,  257,  258,  259,  260,  261,  262,  263,
  264,  265,  266,  267,  268,  269,  270,  271,  272,  273,
  274,  275,  276,  277,  278,  279,  280,  281,  282,  257,
  258,  259,  260,  261,  262,  263,  264,  265,  266,  267,
  268,  269,  270,  271,  272,  273,  274,  275,  276,  277,
  278,  279,  280,  281,  282,   -1,   -1,  257,  258,  259,
  260,  261,  262,  263,  264,  265,  266,  267,  268,  269,
  270,  271,  272,  273,  274,  275,  276,  277,  278,  279,
  280,  281,  282,  257,  258,  259,  260,  261,  262,  263,
  264,  265,  266,  267,  268,  269,  270,  271,  272,  273,
  274,  275,  276,  277,  278,  279,  280,  281,  282,  257,
  258,  259,  260,  261,  262,  263,  264,  265,  266,  267,
  268,  269,  270,  271,  272,  273,  274,  275,  276,  277,
  278,  279,  280,  281,  282,  257,  258,  259,  260,  261,
  262,  263,  264,  265,  266,  267,  268,  269,  270,  271,
  272,  273,  274,  275,  276,  277,  278,  279,  280,  281,
  282,  256,   -1,   -1,   -1,   -1,  261,  262,  263,  264,
  265,  266,   -1,   -1,  256,   -1,   -1,  272,  273,  261,
  262,  263,  264,  265,  266,   -1,   -1,  256,   -1,   -1,
  272,  273,  261,  262,  263,  264,  265,  266,   -1,   -1,
   -1,   -1,   -1,  272,  273,  259,  260,  261,  262,  263,
  264,  265,  266,   -1,   -1,   -1,  267,  268,  269,  270,
  271,   -1,  276,  274,  275,   -1,  277,  278,  279,  280,
  281,  282,  283,  267,  268,  269,  270,  271,   -1,   -1,
  274,  275,   -1,  277,  278,  279,  280,  281,  282,  283,
  267,  268,  269,  270,  271,   -1,   -1,  274,  275,   -1,
  277,  278,  279,  280,  281,  282,  283,  267,  268,  269,
  270,  271,   -1,   -1,  274,  275,   -1,  277,  278,  279,
  280,  281,  282,  283,  267,  268,  269,  270,  271,   -1,
   -1,  274,  275,   -1,  277,  278,  279,  280,  281,  282,
  283,  257,   -1,  259,  260,  261,  262,  263,  264,  265,
  266,  257,   -1,  259,  260,  261,  262,  263,  264,  265,
  266,  257,   -1,  259,  260,  261,  262,  263,  264,  265,
  266,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 283
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"tIDENT","tGARBAGE","tINT",
"tFLOAT","tZERO","tONE","tTWO","tTHREE","tFOUR","tFIVE","tSTEP","tPAUSE",
"tWRITE","tCLEAR","tSAVE","tEOL","tEOF","tROTATE","tTRANSLATE","tEND","tSCALE",
"tTRANSFORM","tCOLOR","tALIAS","tPOINT","tMATRIX","tFILE",
};
char *yyrule[] = {
"$accept : LdrawModel",
"LdrawModel : LdrawLines",
"LdrawLines :",
"LdrawLines : LdrawLines LdrawLine",
"LdrawLine : StepLine",
"LdrawLine : PauseLine",
"LdrawLine : WriteLine",
"LdrawLine : ClearLine",
"LdrawLine : SaveLine",
"LdrawLine : ObjectLine",
"LdrawLine : LineLine",
"LdrawLine : TriLine",
"LdrawLine : QuadLine",
"LdrawLine : FiveLine",
"LdrawLine : TranslateStartLine LdrawLines TranslateEndLine",
"LdrawLine : RotateStartLine LdrawLines RotateEndLine",
"LdrawLine : ScaleStartLine LdrawLines ScaleEndLine",
"LdrawLine : TransformStartLine LdrawLines TransformEndLine",
"LdrawLine : ColorLine",
"LdrawLine : ColorAliasLine",
"LdrawLine : PointAliasLine",
"LdrawLine : MatrixAliasLine",
"LdrawLine : MPDFileLine",
"LdrawLine : BlankLine",
"LdrawLine : CommentLine",
"LdrawLine : error tEOL",
"LdrawLine : error tEOF",
"Number : tZERO",
"Number : tONE",
"Number : tTWO",
"Number : tTHREE",
"Number : tFOUR",
"Number : tFIVE",
"Number : tINT",
"Number : tFLOAT",
"StepLine : tZERO tSTEP Words tEOL",
"StepLine : tZERO tSTEP Words tEOF",
"PauseLine : tZERO tPAUSE Words tEOL",
"PauseLine : tZERO tPAUSE Words tEOF",
"WriteLine : tZERO tWRITE Words tEOL",
"WriteLine : tZERO tWRITE Words tEOF",
"ClearLine : tZERO tCLEAR Words tEOL",
"ClearLine : tZERO tCLEAR Words tEOF",
"ColorLine : tZERO tCOLOR Number tIDENT Number Number Number Number Number Number Number Number Number tEOL",
"ColorLine : tZERO tCOLOR Number tIDENT Number Number Number Number Number Number Number Number Number tEOF",
"ColorAliasLine : tZERO tALIAS tIDENT Color tEOL",
"ColorAliasLine : tZERO tALIAS tIDENT Color tEOF",
"PointAliasLine : tZERO tPOINT tIDENT Position tEOL",
"PointAliasLine : tZERO tPOINT tIDENT Position tEOF",
"MatrixAliasLine : tZERO tMATRIX tIDENT TransMatrix tEOL",
"MatrixAliasLine : tZERO tMATRIX tIDENT TransMatrix tEOF",
"MPDFileLine : tZERO tFILE tIDENT tEOL",
"MPDFileLine : tZERO tFILE tIDENT tEOF",
"SaveLine : tZERO tSAVE Words tEOL",
"SaveLine : tZERO tSAVE Words tEOF",
"TranslateStartLine : tZERO tTRANSLATE Position tEOL",
"TranslateStartLine : tZERO tTRANSLATE Position tEOF",
"TranslateEndLine : tZERO tTRANSLATE tEND tEOL",
"TranslateEndLine : tZERO tTRANSLATE tEND tEOF",
"RotateStartLine : tZERO tROTATE Number Position tEOL",
"RotateStartLine : tZERO tROTATE Number Position tEOF",
"RotateEndLine : tZERO tROTATE tEND tEOL",
"RotateEndLine : tZERO tROTATE tEND tEOF",
"ScaleStartLine : tZERO tSCALE Number tEOL",
"ScaleStartLine : tZERO tSCALE Number tEOF",
"ScaleEndLine : tZERO tSCALE tEND tEOL",
"ScaleEndLine : tZERO tSCALE tEND tEOF",
"TransformStartLine : tZERO tTRANSFORM Position TransMatrix tEOL",
"TransformStartLine : tZERO tTRANSFORM Position TransMatrix tEOF",
"TransformEndLine : tZERO tTRANSFORM tEND tEOL",
"TransformEndLine : tZERO tTRANSFORM tEND tEOF",
"Word : tIDENT",
"Word : tGARBAGE",
"Word : Number",
"Word : tSTEP",
"Word : tPAUSE",
"Word : tWRITE",
"Word : tCLEAR",
"Word : tSAVE",
"Word : tTRANSLATE",
"Word : tROTATE",
"Word : tEND",
"Word : tCOLOR",
"Word : tPOINT",
"Word : tSCALE",
"Word : tTRANSFORM",
"Word : tALIAS",
"Word : tMATRIX",
"Words :",
"Words : Words Word",
"CommentLine : tZERO Words tEOL",
"CommentLine : tZERO Words tEOF",
"BlankLine : tEOL",
"BlankLine : tEOF",
"Color : Number",
"Color : tIDENT",
"TransMatrix : Number Number Number Number Number Number Number Number Number",
"TransMatrix : tIDENT",
"Position : Number Number Number",
"Position : tIDENT",
"PartName : tIDENT",
"ObjectLine : tONE Color Position TransMatrix PartName tEOL",
"ObjectLine : tONE Color Position TransMatrix PartName tEOF",
"LineLine : tTWO Color Position Position tEOL",
"LineLine : tTWO Color Position Position tEOF",
"TriLine : tTHREE Color Position Position Position tEOL",
"TriLine : tTHREE Color Position Position Position tEOF",
"QuadLine : tFOUR Color Position Position Position Position tEOL",
"QuadLine : tFOUR Color Position Position Position Position tEOF",
"FiveLine : tFIVE Color Position Position Position Position tEOL",
"FiveLine : tFIVE Color Position Position Position Position tEOF",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE

void yyerror(s)
  char *s;
{
  char buf[256];
  sprintf(buf,"Syntax error on line %d", yylineno);
  zWrite(buf);
}

vector3d * savevec(float x, float y, float z)
{
  vector3d *tmp;
  
#ifdef USE_QBUF_MALLOC
  tmp = (vector3d *)qbufGetPtr(vector_pool);
#else
  tmp = (vector3d *)malloc(sizeof(vector3d));
#endif
  tmp->x = x;
  tmp->y = y;
  tmp->z = z;
  return tmp;
}

vector3d * copyvec(vector3d *vec)
{
  vector3d *tmp;
  
#ifdef USE_QBUF_MALLOC
  tmp = (vector3d *)qbufGetPtr(vector_pool);
#else
  tmp = (vector3d *)malloc(sizeof(vector3d));
#endif
  *tmp = *vec;
  return tmp;
}

void transform_vec_inplace(vector3d *v)
{
  vector3d tmp;
  matrix3d *m;
  vector3d *t;
  m = current_transform[transform_stack_ptr];
  t = current_translation[transform_stack_ptr];
  tmp.x = m->a * v->x + m->b * v->y + m->c * v->z + t->x;
  tmp.y = m->d * v->x + m->e * v->y + m->f * v->z + t->y;
  tmp.z = m->g * v->x + m->h * v->y + m->i * v->z + t->z;
  *v = tmp;
  return;
}

void transform_mat_inplace(matrix3d *m)
{
  matrix3d newm;
  matrix3d *oldm;

  oldm = current_transform[transform_stack_ptr];

  newm.a = oldm->a * m->a + oldm->b * m->d + oldm->c * m->g;
  newm.b = oldm->a * m->b + oldm->b * m->e + oldm->c * m->h;
  newm.c = oldm->a * m->c + oldm->b * m->f + oldm->c * m->i;
  newm.d = oldm->d * m->a + oldm->e * m->d + oldm->f * m->g;
  newm.e = oldm->d * m->b + oldm->e * m->e + oldm->f * m->h;
  newm.f = oldm->d * m->c + oldm->e * m->f + oldm->f * m->i;
  newm.g = oldm->g * m->a + oldm->h * m->d + oldm->i * m->g;
  newm.h = oldm->g * m->b + oldm->h * m->e + oldm->i * m->h;
  newm.i = oldm->g * m->c + oldm->h * m->f + oldm->i * m->i;
  *m = newm;
  return;
}

int print_transform(vector3d *t,matrix3d *m)
{
  fprintf(stdout,"Translation: %g %g %g\n",t->x, t->y, t->z);
  fprintf(stdout,"Matrix:      %g %g %g\n",m->a, m->b, m->c);
  fprintf(stdout,"             %g %g %g\n",m->d, m->e, m->f);
  fprintf(stdout,"             %g %g %g\n",m->g, m->h, m->i);
  return 0;
}

void pop_transform()
{
  if(transform_stack_ptr < 0) {
	zWrite("Error in transform stack\n");
	exit(-1);
  }
#ifdef USE_QBUF_MALLOC
  qbufReleasePtr(vector_pool, (char *)current_translation[transform_stack_ptr]);
  qbufReleasePtr(matrix_pool, (char *)current_transform[transform_stack_ptr]);
#else
  free(current_transform[transform_stack_ptr]);
  free(current_translation[transform_stack_ptr]);
#endif
  transform_stack_ptr--;
}

int push_rotation(vector3d *axis, double degrees)
{
  // convert axis - degrees into rotation matrix
  matrix3d *m;
  double a,b,c,s, veclen_inv, sin2a;

	// convert axis and degrees into a quaternion
    veclen_inv = 1.0/sqrt(axis->x*axis->x + axis->y*axis->y + axis->z*axis->z);
	sin2a = sin((3.1415927*degrees)/360.0);
	a = sin2a * axis->x * veclen_inv;
	b = sin2a * axis->y * veclen_inv;
	c = sin2a * axis->z * veclen_inv;
	s = cos((3.1415927*degrees)/360.0);
	// convert quaternion into a rotation matrix.
#ifdef USE_QBUF_MALLOC
      m = (matrix3d *)qbufGetPtr(matrix_pool);
#else
      m = (matrix3d *)malloc(sizeof(matrix3d));
#endif
	m->a = (float)(1 - 2*b*b-2*c*c);
	m->b = (float)(2*a*b - 2*s*c);
	m->c = (float)(2*a*c + 2*s*b);
	m->d = (float)(2*a*b+2*s*c);
	m->e = (float)(1 - 2*a*a - 2*c*c);
	m->f = (float)(2*b*c - 2*s*a);
	m->g = (float)(2*a*c - 2*s*b);
	m->h = (float)(2*b*c + 2*s*a);
	m->i = (float)(1 - 2*a*a - 2*b*b);

	push_transform(NULL,m);
#ifdef USE_QBUF_MALLOC
  qbufReleasePtr(vector_pool, (char *)axis);
#else
  free(axis);
#endif
	return 0;
}

int push_transform(vector3d *t,matrix3d *m)
{
  vector3d *oldt;
  matrix3d *oldm; 
  
  if(transform_stack_ptr >= (MAX_TRANSFORM_DEPTH-1)) {
	zWrite("Transform stack overflow\n");
	exit(-1);
  }
  if (transform_stack_ptr < 0) {
	if (m == NULL) {
#ifdef USE_QBUF_MALLOC
      m = (matrix3d *)qbufGetPtr(matrix_pool);
#else
      m = (matrix3d *)malloc(sizeof(matrix3d));
#endif
	  m->a = 1.0; m->b = 0.0; m->c = 0.0;
	  m->d = 0.0; m->e = 1.0; m->f = 0.0;
	  m->g = 0.0; m->h = 0.0; m->i = 1.0;
	}
	if (t == NULL) {
#ifdef USE_QBUF_MALLOC
     t = (vector3d *)qbufGetPtr(vector_pool);
#else
     t= (vector3d *)malloc(sizeof(vector3d));
#endif
	 t->x = 0.0; t->y = 0.0; t->z = 0.0;
	}
  } else {
  oldm = current_transform[transform_stack_ptr];
  oldt = current_translation[transform_stack_ptr];
#if 0
  fprintf(stdout,"Transform %d:\n",transform_stack_ptr;
  print_transform(oldt, oldm);
  fprintf(stdout,"Transform current:\n");
  print_transform(t, m);
#endif
	if (m == NULL) {
#ifdef USE_QBUF_MALLOC
      m = (matrix3d *)qbufGetPtr(matrix_pool);
#else
      m = (matrix3d *)malloc(sizeof(matrix3d));
#endif
      *m = *oldm;
	} else {
      matrix3d newm;
      newm.a = oldm->a * m->a + oldm->b * m->d + oldm->c * m->g;
      newm.b = oldm->a * m->b + oldm->b * m->e + oldm->c * m->h;
      newm.c = oldm->a * m->c + oldm->b * m->f + oldm->c * m->i;
      newm.d = oldm->d * m->a + oldm->e * m->d + oldm->f * m->g;
      newm.e = oldm->d * m->b + oldm->e * m->e + oldm->f * m->h;
      newm.f = oldm->d * m->c + oldm->e * m->f + oldm->f * m->i;
      newm.g = oldm->g * m->a + oldm->h * m->d + oldm->i * m->g;
      newm.h = oldm->g * m->b + oldm->h * m->e + oldm->i * m->h;
      newm.i = oldm->g * m->c + oldm->h * m->f + oldm->i * m->i;
      *m = newm;
  }
  if (t == NULL) {  
#ifdef USE_QBUF_MALLOC
     t = (vector3d *)qbufGetPtr(vector_pool);
#else
     t= (vector3d *)malloc(sizeof(vector3d));
#endif
     *t = *oldt;
  } else {
      vector3d newt;
      newt.x = oldm->a * t->x + oldm->b * t->y + oldm->c * t->z + oldt->x;
      newt.y = oldm->d * t->x + oldm->e * t->y + oldm->f * t->z + oldt->y;
      newt.z = oldm->g * t->x + oldm->h * t->y + oldm->i * t->z + oldt->z;
      *t = newt;
  }
  }
  transform_stack_ptr++;
  current_transform[transform_stack_ptr] = m;
  current_translation[transform_stack_ptr] = t;
#if 0
  fprintf(stdout,"Transform %d:\n",transform_stack_ptr);
  print_transform(current_translation[transform_stack_ptr],
	          current_transform[transform_stack_ptr]);
#endif
  return 0;
}

matrix3d * savemat(float a, float b, float c,
		   float d, float e, float f,
		   float g, float h, float i)
{
  matrix3d *tmp;
  
#ifdef USE_QBUF_MALLOC
  tmp = (matrix3d *)qbufGetPtr(matrix_pool);
#else
  tmp = (matrix3d *)malloc(sizeof(matrix3d));
#endif
  tmp->a = a;
  tmp->b = b;
  tmp->c = c;
  tmp->d = d;
  tmp->e = e;
  tmp->f = f;
  tmp->g = g;
  tmp->h = h;
  tmp->i = i;
  return tmp;
}

matrix3d * copymat(matrix3d *mat)
{
  matrix3d *tmp;
  
#ifdef USE_QBUF_MALLOC
  tmp = (matrix3d *)qbufGetPtr(matrix_pool);
#else
  tmp = (matrix3d *)malloc(sizeof(matrix3d));
#endif
  *tmp = *mat;
  return tmp;
}
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 27:
{
			yyval.d = 0.0;
		}
break;
case 28:
{
			yyval.d = 1.0;
		}
break;
case 29:
{
			yyval.d = 2.0;
		}
break;
case 30:
{
			yyval.d = 3.0;
		}
break;
case 31:
{
			yyval.d = 4.0;
		}
break;
case 32:
{
			yyval.d = 5.0;
		}
break;
case 33:
{
			yyval.d = 1.0 * (yyvsp[0].i);
#ifndef NO_COMMENTS
			printf("tINT: %f\n",yyvsp[0].i);
#endif
		}
break;
case 34:
{
			yyval.d =1.0 * (yyvsp[0].d);
#ifndef NO_COMMENTS
			printf("tFLOAT: %f\n",yyvsp[0].d);
#endif
		}
break;
case 35:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
		  } else {
			zStep(stepcount,1);
			stepcount++;
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		}
break;
case 36:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
		  } else {
			zStep(stepcount,1);
			stepcount++;
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		  if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 37:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zPause();
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		}
break;
case 38:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zPause();
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 39:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zWrite(yyvsp[-1].c);
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		}
break;
case 40:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zWrite(yyvsp[-1].c);
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 41:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zClear();
		  }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		}
break;
case 42:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zClear();
		  }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 43:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d not supported: %s %g %s %g %g %g %g %g %g %g %g %g\n",
			  yyvsp[-13].i, yyvsp[-12].c, yyvsp[-11].d, yyvsp[-10].c, yyvsp[-9].d, yyvsp[-8].d, yyvsp[-7].d, yyvsp[-6].d, yyvsp[-5].d, yyvsp[-4].d, yyvsp[-3].d, yyvsp[-2].d, yyvsp[-1].d);
			/* alias the new color name to grey */
			zcolor_alias(7,yyvsp[-10].c);
	      } else {
			zcolor_modify((int)yyvsp[-11].d,yyvsp[-10].c,(int)yyvsp[-9].d,(int)yyvsp[-8].d,(int)yyvsp[-7].d,(int)yyvsp[-6].d,(int)yyvsp[-5].d,(int)yyvsp[-4].d,(int)yyvsp[-3].d,(int)yyvsp[-2].d,(int)yyvsp[-1].d);
          }
		}
break;
case 44:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d not supported: %s %g %s %g %g %g %g %g %g %g %g %g\n",
			  yyvsp[-13].i, yyvsp[-12].c, yyvsp[-11].d, yyvsp[-10].c, yyvsp[-9].d, yyvsp[-8].d, yyvsp[-7].d, yyvsp[-6].d, yyvsp[-5].d, yyvsp[-4].d, yyvsp[-3].d, yyvsp[-2].d, yyvsp[-1].d);
			/* alias the new color name to grey */
			zcolor_alias(7,yyvsp[-10].c);
	      } else {
			zcolor_modify((int)yyvsp[-11].d,yyvsp[-10].c,(int)yyvsp[-9].d,(int)yyvsp[-8].d,(int)yyvsp[-7].d,(int)yyvsp[-6].d,(int)yyvsp[-5].d,(int)yyvsp[-4].d,(int)yyvsp[-3].d,(int)yyvsp[-2].d,(int)yyvsp[-1].d);
          }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 45:
{
			zcolor_alias((int)yyvsp[-1].i,yyvsp[-2].c);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
		    }
		}
break;
case 46:
{
			zcolor_alias((int)yyvsp[-1].i,yyvsp[-2].c);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
		    }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 47:
{
			zpoint_alias(yyvsp[-2].c,yyvsp[-1].v);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
		    }
		}
break;
case 48:
{
			zpoint_alias(yyvsp[-2].c,yyvsp[-1].v);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
            }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 49:
{
			zmatrix_alias(yyvsp[-2].c,yyvsp[-1].m);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
		    }
		}
break;
case 50:
{
			zmatrix_alias(yyvsp[-2].c,yyvsp[-1].m);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
		    }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 51:
{
            if (cache_mpd_subfiles(yyvsp[-1].c) == 0) {
			  mpd_subfile_name = yyvsp[-1].c;
			} else {
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
            }
			YYACCEPT;
		}
break;
case 52:
{
            if (cache_mpd_subfiles(yyvsp[-1].c) == 0) {
			  mpd_subfile_name = yyvsp[-1].c;
			} else {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		      free(yyvsp[-1].c);
#endif
            }
			YYACCEPT;
		}
break;
case 53:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zSave(stepcount);
		  }
			stepcount++;
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		}
break;
case 54:
{
		  if (ldraw_commandline_opts.output == 1) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zSave(stepcount);
		  }
			stepcount++;
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 55:
{
		push_transform(yyvsp[-1].v, NULL);
		}
break;
case 56:
{
		push_transform(yyvsp[-1].v, NULL);
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 57:
{
		pop_transform();
		}
break;
case 58:
{
		pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 59:
{
		  /* axis degree representation */
		  push_rotation(yyvsp[-1].v, yyvsp[-2].d);
		}
break;
case 60:
{
		  /* axis degree representation */
		  push_rotation(yyvsp[-1].v, yyvsp[-2].d);
		  if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 61:
{
			pop_transform();
		}
break;
case 62:
{
		    pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 63:
{
		push_transform(NULL, savemat((float)(yyvsp[-1].d),0,0,0,(float)(yyvsp[-1].d),0,0,0,(float)(yyvsp[-1].d)));
		}
break;
case 64:
{
		push_transform(NULL, savemat((float)(yyvsp[-1].d),0,0,0,(float)(yyvsp[-1].d),0,0,0,(float)(yyvsp[-1].d)));
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 65:
{
		pop_transform();
		}
break;
case 66:
{
		pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 67:
{
		  push_transform(yyvsp[-2].v, yyvsp[-1].m);
		}
break;
case 68:
{
		  push_transform(yyvsp[-2].v, yyvsp[-1].m);
		  if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 69:
{
		pop_transform();
		}
break;
case 70:
{
		pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 71:
{
		  if (is_current_file_not_cached()) {
			yyval.c = yyvsp[0].c;
		  } else {
#ifdef USE_QBUF_MALLOC
		    yyval.c = (char *)qbufGetPtr(word_pool);
#else
	        yyval.c = malloc(128);
#endif
	        strcpy(yyval.c,yyvsp[0].c);
		  }
#if 0
		  fprintf(stderr,"Got word \"%s\"\n",yyvsp[0].c);
		  fflush(stderr);
#endif
		 }
break;
case 72:
{ 
		  if (is_current_file_not_cached()) {
			yyval.c = yyvsp[0].c;
		  } else {
#ifdef USE_QBUF_MALLOC
		    yyval.c = (char *)qbufGetPtr(word_pool);
#else
	        yyval.c = malloc(128);
#endif
	        strcpy(yyval.c,yyvsp[0].c);
		  }
#if 0
          fprintf(stderr,"Got garbage characters \"%s\"\n",yyvsp[0].c);
	      fflush(stderr);
#endif
		}
break;
case 73:
{ 
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  sprintf(yyval.c,"%g",yyvsp[0].d); 
#if 0
		  fprintf(stderr,"Got word \"%s\"\n",yyval.c);
		  fflush(stderr);
#endif
		}
break;
case 74:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 75:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 76:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 77:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 78:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 79:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 80:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 81:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 82:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 83:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 84:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 85:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 86:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 87:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		}
break;
case 88:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,"");
		}
break;
case 89:
{ 
		/* any Word string should be long enough hold all concatinated Words,*/
		/* so we do not need to malloc a new string, just strcat them.*/
		yyval.c = yyvsp[-1].c;
		strcat(yyvsp[-1].c, " ");
		strcat(yyvsp[-1].c, yyvsp[0].c);
#ifdef USE_QBUF_MALLOC
		  qbufReleasePtr(word_pool, (char *)yyvsp[0].c);
#else
		  free(yyvsp[0].c);
#endif
		}
break;
case 90:
{
#ifndef NO_COMMENTS
		printf("# %s\n",yyvsp[-1].c);
		fflush(stdout);
#endif
		if (ldraw_commandline_opts.output == 1) {
		  fprintf(output_file,"%d %s\n",yyvsp[-2].i, yyvsp[-1].c);
	    } else {
		  platform_comment(yyvsp[-1].c, include_stack_ptr);
        }
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		free(yyvsp[-1].c);
#endif
		}
break;
case 91:
{
#ifndef NO_COMMENTS
		printf("# %s\n",yyvsp[-1].c);
		fflush(stdout);
#endif
		if (ldraw_commandline_opts.output == 1) {
		  fprintf(output_file,"%d %s\n",yyvsp[-2].i, yyvsp[-1].c);
	    } else {
 		  platform_comment(yyvsp[-1].c, include_stack_ptr);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		free(yyvsp[-1].c);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 92:
{
		}
break;
case 93:
{
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 94:
{
			if (yyvsp[0].d == 16.0) {
			  yyval.i = current_color[include_stack_ptr];
			} else if (yyvsp[0].d == 24.0) {
			  yyval.i = edge_color(current_color[include_stack_ptr]);
			} else {
			  yyval.i = (int) yyvsp[0].d;
			}
		}
break;
case 95:
{
			yyval.i = zcolor_lookup(yyvsp[0].c);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[0].c);
#else
		      free(yyvsp[0].c);
#endif
		    }
		}
break;
case 96:
{
#if 1
			yyval.m = savemat(yyvsp[-8].d,yyvsp[-7].d,yyvsp[-6].d,yyvsp[-5].d,yyvsp[-4].d,yyvsp[-3].d,yyvsp[-2].d,yyvsp[-1].d,yyvsp[0].d);
#else
			yyval.m = savemat(yyvsp[0].d,yyvsp[-1].d,yyvsp[-2].d,yyvsp[-3].d,yyvsp[-4].d,yyvsp[-5].d,yyvsp[-6].d,yyvsp[-7].d,yyvsp[-8].d);
#endif
		}
break;
case 97:
{
		    yyval.m = zmatrix_lookup(yyvsp[0].c);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[0].c);
#else
		      free(yyvsp[0].c);
#endif
		    }
		}
break;
case 98:
{
			double a, b, c;
			vector3d *v;
			a = yyvsp[-2].d;
			b = yyvsp[-1].d;
			c = yyvsp[0].d;
			v = savevec(a,b,c);
			yyval.v = v;
#ifndef NO_COMMENTS
			printf("Position (%f, %f, %f)\n",v->x, v->y, v->z);
#endif
		}
break;
case 99:
{
		    yyval.v = zpoint_lookup(yyvsp[0].c);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[0].c);
#else
		      free(yyvsp[0].c);
#endif
		    }
		}
break;
case 100:
{
		  if (is_current_file_not_cached()) {
			yyval.c = yyvsp[0].c;
		  } else {
#ifdef USE_QBUF_MALLOC
		    yyval.c = (char *)qbufGetPtr(word_pool);
#else
	        yyval.c = malloc(128);
#endif
	        strcpy(yyval.c,yyvsp[0].c);
		  }
		}
break;
case 101:
{
		if (ldraw_commandline_opts.output == 1) {
  		  transform_vec_inplace(yyvsp[-3].v);
  		  transform_mat_inplace(yyvsp[-2].m);
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g %s\n",
		  yyvsp[-5].i, yyvsp[-4].i, (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z,
		  (yyvsp[-2].m)->a, (yyvsp[-2].m)->b, (yyvsp[-2].m)->c, (yyvsp[-2].m)->d, (yyvsp[-2].m)->e, (yyvsp[-2].m)->f, 
		  (yyvsp[-2].m)->g, (yyvsp[-2].m)->h, (yyvsp[-2].m)->i, yyvsp[-1].c);
	    } else {
	      if (start_include_file(yyvsp[-1].c) == 0) {
	  	  /* update transform matricies */
		  push_transform(yyvsp[-3].v, yyvsp[-2].m);
		  znamelist_push();
		  current_color[include_stack_ptr] = yyvsp[-4].i;
		  } else {
#ifndef NO_COMMENTS
		  printf("# Cannot find %s, ignoring.\n",yyvsp[-1].c);
#endif
		  }
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		}
break;
case 102:
{
		if (ldraw_commandline_opts.output == 1) {
  		  transform_vec_inplace(yyvsp[-3].v);
  		  transform_mat_inplace(yyvsp[-2].m);
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g %s\n",
		  yyvsp[-5].i, yyvsp[-4].i, (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z,
		  (yyvsp[-2].m)->a, (yyvsp[-2].m)->b, (yyvsp[-2].m)->c, (yyvsp[-2].m)->d, (yyvsp[-2].m)->e, (yyvsp[-2].m)->f, 
		  (yyvsp[-2].m)->g, (yyvsp[-2].m)->h, (yyvsp[-2].m)->i, yyvsp[-1].c);
	    } else {
	        if (start_include_file(yyvsp[-1].c) == 0) {
	  	  /* update transform matricies */
		  push_transform(yyvsp[-3].v, yyvsp[-2].m);
		  znamelist_push();
		  current_color[include_stack_ptr] = yyvsp[-4].i;
		  defer_stop_include_file();
		  } else {
#ifndef NO_COMMENTS
		  printf("# Cannot find %s, ignoring.\n",yyvsp[-1].c);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		  }
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		free(yyvsp[-1].c);
#endif
		}
break;
case 103:
{
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if (ldraw_commandline_opts.output == 1) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g\n",
		  yyvsp[-4].i, yyvsp[-3].i, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_line(yyvsp[-2].v, yyvsp[-1].v, yyvsp[-3].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		}
break;
case 104:
{
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if (ldraw_commandline_opts.output == 1) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g\n",
		  yyvsp[-4].i, yyvsp[-3].i, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_line(yyvsp[-2].v, yyvsp[-1].v, yyvsp[-3].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 105:
{
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if (ldraw_commandline_opts.output == 1) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-5].i, yyvsp[-4].i, (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z,
		  (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z, (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_triangle(yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-4].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		}
break;
case 106:
{
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if (ldraw_commandline_opts.output == 1) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-5].i, yyvsp[-4].i, (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z,
		  (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z, (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_triangle(yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-4].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 107:
{
		transform_vec_inplace(yyvsp[-4].v);
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if (ldraw_commandline_opts.output == 1) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-6].i, yyvsp[-5].i, (yyvsp[-4].v)->x, (yyvsp[-4].v)->y, (yyvsp[-4].v)->z,
		  (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
  		  render_quad(yyvsp[-4].v, yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-5].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-4].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-4].v);
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		}
break;
case 108:
{
		transform_vec_inplace(yyvsp[-4].v);
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if (ldraw_commandline_opts.output == 1) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-6].i, yyvsp[-5].i, (yyvsp[-4].v)->x, (yyvsp[-4].v)->y, (yyvsp[-4].v)->z,
		  (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_quad(yyvsp[-4].v, yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-5].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-4].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-4].v);
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
case 109:
{
		transform_vec_inplace(yyvsp[-4].v);
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if (ldraw_commandline_opts.output == 1) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-6].i, yyvsp[-5].i, (yyvsp[-4].v)->x, (yyvsp[-4].v)->y, (yyvsp[-4].v)->z,
		  (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_five(yyvsp[-4].v, yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-5].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-4].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-4].v);
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		}
break;
case 110:
{
		transform_vec_inplace(yyvsp[-4].v);
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if (ldraw_commandline_opts.output == 1) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-6].i, yyvsp[-5].i, (yyvsp[-4].v)->x, (yyvsp[-4].v)->y, (yyvsp[-4].v)->z,
		  (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_five(yyvsp[-4].v, yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-5].i);
	    }
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-4].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-4].v);
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
break;
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
