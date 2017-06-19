#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "TMesh.h"

#include <iostream>

using namespace std;

class Scene {
public:


  FrameBuffer *fb; // SW framebuffer
  FrameBuffer *fb1;
  FrameBuffer *fb2;
  FrameBuffer *fb3;
  FrameBuffer *fb4;
  FrameBuffer *fb5;
  
  GUI * gui; // graphical user interface

  PPC *ppc; // camera used to render the scene from views chosen by user

  TMesh **tms; // an array of pointers to TMesh objects
  int tmsN; // how many TMeshes there are

  Scene();
  void DBG();
  void NewB();
  

};

extern Scene *scene;

