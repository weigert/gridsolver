# solver

![Example 2](https://github.com/weigert/tectonics/blob/master/out/solver.png)

This repository consists of a solver class, that allows for time-step manipulation or time-step integration on multiple grids of arbitrary size. The grids are coupled through an integrator function, that can take differential form (PDEs, ODEs) or absolute state-changes.

Additionally, it contains a renderer class that can draw the solver's results in real time. The renderer has ImGui support for easy manipulation of the simulation (see examples).

#### Motivation

This was originally created to perform a three-step, multi-grid world generation simulation (Tectonics, Climate, Erosion).

If you are interested in using the climate model, there is a separate example on how you can run it on your own height-map.

This and more in the examples.

### Usage

#### Constructing a Model

To use this, you will need to write your own model class.

    //General Model Class
    class Model{
      public:
        //Some setup...
        bool setup();
      
        //Initializer Function
        std::vector<CArray> modelInitialize();  //Returns intial fields

        //Integrator Function
        std::vector<CArray> modelIntegrator(std::vector<CArray> &_fields);  //Executes a time-step
        
      private:
        //parameters...
        glm::vec2 d = glm::vec2(200);
    };

**Parameters:** Arbitrary parameters of your model. These can be accessed and changed through the integrator function at every step, as the integration function is a member of the model class. The model *needs* to have a grid dimension defined.

**Initializer:** Returns a vector of complex arrays that corresponds to the initial values of a set of fields.

**Integrator:** A function that can either act directly on the fields it is supplied, or otherwise returns a set of fields that are then handled by the solver, depending on the integration mode you specify.

##### Notes
- The models themselves don't contain the field data. It is simply generated from the model, and manipulated by the model. Storage and manipulation beyond this is handled by the solver.
- Models can have multiple intializer and integrator functions that act differently on the fields.
- You can construct multiple models and couple them using the solver.

#### Using the Solver

The solver is a helper class that does lots of math for you. It also supplies a namespace ("solve::") with a number of helper functions that make construction of the integrator function very quick and painless. The solver class is templated so it can access the members of the model class.

Once a model has been constructed, you can construct a solver and perform an integration:

    //Setup Model and Solver
    Model model;
    Solver<Model> solver;
    solver.setup("Example Solver", model.d, 0.001);
    
    //Do Integration
    solver._caller = &Model::modelIntegrator; //Set the Integrator Function
    solver.steps = 1;                         //Set the Number of Steps (-1 for inf)
    solver.integrate<Model>(model, &Solver<Model>::integrationMethod);

The solver itself can exist as a member of your model class or entirely separately (see examples).

**Grids:** Stored as a vector of flat, complex arrays. A number of construction methods are possible, including loading from an array or intializing to some value. The base type of the complex array is an std::valarray, allowing for simple boolean masking.

**Integration Method:** 

*Direct Method:* Means that the fields you pass to the integrator, will be assumed to be edited directly by the integrator function at every time-step. This is basically just executing the integrator function every tick. The solver is still usefull though, because of the many helper functions / boolean masking / etc.

    //Direct Method
    solver.integrate<Model>(model, &Solver<Model>::DIRECT);

*Explicit Euler Method:* Assumes integrator function returns a vector of right-hand-side grids of the time-derivative of the grid. Then performs the explicit euler step.

    //Explicit Euler Method
    solver.integrate<Model>(model, &Solver<Model>::EE);
    
Currently, only direct integration and explicit euler integratoin are available due to numerical issues (I'm working on it). Unstable differential equations therefore don't work at the moment.
    
**Integration Function:**

Constructing integrator functions:

In order to do this, it needs to be supplied with
- A set of "fields"
- An integration function (instructions)
- An integration method (how to execute instructions)

The fields are contained in the solver as arrays of complex numbers. The solver class is templated so that it has access to members of the model class, and can use the model class for initialization and integration.


Once the integration function has been set up, you can perform the integration using:

    inte

Additionally, I supply a **solve::** namespace which contains a number of handy functions when constructing integrator functions. This includes constructors for the complex arrays, methods for adding fields to the solver quickly, etc.


All of this is templated, so it is simple to construct an arbitrary model class, that contains parameters as its.

The solver integrator function has full access to these members.



#### Using the Renderer


If you have multiple models, where the input from one model depends on the other model, t

- If you plan on using the renderer, the solver must be a member of the model.

The renderer is written in OpenGL3 and SDL2. Feel free to use the renderer's code too.

#### Recommendations





The grids are coupled through

This is all templated

Additionally, it comes with a small renderer written for SDL2 and OpenGL3.

The renderer is templated so that you can define your own

(You don't need to know opengl, only install the libraries)

that knows how to work with the solver




#### Files

The solver can be used independently by including **source/solver/solver.h** in your program.

The renderer can be used additionally by including **source/render/render.h** in your program.

Remember to also download the other files in the respective folders.
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

If you have any questions feel free to ask.
