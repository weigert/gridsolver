CC = g++ -std=c++14
COMPILER_FLAGS = -Wall -fopenmp
LINKER_FLAGS = -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -lnoise -lfftw

all: source/tectonics.cpp
			$(CC) source/tectonics.cpp $(COMPILER_FLAGS) $(LINKER_FLAGS) -o tectonics
