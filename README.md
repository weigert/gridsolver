# solver

![Example Terrain](https://github.com/weigert/tectonics/blob/master/out/height.png)

This repository consists of a solver class, that allows for time-step manipulation or time-step integration on multiple grids of arbitrary size. The grids are coupled through an integrator function, that can take differential form (PDEs, ODEs) or absolute state-changes.

Additionally, it contains a renderer class that can draw the solver's results in real time. The renderer has ImGui support for easy manipulation of the simulation (see examples).

#### Motivation

This was originally created to perform a three-step, multi-grid world generation simulation (Tectonics, Climate, Erosion).

If you are interested in using the climate model, there is a separate example on how you can run it on your own height-map.

This and more in the examples.

### Usage

#### Constructing a Model

#### Using the Solver

#### Using the Renderer

#### Recommendations
-

### Renderer


Written in OpenGL3 and SDL2



This is written in OpenGL3 and SDL2

All of this is templated, so it is simple to construct an arbitrary model class, that contains parameters as its.

The solver integrator function has full access to these members

The grids are coupled through

This is all templated

Additionally, it comes with a small renderer written for SDL2 and OpenGL3.

The renderer is templated so that you can define your own

(You don't need to know opengl, only install the libraries)

that knows how to work with the solver



This is a small solver class for doing time-step manipulation or time-step integration on grid-based models.

### Usage


### Compiling

Linking the renderer

### Examples

I mainly applied this to a 3-step model










### Notes
- Probably wont work very well on very large grids.
- Despite many attempts, currently only direct (absolute step) integration is stable.



This is a proof of concept for a simple numerical PDE solver system using fft.

I applied it to a simple model system (plate tectonics).

Solver also has a number of helper functions (e.g. clustering an image, drawing an image) and a few overloaded operators for simple array masking.

Known Issues:
The numerics are still a little buggy but I'm working out the kinks.
If you actually don't apply any differential operators, then there's no issue and you have simple integrators.

### Compiling

I compiled using g++ on c++14.

Dependencies:
- glm
- fftw
- libnoise

If you're not sure, check the makefile to see what I'm linking

### License

MIT License

I know there are better solvers - this is just for fun.
