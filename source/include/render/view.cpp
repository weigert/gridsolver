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

  ImGui::ShowDemoWindow();

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

  return NULL;
}

//One way to edit asurface
/* This assumes that color value zero is black. Use
   SDL_MapRGBA() for more robust surface color mapping!
   height times pitch is the size of the surface's whole buffer.*/
/*
SDL_LockSurface(surface);
SDL_memset(surface->pixels, 0, surface->h * surface->pitch);
SDL_UnlockSurface(surface);
*/

/*

//Make some actual informative plots
solve::colorgrad(_fields[1], "out/wind.png", glm::vec3(0, 0, 0), glm::vec3(255, 255, 255));

//Humidity Temperature Map
solve::overlay((complex)0.5*_fields[2], "out/height.png", "out/temphumid.png", glm::vec3(255, 0, 0));
solve::overlay((complex)0.5*_fields[3], "out/temphumid.png", "out/temphumid.png", glm::vec3(0, 0, 255));

//Test the Clouds Overlay
solve::overlay(_fields[5], "out/height.png", "out/downfall.png", glm::vec3(229));
solve::overlay(_fields[4], "out/downfall.png", "out/downfall.png", glm::vec3(10));



void PutPixel32_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    *((Uint32*)pixel) = color;
}

void PutPixel24_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint8) * 3);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    pixel[0] = (color >> 24) & 0xFF;
    pixel[1] = (color >> 16) & 0xFF;
    pixel[2] = (color >> 8) & 0xFF;
#else
    pixel[0] = color & 0xFF;
    pixel[1] = (color >> 8) & 0xFF;
    pixel[2] = (color >> 16) & 0xFF;
#endif
}

void PutPixel16_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint16));
    *((Uint16*)pixel) = color & 0xFFFF;
}

void PutPixel8_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint8));
    *pixel = color & 0xFF;
}

void PutPixel32(SDL_Surface * surface, int x, int y, Uint32 color)
{
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
    PutPixel32_nolock(surface, x, y, color);
    if( SDL_MUSTLOCK(surface) )
        SDL_UnlockSurface(surface);
}

void PutPixel24(SDL_Surface * surface, int x, int y, Uint32 color)
{
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
    PutPixel24_nolock(surface, x, y, color);
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
}

void PutPixel16(SDL_Surface * surface, int x, int y, Uint32 color)
{
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
    PutPixel16_nolock(surface, x, y, color);
    if( SDL_MUSTLOCK(surface) )
        SDL_UnlockSurface(surface);
}

void PutPixel8(SDL_Surface * surface, int x, int y, Uint32 color)
{
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
    PutPixel8_nolock(surface, x, y, color);
    if( SDL_MUSTLOCK(surface) )
        SDL_UnlockSurface(surface);
}
*/

/*
void View::renderMap(World territory, SDL_Renderer* gRenderer, int xview, int yview) {
	//Set rendering space and render to screen
  //Isometric Tiling Logic Based on Height and Surface Map
  float tileScale = 2;
  for(int i=0; i<100; i++){
    for(int j=0; j<100; j++){
      //For the Depth
      for(int k =0; k<40; k++){
        //Take Sourcequad from Territory Surface Tile
        SDL_Rect sourceQuad;
          //Replace this with logic based on territory.terrain.surfaceMap[i][j];
          sourceQuad.x=0;
          sourceQuad.y=territory.terrain.biomeMap[i][j]*25;
          sourceQuad.w=33;
          sourceQuad.h=25;
        //Take Renderquad from current i and j numbers
        //Height is in Intervals of 100
        if((int)territory.terrain.depthMap[i][j]/100>=k){
          SDL_Rect renderQuad;
            renderQuad.x=((territory.terrain.worldWidth-tileScale*(tilesize))/2)+j*tileScale*(tilesize)/2-i*tileScale*(tilesize)/2-territory.xview;
            renderQuad.y=j*8*tileScale+i*8*tileScale-territory.yview-k*(8)*tileScale;
            renderQuad.w=tileScale*tilesize;
            renderQuad.h=tileScale*25;
          //Render
        if(renderQuad.x > -10 && renderQuad.x < 1000 && renderQuad.y < 1000 && renderQuad.y > -10){
          SDL_RenderCopy( gRenderer, mTexture, &sourceQuad, &renderQuad);
        }
        }
      }
    }
  }
  SDL_Rect rect;
  rect.x=1000;
  rect.y=0;
  rect.w=400;
  rect.h=1000;
  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 100);
  SDL_RenderFillRect(gRenderer, &rect);
}*/

