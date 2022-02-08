CC=/usr/bin/gcc
OBJS=sl.c
TARGET=sl
LIBS=-lm

all:
	$(CC)  -o $(TARGET) $(OBJS) $(LIBS)
