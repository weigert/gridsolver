/*
PDE Solver for Grids - Test

Author: Nicholas McDonald
Version: 1.0
*/
#include "tectonics.h"

int main( int argc, char* args[] ) {
	//Construct a Model Class
	Geology geology;
	geology.initialize();

	//Construct a Solver
	Solver solver(geology.d, 0.001);
	solver.addField(geology.volcanism);
	solver.addField(geology.plates);
	solver.addField(geology.height);

	//Integrate with the Model Class Member Integrator, and a time integrator
	solver.integrate<Geology>(geology, 100, &Solver::EE, &Geology::geologyIntegrator);

	//Rescale the Heightmap, set the sealevel and output an image
	solver.fields[2] = solve::scale(solver.fields[2], 0.0, 1.0);
	geology.sealevel = solve::autothresh(solver.fields[2], geology.sealevel, 0.3);
	solve::colorthreshgrad(solver.fields[2], "out/height.png", geology.sealevel, glm::vec3(0, 135, 68), glm::vec3(224, 171, 138), glm::vec3(54, 74, 97), glm::vec3(76, 106, 135));

	return 0;
}
