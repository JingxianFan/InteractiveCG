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
  fb->label("light");

 
  fbt = new FrameBuffer(u0, v0, w, h);
  fbt->label("texture");


  // create camera
  float hfov = 55.0f;
  ppc = new PPC(hfov, w, h);
  ppc->C= ppc->C + V3(-40.0f, 40.0f, 50.0f);

  ppct = new PPC(*ppc);
  ppct->C= ppct->C + V3(-40.0f, 40.0f, 50.0f);

  // allocate pointer of TMesh objects
  tmsN = 6;
  tms = new TMesh*[tmsN];
  for (int tmi = 0; tmi < tmsN; tmi++)
    tms[tmi] = 0;

  tms[0] = new TMesh();
  tms[0]->LoadBin("geometry/teapot1K.bin");
  tms[0]->enabled = true;
  
  tms[1] = new TMesh();
  tms[2] = new TMesh();
 

  V3 verts[4], colors[4];
  V3 backgroundcenter=tms[0]->GetCenter();
  verts[0] = backgroundcenter+V3(1.0f,-10.0f,40.0f);
  colors[0]=V3(1.0f,0.0f,0.0f);
  verts[1] = backgroundcenter+V3(-3.0f,20.0f,38.0f);
  colors[1]=V3(0.0f,1.0f,0.0f);
  verts[2] = backgroundcenter+V3(-3.0f,-10.0f,38.0f);
  colors[2]=V3(0.0f,0.0f,1.0f);
  verts[3] = backgroundcenter+V3(-3.0f,20.0f,40.0f);
  colors[3]=V3(0.5f,0.5f,0.0f);
  tms[3] = new TMesh(verts, colors);
  tms[3]->enabled = false;


  V3 verts_[4], colors_[4];
  verts_[0] = backgroundcenter+V3(380.0f,-80.0f,-80.0f);
  colors_[0]=V3(1.0f,0.0f,0.0f);
  verts_[1] = backgroundcenter+V3(-220.0f,250.0f,-80.0f);
  colors_[1]=V3(0.0f,1.0f,0.0f);
  verts_[2] = backgroundcenter+V3(-220.0f,-100.0f,-75.0f);
  colors_[2]=V3(0.0f,0.0f,1.0f);
  verts_[3] = backgroundcenter+V3(-220.0f,250.0f,-75.0f);
  colors_[3]=V3(0.5f,0.5f,0.0f);
  tms[4] = new TMesh(verts_,colors_);
  tms[4]->Translate(V3(2.0f,2.0f,-150.0f));
  tms[4]->enabled = true;

  tms[5] = new TMesh();
  tms[5]->LoadBin("geometry/teapot1K.bin");
  tms[5]->Translate(V3(25.0f,20.0f,-130.0f));
 
  tms[5]->enabled =true;

  
  V3 corners[4];
  float rw2 = 40.0f, rh2 = 15.0f, nearz = -100.0f, farz = -300.0f;
  // farz = nearz;
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
  texture->SetCheckerboard(8,0XFFAAAAF , 0XFFAAAAFF);
  tms[1]->texture = texture;
  tms[2]->texture = texture;
  tms[3]->texture = 0;
  tms[4]->texture = 0;
 

  tms[1]->enabled = false;
  tms[2]->enabled = false;
  ppc->C = tms[0]->GetCenter() + V3(0.0f, 0.0f, 110.0f);

  fb->show();


  
  // position UI window
  gui->uiw->position(fb->w+u0 + 2*20, v0);

}

void Scene::RenderFB(FrameBuffer *fbToRender, PPC *fbPPC, PPC *currPPC, 
  FrameBuffer *currFB, float constz) {

  for (int v = 0; v < currFB->h; v++) {
    for (int u = 0; u < currFB->w; u++) {
      int uv = (currFB->h-v-1)*currFB->w+u;
      if (currFB->zb[uv] == 0.0f)
        continue;
      V3 projP = V3(.5f+(float)u, .5f+(float)v, currFB->zb[uv]);
      if (constz > 0.0f) {
        projP[2] = constz;
      }
      V3 P = currPPC->Unproject(projP);
      V3 projP2;
      if (fbPPC->Project(P, projP2)){
		  if(0<projP2[0] && projP2[0]<fbToRender->w){
			if(0<projP2[1]&& projP2[1]<fbToRender->h){
				 int u2v2 = (fbToRender->h-(int)(projP2[1])-1)*fbToRender->w+(int)(projP2[0]);
				 V3 currCol;
				 currCol.SetFromColor(fbToRender->pix[u2v2]);
				 if(currCol.GetColor()>0x00ffffff){
					currFB->pix[uv]=currCol.GetColor();
				 }
				 
		  }
		 }
	  }
    }
  }

}

