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

/*
Instantly set the sealevel on the geology
Do a single step for either model
Change the parameters of the simulation

Do a series of steps in the simulation
(i.e. run and enter number and time steps)
it then runs until it is finished, but you can change view parameters

You should be able to reset a specific map
You should be able to view different maps live

Change the color-scheme live maybe

This is really a question of GOOD visualization...
*/

/*
What do I pass to the renderer - fields or the whole object?
Or just the solver?

If I want to render at every tick, then the solver should really make the call to render.

Solver doesn't write to the object back automatically.

We should just rerender every time we do an update
but only accept input when it is not working.


At every tick, the solver contains the information
Therefore, we should be able to run the simulation for a number of ticks

We should have control over the solver
Therefore we should really be passing SOLVERS not objects to the guy
*/



/*
1. Generate the Surfaces from the actual fields
2. Render the surfaces appropriately
3. Maybe construct some surface helpers

Then:

1. Add the controller mockups for the inidividual model types
2. Add the controller mockup for the solver class
3. Find a way to let the solvers run elegantly while being displayed at the same time
4. Find a way to let the control panel reset the models, etc.

Fin.

*/

/*
Finished:
-> Initial Geology Model is fine
-> Initial Climate Model is fine

I will only distribute this as an ODE solver thingy
along with the models

Need to:
-> Also, output an image of a height gradient map
  or rather drainage map
-> Design hydraulic model

-> Generate some gifs
-> Write an article on it

Hydraulics:
-> Evaporation
-> Carray capacity
-> Intertia
-> Radius
-> Sedimentation!!

Numerics:
-> Test implicit integrator
-> Write down geological diff. equation
-> Remove diffusion steps
-> Remove Dimensionality
-> Do the upsampling / modes experiment
-> Solve the erosion model at a higher detail level


STD Algorithms:

std::transform (something like apply to all)
std::transform(begin(collection), end(collection), begin(second), std::back_inserter(results), std::plus);

-> std::count

transform and reduce for doing large operations on these valarray sets?
instead of clustering and shit? or accumulate where you pass custom functions

std sample

adjacent difference

inner product

binary search -> see if element exists
can we use this to speed up clustering?
*/
