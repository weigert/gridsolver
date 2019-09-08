#include "interface.h"
#include "../model/geology.h"
#include "../model/climate.h"
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

  /*
  //Theoretical Example Template:

  //Array for the Color
  CArray R(0.0, geology.d.x*geology.d.y);
  CArray G(0.0, geology.d.x*geology.d.y);
  CArray B(0.0, geology.d.x*geology.d.y);
  CArray A(255.0, geology.d.x*geology.d.y);


  //Manipulate R, G, B, A as a function of your solvers fields.


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
  */


  return NULL;
}

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

template<>
SDL_Surface* View::getImage<Climate>(Climate &climate){
  //Here we want to actually draw whatever the current selected field is.
  //Array for the Color
  CArray R(0.0, climate.d.x*climate.d.y);
  CArray G(0.0, climate.d.x*climate.d.y);
  CArray B(0.0, climate.d.x*climate.d.y);
  CArray A(255.0, climate.d.x*climate.d.y);

  //Switch the Current Field
  switch(curField){
    case 0:{ //Height
      //Do a colorthreshold gradient
      BArray test = climate.solver.fields[0] > climate.sealevel;
      R = climate.solver.fields[0]*(complex)76+((complex)1.0-climate.solver.fields[0])*(complex)54;
      R[test] = (climate.solver.fields[0]*(complex)224+((complex)1.0-climate.solver.fields[0])*(complex)0)[test];

      G = climate.solver.fields[0]*(complex)106+((complex)1.0-climate.solver.fields[0])*(complex)74;
      G[test] = (climate.solver.fields[0]*(complex)171+((complex)1.0-climate.solver.fields[0])*(complex)135)[test];

      B = climate.solver.fields[0]*(complex)135+((complex)1.0-climate.solver.fields[0])*(complex)97;
      B[test] = (climate.solver.fields[0]*(complex)138+((complex)1.0-climate.solver.fields[0])*(complex)68)[test];
      break;}
    case 1:{ //Wind
      //Simple Grayscale
      R = climate.solver.fields[1]*(complex)255;
      G = climate.solver.fields[1]*(complex)255;
      B = climate.solver.fields[1]*(complex)255;
      break;}
    case 2:{ //Temperature
      //Simple Color Gradient
      R = climate.solver.fields[2]*(complex)255+((complex)1.0-climate.solver.fields[2])*(complex)255;
      G = climate.solver.fields[2]*(complex)0  +((complex)1.0-climate.solver.fields[2])*(complex)255;
      B = climate.solver.fields[2]*(complex)255+((complex)1.0-climate.solver.fields[2])*(complex)0;
      break;}
    case 3:{ //Humidity
      //Simple Color Gradient
      R = climate.solver.fields[3]*(complex)255+((complex)1.0-climate.solver.fields[3])*(complex)0;
      G = climate.solver.fields[3]*(complex)255+((complex)1.0-climate.solver.fields[3])*(complex)0;
      B = climate.solver.fields[3]*(complex)255+((complex)1.0-climate.solver.fields[3])*(complex)255;
      break;}
    case 4:{ //Downfall
      //Do a colorthreshold gradient
      BArray test = climate.solver.fields[0] > climate.sealevel;
      R = climate.solver.fields[0]*(complex)76+((complex)1.0-climate.solver.fields[0])*(complex)54;
      R[test] = (climate.solver.fields[0]*(complex)224+((complex)1.0-climate.solver.fields[0])*(complex)0)[test];

      G = climate.solver.fields[0]*(complex)106+((complex)1.0-climate.solver.fields[0])*(complex)74;
      G[test] = (climate.solver.fields[0]*(complex)171+((complex)1.0-climate.solver.fields[0])*(complex)135)[test];

      B = climate.solver.fields[0]*(complex)135+((complex)1.0-climate.solver.fields[0])*(complex)97;
      B[test] = (climate.solver.fields[0]*(complex)138+((complex)1.0-climate.solver.fields[0])*(complex)68)[test];

      //Add the downfall overlay
      R *= ((complex)1.0-climate.solver.fields[4]);
      G *= ((complex)1.0-climate.solver.fields[4]);
      B *= ((complex)1.0-climate.solver.fields[4]);
      R += ((complex)0*climate.solver.fields[4]);
      G += ((complex)0*climate.solver.fields[4]);
      B += ((complex)0*climate.solver.fields[4]);

      break;}
    case 5:{ //Clouds
      //Do a colorthreshold gradient
      BArray test = climate.solver.fields[0] > climate.sealevel;
      R = climate.solver.fields[0]*(complex)76+((complex)1.0-climate.solver.fields[0])*(complex)54;
      R[test] = (climate.solver.fields[0]*(complex)224+((complex)1.0-climate.solver.fields[0])*(complex)0)[test];

      G = climate.solver.fields[0]*(complex)106+((complex)1.0-climate.solver.fields[0])*(complex)74;
      G[test] = (climate.solver.fields[0]*(complex)171+((complex)1.0-climate.solver.fields[0])*(complex)135)[test];

      B = climate.solver.fields[0]*(complex)135+((complex)1.0-climate.solver.fields[0])*(complex)97;
      B[test] = (climate.solver.fields[0]*(complex)138+((complex)1.0-climate.solver.fields[0])*(complex)68)[test];

      //Add the downfall overlay
      R *= ((complex)1.0-climate.solver.fields[4]);
      G *= ((complex)1.0-climate.solver.fields[4]);
      B *= ((complex)1.0-climate.solver.fields[4]);
      R += ((complex)255*climate.solver.fields[4]);
      G += ((complex)255*climate.solver.fields[4]);
      B += ((complex)255*climate.solver.fields[4]);

      break;}
    default:
      break;
  }

  //Construct and Return the Surface
  SDL_Surface *s = SDL_CreateRGBSurface(0, climate.d.x, climate.d.y, 32, 0, 0, 0, 0);
  SDL_LockSurface(s);

  //Create raw data pointer
  unsigned char *img_raw = (unsigned char*)s->pixels;

  for(int i = 0; i < climate.d.x*climate.d.y; i++){
  	//Raw Pointer Stuff
    *(img_raw+4*i)    = (unsigned char)R[i].real();
    *(img_raw+4*i+1)  = (unsigned char)G[i].real();
    *(img_raw+4*i+2)  = (unsigned char)B[i].real();
    *(img_raw+4*i+3)  = (unsigned char)A[i].real();
  }

  SDL_UnlockSurface(s);
  return s;
}
