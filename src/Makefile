CC ?= gcc
CFLAGS ?= -O3 -Wall -I../3rd/libjpeg/include -I../3rd/quirc/include
LDFLAGS ?= -L../3rd/libjpeg/lib64 -L../3rd/quirc/lib

all: decoder

decoder: MainEntry.o
	$(CC) $(LDFLAGS) -Wall -o decoder MainEntry.o -l:libjpeg.a -l:libquirc.a

MainEntry.o: MainEntry.c
	$(CC) $(CFLAGS) -g -Wall -c MainEntry.c

clean:
	rm -f decoder
	rm -f *.o
