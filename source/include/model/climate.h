#pragma once
#include "../forward/climate.fwd.h"
#include "../forward/geology.fwd.h"

class Climate{
public:
  //Size
  glm::vec2 d;
  int day = 0;
  int SEED;
  float sealevel;
  noise::module::Perlin _wind;

  //Setter Upper
  Geology* geologyptr;  //Pointer the the geology class (we need this!)
  bool setup(Geology &geology);

  //Field Initializers and Integrators
  Solver<Climate> solver;
  std::vector<CArray> climateInitialize();
  std::vector<CArray> climateIntegrator(std::vector<CArray> &_fields);
  std::vector<CArray> erosionIntegrator(std::vector<CArray> &_fields);
};
