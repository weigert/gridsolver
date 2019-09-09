CC = g++ -std=c++17
COMPILER_FLAGS = -Wall
OBJS = rendersolve.cpp
TARGET = rendersolve

#Solver Stuff
SOLVER_FLAGS = -lfftw

#Renderer Stuff
RENDER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lGL -lGLEW -lboost_system -lboost_filesystem
IMGUI = ../../source/render/imgui/imgui.cpp ../../source/render/imgui/imgui_demo.cpp ../../source/render/imgui/imgui_draw.cpp ../../source/render/imgui/imgui_widgets.cpp ../../source/render/imgui/imgui_impl_opengl3.cpp ../../source/render/imgui/imgui_impl_sdl.cpp

#Target All and include the stuff from above
all: $(OBJS)
			$(CC) $(OBJS) $(COMPILER_FLAGS) $(SOLVER_FLAGS) $(RENDER_FLAGS) $(IMGUI) -o $(TARGET)
