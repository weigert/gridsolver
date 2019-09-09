#pragma once
#include "interface.fwd.h"
#include "view.fwd.h"

class Interface{
public:
  //Get the Tab Number
  void drawTabBar(View &view);

  //Model allows for parameter manipulation
  template<typename Model>
  void drawModel(View &view, Model &model);
};