void Scene::RenderShadow(FrameBuffer *fbToRender, PPC *fbPPC, PPC *currPPC, 
  FrameBuffer *currFB, float constz) {
	
    for (int v = 0; v < currFB->h; v++) {
    for (int u = 0; u < currFB->w; u++) {
      int uv = (currFB->h-v-1)*currFB->w+u;
      V3 projP = V3(0.5f+(float)u, 0.5f+(float)v, currFB->zb[uv]);
	  if (currFB->zb[uv]!=0){
		 V3 P1 = currPPC->Unproject(projP);
	     V3 projP2;
		 if (!fbPPC->Project(P1, projP2))
			continue;
		 int u2v2 = (fbToRender->h-(int)(projP2[1])-1)*fbToRender->w+(int)(projP2[0]);
		 V3 black(0.0f,0.0f,0.0f); 
	
		 if(0<projP2[0] && projP2[0]<fbToRender->w){
			if(0<projP2[1]&& projP2[1]<fbToRender->h){
				if(fbToRender->zb[u2v2] > projP2[2]+0.1f){
					currFB->pix[uv]=black.GetColor();
				}
			}
		}
	  }
    }
  }

}

void Scene::Render(PPC *currPPC, FrameBuffer *currFB,V3 lightPosition) {

  currFB->Set(0xFFFFFFFF);
  currFB->ClearZB(0.0f);
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
  ppc3->C = ppc3->C + V3(20.0f, 20.0f, 0.0f);
  ppc3->C = ppc3->C + V3(0.0f, 20.0f, 50.0f);
  fb3 = new FrameBuffer(fb->w+20+20, 50, ppc3->w, ppc3->h);
  fb3->label("Third person FB");
  fb3->Set(0xFF00AAFF);
  fb3->show();
  fbt->LoadTiff("water.tif");
 
  // position UI window
  gui->uiw->position(fb->w+ 3*20 +fb3->w,50);

  int framesN = 450;
  V3 lightPosition;
  float ambientk;
  V3 stepV(-0.005f,0.0f,0.0f);
  for(int fi=0;fi<framesN;fi++){
	 ppc->C=ppc->C+stepV*fi;
	 ppct->C=ppct->C+stepV*fi;
	 lightPosition=ppc->a*(float)ppc->w/2+ppc->b*(float)ppc->h/2+ppc->c;
	 ambientk=0.4f;
   
	 V3 camTransV(0.0f, 150.0f, 0.0f);
	 ppc3->PositionAndOrient(ppc3->C, tms[0]->GetCenter(), V3(0.0f, 1.0f, 0.0f));
	 fb3->ClearZB(0.0f);
	 fb3->Set(0xFFFFFFFF);
	 float constz=0;
	 Render(ppc, fb,lightPosition);
	 Render(ppc3,fb3,lightPosition);
	 RenderFB(fbt,ppct,ppc3,fb3,constz);

	RenderShadow(fb,ppc,ppc3,fb3,constz);

	 
	 //Render(ppct,fbt,lightPosition);
	 //RenderFB(fbt,ppct,ppc3,fb3,constz);
	 // float visF = 30.0f;
	 // ppc->VisualizeCamera(ppc3, fb3, visF);
	 // RenderFB(fb, ppc, ppc3, fb3, ppc3->GetF() / visF);
	 // fb3->DrawFilledMode(tms[3], 4,ppc3, lightPosition,ambientk);
	 fb3->redraw();
	 Fl::check();

  }
  
  return;

 

  V3 center = tms[0]->GetCenter();
  ppc->C = center + V3(0.0f, 0.0f, 110.0f);
  fb->Set(0xFFFFFFFF);
  fb->ClearZB(0.0f);
//  fb->DrawWireFrame(tms[0], ppc);
  
  //float ambientk=0.4f;
  //V3 lightPosition;
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