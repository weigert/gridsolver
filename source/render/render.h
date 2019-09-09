//File IO
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <fstream>
#include <sstream>

//ImGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

//OpenGL and SDL
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

//Dependencies
#include "include/view.cpp"
#include "include/interface.cpp"
#include "include/billboard.cpp"
#include "include/shader.cpp"
