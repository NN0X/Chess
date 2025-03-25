CXX = clang++
CFLAGS = -O3 -Wall -Wextra -Wpedantic -I./include/
LDFLAGS = -L./lib/ -lGL -lGLEW -lglfw3
SRC = src
OUT = main

SRCS = $(shell find $(SRC) -name '*.cpp')

all:
	rm -f -r build
	mkdir build
	$(CXX) $(CFLAGS) $(SRCS) -o build/$(OUT) $(LDFLAGS) 2> build/make.log
	@echo "Build complete. Executable is located at build/$(OUT)"
