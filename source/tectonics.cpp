/*
PDE Solver for Grids - Test

Author: Nicholas McDonald
Version: 1.0
*/

#include "tectonics.h"

int main( int argc, char* args[] ) {
	//Construct a Renderer
	View view;
	//Initialize the View
	if(!view.Init()){
		std::cout<<"View could not be initialized."<<std::endl;
		return 0;
	}

	//Construct a Model Class
	Geology geology;
	if(!geology.setup()){
		std::cout<<"Failed to load geology configuration."<<std::endl;
		return 0;
	}

	//Now do the climate model
	Climate climate;
	if(!climate.setup(geology)){
		std::cout<<"Failed to load climate configuration."<<std::endl;
		return 0;
	}

	//Game Loop
	bool quit = false;
	SDL_Event e;

	//Main Game Loop
	while(!quit){
		//Quit Condition
		if( SDL_PollEvent( &e ) != 0 ) {
			//Quit Event
			if( e.type == SDL_QUIT ) { quit = true; }
		}

		//Handle with IMGUI
		ImGui_ImplSDL2_ProcessEvent(&e);

		//Choose which guy to display
		if(view.curModel == 0){
			//Render the Output
			view.render<Geology>(geology);
		}
		else if(view.curModel == 1){
			view.render<Climate>(climate);
		}

		//Perform the Integration for both models
		geology.solver.integrate(geology, &Solver<Geology>::EE);
		climate.solver.integrate(climate, &Solver<Climate>::EE);
	}

	/*
	How do we copy variables from one field to another?

	so if solver are members of model classes...
	their fields are contained inside there.
	Where in this program would it make sense to add that?
	*/

	/*
	//Construct another Model Class

	//Integrate with the Model Class Member Integrator, and a time integrator
	geologySolver.integrate<Geology>(geology, 1, &Solver::EE, &Geology::geologyIntegrator);

	//Extract the Quantities
	geology.volcanism = geologySolver.fields[0];
	geology.plates = geologySolver.fields[1];
	geology.height = solve::scale(geologySolver.fields[2], 0.0, 1.0);

	//Set sealevel and output the heightmap
	geology.sealevel = solve::autothresh(geology.height, geology.sealevel, 0.3);
	solve::colorthreshgrad(geology.height, "out/height.png", geology.sealevel, glm::vec3(54, 74, 97), glm::vec3(76, 106, 135), glm::vec3(0, 135, 68), glm::vec3(224, 171, 138));

	//Delete the solver to free space
	geologySolver.fields.clear();

	Solver climateSolver(climate.d, 0.001);
	climateSolver.addField(geology.height);
	climateSolver.addField(climate.wind);
	climateSolver.addField(climate.temperature);
	climateSolver.addField(climate.humidity);
	climateSolver.addField(climate.downfall);
	climateSolver.addField(climate.clouds);
	climateSolver.integrate<Climate>(climate, 1, &Solver::EE, &Climate::climateIntegrator);

	//Finally, do the erosion model
	*/

	//Clean up
	view.cleanup();

	return 0;
}
