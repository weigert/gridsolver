#pragma once
#include "../forward/view.fwd.h"
#include "../forward/shader.fwd.h"
#include "../forward/interface.fwd.h"
#include "../forward/geology.fwd.h"
#include "../forward/climate.fwd.h"
#include "../forward/billboard.fwd.h"

class View{
  public:
    //Initialization
    bool Init();
    void cleanup();
    SDL_Window* gWindow;
    SDL_GLContext gContext;
    ImGuiIO io;
    const unsigned int SCREEN_WIDTH = 1200, SCREEN_HEIGHT = 800;

    //GUI Handler
    Shader billboardShader;
    Interface* interface;
    Billboard field;
    int curModel = 0;
    int curField = 0;

    //Render the entire drawing pipeline
    template<typename Model>
    void render(Model &model);
    template<typename Model>
    void renderField(Model &model);
    template<typename Model>
    void renderGUI(Model &model);

    //Get an Image from a field
    template<typename Model>
    SDL_Surface* getImage(Model &model);

    //FPS Calculator
    void calcFPS();
    int ticks = 0;
    const int plotSize = 100;
    float FPS = 0.0f;
    float arr[100] = {0};
};
