all: main.c
	gcc main.c tools/image_tools.c -lX11 -lasound -lm -o main
