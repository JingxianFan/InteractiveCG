#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"

#include <iostream>

using namespace std;

class Scene {
public:


  FrameBuffer *fb; // SW framebuffer
  
  GUI * gui; // graphical user interface

  PPC *ppc; // camera used to render the scene from views chosen by user

  Scene();
  void DBG();
  void NewB();
  

};

extern Scene *scene;

