#include "geology.h"
#include "climate.h"

bool Climate::setup(Geology &geology){
  //Set the Geology pointer
  geologyptr = &geology;

  //Setup the Solver
  solver.name = "Climate Solver";
  solver.dim = geology.d;
  solver.timeStep = 0.001;
  solver.fields = climateInitialize();
  solver._caller = &Climate::climateIntegrator; //Set the Caller

  return true;
}

/*
================================================================================
                        Climate Model Integrators
================================================================================
*/

//
std::vector<CArray> Climate::climateInitialize(){
  //Reset these Values (might have changed)
  d = geologyptr->d;
  sealevel = geologyptr->sealevel;
  SEED = geologyptr->SEED;

  //Blank Fields
  solve::modes = d;
  std::vector<CArray> fields;
  solve::emptyArray(fields, 6);

  //Perlin Noise Module
  _wind.SetOctaveCount(2);
  _wind.SetFrequency(12);

  //Set intial Values
  fields[0] = geologyptr->solver.fields[2];
  fields[1] = 0.0;        //Wind
  fields[2] = 0.7;        //Temperature
  fields[3] = 0.4;        //Humidity
  fields[4] = 0.0;        //Downfall
  fields[5] = 0.0;        //Clouds

  //Modify Values
  fields[1][geologyptr->solver.fields[2] > sealevel] = 0.4;
  fields[2][geologyptr->solver.fields[2] > sealevel] = 0.2;

  //Return the Fields
  return fields;
}


//Integrators
std::vector<CArray> Climate::climateIntegrator(std::vector<CArray> &_fields){
  //Create a new field vector
  std::vector<CArray> delta;
  solve::emptyArray(delta, _fields.size());

  //Compute the Wind
  //Shift height by wind direction, form difference and divide by gridsize
  day++;
  float dayfrac = (float)day/(5*365.0);
  float theta = 2*3.14159265*_wind.GetValue(dayfrac, SEED, SEED);
  glm::vec2 _winddir = glm::vec2(0.5)*glm::vec2(cos(theta), sin(theta));
  //Get the Windstrength
  CArray heightproject = solve::roll(_fields[0], glm::floor(glm::vec2(10)*_winddir));
  _fields[1] = solve::scale(heightproject - _fields[0], 0.0, 1.0);

  //Compute the Temperature Update, by shifting it with the wind
  _fields[2] = solve::diffuse(_fields[2], 0.0000005, 10);  //Diffuse temperature map
  CArray ones(1.0, solve::modes.x*solve::modes.y);
  CArray tempshift = solve::shift(_fields[2], (complex)(10.0*_winddir.x)*_fields[1], (complex)(10.0*_winddir.y)*_fields[1]);
  _fields[2][tempshift > 0.0] = tempshift[tempshift > 0.0];
  _fields[2][_fields[0]>sealevel] += ((complex)0.1*solve::scale(_fields[1], -1.0, 1.0))[_fields[0]>sealevel];  //Rising air cools, sinking air heats
  _fields[2] += ((complex)0.015*((complex)1.2-_fields[5])); //Sunlight on land
  _fields[2][_fields[0]<sealevel] -= ((complex)0.01*ones)[_fields[0]<sealevel]; //Cooling over the sea, evaporation
  _fields[2] -= (complex)0.03*_fields[4]; //If its raining, cool down

  //Compute the Humidity Map
  _fields[3] = solve::diffuse(_fields[3], 0.0000005, 10);  //Diffuse temperature map
  CArray humidshift = solve::shift(_fields[3], (complex)(10.0*_winddir.x)*_fields[1], (complex)(10.0*_winddir.y)*_fields[1]);
  _fields[3][humidshift > 0.0] = humidshift[humidshift > 0.0];
  _fields[3][_fields[0] < sealevel] += (((complex)1.0-_fields[3])*0.05*_fields[2])[_fields[0] < sealevel]; //Over body of water, grow proportional to temperature
  _fields[3] -= (complex)0.3*_fields[3]*_fields[3]*_fields[4];     //When raining, remove

  //Downfall condition
  CArray downfallshift = solve::shift(_fields[4], (complex)(10.0*_winddir.x)*_fields[1], (complex)(10.0*_winddir.y)*_fields[1]);
  _fields[4][downfallshift > 0.0] = downfallshift[downfallshift > 0.0];
  CArray _test = (complex)0.56+(complex)0.25*_fields[2]; //If temperature is zero, moisture freezes
  _fields[4][_fields[3] > _test] += ((complex)0.007*ones)[_fields[3] > _test];
  _fields[4][_fields[3] < _test] -= ((complex)0.07*ones)[_fields[3] < _test];

  //Cloud Condition
  CArray cloudshift = solve::shift(_fields[5], (complex)(10.0*_winddir.x)*_fields[1], (complex)(10.0*_winddir.y)*_fields[1]);
  _fields[5][cloudshift > 0.0] = cloudshift[cloudshift > 0.0];
  _test = (complex)0.54+(complex)0.23*_fields[2];
  _fields[5][_fields[3] > _test] += ((complex)0.007*ones)[_fields[3] > _test];
  _fields[5][_fields[3] < _test] -= ((complex)0.3*ones)[_fields[3] < _test];

  //Clamp the Quantities
  _fields[2] = solve::clamp(_fields[2], 0.0, 1.0);
  _fields[3] = solve::clamp(_fields[3], 0.0, 1.0);
  _fields[4] = solve::clamp(_fields[4], 0.0, 1.0);
  _fields[5] = solve::clamp(_fields[5], 0.0, 1.0);

  return delta;
}

std::vector<CArray> Climate::erosionIntegrator(std::vector<CArray> &_fields){
  //Create a new field vector
  std::vector<CArray> delta;
  solve::emptyArray(delta, _fields.size());

  return delta;
}
