#include "../model/geology.h"
#include "../model/climate.h"
#include "view.h"
#include "interface.h"

void Interface::drawTabBar(View &view){
  //Start a tabbar
  if(ImGui::BeginTabBar("Model Selector", ImGuiTabBarFlags_None)){
    //Simply List All Models
    if(ImGui::BeginTabItem("Geology")){
      view.curModel = 0;
      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Climate")){
      view.curModel = 1;
      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Settings")){
      ImGui::TextUnformatted("Test Setting");
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
}

/*
================================================================================
                          Templated Model Interfaces
================================================================================
*/

//Default
template<typename Model>
void Interface::drawModel(View &view, Model &model){
  //No custom interface defined.
  ImGui::Text("No custom interface defined for this model type.");
}

//Custom - Define your own if you want a controller!

//Geology
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
    //Set the Integrator and Raise the Timesteps
    i0 = rand()%1000000;
    geology.SEED = i0;
  }
  ImGui::SameLine();
  //I would like to load the default configuration...
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
    //Set the Integrator and Raise the Timesteps
    geology.solver._caller = &Geology::geologyIntegrator;
    geology.solver.steps = timeSteps;
    geology.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Run Inf")){
    //Set the Integrator and Raise the Timesteps
    geology.solver._caller = &Geology::geologyIntegrator;
    geology.solver.steps = -1;
    geology.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Stop")){
    //Set the Integrator and Raise the Timesteps
    geology.solver.steps = 0;
  }

  ImGui::TextUnformatted("Fields");

  //Listbox
  const char* listbox_items[] = {"Volcanism", "Plates", "Height"};
  static int listbox_item_current = 0;
  ImGui::ListBox("Field", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
  view.curField = listbox_item_current;
}

//Climate
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
    climate.solver._caller = &Climate::climateIntegrator;
    climate.solver.steps = timeSteps;
    climate.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Run Inf")){
    //Set the Integrator and Raise the Timesteps
    climate.solver._caller = &Climate::climateIntegrator;
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
    climate.solver._caller = &Climate::erosionIntegrator;
    climate.solver.steps = timeSteps;
    climate.solver.timeStep = f2;
  }
  ImGui::SameLine();
  if (ImGui::Button("Run Inf")){
    //Set the Integrator and Raise the Timesteps
    climate.solver._caller = &Climate::erosionIntegrator;
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
