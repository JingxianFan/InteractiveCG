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

  // create camera
  float hfov = 55.0f;
  ppc = new PPC(hfov, w, h);

  // allocate pointer of TMesh objects
  tmsN = 3;
  tms = new TMesh*[tmsN];
  for (int tmi = 0; tmi < tmsN; tmi++)
    tms[tmi] = 0;

  tms[0] = new TMesh();
  tms[0]->LoadBin("geometry/teapot1K.bin");
  tms[0]->enabled = true;
  
  tms[1] = new TMesh();
  tms[2] = new TMesh();
  V3 corners[4];
  float rw2 = 40.0f, rh2 = 15.0f, nearz = -100.0f, farz = -300.0f;
//  farz = nearz;
  corners[0] = V3(-rw2, +rh2, nearz);
  corners[1] = V3(-rw2, -rh2, nearz);
  corners[2] = V3(+rw2, -rh2, farz);
  corners[3] = V3(+rw2, +rh2, farz);
  tms[1]->SetAsRectangle(corners);
  float eps = 4.0f;
  tms[1]->Translate(V3(0.0f, -rh2-eps, 0.0f));
  tms[2]->SetAsRectangle(corners);
  tms[2]->Translate(V3(0.0f, +rh2+eps, 0.0f));
  FrameBuffer *texture = new FrameBuffer(0, 0, 64, 64);
  texture->SetCheckerboard(8, 0xFF000000, 0XFFAAAAFF);
  tms[1]->texture = texture;
  tms[2]->texture = texture;

  tms[1]->enabled = false;
  tms[2]->enabled = false;
  ppc->C = tms[0]->GetCenter() + V3(0.0f, 0.0f, 110.0f);

  fb->show();
  
  // position UI window
  gui->uiw->position(fb->w+u0 + 2*20, v0);

}

void Scene::RenderFB(FrameBuffer *fbToRender, PPC *fbPPC, PPC *currPPC, 
  FrameBuffer *currFB) {

  currFB->ClearZB(0.0f);
  currFB->Set(0xFFFFFFFF);
  for (int v = 0; v < fbToRender->h; v++) {
    for (int u = 0; u < fbToRender->w; u++) {
      int uv = (fbToRender->h-v-1)*fbToRender->w+u;
      if (fbToRender->zb[uv] == 0.0f)
        continue;
      V3 projP = V3(.5f+(float)u, .5f+(float)v, fbToRender->zb[uv]);
      V3 P = fbPPC->Unproject(projP);
      V3 projP2;
      if (!currPPC->Project(P, projP2))
        continue;
      V3 currCol;
      currCol.SetFromColor(fbToRender->pix[uv]);
      currFB->SetIfCloser(projP2, currCol);
    }
  }

  currFB->redraw();


}

void Scene::Render(PPC *currPPC, FrameBuffer *currFB) {

  currFB->Set(0xFFFFFFFF);
  currFB->ClearZB(0.0f);
  V3 lightPosition;
  float ambientk = 0.4f;
  for (int tmi = 0; tmi < tmsN; tmi++) {
    if (!(tms[tmi] && tms[tmi]->enabled))
      continue;
    currFB->DrawFilledMode(tms[tmi], tmi, currPPC, lightPosition, ambientk);
  }
  currFB->redraw();

}

