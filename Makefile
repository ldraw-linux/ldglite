CC=gcc

# Comment these out to disable PNG output if no PNG lib is available.
#
PNG_FLAGS=-DUSE_PNG
PNG_LIBS=-L. -lpng -lz

# Comment these out to disable tile rendering option.
#
TR_FLAGS=-DTILE_RENDER_OPTION
TR_SRC=tr.c

# UnComment this to build in support for Mesa Offscreen rendering
OFFSCREEN_FLAGS=-DWIN_DIB_OPTION
OFFSCREEN_LIBS= -lgdi32

# Comment these out to disable the test GUI made with MUI.
#
#GUI_FLAGS=-DTEST_MUI_GUI
#GUI_SRC=ldglgui.c
#GUI_LIBS=-lmui

RES_SRC=ldglite.rc

# NOTE: -mwindows makes it detach from the console.
#       This is good for gui apps but bad if launched from dos
#       because we lose stdin.  Perhaps I should make 2 versions
#       or make it a makefile option.
#
CFLAGS=-ggdb -DUSE_OPENGL -DUSE_L3_PARSER -DUSE_BMP8 $(PNG_FLAGS) $(TR_FLAGS) $(OFFSCREEN_FLAGS) $(GUI_FLAGS)
#CFLAGS=-ggdb -DUSE_OPENGL -DUSE_L3_PARSER -DUSE_BMP8 $(PNG_FLAGS) $(TR_FLAGS) $(OFFSCREEN_FLAGS) $(GUI_FLAGS) -mwindows

AR = ar
RANLIB = ranlib

## This is lame.  I don't know how to do both .c and .cpp for the OBJS line
## so I pretend L3*.cpp is L3*.c and make rules for them later.
#
SRCS = ldliteVR_main.c platform.c dirscan.c gleps.c camera.c f00QuatC.c quant.c stub.c lcolors.c y.tab.c lex.yy.c qbuf.c main.c ldglpr.c glfont.c L3Edit.c L3Math.c L3Input.c L3View.c $(TR_SRC) 
OBJS = $(SRCS:.c=.o) $(RES_SRC:.rc=.o)

all	: ldglite

ldglite:   $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o ldglite.exe -I. $(PNG_LIBS) $(OFFSCREEN_LIBS) $(GUI_LIBS) -lglut32 -lglu32 -lopengl32
	cp ldglite.exe l3glite.exe
	cp ldglite.exe l3gledit.exe

l3glite:   ldglite

ldglitepng:   ldglite

l3glitepng:   ldglite

ldglite.o: ldglite.rc
	windres -i ldglite.rc -o ldglite.o

## This is lame.  I don't know how to do both .c and .cpp for the OBJS line.
#
L3Math.o: L3Math.cpp
	$(CC) -c $(CFLAGS) L3Math.cpp
L3Input.o: L3Input.cpp
	$(CC) -c $(CFLAGS) L3Input.cpp
L3View.o: L3View.cpp
	$(CC) -c $(CFLAGS) L3View.cpp

