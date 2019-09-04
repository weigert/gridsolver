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

### License

MIT License

I know there are better solvers - this is just for fun.
