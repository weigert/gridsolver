#include "billboard.h"

bool Billboard::fromRaw(SDL_Surface* TextureImage){
  //Load the File
  if(TextureImage == NULL){
    std::cout<<"Error loading billboard from surface."<<std::endl;
    return false;
  }

  glGenTextures( 1, &texture );
  glBindTexture( GL_TEXTURE_2D, texture );

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage->w, TextureImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage->pixels);

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  return true;
}

void Billboard::setupBuffer(){
  GLfloat vert[] = {-0.3, -1.0,
                    -0.3, 1.0,
                     1.0, -1.0,
                     1.0, 1.0,};

  GLfloat tex[] = {0.0, 1.0,
                   0.0, 0.0,
                   1.0, 1.0,
                   1.0, 0.0};

   //Load it into a buffer thingy
   glGenVertexArrays(1, vao);
   glBindVertexArray(vao[0]);
   glGenBuffers(2, vbo);

   //Buff' it
   glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
   glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

   glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
   glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &tex[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void Billboard::cleanup(){
  //Cleanup Depthmap Stuff
  glDeleteTextures(1, &texture);

  //Delete the Stuff
  glDisableVertexAttribArray(vao[0]);
  glDeleteBuffers(2, vbo);
  glDeleteVertexArrays(1, vao);
}
