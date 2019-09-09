#pragma once
#include "climate.fwd.h"
#include "geology.fwd.h"

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

/*
================================================================================
                            Climate Field Renderer
================================================================================
*/

template<>
SDL_Surface* View::getSurface<Climate>(Climate &climate){
  //Here we want to actually draw whatever the current selected field is.
  //Array for the Color
  CArray R(0.0, climate.d.x*climate.d.y);
  CArray G(0.0, climate.d.x*climate.d.y);
  CArray B(0.0, climate.d.x*climate.d.y);
  CArray A(255.0, climate.d.x*climate.d.y);

  //Switch the Current Field
  switch(curField){
    case 0:{ //Height
      //Do a colorthreshold gradient
      BArray test = climate.solver.fields[2] > climate.sealevel;
      //Get the sea gradient
      view::gradRGB(glm::vec3(54, 74, 97), glm::vec3(76, 106, 135), climate.solver.fields[2], R, G, B);
      //Update certain portions
      R[test] = view::gradC(0, 224, climate.solver.fields[2])[test];
      G[test] = view::gradC(135, 171, climate.solver.fields[2])[test];
      B[test] = view::gradC(68, 138, climate.solver.fields[2])[test];
      break;}
    case 1:{ //Wind
      //Simple Grayscale
      view::gradRGB(glm::vec3(0), glm::vec3(255), climate.solver.fields[1], R, G, B);
      break;}
    case 2:{ //Temperature
      //Simple Color Gradient
      view::gradRGB(glm::vec3(255, 255, 0), glm::vec3(255, 0, 255), climate.solver.fields[2], R, G, B);
      break;}
    case 3:{ //Humidity
      //Simple Color Gradient
      view::gradRGB(glm::vec3(0, 0, 255), glm::vec3(255), climate.solver.fields[3], R, G, B);
      break;}
    case 4:{ //Downfall
      //Do a colorthreshold gradient
      BArray test = climate.solver.fields[2] > climate.sealevel;
      //Get the sea gradient
      view::gradRGB(glm::vec3(54, 74, 97), glm::vec3(76, 106, 135), climate.solver.fields[2], R, G, B);
      //Update certain portions
      R[test] = view::gradC(0, 224, climate.solver.fields[2])[test];
      G[test] = view::gradC(135, 171, climate.solver.fields[2])[test];
      B[test] = view::gradC(68, 138, climate.solver.fields[2])[test];
      //Add the downfall overlay
      R *= ((complex)1.0-climate.solver.fields[4]);
      G *= ((complex)1.0-climate.solver.fields[4]);
      B *= ((complex)1.0-climate.solver.fields[4]);
      R += ((complex)0*climate.solver.fields[4]);
      G += ((complex)0*climate.solver.fields[4]);
      B += ((complex)0*climate.solver.fields[4]);

      break;}
    case 5:{ //Clouds
      //Do a colorthreshold gradient
      BArray test = climate.solver.fields[2] > climate.sealevel;
      //Get the sea gradient
      view::gradRGB(glm::vec3(54, 74, 97), glm::vec3(76, 106, 135), climate.solver.fields[2], R, G, B);
      //Update certain portions
      R[test] = view::gradC(0, 224, climate.solver.fields[2])[test];
      G[test] = view::gradC(135, 171, climate.solver.fields[2])[test];
      B[test] = view::gradC(68, 138, climate.solver.fields[2])[test];
      //Add the downfall overlay
      R *= ((complex)1.0-climate.solver.fields[4]);
      G *= ((complex)1.0-climate.solver.fields[4]);
      B *= ((complex)1.0-climate.solver.fields[4]);
      R += ((complex)255*climate.solver.fields[4]);
      G += ((complex)255*climate.solver.fields[4]);
      B += ((complex)255*climate.solver.fields[4]);

      break;}
    default:
      break;
  }

  return view::makeSurface(climate.d, R, G, B, A);
}

/*
================================================================================
                              Climate Controller
================================================================================
*/

template<>
void Interface::drawModel<Climate>(View &view, Climate &climate){
  //Climate Interface
  ImGui::Text("Climate Model");

  //Output the Grid Dimension
  ImGui::TextUnformatted("Grid Dimensions: ");
  ImGui::SameLine();
  ImGui::Text("%d", (int)climate.d.x);
  ImGui::SameLine();
  ImGui::Text("%d", (int)climate.d.y);

  //Seed and Regeneration
  ImGui::TextUnformatted("Seed: ");
  ImGui::SameLine();
  ImGui::Text("%d", (int)climate.SEED);

  //I would like to load the default configuration...
  if (ImGui::Button("Initialize")){
    climate.solver.fields = climate.climateInitialize();
    climate.solver.updateFields = true;
  }

  //Simulation Day
  ImGui::TextUnformatted("Day: ");
  ImGui::SameLine();
  ImGui::Text("%d", (int)climate.day);

  //Fixed Sealevel
  ImGui::TextUnformatted("Sealevel: ");
  ImGui::SameLine();
  ImGui::Text("%f", climate.sealevel);

  //Output the Solver
  ImGui::TextUnformatted("Climate Solver");

  //Time Increment
  static float f2 = 0.001f;
  ImGui::DragFloat("Time Increment", &f2, 0.0005f, 0.000f, 1.0f, "%f");

  //Time Steps
  static int timeSteps = climate.solver.steps;
  ImGui::DragInt("Time Steps", &timeSteps, 1, 1, 500, "%i");

  ImGui::TextUnformatted("Climate Integrator");
  if (ImGui::Button("Run N-Steps")){
    //Set the Integrator and Raise the Timesteps
    climate.solver.integrator = &Climate::climateIntegrator;
    climate.solver.steps = timeSteps;
    climate.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Run Inf")){
    //Set the Integrator and Raise the Timesteps
    climate.solver.integrator = &Climate::climateIntegrator;
    climate.solver.steps = -1;
    climate.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Stop")){
    //Set the Integrator and Raise the Timesteps
    climate.solver.steps = 0;
  }

  ImGui::TextUnformatted("Erosion Integrator");
  if (ImGui::Button("Run N-Steps")){
    //Set the Integrator and Raise the Timesteps
    climate.solver.integrator = &Climate::erosionIntegrator;
    climate.solver.steps = timeSteps;
    climate.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Run Inf")){
    //Set the Integrator and Raise the Timesteps
    climate.solver.integrator = &Climate::erosionIntegrator;
    climate.solver.steps = -1;
    climate.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Stop")){
    //Set the Integrator and Raise the Timesteps
    climate.solver.steps = 0;
  }

  ImGui::TextUnformatted("Fields");

  //Listbox
  const char* listbox_items[] = {"Height", "Wind", "Temperature", "Humidity", "Downfall", "Clouds"};
  static int listbox_item_current = 0;
  ImGui::ListBox("Field", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
  view.curField = listbox_item_current;
}
