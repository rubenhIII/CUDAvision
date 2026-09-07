PLATFORM ?= linux

CC = nvcc
SRC = prueba.cpp match_visualization.c image_process_cuda.cu harris_cuda.cu image_descriptor_cuda.cu

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