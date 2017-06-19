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
  int h = sci*160;
  fb = new FrameBuffer(u0, v0, w, h);
  fb->label("SW Framebuffer");

  // create camera
  float hfov = 55.0f;
  ppc = new PPC(hfov, w, h);

  // allocate pointer of TMesh objects
  tmsN = 1;
  tms = new TMesh*[tmsN];
  tms[0] = 0;

  fb->show();
  
  // position UI window
  gui->uiw->position(fb->w+u0 + 2*20, v0);

}



// function linked to the DBG GUI button for testing new features
void Scene::DBG() {
  tms[0] = new TMesh();
  tms[0]->LoadBin("geometry/teapot1K.bin");
  


  V3 verts0[4], colors0[4];
  verts0[0] = V3(50.0f, -25.0f, -100.0f);
  colors0[0] = V3(0.0f, 0.0f, 0.0f);
  verts0[1] = V3(30.0f, -75.0f, -100.0f);
  colors0[1] = V3(1.0f, 0.0f, 0.0f);
  verts0[2] = V3(57.0f, -75.0f, -50.0f);
  colors0[2] = V3(0.0f, 1.0f, 0.0f);
  verts0[3] = V3(90.0f, -75.0f, -130.0f);
  colors0[3] = V3(0.0f, 0.0f, 1.0f);
  tms[1] = new TMesh(verts0, colors0);
 
 
 V3 verts1[5], colors1[5];
  verts1[0] = V3(150.0f, -25.0f, -100.0f);
  colors1[0] = V3(0.5f, 0.5f, 0.0f);
  verts1[1] = V3(130.0f, -75.0f, -100.0f);
  colors1[1] = V3(0.0f, 0.5f, 0.5f);
  verts1[2] = V3(157.0f, -75.0f, -50.0f);
  colors1[2] = V3(0.8f, 0.0f, 0.0f);
  verts1[3] = V3(190.0f, -75.0f, -130.0f);
  colors1[3] = V3(0.0f, 0.0f, 0.7f);
  tms[2] = new TMesh(verts1, colors1);

   V3 verts2[4], colors2[4];
  verts2[0] = V3(5.0f, -25.0f, -100.0f);
  colors2[0] = V3(0.0f, 0.5f, 0.0f);
  verts2[1] = V3(-20.0f, -75.0f, -100.0f);
  colors2[1] = V3(0.0f, 0.5f, 0.5f);
  verts2[2] = V3(9.0f, -75.0f, -50.0f);
  colors2[2] = V3(0.8f, 0.0f, 0.0f);
  verts2[3] = V3(15.0f, -75.0f, -130.0f);
  colors2[3] = V3(0.0f, 0.0f, 0.7f);
 tms[3] = new TMesh(verts2, colors2);

  V3 verts3[5], colors3[5];
  verts3[0] = V3(150.0f, 65.0f, -100.0f);
  colors3[0] = V3(0.9f, 0.5f, 0.0f);
  verts3[1] = V3(130.0f, 0.0f, -100.0f);
  colors3[1] = V3(0.0f, 0.9f, 0.5f);
  verts3[2] = V3(157.0f, 0.0f, -50.0f);
  colors3[2] = V3(0.8f, 0.6f, 0.0f);
  verts3[3] = V3(190.0f, 0.0f, -130.0f);
  colors3[3] = V3(0.0f, 0.0f, 1.0f);
  tms[4] = new TMesh(verts3, colors3);

  V3 verts4[5], colors4[5];
  verts4[0] = V3(75.0f, 35.0f, -100.0f);
  colors4[0] = V3(0.9f, 0.5f, 0.0f);
  verts4[1] = V3(65.0f, 0.0f, -100.0f);
  colors4[1] = V3(0.4f, 0.9f, 0.5f);
  verts4[2] = V3(78.0f, 0.0f, -50.0f);
  colors4[2] = V3(0.6f, 0.6f, 0.0f);
  verts4[3] = V3(95.0f, 0.0f, -130.0f);
  colors4[3] = V3(0.7f, 0.0f, 1.0f);
  tms[5] = new TMesh(verts4, colors4);


  ppc->C = V3(10.0f, 8.0f, 400.0f);
  int stepsN = 361;
  float theta = 1.0f;
  V3 center = tms[0]->GetCenter();
  V3 aDir=ppc->b;
  aDir.normalize();
  float hfov1 = 55.0f;
  PPC *ppc1;
  ppc1 = new PPC(hfov1, 960, 480);
  ppc1->Transtoright(-1);

  for (int si = 0; si < stepsN; si++) {
    fb->Set(0xFFFFFFFF);
    fb->ClearZB(0.0f);
    fb->DrawFilledTMesh(tms[0], ppc);
	fb->DrawWireFrame(tms[1],ppc);
	fb->DrawWireFrame(tms[2],ppc);
	fb->DrawWireFrame(tms[3],ppc);
	fb->DrawWireFrame(tms[4],ppc);
	fb->DrawWireFrame(tms[5],ppc);
    fb->redraw();
    Fl::check();
	if(si>=int((float)stepsN/(3.5f))){
	ppc->SetByInterpolation(ppc,ppc1,1,100);
	}
    tms[0]->RotateAboutAxis(center, aDir, theta);
	tms[1]->RotateAboutAxis(center, aDir, theta);
	tms[2]->RotateAboutAxis(center, aDir, theta);
	tms[3]->RotateAboutAxis(center, aDir, theta);
	tms[4]->RotateAboutAxis(center, aDir, theta);
	tms[5]->RotateAboutAxis(center, aDir, theta);

  }
  
  

  return;
  /*fb->Set(0xFFFFFFFF);
  ppc->C = V3(10.0f, 8.0f, 400.0f);
  fb->DrawFilledTMesh(tms[0], ppc);
  fb->DrawFilledTMesh(tms[1],ppc);
  fb->DrawFilledTMesh(tms[2],ppc);
  fb->DrawWireFrame(tms[3],ppc);
  fb->DrawFilledTMesh(tms[4],ppc);
  fb->DrawWireFrame(tms[5],ppc);
  int dotSize = 3;*/



  // fb->DrawVertexDots(tms[0], dotSize, ppc);
  center = tms[0]->GetCenter();
  V3 projCenter;
  ppc->Project(center, projCenter);
  fb->DrawCircle(projCenter[0], projCenter[1], 7, 0xFF0000FF);
  AABB aabb = tms[0]->ComputeAABB();
  fb->DrawAABB(aabb, 0xFF00FF00, 0xFF000000, ppc);
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