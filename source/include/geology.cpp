#include "geology.h"

//Initialize ensures that the fields have initial values

void Geology::initialize(){
  //Reset the Seed
  srand(SEED);

  //Perlin Noise Module
  noise::module::Perlin perlin;
  perlin.SetOctaveCount(6);
  perlin.SetFrequency(3);
  perlin.SetPersistence(0.6);

  noise::module::Voronoi voronoi;
  voronoi.SetFrequency(8);

  float a = (float)(rand()%SEED)/((SEED));

  //Resize
  volcanism.resize(d.x*d.y);
  plates.resize(d.x*d.y);
  height.resize(d.x*d.y);

  //Seed the Maps
  for(int i = 0; i < d.x; i++){
    for(int j = 0; j < d.y; j++){
      //Generate a Volcanism Value
      volcanism[i*(int)d.y+j] = ((perlin.GetValue((float)i/d.x, (float)j/d.y, SEED))*0.5+1)/2;
      plates[i*(int)d.y+j] = ((voronoi.GetValue((float)i/d.x, (float)j/d.y, a)));
      height[i*(int)d.y+j] = 0.25;
    }
  }
}

/*
================================================================================
                            Integrator Function
================================================================================
*/

/*
The solver class is templated so that the integrator function can be a member in
some model class to have access to members, which might be important things such
as constants or vectors that you need to persist that aren't a full field.
*/

std::vector<CArray> Geology::geologyIntegrator(std::vector<CArray> &_fields){
  //Create a new field vector
  std::vector<CArray> delta;
  solve::emptyArray(delta, _fields.size());

  //Label for all plates, and number of clusters
  int nclusters = 0;
  CArray label = solve::cluster(_fields[1], nclusters);

  //Compute the Force Vectors
  CArray gradx = solve::diff(_fields[0], 1, 0);  //Gradient of the Volcanism Map
  CArray grady = solve::diff(_fields[0], 0, 1);  //Gradient of the Volcanism Map

  //New Plate Arrary
  CArray overlap(-1.0, solve::modes.x*solve::modes.y);
  CArray winner = _fields[1];
  CArray newplate(0.0, solve::modes.x*solve::modes.y);        //New Plate Configuration is empty

  //Loop over all cluster labels
  for(int i = 0; i < nclusters; i++){
    //Mask the force array with
    CArray forcex(0.0, solve::modes.x*solve::modes.y);
    CArray forcey(0.0, solve::modes.x*solve::modes.y);
    BArray test = label == (complex)i;
    forcex[test] = gradx[test];
    forcey[test] = grady[test];
    glm::vec2 force = glm::vec2(forcex.sum().real(), forcey.sum().real());

    //Get the Label Mask
    CArray projected(0.0+0.0i, solve::modes.x*solve::modes.y);
    projected[test] = _fields[1][test];

    //Roll it a bit
    glm::vec2 shift = glm::vec2(4)*force / (solve::modes*solve::modes);
    projected = solve::roll(projected, glm::floor(shift));

    //Overlap increments everywhere where we project, so where projected isn't 0
    CArray ones(1.0, solve::modes.x*solve::modes.y);
    overlap[projected > 0.0] += ones[projected > 0.0];

    //Winner is whichever is larger, projection or winner
    BArray b = projected > winner;
    winner[b] = projected[b];
  }

  //Volcanism
  _fields[0] = solve::diffuse(_fields[0], 0.00000001, 10);
  delta[0] -= (complex)120.0*overlap*_fields[2];        //Subtract the Overlap

  //Set the Plates
  winner[winner == 0.0] += _fields[1][winner == 0.0];
  _fields[1] = winner;

  //Height
  _fields[2] = solve::diffuse(_fields[2], 0.00000005, 10);  //Diffuse Height
  CArray active = ((complex)1.0-_fields[1])*_fields[0]; //Activity is 1- Plate Density times volcanism
  CArray hotspot(0.0, solve::modes.x*solve::modes.y);
  hotspot[solve::diffuse(active, 0.00000005, 10) > 0.85] = 1;
  delta[2] += (complex)10.0*(active + hotspot + overlap);

  return delta;
}
