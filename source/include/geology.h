#pragma once
#include "forward/geology.fwd.h"

//This is an example for a general model class

//Geology Container Class
class Geology {
public:
  //Image
  glm::vec2 d = glm::vec2(200);
  int SEED = time(NULL);
  float sealevel = 0.34;

  //Fields as Flat Arrays
  CArray volcanism;
  CArray plates;
  CArray height;

  //Simulation
  void initialize();

  //Integrators
  std::vector<CArray> geologyIntegrator(std::vector<CArray> &_fields);
};
