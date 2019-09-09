/*
Gridsolve Base Example

Author: Nicholas McDonald
Version: 1.0
*/

#include "gridsolve.h"

int main( int argc, char* args[] ) {
	//Construct a Model
	Example example;
	if(!example.setup()){
		std::cout<<"Failed to load geology configuration."<<std::endl;
		return 0;
	}

	//Construct a Solver
	Solver<Example> exampleSolver;
	exampleSolver.setup("Example Solver", example.d, 0.001);
	exampleSolver.dim = example.d;

	//Set the Integrator Function
	exampleSolver.integrator = &Example::exampleIntegrator;

	//Initialize the Fields
	exampleSolver.fields = example.exampleInitialize();

	//Run the Solver with a specified method (for 100 steps)
	exampleSolver.integrate(example, 100, &Solver<Example>::DIRECT);

	return 0;
}
