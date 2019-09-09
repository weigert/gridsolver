/*
Gridsolve Base Example

This does NOT include the renderer

Author: Nicholas McDonald
Version: 1.0
*/

#include "rendersolve.h"

int main( int argc, char* args[] ) {
	//Construct a Renderer
	View view;
	//Initialize the View
	if(!view.Init()){
		std::cout<<"View could not be initialized."<<std::endl;
		return 0;
	}

	//Construct a Model
	Example example;
	if(!example.setup()){
		std::cout<<"Failed to load geology configuration."<<std::endl;
		return 0;
	}

	//Add all existing models to the view's model tracker!
	view.models.push_back("Example"); //If you don't do this, then it won't render.

	//Game Loop
	bool quit = false;
	SDL_Event e;

	//Render Loop
	while(!quit){
		//Quit Condition
		if( SDL_PollEvent( &e ) != 0 ) {
			//Quit Event
			if( e.type == SDL_QUIT ) { quit = true; }
			if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) { quit = true; }
		}

		//Handle with IMGUI
		ImGui_ImplSDL2_ProcessEvent(&e);

		//Switch the Current Model and Render that Guy (this needs to be done manually)
		if(view.curModel == 0){
			//Render the Guy
			view.render<Example>(example);
		}

		//Perform a single integration step
		example.solver.step(example, &Solver<Example>::DIRECT);
	}

	return 0;
}