// function linked to the DBG GUI button for testing new features
void Scene::DBG() {

  ppc3 = new PPC(*ppc);
//  ppc3->C = ppc3->C + V3(50.0f, 120.0f, 30.0f);
  ppc3->C = ppc3->C + V3(0.0f, 0.0f, -30.0f);
  fb3 = new FrameBuffer(fb->w+20+20, 50, ppc3->w, ppc3->h);
  fb3->label("Third person FB");
  fb3->Set(0xFF00AAFF);
  fb3->show();
  // position UI window
  gui->uiw->position(fb->w+ 3*20 +fb3->w, 50);


  ppc3->PositionAndOrient(ppc3->C, tms[0]->GetCenter(), V3(0.0f, 1.0f, 0.0f));
  Render(ppc, fb);

  RenderFB(fb, ppc, ppc3, fb3);
   
  return;

  Render(ppc, fb);
  return;

  V3 center = tms[0]->GetCenter();
  ppc->C = center + V3(0.0f, 0.0f, 110.0f);
  fb->Set(0xFFFFFFFF);
  fb->ClearZB(0.0f);
//  fb->DrawWireFrame(tms[0], ppc);
  V3 lightPosition = ppc->C;
  float ambientk = 0.1f;

  {
    V3 L0 = lightPosition;
    V3 L1 = center + V3(0.0f, 100.0f, 0.0f);

    for (int i = 0; i < 100; i++) {
      lightPosition = L0 + (L1-L0)*(float)i / 99.0f;
      fb->Set(0xFFFFFFFF);
      fb->ClearZB(0.0f);
      fb->DrawFilledMode(tms[0], 0, ppc, lightPosition, ambientk);
      fb->redraw();
      Fl::check();
    }
  }
  return;

  for (int i = 0; i < 100; i++) {
    ppc->PositionAndOrient(center+V3(100.0f, 100.0f, -50.0f+(float)i), center, 
      V3(0.0f, 1.0f, 0.0f));
    fb->Set(0xFFFFFFFF);
    fb->ClearZB(0.0f);
    fb->DrawWireFrame(tms[0], ppc);
    fb->redraw();
    Fl::check();
  }

  return;

  PPC cam(60.0f, fb->w, fb->h);
  cam.Translate(V3(0.0f, 0.0f, -100.0f));
  for (int i = 0; i < 361; i++) {
    fb->ClearZB(0.0f);
    fb->Set(0xFFFFFFFF);
    cam.VisualizeCamera(ppc, fb, 40.0f);
    fb->redraw();
    Fl::check();
    cam.Pan(1.0f);
  }
  return;


  int stepsN = 361;
  float theta = 1.0f;
  V3 aDir(1.0f, 1.0f, 0.0f);
  aDir = aDir.UnitVector();

  for (int si = 0; si < stepsN; si++) {
    fb->Set(0xFFFFFFFF);
    fb->ClearZB(0.0f);
    fb->DrawWireFrame(tms[0], ppc);
    fb->redraw();
    Fl::check();
    tms[0]->RotateAboutAxis(center, aDir, theta);
  }

  return;

  int dotSize = 3;
//  fb->DrawVertexDots(tms[0], dotSize, ppc);
  V3 projCenter;
  ppc->Project(center, projCenter);
  fb->DrawCircle(projCenter[0], projCenter[1], 7, 0xFF0000FF);
  AABB aabb = tms[0]->ComputeAABB();
  fb->DrawAABB(aabb, 0xFF00FF00, 0xFF000000, ppc);
  fb->redraw();
  return;

  V3 verts[4], colors[4];
  verts[0] = V3(0.0f, 35.0f, -100.0f);
  colors[0] = V3(0.0f, 0.0f, 0.0f);
  verts[1] = V3(-20.0f, -15.0f, -100.0f);
  colors[1] = V3(1.0f, 0.0f, 0.0f);
  verts[2] = V3(7.0f, -15.0f, -50.0f);
  colors[2] = V3(0.0f, 1.0f, 0.0f);
  verts[3] = V3(40.0f, -15.0f, -130.0f);
  colors[3] = V3(0.0f, 0.0f, 1.0f);
  tms[0] = new TMesh(verts, colors);

  fb->Set(0xFFFFFFFF);
  fb->DrawWireFrame(tms[0], ppc);
  fb->redraw();
  return;

  float hfov = 55.0f;
  ppc = new PPC(hfov, fb->w, fb->h);

  V3 P, projP;
  P = V3(0.0f, 0.0f, -100.0f); // point along -z axis
  P = ppc->C + ppc->c; // top left image corner
  P = ppc->C + ppc->c + ppc->a*ppc->w + ppc->b*ppc->h; // top left image corner
  bool projResult = ppc->Project(P, projP);
  cerr << projP << endl;


  V3 L0(-40.0f, 0.0f, -400.0f);
  V3 L1(+40.0f, 0.0f, -100.0f);
  V3 color0(1.0f, 0.0f, 0.0f);
  V3 color1(0.0f, 1.0f, 0.0f);
  int n = 100;
  fb->Set(0xFFFFFFFF);
  for (int i = 0; i < n; i++) {
    float fraction = (float) i / (float) (n-1);
    V3 P = L0 + (L1-L0)*fraction;
    V3 color = color0 + (color1 - color0)*fraction;
    ppc->Project(P, projP);
    fb->DrawCircle(projP[0], projP[1], 5.0f, color.GetColor());
    fb->redraw();
    Fl::check();
  }

  return;

  fb->Set(0xFFFFFFFF);
  float cuf = 135.3f;
  float cvf = 81.9f;
  float radius = 51.0f;
  unsigned int color = 0xFF0000FF;
  fb->DrawCircle(cuf, cvf, radius, color);
  fb->redraw();

  float step = 1.0f;
  for (int stepsi = 0; stepsi < 100; stepsi++) {
    cuf += step;
    cvf += step;
    cvf += step;
    fb->Set(0xFFFFFFFF);
    fb->DrawCircle(cuf, cvf, radius, color);
    fb->redraw();
    Fl::check();
  }

  return;

  V3 v0(1.0f, 0.0f, -4.0f);
  V3 v1(2.0f, 1.0f, 0.0f);
  V3 v2(-1.0f, 2.0f, -1.0f);

  M33 m;
  m[0] = v0;
  m[1] = v1;
  m[2] = v2;
  M33 mi = m.Inverted();
  cerr << mi << endl;

  return;
  V3 v;
  unsigned int colorui;
  v.SetFromColor(0xFF0000FF);
  colorui = v.GetColor();
  v.SetFromColor(0xFF00FF00);
  colorui = v.GetColor();
  v.SetFromColor(0xFFFF0000);
  colorui = v.GetColor();
  v.SetFromColor(0xFF00FFFF);
  colorui = v.GetColor();
  v.SetFromColor(0xFF000088);
  colorui = v.GetColor();


  m[0] = V3(1.0f, 0.0f, 0.0f);
  m[1] = V3(0.0f, 1.0f, 0.0f);
  m[2] = V3(0.0f, 0.0f, 1.0f);

  v1 = m * v0;

  cerr << "Matrix: " << endl;
  cerr << m;

  return;

  
  float v0v1 = v0*v1;
  cerr << v0v1 << endl;
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