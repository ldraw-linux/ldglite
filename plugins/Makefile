CC=gcc

#CFLAGS=-ggdb

SRCS = pluginldlist.c
OBJS = $(SRCS:.c=.o)

all	: pluginldlist.dll

pluginldlist.dll: $(OBJS) pluginldlist.def
	dllwrap --def pluginldlist.def -o pluginldlist.dll pluginldlist.o --output-lib pluginldlist.lib


