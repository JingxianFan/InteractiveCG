#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "TMesh.h"

#include <iostream>

using namespace std;

class Scene {
public:


  FrameBuffer *fb, *fb3; // SW framebuffer and third person FB
  
  GUI * gui; // graphical user interface

  PPC *ppc, *ppc3; // camera used to render the scene from views chosen by user
      // and third person camera

  TMesh **tms; // an array of pointers to TMesh objects
  int tmsN; // how many TMeshes there are

  Scene();
  void DBG();
  void NewB();
  void Render(PPC *currPPC, FrameBuffer *currFB);
  void RenderFB(FrameBuffer *fbToRender, PPC *fbPPC, PPC *currPPC, 
    FrameBuffer *currFB);
  

};

extern Scene *scene;

