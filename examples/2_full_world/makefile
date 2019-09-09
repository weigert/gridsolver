CC = g++ -std=c++17
COMPILER_FLAGS = -Wall -fopenmp
OBJS = worldgen.cpp
TARGET = worldgen

#Solver Flags
SOLVER_FLAGS = -lfftw

#Render Stuff (only include if you use the renderer)
RENDER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lGL -lGLEW -lboost_system -lboost_filesystem
IMGUI = ../../source/render/imgui/imgui.cpp ../../source/render/imgui/imgui_demo.cpp ../../source/render/imgui/imgui_draw.cpp ../../source/render/imgui/imgui_widgets.cpp ../../source/render/imgui/imgui_impl_opengl3.cpp ../../source/render/imgui/imgui_impl_sdl.cpp

#Flags for this specific program
LINKER_FLAGS = -I/usr/local/include -L/usr/local/lib -L/usr/X11R6/lib -lm -lpthread -lX11 -lnoise  -O2

#Target All
all: $(OBJS)
			$(CC) $(OBJS) $(IMGUI) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(RENDER_FLAGS) $(SOLVER_FLAGS) -o $(TARGET)
