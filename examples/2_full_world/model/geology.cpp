#include "geology.h"

bool Geology::setup(){
  //Setup the Solver
  solver.name = "Geology Solver";
  solver.dim = d;
  solver.timeStep = 0.001;
  solver.fields = geologyInitialize();
  solver._caller = &Geology::geologyIntegrator; //Set the Caller

  return true;
}

/*
================================================================================
                    Initializer and Integrator Functions
================================================================================
*/

//Initializer

std::vector<CArray> Geology::geologyInitialize(){
  //Reset the Seed
  srand(SEED);

  //Blank Fields
  solve::modes = d;
  std::vector<CArray> fields;
  solve::emptyArray(fields, 3);

  //Set the Initial Value
  fields[2] = (complex)0.4;

  //Perlin Noise Module
  noise::module::Perlin perlin;
  perlin.SetOctaveCount(6);
  perlin.SetFrequency(3);
  perlin.SetPersistence(0.6);

  noise::module::Voronoi voronoi;
  voronoi.SetFrequency(8);

  float a = (float)(rand()%SEED)/((SEED));

  //Seed the Maps
  for(int i = 0; i < d.x; i++){
    for(int j = 0; j < d.y; j++){
      //Generate a Volcanism Value
      fields[0][i*(int)d.y+j] = (float)(rand()%SEED)/(SEED);
      fields[1][i*(int)d.y+j] = ((voronoi.GetValue((float)i/d.x, (float)j/d.y, a)));
      fields[2][i*(int)d.y+j] = 0.4;
    }
  }

  //Return the Fields
  return fields;
}

//Integrator

std::vector<CArray> Geology::geologyIntegrator(std::vector<CArray> &_fields){
  //Create a new field vector
  std::vector<CArray> delta;
  solve::emptyArray(delta, _fields.size());

  //Label for all plates, and number of clusters
  int nclusters = 0;
  CArray label = solve::cluster(_fields[1], nclusters);

  //Compute the Force Vectors
  CArray gradx = solve::scale(solve::diff(_fields[0], 1, 0), -1.0, 1.0);  //Gradient of the Volcanism Map
  CArray grady = solve::scale(solve::diff(_fields[0], 0, 1), -1.0, 1.0);  //Gradient of the Volcanism Map

  //New Plate Arrary
  CArray overlap(-1.0, solve::modes.x*solve::modes.y);
  CArray winner(0.0, solve::modes.x*solve::modes.y);
  CArray newplate(0.0, solve::modes.x*solve::modes.y);        //New Plate Configuration is empty

  //Loop over all cluster labels
  for(int i = 0; i < nclusters; i++){
    //Mask the force array with
    CArray forcex(0.0, solve::modes.x*solve::modes.y);
    CArray forcey(0.0, solve::modes.x*solve::modes.y);
    BArray test = label == (complex)i;
    CArray ones(1.0, solve::modes.x*solve::modes.y);
    forcex[test] = gradx[test];
    forcey[test] = grady[test];
    glm::vec2 force = glm::vec2(forcex.sum().real(), forcey.sum().real());
    //Normalize the Force Vector
    forcex[test] = 1.0;
    forcey[test] = 1.0;
    force /= glm::vec2(forcex.sum().real(), forcey.sum().real());
    //Get the Label Mask
    CArray projected(0.0, solve::modes.x*solve::modes.y);
    projected[test] = _fields[1][test];

    //Roll it a bit
    glm::vec2 shift = glm::round((force)*glm::vec2(10));
    projected = solve::roll(projected, shift);

    //Overlap increments everywhere where we project, so where projected isn't 0
    overlap[projected > 0.0] += ones[projected > 0.0];

    //Winner is whichever is larger, projection or winner
    BArray b = projected > winner;
    winner[b] = projected[b];
  }

  winner[winner == 0.0] += _fields[1][winner == 0.0];

  //Diffuse
  //CArray volcdiff = solve::fft(_fields[0]);
  //delta[0] = (complex)0.00005*solve::ifft(solve::fdiff(volcdiff, 2, 0) + solve::fdiff(volcdiff, 0, 2));
  _fields[0] = solve::diffuse(_fields[0], 0.0000001, 10);
  _fields[1] = winner;
  _fields[2] = solve::diffuse(_fields[2], 0.00000005, 10);  //Diffuse Height

  //Height
  CArray activity = ((complex)1.0-_fields[1])*_fields[0]; //Activity is 1- Plate Density times volcanism
  CArray hotspot(0.0, solve::modes.x*solve::modes.y);
  hotspot[activity > 0.7] = 1.0;

  //Add the Overlap
  delta[0] += (complex)100.0*overlap*_fields[2];

  //Add the Overlap to Height
  delta[2] += (complex)50.0*overlap;
  delta[2] += (complex)10.0*activity;
  delta[2] += (complex)30.0*hotspot;

  return delta;
}
