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
