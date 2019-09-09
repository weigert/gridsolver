/*
PDE Solver for Grids - Test

Author: Nicholas McDonald
Version: 1.0
*/

#include "worldgen.h"

int main( int argc, char* args[] ) {
	//Construct a Renderer
	View view;
	//Initialize the View
	if(!view.Init()){
		std::cout<<"View could not be initialized."<<std::endl;
		return 0;
	}

	//Construct a Geology Model
	Geology geology;
	if(!geology.setup()){
		std::cout<<"Failed to load geology configuration."<<std::endl;
		return 0;
	}

	//Construct a Climate Model
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
			if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) { quit = true; }
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

		//Perform the Integration (one step per tick!) for both models
		geology.solver.step(geology, &Solver<Geology>::EE);
		climate.solver.step(climate, &Solver<Climate>::DIRECT);
	}

	//Clean up
	view.cleanup();

	return 0;
}
