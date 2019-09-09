#include "view.h"
#include "interface.h"

void Interface::drawTabBar(View &view){
  //Start a tabbar
  if(ImGui::BeginTabBar("Model Selector", ImGuiTabBarFlags_None)){
    //Loop over all Model
    for(unsigned int i = 0; i < view.models.size(); i++){
      //Simply List All Models
      if(ImGui::BeginTabItem(view.models[i].c_str())){
        view.curModel = i;
        ImGui::EndTabItem();
      }
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
