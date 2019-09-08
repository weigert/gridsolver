#include "../model/geology.h"
#include "../model/climate.h"
#include "view.h"
#include "interface.h"

void Interface::drawTabBar(View &view){
  //Start a tabbar
  if(ImGui::BeginTabBar("Model Selector", ImGuiTabBarFlags_None)){
    //Simply List All Models
    if(ImGui::BeginTabItem("Geology")){
      view.curTab = 0;
      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Climate")){
      view.curTab = 1;
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

  //Seed and Regeneration
  static int i0 = geology.SEED;
  ImGui::InputInt("Seed", &i0);

  //Output the Grid Dimension
  //....

  //I would like to load the default configuration...
  //...

  //Manual Sealevel
  static float a = geology.sealevel;
  ImGui::Text("Sealevel: ");
  ImGui::SameLine();
  ImGui::SliderFloat("Sealevel", &a, 0.0, 1.0);

  //Autosealevel
  static float b = 0.5;
  ImGui::DragFloat("Land Fraction", &b, 0.01f, 0.0f, 1.0f, "%f");
  if (ImGui::Button("Autosealevel")){
    //Do the integration
    a = solve::autothresh(geology.solver.fields[2], a, b);
  }

  //Set the Sealevel
  geology.sealevel = a;

  //Output the Solver
  ImGui::TextUnformatted("Tectonics Integrator");

  //Time Increment
  static float f2 = 0.001f;
  ImGui::DragFloat("Time Increment", &f2, 0.0005f, 0.000f, 1.0f, "%f");

  //Time Steps
  static int timeSteps = geology.solver.steps;
  ImGui::DragInt("Time Steps", &timeSteps, 1, 1, 500, "%i");

  if (ImGui::Button("Perform Integration")){
    geology.solver.steps = timeSteps;
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
}
