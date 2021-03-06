CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu99 -fPIC

PLUGIN_SRCS = swapbg.c expose.c mirrorv.c mirrorh.c tile.c

SRCS = imgproc.cpp pnglite.c $(PLUGIN_SRCS)

OBJS = $(SRCS:%.c=%.o)
PLUGINS = $(PLUGIN_SRCS:%.c=plugins/%.so)

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

plugins/%.so : %.o
	mkdir -p plugins
	$(CC) -o plugins/$*.so -shared $*.o

all : imgproc $(PLUGINS)

imgproc : imgproc.o image.o pnglite.o
	g++ -export-dynamic -o $@ imgproc.o image.o pnglite.o -lz -ldl

plugins/swapbg.so : swapbg.o

clean :
	rm -f *.o imgproc plugins/*.so depend.mak

# Running
#    make depend
# will automatically generate header file dependencies.
depend :
	g++ -g -Wall -Wextra -pedantic -std=c++11 -fPIC -M $(SRCS) >> depend.mak

depend.mak :
	touch $@

include depend.mak
