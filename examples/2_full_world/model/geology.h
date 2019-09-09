#pragma once
#include "geology.fwd.h"

//Geology Container Class
class Geology{
public:
  //Image
  glm::vec2 d = glm::vec2(200);
  int SEED = 1;
  float sealevel = 0.24;

  //Simulation
  bool setup();

  //Field Initializers and Integrators
  Solver<Geology> solver;
  std::vector<CArray> geologyInitialize();                              //Returns intial fields
  std::vector<CArray> geologyIntegrator(std::vector<CArray> &_fields);  //Returns time-stepped fields
};

/*
================================================================================
                            Geology Field Renderer
================================================================================
*/

template<>
SDL_Surface* View::getImage<Geology>(Geology &geology){
  //Array for the Color
  CArray R(0.0, geology.d.x*geology.d.y);
  CArray G(0.0, geology.d.x*geology.d.y);
  CArray B(0.0, geology.d.x*geology.d.y);
  CArray A(255.0, geology.d.x*geology.d.y);

  //Switch the Current Field
  switch(curField){
    case 0: //Volcanism
      //Simple Color Gradient
      R = (complex)255;
      G = geology.solver.fields[0]*(complex)255+((complex)1.0-geology.solver.fields[0])*(complex)51;
      B = geology.solver.fields[0]*(complex)102+((complex)1.0-geology.solver.fields[0])*(complex)51;
      break;
    case 1: //Plates
      //Simple Grayscale
      R = geology.solver.fields[1]*(complex)255;
      G = geology.solver.fields[1]*(complex)255;
      B = geology.solver.fields[1]*(complex)255;
      break;
    case 2: //Height
      //Do a colorthreshold gradient
      BArray test = geology.solver.fields[2] > geology.sealevel;
      R = geology.solver.fields[2]*(complex)76+((complex)1.0-geology.solver.fields[2])*(complex)54;
      R[test] = (geology.solver.fields[2]*(complex)224+((complex)1.0-geology.solver.fields[2])*(complex)0)[test];

      G = geology.solver.fields[2]*(complex)106+((complex)1.0-geology.solver.fields[2])*(complex)74;
      G[test] = (geology.solver.fields[2]*(complex)171+((complex)1.0-geology.solver.fields[2])*(complex)135)[test];

      B = geology.solver.fields[2]*(complex)135+((complex)1.0-geology.solver.fields[2])*(complex)97;
      B[test] = (geology.solver.fields[2]*(complex)138+((complex)1.0-geology.solver.fields[2])*(complex)68)[test];

      break;
  }

  //Construct and Return the Surface
  SDL_Surface *s = SDL_CreateRGBSurface(0, geology.d.x, geology.d.y, 32, 0, 0, 0, 0);
  SDL_LockSurface(s);

  //Create raw data pointer
  unsigned char *img_raw = (unsigned char*)s->pixels;

  for(int i = 0; i < geology.d.x*geology.d.y; i++){
  	//Raw Pointer Stuff
    *(img_raw+4*i)    = (unsigned char)R[i].real();
    *(img_raw+4*i+1)  = (unsigned char)G[i].real();
    *(img_raw+4*i+2)  = (unsigned char)B[i].real();
    *(img_raw+4*i+3)  = (unsigned char)A[i].real();
  }

  SDL_UnlockSurface(s);
  return s;
}

/*
================================================================================
                              Geology Controller
================================================================================
*/

template<>
void Interface::drawModel<Geology>(View &view, Geology &geology){
  //Geology Interface
  ImGui::Text("Geology Model");

  //Output the Grid Dimension
  ImGui::TextUnformatted("Grid Dimensions: ");
  ImGui::SameLine();
  ImGui::Text("%d", (int)geology.d.x);
  ImGui::SameLine();
  ImGui::Text("%d", (int)geology.d.y);

  //Seed and Regeneration
  static int i0 = geology.SEED;
  ImGui::InputInt("Seed", &i0);
  geology.SEED = i0;

  if (ImGui::Button("Randomize")){
    i0 = rand()%1000000;
    geology.SEED = i0;
  }
  ImGui::SameLine();
  if (ImGui::Button("Initialize")){
    geology.solver.fields = geology.geologyInitialize();
    geology.solver.updateFields = true;
    geology.solver.steps = 0;
  }

  //Manual Sealevel
  static float a = geology.sealevel;
  ImGui::Text("Sealevel: ");
  ImGui::SameLine();
  ImGui::SliderFloat("Sealevel", &a, 0.0, 1.0);

  //Autosealevel
  static float b = 0.5;
  ImGui::DragFloat("Land Fraction", &b, 0.01f, 0.0f, 1.0f, "%f");
  if (ImGui::Button("Autosealevel")){
    a = solve::autothresh(geology.solver.fields[2], a, b);
  }

  //Set the Sealevel
  geology.sealevel = a;

  //Output the Solver
  ImGui::TextUnformatted("Geology Solver");

  //Time Increment
  static float f2 = 0.001f;
  ImGui::DragFloat("Time Increment", &f2, 0.0005f, 0.000f, 1.0f, "%f");

  //Time Steps
  static int timeSteps = geology.solver.steps;
  ImGui::DragInt("Time Steps", &timeSteps, 1, 1, 500, "%i");

  ImGui::TextUnformatted("Geology Integrator");
  if (ImGui::Button("Run N-Steps")){
    geology.solver.integrator = &Geology::geologyIntegrator;
    geology.solver.steps = timeSteps;
    geology.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Run Inf")){
    geology.solver.integrator = &Geology::geologyIntegrator;
    geology.solver.steps = -1;
    geology.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Stop")){
    geology.solver.steps = 0;
  }

  ImGui::TextUnformatted("Fields");

  const char* listbox_items[] = {"Volcanism", "Plates", "Height"};
  static int listbox_item_current = 0;
  ImGui::ListBox("Field", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
  view.curField = listbox_item_current;
}
