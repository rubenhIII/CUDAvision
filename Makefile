PLATFORM ?= linux
CUDA ?= n

CC = gcc
SRC = main.c tools/image_tools.c tools/harris.c tools/image_descriptors.c tools/image_process.c
OUT = main

NVCC = nvcc
SRC_CU = tools_cuda/example.cu

LOG_DIR := tests/logs/cppcheck
LOG_FILE := $(LOG_DIR)/cppcheck.log

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

cuda:
	NVCC_ARCH = -arch=sm_75
    NVCCFLAGS = -Xcompiler -std=c11 $(NVCC_ARCH)
    LIBS += -lcudart

	$(NVCC) -g $(NVCCFLAGS) -c $(SRC_CU)
	$(CC) -g $(SRC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $(OUT)

.PHONY: check

check:
	cppcheck --enable=warning,style,performance,portability \
		--inconclusive \
		--std=c11 \
		--suppress=*:tools/stb_image.h \
		tools/image_tools.c \
		2> tests/logs/cppcheck/cppcheck.log