CC=gcc

# Comment these out to disable PNG output if no PNG lib is available.
#
PNG_FLAGS=-DUSE_PNG 
PNG_LIBS=-L. -lpng -lz 

# NOTE: -mwindows makes it detach from the console.
#       This is good for gui apps but bad if launched from dos
#       because we lose stdin.  Perhaps I should make 2 versions
#       or make it a makefile option.
#
CFLAGS=-ggdb -DUSE_OPENGL -DUSE_L3_PARSER $(PNG_FLAGS)
#CFLAGS=-ggdb -DUSE_OPENGL -DUSE_L3_PARSER $(PNG_FLAGS) -mwindows

all	: ldglite

ldglite:   ldliteVR_main.o platform.o dirscan.o camera.o f00QuatC.o quant.o stub.o y.tab.o lex.yy.o qbuf.o main.o L3Math.o L3Input.o L3View.o
	$(CC) $(CFLAGS) ldliteVR_main.o platform.o dirscan.o camera.o f00QuatC.o quant.o stub.o y.tab.o lex.yy.o qbuf.o main.o L3Math.o L3Input.o L3View.o -o ldglite.exe -I. $(PNG_LIBS) -lglut32 -lglu32 -lopengl32
	cp ldglite.exe l3glite.exe

l3glite:   ldglite

ldglitepng:   ldglite

l3glitepng:   ldglite

ldliteVR_main.o: ldliteVR_main.c
	$(CC) -c $(CFLAGS) ldliteVR_main.c

main.o: main.c
	$(CC) -c $(CFLAGS) main.c

stub.o: stub.c
	$(CC) -c $(CFLAGS) stub.c

platform.o: platform.c
	$(CC) -c $(CFLAGS) platform.c

camera.o: f00QuatC.h camera.c
	$(CC) -c $(CFLAGS) camera.c

f00QuatC.o: f00QuatC.h
	$(CC) -c $(CFLAGS) f00QuatC.c

quant.o: quant.c
	$(CC) -c $(CFLAGS) quant.c

y.tab.o: y.tab.c
	$(CC) -c $(CFLAGS) y.tab.c

lex.yy.o: lex.yy.c
	$(CC) -c $(CFLAGS) lex.yy.c

qbuf.o: qbuf.c
	$(CC) -c $(CFLAGS) qbuf.c

dirscan.o: dirscan.c
	$(CC) -c $(CFLAGS) dirscan.c

L3Math.o: L3Math.cpp
	$(CC) -c $(CFLAGS) L3Math.cpp
L3Input.o: L3Input.cpp
	$(CC) -c $(CFLAGS) L3Input.cpp
L3View.o: L3View.cpp
	$(CC) -c $(CFLAGS) L3View.cpp

