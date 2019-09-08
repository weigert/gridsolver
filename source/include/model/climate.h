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
  bool setup(Geology geology);

  //Field Initializers and Integrators
  Solver solver;
  std::vector<CArray> climateInitialize(CArray height);
  std::vector<CArray> climateIntegrator(std::vector<CArray> &_fields);
  std::vector<CArray> erosionIntegrator(std::vector<CArray> &_fields);
};