/*
*/
/*
//Surface from File:
boost::filesystem::path data_dir(boost::filesystem::current_path());
data_dir /= "out/hunter.png";

//Load the Image to a surface
return IMG_Load((data_dir).c_str());
*/

/*

void image(std::string name, CArray a, CArray b, CArray c){
  CImg<unsigned char> img(modes.x, modes.y, 1, 3);
  //Draw the Image
  for(int i = 0; i < img.width(); i++){
    for(int j = 0; j < img.height(); j++){
      //Create the Image
      img(i, j, 0, 0) = a[ind(glm::vec2(i, j))].real();
      img(i, j, 0, 1) = b[ind(glm::vec2(i, j))].real();
      img(i, j, 0, 2) = c[ind(glm::vec2(i, j))].real();
    }
  }
  //Save to File
  img.save_png(name.c_str());
}

void mix(std::string in, std::string out, CArray a, CArray b, CArray c, CArray m){
  //Load the in image
  CImg<unsigned char> img(in.c_str());
  //Draw the Image
  for(int i = 0; i < img.width(); i++){
    for(int j = 0; j < img.height(); j++){
      //Create the image blend
      double frac = m[ind(glm::vec2(i, j))].real();
      img(i, j, 0, 0) *= (1.0-frac);
      img(i, j, 0, 1) *= (1.0-frac);
      img(i, j, 0, 2) *= (1.0-frac);

      img(i, j, 0, 0) += frac*a[ind(glm::vec2(i, j))].real();
      img(i, j, 0, 1) += frac*b[ind(glm::vec2(i, j))].real();
      img(i, j, 0, 2) += frac*c[ind(glm::vec2(i, j))].real();
    }
  }
  //Save to File
  img.save_png(out.c_str());
}

void grayscale(CArray field, std::string name){
  //Do the Grayscale Image
  image(name, (complex)255.0*field, (complex)255.0*field, (complex)255.0*field);
}

void colorthresh(CArray field, std::string name, float t, glm::vec3 c1, glm::vec3 c2){
  //Do a colorthreshold
  CArray a(c2.x, modes.x*modes.y);
  a[field > t] = (complex)c1.x;

  CArray b(c2.y, modes.x*modes.y);
  b[field > t] = (complex)c1.y;

  CArray c(c2.z, modes.x*modes.y);
  c[field > t] = (complex)c1.z;

  image(name, a, b, c);
}

void colorgrad(CArray field, std::string name, glm::vec3 c1, glm::vec3 c2){

}

//Low low color, High low color, low high color, high high color
void colorthreshgrad(CArray field, std::string name, float t, glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4){
  //Do a colorthreshold
  BArray test = field > t;
  CArray a =  field*(complex)c2.x+((complex)1.0-field)*(complex)c1.x;
  a[test] = (field*(complex)c4.x+((complex)1.0-field)*(complex)c3.x)[test];

  CArray b =  field*(complex)c2.y+((complex)1.0-field)*(complex)c1.y;
  b[test] = (field*(complex)c4.y+((complex)1.0-field)*(complex)c3.y)[test];

  CArray c =  field*(complex)c2.z+((complex)1.0-field)*(complex)c1.z;
  c[test] = (field*(complex)c4.z+((complex)1.0-field)*(complex)c3.z)[test];

  image(name, a, b, c);
}

void overlay(CArray field, std::string in, std::string out, glm::vec3 c1){
  //Do a colorthreshold
  CArray a((complex)c1.x, modes.x*modes.y);
  CArray b((complex)c1.y, modes.x*modes.y);
  CArray c((complex)c1.z, modes.x*modes.y);
  mix(in, out, a, b, c, field);
}


//solve::grayscale(hotspot, "out/hotspot.png");
//solve::grayscale(activity, "out/activity.png");
//solve::grayscale(overlap, "out/overlap.png");
//solve::colorgrad(_fields[0], "out/volcanism.png", glm::vec3(255, 51, 51), glm::vec3(255, 255, 102));
//solve::grayscale(_fields[1], "out/plates.png");


*/
