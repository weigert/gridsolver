//View Class Dependencies
#include "interface.h"
#include "billboard.h"
#include "shader.h"
//Load our Own Type!
#include "view.h"

/*
================================================================================
                              Setup Stuff
================================================================================
*/

bool View::Init(){
  //Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    return false;
  }
  //Initialize SDL_TTF
	TTF_Init();

  //Initialize the Window and Context
  gWindow = SDL_CreateWindow( "World Generator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
  if( gWindow == NULL ){
    printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    return false;
  }
  gContext = SDL_GL_CreateContext(	gWindow );

  //Initialize OPENGL Stuff
  SDL_GL_SetSwapInterval(1);
  glewExperimental = GL_TRUE;
  glewInit();

  //IMGUI Stuff
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = ImGui::GetIO(); (void)io;
  ImGui_ImplSDL2_InitForOpenGL(gWindow, gContext);
  ImGui_ImplOpenGL3_Init("#version 130");
  ImGui::StyleColorsCustom();

  //Configure Global OpenGL State
  glEnable( GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND) ;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);

  //Initialize the Sprite
  field.setupBuffer();

  //Setup Spriteshader
  billboardShader.setup("billboard.vs", "billboard.fs");
  billboardShader.addAttribute(0, "in_Quad");
  billboardShader.addAttribute(1, "in_Tex");

  return true;
}

void View::cleanup(){
  //Shutdown IMGUI
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  //Cleanup the Billboard
  field.cleanup();
  billboardShader.cleanup();

  //Destroy Context and Window
	SDL_GL_DeleteContext( gContext );
	SDL_DestroyWindow( gWindow );

  //Quit SDL subsystems
  TTF_Quit();
  SDL_Quit();
}

//Calculate the FPS
void View::calcFPS(){
  //Loop over the FPS
  //We getting 60 FPS
  FPS = (int)(1000.0f/(SDL_GetTicks()-ticks));
  ticks = SDL_GetTicks();
  //Set the FPS
  for(int i = 0; i < plotSize-1; i++){
    arr[i] = arr[i+1];
  }
  arr[plotSize-1] = FPS;
}

/*
================================================================================
                              Render Stuff
================================================================================
*/

template<typename Model>
void View::render(Model &model){
  //Clear the Background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Blue
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Render the GUI
  renderGUI<Model>(model);

  //Render the Fields
  renderField<Model>(model);

  //Swap the Window
  SDL_GL_SwapWindow(gWindow);
}

template<typename Model>
void View::renderGUI(Model &model){

  //Create IMGUI Interface
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(gWindow);
  ImGui::NewFrame();

  //Window Flags
  ImGuiWindowFlags window_flags = 0;

  //Set the Windowflags
  window_flags |= ImGuiWindowFlags_NoTitleBar;
  window_flags |= ImGuiWindowFlags_NoScrollbar;
  window_flags |= ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoResize;

  //Set the Window Position
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(400, 800), ImGuiCond_Once);

  //Do some Drawing
  ImGui::Begin("Model Controller", NULL, window_flags);

  //Here, we just draw the individual stuff
  interface->drawTabBar(*this);
  interface->drawModel<Model>(*this, model);

  //End Drawing
  ImGui::End();

  //Widget Helper
  //ImGui::ShowDemoWindow();

  //Render IMGUI
  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//Render the Model
template<typename Model>
void View::renderField(Model &model){
  if(model.solver.updateFields){
    //Get the Surface
    if(!field.fromRaw(getImage<Model>(model))){
      std::cout<<"Failed to load surface from model."<<std::endl;
      return;
    };
    model.solver.updateFields = false;
  }

  //Render the Billboard
  billboardShader.useProgram();
  glActiveTexture(GL_TEXTURE0);

  //Bind the Appropriate Texture and add it to the shader as a uniform
  glBindTexture(GL_TEXTURE_2D, field.texture);
  billboardShader.setInt("imageTexture", 0);

  //Draw
  glBindVertexArray(field.vao[0]);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/*
================================================================================
                      Templated Field Drawing Rules
================================================================================
*/

template<typename Model>
SDL_Surface* View::getImage(Model &model){
  std::cout<<"No rendering rules for the fields of this model kind."<<std::endl;
  return NULL;
}
