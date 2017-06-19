#include "scene.h"
#include "v3.h"
#include "m33.h"
#include <float.h>

#include <iostream>
#include <fstream>

using namespace std;

Scene *scene;

Scene::Scene() {

  // create user interface
  gui = new GUI();
  gui->show();

  // create SW framebuffer
  int u0 = 20;
  int v0 = 50;
  int sci = 2;
  int w = sci*240;
  int h = sci*180;
  fb = new FrameBuffer(u0, v0, w, h);
  fb->label("SW Framebuffer");
  fb->show();
  
  // position UI window
  gui->uiw->position(fb->w+u0 + 2*20, v0);

}



// function linked to the DBG GUI button for testing new features
void Scene::DBG() {

  V3 v0(1.0f, 0.0f, 0.0f);
  V3 v1(0.0f, 1.0f, 0.0f);

  M33 m;
  m[0] = v0;
  m[1] = v1;
  m[2] = v0;

  return;

  v0[2] = 10.0f;
  
  float v0v1 = v0*v1;
  cerr << v0v1 << endl;
  return;

  fb->Set(0xFFFFFFFF);
  float cuf = 135.3f;
  float cvf = 81.9f;
  float radius = 51.0f;
  unsigned int color = 0xFF0000FF;
  fb->DrawCircle(cuf, cvf, radius, color);
  fb->redraw();
  return;

  fb->SetCheckerboard(40, 0xFFAAAAAA, 0xFFFFFFFF);
  fb->redraw();
  return;

  fb->Set(0xFFFFFFFF);
  int u = fb->w/2;
  color = 0xFF000000;
  for (int v = 0; v < fb->h; v++) {
    fb->Set(u, v, color);
  }
  fb->redraw();
  return;


  color = 0xFF00FFFF;
  ((unsigned char*)(&color))[1] = 0x80;

  unsigned char red = ((unsigned char*)(&color))[0];
  unsigned char green = ((unsigned char*)(&color))[1];
  unsigned char blue = ((unsigned char*)(&color))[2];


  cerr << "red: " << (int) red << "; green: " << (int) green << "; blue: " << (int) blue << endl;

  fb->Set(color);
  fb->redraw();

}

void Scene::NewB() {

  cerr << "NewB pressed" << endl;

}