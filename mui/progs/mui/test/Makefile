CC=gcc

CFLAGS=-ggdb -DUSE_OPENGL 
#CFLAGS=-ggdb -DUSE_OPENGL -mwindows

# NOTE: -mwindows makes it detach from the console.
#       This is good for gui apps but bad if launched from dos
#       because we lose stdin.  Perhaps I should make 2 versions
#       or make it a makefile option.
#
mui_test:   mui_test.o
	$(CC) $(CFLAGS) mui_test.o -o mui_test.exe -lmui -lglut32 -lglu32 -lopengl32

mui_test.o: mui_test.c
	$(CC) -c $(CFLAGS) mui_test.c

