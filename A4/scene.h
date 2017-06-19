#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "TMesh.h"

#include <iostream>

using namespace std;

class Scene {
public:


  FrameBuffer *fb,*fb3,*fbt; // SW framebuffer and third person FB
  
  GUI * gui; // graphical user interface

  PPC *ppc, *ppc3, *ppct; // camera used to render the scene from views chosen by user
      // and third person camera

  TMesh **tms; // an array of pointers to TMesh objects
  int tmsN; // how many TMeshes there are

  Scene();
  void DBG();
  void NewB();
  void Render(PPC *currPPC, FrameBuffer *currFB,V3 lightPosition);
  void RenderFB(FrameBuffer *fbToRender, PPC *fbPPC, PPC *currPPC, 
    FrameBuffer *currFB, float constz = -1.0f);
  void RenderShadow(FrameBuffer *fbToRender, PPC *fbPPC, PPC *currPPC, 
    FrameBuffer *currFB, float constz = -1.0f);
  
};

extern Scene *scene;

