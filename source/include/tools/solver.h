#include "solver.cpp"
/*
================================================================================
                            PDE Solving Helper Class
================================================================================
*/

class Solver{
public:
  //Constructor
  Solver(glm::vec2 _dim, double _timeStep){
    dim = _dim;
    timeStep = _timeStep;
  }

  //Setup the Simulation
  std::vector<CArray> fields;
  glm::vec2 dim;

  //Grid Set Manipulators
  void addField(float a[]);
  void addField(CArray a);

  //Master Integrator (this is called every tick to integrate a single step)
  template<typename Model>
  bool integrate(Model model, int nsteps, std::vector<CArray> (Solver::*_inte)( Model model, std::vector<CArray>(Model::*_call)(std::vector<CArray>&_fields)), std::vector<CArray>(Model::*_call)(std::vector<CArray>&_fields));
//  bool doTimeStep(std::vector<CArray> (Solver::*_inte)(std::vector<CArray> (*_call)(std::vector<CArray> &_fields)), std::vector<CArray> (*_call)(std::vector<CArray> &_fields));
  double timeStep = 1.0;

  template<typename Model>
  std::vector<CArray> EE(Model model, std::vector<CArray> (Model::*_call)( std::vector<CArray> &_fields ) );

/*
  Currently not functional due to numerical issues getting stuff to work in the fourier space

  template<typename Model>
  std::vector<CArray> IE(Model model, std::vector<CArray> (Model::*_call)( std::vector<CArray> &_fields ) );
  template<typename Model>
  std::vector<CArray> CN(Model model, std::vector<CArray> (Model::*_call)( std::vector<CArray> &_fields ) );
*/

};

/*
================================================================================
                      Helpers (Field Adders / Handling)
================================================================================
*/

void Solver::addField(CArray a){
  //Make sure to set the modes!
  solve::modes = dim;
  fields.push_back(a);
}

void Solver::addField(float a[]){
  //Make sure to set the modes!
  solve::modes = dim;
  fields.push_back(solve::fromArray(a));
}

/*
================================================================================
                              Solver Functions
================================================================================
*/
template<typename Model>
bool Solver::integrate(Model model, int nsteps, std::vector<CArray> (Solver::*_inte)( Model model, std::vector<CArray> (Model::*_call)(std::vector<CArray> &_fields) ), std::vector<CArray> (Model::*_call)(std::vector<CArray> &_fields)){
  //Set the modes
  solve::modes = dim;

  //Loop over the nSteps
  for(int i = 0; i < nsteps; i++){
    //Get the Deltas
    std::vector<CArray> deltas = (*this.*_inte)(model, _call);

    //Add the Deltas
    for(unsigned int i = 0; i < fields.size(); i++){
      //Add the Terms to the fields
      fields[i] += deltas[i];
    }
  }

  //Really, everything should be converted into the fourier space here...
  return true;
}

/*
================================================================================
                              Time Integrators
================================================================================
*/

//Explicit Euler Integrator
template<typename Model>
std::vector<CArray> Solver::EE(Model model, std::vector<CArray> (Model::*_call)( std::vector<CArray> &_fields ) ){
  //Get the Lambdas
  std::vector<CArray> lambdas = (model.*_call)(fields);
  std::for_each(lambdas.begin(), lambdas.end(), [this](CArray &l){ l = (complex)timeStep*l;});
  return lambdas;
}

/*

//Implicit Euler Integration
template<typename Model>
std::vector<CArray> Solver::IE(Model model, std::vector<CArray> (Model::*_call)( std::vector<CArray> &_fields ) ){
  //Get the Lambdas
  std::vector<CArray> lambdas = (model.*_call)(fields);
  //Apply the Transformation to every lambda
  std::for_each(lambdas.begin(), lambdas.end(), [this](CArray &l){ l = 1.0/(1.0-(complex)timeStep*l);});
  return lambdas;
}

template<typename Model>
std::vector<CArray> Solver::CN(Model model, std::vector<CArray> (Model::*_call)( std::vector<CArray> &_fields ) ){
  //Get the Lambdas
  std::vector<CArray> lambdas = (model.*_call)(fields);
  //Apply the Transformation to every lambda
  std::for_each(lambdas.begin(), lambdas.end(), [this](CArray &l){ l = (1.0+(complex)timeStep*l)/(1.0-(complex)timeStep*l);});
  return lambdas;
}

*/
