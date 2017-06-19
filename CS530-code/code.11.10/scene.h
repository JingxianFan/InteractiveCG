#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "TMesh.h"

#include <iostream>

using namespace std;

class Scene {
public:

  PPC *ppc0, *ppc1, *ppc2; // cameras used to take the construction images
  FrameBuffer *im0, *im1, *im2; // construction images
  FrameBuffer *canvas; // visualization of stitching
  PPC *ppcc; // camera of canvas
  void SetupStitching();

  FrameBuffer *fb, *fb3; // SW framebuffer and third person FB

  FrameBuffer *hwfb; // HW framebuffer

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
    FrameBuffer *currFB, float constz = -1.0f);

  void RenderHW();

};

extern Scene *scene;

