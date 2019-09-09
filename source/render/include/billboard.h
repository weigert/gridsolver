#pragma once
#include "billboard.fwd.h"

//Class
class Billboard{
public:
  //Draw Stuff
  GLuint vao[1];
  GLuint vbo[2];
  GLuint texture;

  //Load the Texture from some surface
  bool fromRaw(SDL_Surface* TextureImage);
  bool fromImage(std::string file);
  void setupBuffer();
  void cleanup();
};
