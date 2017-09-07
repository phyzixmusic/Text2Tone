# Makefile for wa2aiff 

INCLUDES = -I ./include
LIBS = -L./lib -lportsf -lm
PORTSF = ./lib/libportsf.a
CC = gcc

all: w0r23

w0r23:	$(PORTSF) Final_Assessment2.c
	$(CC) -o w0r23 Final_Assessment2.c $(INCLUDES) $(LIBS)

$(PORTSF):
	cd portsf; make; make install

