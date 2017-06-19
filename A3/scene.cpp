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
  fb1 = new FrameBuffer(u0, v0, w, h);
  fb2 = new FrameBuffer(u0, v0, w, h);
  fb3 = new FrameBuffer(u0, v0, w, h);
  fb4 = new FrameBuffer(u0, v0, w, h);
  fb5 = new FrameBuffer(u0, v0, w, h);
  fb->label("SW Framebuffer");
  fb1->label("SW Framebuffer1");
  fb2->label("SW Framebuffer2");
  fb3->label("SW Framebuffer3");
  fb4->label("SW Framebuffer4");
  fb5->label("SW Framebuffer5");
  // create camera
  float hfov = 55.0f;
  ppc = new PPC(hfov, w, h);

  // allocate pointer of TMesh objects
  tmsN = 1;
  tms = new TMesh*[tmsN];
  tms[0] = 0; 

  
  fb1->show();
  
  
  // position UI window  
   gui->uiw->position(fb->w+u0 + 2*20, v0);
   
}



// function linked to the DBG GUI button for testing new features
void Scene::DBG() {  
  float kp=0.0f;
  float ambientk = 0.1f;
  fb->LoadTiff("brick.tif");// this tiff is from web
  fb1->Set(0xFFFFFFFF);
  V3 vs[3];
  V3 vertA(5.0f,5.0f,65.0f);
  V3 vertB(15.0f,5.0f,65.0f);
  V3 vertC(5.0f,15.0f,65.0f);
  V3 vertD(15.0f,15.0f,65.0f);
  V3 normals=(vertA-vertB)^(vertB-vertC);
  normals=normals.UnitVector();
  
  ppc->C = V3(0.0f,0.0f,100.0f);
  ppc->Translate(V3(25,0,0));
  V3 lightPosition = ppc->C;
  vs[0]=vertA;
  vs[1]=vertB;
  vs[2]=vertC;
  V3 tile(10.0f,0.0f,0.0f);
  V3 s(0,1,0);
  V3 t(1,1,0);
  for(int i=0;i<=3;i++){
  fb1->DrawTextureTri(vs,ppc,s,t,fb,normals,lightPosition, ambientk,kp);
  fb1->redraw();
  vs[0]=vs[0]+tile;
  vs[1]=vs[1]+tile;
  vs[2]=vs[2]+tile;
  }
  V3 vs2[3];
  vs2[0]=vertB;
  vs2[1]=vertC;
  vs2[2]=vertD;
  s=V3(1,0,1);
  t=V3(0,1,1);
  for(int i=0;i<=3;i++){
  fb1->DrawTextureTri(vs2,ppc,s,t,fb,normals,lightPosition, ambientk,kp);
  fb1->redraw();
  vs2[0]=vs2[0]+tile;
  vs2[1]=vs2[1]+tile;
  vs2[2]=vs2[2]+tile;
  }


  fb2->LoadTiff("knit.tif");// this tiff is from web.
  s=V3(0,1,0);
  t=V3(1,1,0);
  vertA=V3(5.0f,5.0f,65.0f);
  vertB=V3(15.0f,5.0f,65.0f);
  vertC=V3(5.0f,-5.0f,65.0f);
  vertD=V3(15.0f,-5.0f,65.0f);
  vs[0]=vertA;
  vs[1]=vertB;
  vs[2]=vertC;
  fb1->DrawTextureTri(vs,ppc,s,t,fb2,normals,lightPosition, ambientk,kp);
  fb1->redraw();
  vs2[0]=vertB;
  vs2[1]=vertC;
  vs2[2]=vertD;
  s=V3(1,0,1);
  t=V3(1,0,0);
  fb1->DrawTextureTri(vs2,ppc,s,t,fb2,normals,lightPosition,ambientk,kp);
  fb1->redraw();
 

  fb3->LoadTiff("mywallpaper.tif");// this tiff is from my camera.
  
  vertA=V3(15.0f,5.0f,65.0f);
  vertB=V3(25.0f,5.0f,65.0f);
  vertC=V3(15.0f,-5.0f,65.0f);
  vertD=V3(25.0f,-5.0f,65.0f);
  V3 vs3[3];
  vs3[0]=vertA;
  vs3[1]=vertB;
  vs3[2]=vertC;
  V3 vs4[3];
  vs4[0]=vertB;
  vs4[1]=vertC;
  vs4[2]=vertD;
  V3 center(50.0f,0.0f,65.0f);
   
  V3 L0 = lightPosition;
  V3 L1 = center + V3(0.0f, 100.0f, 0.0f);
 
    for (int i = 0; i < 200; i++) {
      lightPosition = L0 + (L1-L0)*(float)i / 99.0f;
      s=V3(0,1,0);
	  t=V3(1,1,0);
      fb1->DrawTextureTri(vs3,ppc,s,t,fb3,normals,lightPosition,ambientk,kp);
      fb1->redraw();
	  s=V3(1,0,1);
	  t=V3(1,0,0);
	  fb1->DrawTextureTri(vs4,ppc,s,t,fb3,normals,lightPosition,ambientk,kp);
      fb1->redraw();
      Fl::check();
    }

  fb4->LoadTiff("reflect.tif");// this tiff is from web
    L0 = lightPosition;
    L1 = center + V3(0.0f, 100.0f, 0.0f);
  vertA=V3(25.0f,5.0f,65.0f);
  vertB=V3(35.0f,5.0f,65.0f);
  vertC=V3(25.0f,-5.0f,65.0f);
  vertD=V3(35.0f,-5.0f,65.0f);
  
  vs3[0]=vertA;
  vs3[1]=vertB;
  vs3[2]=vertC;
 
  vs4[0]=vertB;
  vs4[1]=vertC;
  vs4[2]=vertD;
  kp=0.3f;
  for (int i = 0; i < 100; i++) {
  lightPosition = L0 + (L1-L0)*(float)i / 99.0f;
 
  s=V3(0,1,0);
  t=V3(1,1,0);
  fb1->DrawTextureTri(vs3,ppc,s,t,fb4,normals,lightPosition,ambientk,kp);
  fb1->redraw();
  s=V3(1,0,1);
  t=V3(1,0,0);
  fb1->DrawTextureTri(vs4,ppc,s,t,fb4,normals,lightPosition,ambientk,kp);
  fb1->redraw();
  }

  fb5->LoadTiff("stone.tif");
  V3 vs5[4];
  vs5[0]=V3(35.0f,5.0f,65.0f);
  vs5[1]=V3(45.0f,5.0f,65.0f);
  vs5[2]=V3(35.0f,-5.0f,65.0f);
  vs5[3]=V3(45.0f,-5.0f,65.0f);
  fb1->DrawBilinear(vs5,ppc,fb5);
  fb1->redraw();


  //tms[0] = new TMesh();
  //tms[0]->LoadBin("geometry/teapot1K.bin");
  return;

}

void Scene::NewB() {

  cerr << "NewB pressed" << endl;

}