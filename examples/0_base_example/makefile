CC = g++ -std=c++17
COMPILER_FLAGS = -Wall
OBJS = gridsolve.cpp
TARGET = gridsolve

#Flags only required if using solver
SOLVER_FLAGS = -lfftw

#Target All
all: $(OBJS)
			$(CC) $(OBJS) $(COMPILER_FLAGS) $(SOLVER_FLAGS) -o $(TARGET)
