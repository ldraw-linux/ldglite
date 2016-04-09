CC=gcc

#CFLAGS=-ggdb
RM_F = rm -f

SRCS = pluginldlist.c
OBJS = $(SRCS:.c=.o)

all	: pluginldlist.dll

pluginldlist.dll: $(OBJS) pluginldlist.def
	dllwrap --def pluginldlist.def -o pluginldlist.dll pluginldlist.o --output-lib pluginldlist.lib
	
clean:
	$(RM_F) pluginldlist.dll
	$(RM_F) pluginldlist.lib
	$(RM_F) pluginldlist.o
