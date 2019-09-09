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

Integrator functions either act directly on the fields they are supplied, or return a vector of deltas (which are grids) that correspond to the time derivative of the grids. Integrator functions are best constructed with the use of the helper functions from the solve namespace.

    std::vector<CArray> Geology::geologyIntegrator(std::vector<CArray> &_fields){
        //"Right-Hand-Side" Grids
        std::vector<CArray> delta;
        solve::emptyArray(delta, _fields.size());
        
        //Manipulate Fields and Deltas Here!
        
        //
        return delta;
    }

*Helper Functions:* Include things from fast fourier transform on grid, to computing nth order 2D differentials of grid, to clamping, rolling, scaling, and (bad) clustering. Read "solver.cpp" for all details.

*Boolean Masking:* Allows you to manipulate only specific parts of the grid. I apply this heavily in my models. Comparison operators have been defined for various datatypes.

    //Terrible example
    CArray A(0.5, d.x*d.y); //Construct
    CArray B(0.7, d.x*d.y);
    
    //Either this
    BArray test = B > previously_generated_random_field_between_zero_and_one; 
    A[test] = 1.0;
    
    //Or this
    B[B < previously_generated_random_field_between_zero_and_one] = 0.0;
    
    //etc...

#### Using the Renderer

If you plan on using the renderer, the solver must be a member of the model. This is because of how the renderer works at the moment, and this additionally allows for templated definition of drawing rules for the fields of a model, as well as a control interface specific to the models parameters.

The view class handles all the front-end interaction. To use the renderer, you define a drawing rule function and an interface function. These are templated members of the view function. Both have comprehensive examples below, which can be used as is or changed entirely if desired.

**Drawing Rule Function:** The view class keeps track of the current model and field you are observing. The fields simply contain a complex array of values. The drawing rule function generates an SDL_Surface from the fields of the model. This allows for absolute custom visualization of the data.

        //Drawing Rule Function
        template<> SDL_Surface* View::getImage<Model>(Model &model){
            //...
        }
        
**Interface Function:** As every model has unique parameters that require controlling, the interface function is also a templated member of the view class. Thereby, you can use basic ImGui elements to manipulate the simulation in real time.

        //Interface Function
        template<> void Interface::drawModel<Geology>(View &view, Geology &geology){
            //...
        }
        
Then, you can perform the rendering of the model containing the solver by calling:

        view.render<Model>(model);

For an exact implementation of the render pipeline, see the full examples.

In the future, creating the color-schemes will be more simplified using general rules and some helper functions.

The renderer is written in OpenGL3 and SDL2. Feel free to use the renderer's code too.

#### Final Remarks
- If you have multiple models, where the input from one model depends on the other model, I recommend that during the construction of the models, you save a pointer to the dependency model or the corresponding solver that contains the relevant fields. If you need access to the members (i.e. parameters) of the previous model, then store the model pointer. This helps particularly when reinitializing during runtime.
- Probably wont work very well on very large grids.

### Compiling

I compiled using g++ on c++14. See the makefiles in the individual examples for what / how to link.

#### Files

The solver can be used independently by including **source/solver/solver.h** in your program.

The renderer can be used additionally by including **source/render/render.h** in your program.

Remember to also download the other files in the respective folders.

### Examples

Coming soon... one is already uploaded!

### License

MIT License

I know there are better solvers - this is just for fun.

If you have any questions feel free to ask.
