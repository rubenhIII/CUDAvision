PLATFORM ?= linux

CC = nvcc
SRC = prueba.cpp image_process_cuda.cu

ifeq ($(PLATFORM), linux)
    OUT = prueba
endif

ifeq ($(PLATFORM), windows)
    OUT = prueba.exe
endif

all:
	$(CC) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)