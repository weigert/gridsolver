#pragma once
#include "../forward/geology.fwd.h"

//This is an example for a general model class

//Mineral
struct Mineral{
  //Constructor
  Mineral(std::string _n, int _c1, int _c2, int _c3){
    //Set all Quantities according to constructor
    name = _n;
    color[0] = _c1;
    color[1] = _c2;
    color[2] = _c3;
  }

  //Quantities
  std::string name;
  int color[3] = {0, 0, 0};
};

//Geology Container Class
class Geology{
public:
  //Image
  glm::vec2 d = glm::vec2(200);
  int SEED = time(NULL);
  float sealevel = 0.24;

  //Possible Minerals
  std::vector<Mineral> minerals;

  //Simulation
  bool setup();

  //Field Initializers and Integrators
  Solver solver;
  std::vector<CArray> geologyInitialize();                              //Returns intial fields
  std::vector<CArray> geologyIntegrator(std::vector<CArray> &_fields);  //Returns time-stepped fields
};
