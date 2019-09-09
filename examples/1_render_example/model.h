/*
================================================================================
                              Example Model Class
================================================================================
*/

class Example{
public:
	//Parameters
	glm::vec2 d = glm::vec2(100, 100);   //Size

  /*
  Other Parameters
  */

  //Some setup function
  bool setup();

	//Solver is a member because of the renderer!
	Solver<Example> solver;

  //Initializer and Integrator
  std::vector<CArray> exampleInitialize();
  std::vector<CArray> exampleIntegrator(std::vector<CArray> &_fields);

  /*
  You can have multiple initializers and integrators
  */

};

/*
================================================================================
                        Setup, Initialize, Integrate
================================================================================
*/

bool Example::setup(){
	//Setup the Solver
	solver.setup("Example Solver", d, 0.001);
	solver.dim = d;

	//Set the Integrator Function
	solver.integrator = &Example::exampleIntegrator;

	//Initialize the Fields
	solver.fields = exampleInitialize();

  return true;
}

//Initializer
std::vector<CArray> Example::exampleInitialize(){
  //Set the Solver Size!
  solve::modes = d;

  //Blank Fields
  std::vector<CArray> fields = solve::emptyArray(1);  //Size of the fields!

  /*
  Manipulate the Fields!
  */

  //Return the Fields
  return fields;
}

//Integrator
std::vector<CArray> Example::exampleIntegrator(std::vector<CArray> &_fields){
  //Create a new field vector
  std::vector<CArray> delta = solve::emptyArray(_fields.size());

  /*
  Manipulate the deltas or the fields directly!
  */

	//Simply increase it a little bit, so you can see the effect live.
	_fields[0] += 0.01;	//This adds 0.01 to the entire grid!

  return delta;
}

/*
================================================================================
                            Example Field Renderer
================================================================================
*/

template<>
SDL_Surface* View::getSurface<Example>(Example &example){
  //Here we want to actually draw whatever the current selected field is.
  //Array for the Color
  CArray R(0.0, example.d.x*example.d.y);
  CArray G(0.0, example.d.x*example.d.y);
  CArray B(0.0, example.d.x*example.d.y);
  CArray A(255.0, example.d.x*example.d.y);

  //Switch the Current Field
  switch(curField){
		/*
			Depending on curField parameter, draw a specific thing...
			you can decide how to visualize the fields (or combinatons of these!)

			Note that you can theoretically combine the fields in any way
		*/
		case 0:
			//Some Color Gradient
			view::gradRGB(glm::vec3(255, 255, 0), glm::vec3(255, 0, 255), example.solver.fields[0], R, G, B);
			break;
		case 1:
			//Some (other) Color Gradient
			view::gradRGB(glm::vec3(0, 255, 255), glm::vec3(255, 120, 0), example.solver.fields[0], R, G, B);
			break;
    default:
			//Grayscale
			view::gradRGB(glm::vec3(0), glm::vec3(255), example.solver.fields[0], R, G, B);
      break;
  }

	//Retrieve a surface from the colors.
  return view::makeSurface(example.d, R, G, B, A);
}

/*
================================================================================
                              Example Controller
================================================================================
*/

/*
Here, you can use ImGui to define custom controllers for the parameters of the simulation.

I have a few examples on how it is possible to manipulate the simulation below.
*/

template<>
void Interface::drawModel<Example>(View &view, Example &example){
  //Example Interface
  ImGui::Text("Example Model");

  //Output the Grid Dimension
  ImGui::TextUnformatted("Grid Dimensions: ");
  ImGui::SameLine();
  ImGui::Text("%d", (int)example.d.x);
  ImGui::SameLine();
  ImGui::Text("%d", (int)example.d.y);

  //Reinitialize
  if (ImGui::Button("Initialize")){
    example.solver.fields = example.exampleInitialize();
    example.solver.updateFields = true;
  }

	/*
		Other parameter setters / getters for the model
	*/

  //Output the Solver
  ImGui::TextUnformatted("Example Solver");

  //Time Increment
  static float f2 = 0.001f;
  ImGui::DragFloat("Time Increment", &f2, 0.0005f, 0.000f, 1.0f, "%f");

  //Time Steps
  static int timeSteps = example.solver.steps;
  ImGui::DragInt("Time Steps", &timeSteps, 1, 1, 500, "%i");

	/*
		You can repeat this following block with a different integrator,
		if you want to use a different integrator for n-steps / inf-steps
		during runtime!
	*/

  ImGui::TextUnformatted("Climate Integrator");
  if (ImGui::Button("Run N-Steps")){
    //Set the Integrator and Raise the Timesteps
    example.solver.integrator = &Example::exampleIntegrator;
    example.solver.steps = timeSteps;
    example.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Run Inf")){
    //Set the Integrator and Raise the Timesteps
    example.solver.integrator = &Example::exampleIntegrator;
    example.solver.steps = -1;
    example.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Stop")){
    //Set the Integrator and Raise the Timesteps
    example.solver.steps = 0;
  }

	/*
		Here you can specify a list of possible "visualizations"
		These are written to curfield, which in the drawModel template
		function will control how the fields are combined into an image
		to draw! So this doesn't need to be exactly the fields or the size of the fields.
	*/

  ImGui::TextUnformatted("Visualizations");

  //Fields
  const char* listbox_items[] = {"Vis 1", "Vis 2", "Vis 3"};
  static int listbox_item_current = 0;
  ImGui::ListBox("Vis", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
  view.curField = listbox_item_current;
}
