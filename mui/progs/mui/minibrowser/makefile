CC=gcc

CFLAGS=-ggdb -DUSE_OPENGL 
#CFLAGS=-ggdb -DUSE_OPENGL -mwindows

# NOTE: -mwindows makes it detach from the console.
#       This is good for gui apps but bad if launched from dos
#       because we lose stdin.  Perhaps I should make 2 versions
#       or make it a makefile option.
#

AR = ar
RANLIB = ranlib

SRCS = gizmo.c glutmui.c displaylist.c uicolor.c miscui.c mui.c vslider.c textlist.c button.c pulldown.c hslider.c
OBJS = $(SRCS:.c=.o)

all	: libmui.a mui_test.exe browser.exe

libmui_a_OBJECTS = button.o glutmui.o mui.o uicolor.o displaylist.o \
hslider.o pulldown.o vslider.o gizmo.o miscui.o textlist.o

libmui.a: $(libmui_a_OBJECTS) 
	-rm -f libmui.a
	$(AR) cru libmui.a $(libmui_a_OBJECTS)
	$(RANLIB) libmui.a

mui_test.exe	: mui_test.o $(OBJS)
	$(CC) $(CFLAGS) -o mui_test.exe mui_test.o $(OBJS) -lglut32 -lglu32 -lopengl32

mui_test.o: mui_test.c
	$(CC) -c $(CFLAGS) mui_test.c

browser.exe	: browseparse.o browser.o $(OBJS)
	$(CC) $(CFLAGS) -o browser.exe browser.o browseparse.o $(OBJS) -lglut32 -lglu32 -lopengl32

browser.o: browser.c
	$(CC) -c $(CFLAGS) browser.c

browseparse.o: browseparse.c
	$(CC) -c $(CFLAGS) browseparse.c

