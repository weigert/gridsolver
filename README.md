# tectonics

![Example Terrain](https://github.com/weigert/tectonics/blob/master/out/height.png)

This is a proof of concept for a simple numerical PDE solver system using fft.

I applied it to a simple model system (plate tectonics).

Solver also has a number of helper functions (e.g. clustering an image, drawing an image) and a few overloaded operators for simple array masking.

Known Issues:
The numerics are still a little buggy but I'm working out the kinks.
If you actually don't apply any differential operators, then there's no issue and you have simple integrators.

### Compiling

I compiled using g++ on c++14.

Dependencies:
- CIMg (included in the thingy and no need to link - although you could slice out the code very easily)
- glm
- fftw
- libnoise

If you're not sure, check the makefile to see what I'm linking



/*
Solvers contain Fields
They also contain Integrator functions

Solvers can also apply a variety of methods to perform the integration

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


### License

MIT License

I know there are better solvers - this is just for fun.
