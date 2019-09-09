#pragma once
#include "view.fwd.h"
#include "shader.fwd.h"
#include "interface.fwd.h"
#include "billboard.fwd.h"

class View{
  public:
    //Initialization
    bool Init();
    void cleanup();
    SDL_Window* gWindow;
    SDL_GLContext gContext;
    ImGuiIO io;
    const unsigned int SCREEN_WIDTH = 1200, SCREEN_HEIGHT = 800;

    //GUI Handler and Parameters
    Shader billboardShader;
    Interface* interface;
    Billboard field;
    std::vector<std::string> models;
    int curModel = 0;
    int curField = 0;

    //Model Rendering (Field and Interface)
    template<typename Model>
    void render(Model &model);
    template<typename Model>
    void drawField(Model &model);
    template<typename Model>
    void drawInterface(Model &model);

    //Get an Image from a field
    template<typename Model>
    SDL_Surface* getSurface(Model &model);

    //FPS Calculator
    void calcFPS();
    int ticks = 0;
    const int plotSize = 100;
    float FPS = 0.0f;
    float arr[100] = {0};
};

/*
================================================================================
                                  Drawing Helpers
================================================================================
*/

namespace view{

//Construct Surface from Color Array
SDL_Surface* makeSurface(glm::vec2 d, CArray R, CArray G, CArray B, CArray A);

//Extract Color Arrays
CArray gradC(double c1, double c2, CArray field);
void gradRGB(glm::vec3 c1, glm::vec3 c2, CArray field, CArray &R, CArray &G, CArray &B);

CArray gradC(double c1, double c2, CArray field){
  return field*(complex)c2+((complex)1.0-field)*(complex)c1;
}

void gradRGB(glm::vec3 c1, glm::vec3 c2, CArray field, CArray &R, CArray &G, CArray &B){
  //Gradient RGB between two colors
  R = field*(complex)c2.x+((complex)1.0-field)*(complex)c1.x;
  G = field*(complex)c2.y+((complex)1.0-field)*(complex)c1.y;
  B = field*(complex)c2.z+((complex)1.0-field)*(complex)c1.z;
}

//Define the color
SDL_Surface* makeSurface(glm::vec2 d, CArray R, CArray G, CArray B, CArray A){
  //Construct and Return the Surface
  SDL_Surface *s = SDL_CreateRGBSurface(0, d.x, d.y, 32, 0, 0, 0, 0);
  SDL_LockSurface(s);

  //Create raw data pointer
  unsigned char *img_raw = (unsigned char*)s->pixels;

  for(int i = 0; i < d.x*d.y; i++){
    //Raw Pointer Stuff
    *(img_raw+4*i)    = (unsigned char)R[i].real();
    *(img_raw+4*i+1)  = (unsigned char)G[i].real();
    *(img_raw+4*i+2)  = (unsigned char)B[i].real();
    *(img_raw+4*i+3)  = (unsigned char)A[i].real();
  }

  SDL_UnlockSurface(s);
  return s;
}

//End of Namespace "view"
}
