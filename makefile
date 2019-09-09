CC = g++ -std=c++14
COMPILER_FLAGS = -Wall -fopenmp
OBJS = source/worldgen.cpp source/include/tools/imgui/imgui.cpp source/include/tools/imgui/imgui_demo.cpp source/include/tools/imgui/imgui_draw.cpp source/include/tools/imgui/imgui_widgets.cpp source/include/tools/imgui/imgui_impl_opengl3.cpp source/include/tools/imgui/imgui_impl_sdl.cpp
LINKER_FLAGS = -O2 -I/usr/local/include -L/usr/local/lib -L/usr/X11R6/lib -lm -lpthread -lX11 -lnoise -lfftw -lconfig++ -lSDL2 -lSDL2_image -lSDL2_ttf -lGL -lGLEW -lboost_system -lboost_filesystem

all: $(OBJS)
			$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o worldgen
