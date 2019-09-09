/*
Example Model Class
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

  //Initializer and Integrator
  std::vector<CArray> exampleInitialize();
  std::vector<CArray> exampleIntegrator(std::vector<CArray> &_fields);

  /*
  You can have multiple initializers and integrators
  */

};

bool Example::setup(){
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

  return delta;
}
