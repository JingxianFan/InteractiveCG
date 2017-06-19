#pragma once

#include "gui.h"
#include "framebuffer.h"

#include <iostream>

using namespace std;

class Scene {
public:


  FrameBuffer *fb; // SW framebuffer
  
  GUI * gui; // graphical user interface

  Scene();
  void DBG();
  void NewB();
  

};

extern Scene *scene;

