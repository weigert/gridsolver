#pragma once
#include "../forward/interface.fwd.h"
#include "../forward/view.fwd.h"
#include "../forward/geology.fwd.h"
#include "../forward/climate.fwd.h"

class Interface{
public:
  //Get the Tab Number
  void drawTabBar(View &view);

  //Model allows for parameter manipulation
  template<typename Model>
  void drawModel(View &view, Model &model);
};
