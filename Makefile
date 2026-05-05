PLATFORM ?= linux

CC = gcc
SRC = main.c tools/image_tools.c tools/harris.c
OUT = main

ifeq ($(PLATFORM), linux)
    LIBS = -lX11 -lasound -lm
endif

ifeq ($(PLATFORM), windows)
    OUT = main.exe
    CFLAGS = -std=c11 -Itools/SDL3/include
    LDFLAGS = -Ltools/SDL3/lib
    LIBS = -lSDL3 -lgdi32 -lm
endif

all:
	$(CC) -g $(SRC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $(OUT)
