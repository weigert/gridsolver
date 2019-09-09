//Standard Library Stuff
#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <chrono>
#include <fstream>

//Tools
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "include/tools/imgui/imgui.h"
#include "include/tools/imgui/imgui_impl_sdl.h"
#include "include/tools/imgui/imgui_impl_opengl3.h"
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <glm/glm.hpp>
#include "include/tools/CImg.h"
#include <fftw.h>
#include <noise/noise.h>
using namespace cimg_library;
#include <libconfig.h++>
using namespace libconfig;

//Home Brew
#include "include/tools/solver.h"
#include "include/model/geology.cpp"
#include "include/model/climate.cpp"
#include "include/render/view.cpp"
#include "include/render/interface.cpp"
#include "include/render/billboard.cpp"
#include "include/render/shader.cpp"
