#include "scene.h"
#include "v3.h"
#include "m33.h"
#include <float.h>

#include <iostream>
#include <fstream>

using namespace std;

Scene *scene;

Scene::Scene() {
  animationFraction = 0.0f;
  cgi = 0;
  soi = 0;

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
  hwfb = new FrameBuffer(u0*2+w, v0, w, h);
  hwfb->label("Fixed Pipeline Framebuffer");
  hwfb->ishw = true;

  gpufb = new FrameBuffer(u0*3+w*2, v0, w, h);
  gpufb->label("Progr. Pipeline Framebuffer");
  gpufb->isgpu = true;

  // create camera
  float hfov = 55.0f;
  ppc = new PPC(hfov, w, h);

  // allocate pointer of TMesh objects
  tmsN = 5;
  tms = new TMesh*[tmsN];
  for (int tmi = 0; tmi < tmsN; tmi++)
    tms[tmi] = 0;

  tms[0] = new TMesh();
  tms[0]->LoadBin("geometry/teapot1K.bin");
  tms[0]->enabled = true;
  tms[0]->Translate(V3(50,0,30));
  
  tms[1] = new TMesh();
  tms[2] = new TMesh();
  tms[3] = new TMesh();
  tms[4] = new TMesh();
  V3 corners[4];
  float rw2 = 40.0f, rh2 = 40.0f, nearz = -100.0f, farz = -3000.0f;
//  farz = nearz;
  corners[0] = V3(-rw2, +rh2, nearz);
  corners[1] = V3(-rw2, -rh2, nearz);
  corners[2] = V3(+rw2, -rh2, nearz);
  corners[3] = V3(+rw2, +rh2, nearz);
  tms[1]->SetAsRectangle(corners);
  float eps = 4.0f;
  tms[1]->Translate(V3(0.0f, -rh2-eps, 0.0f));
  tms[2]->SetAsRectangle(corners);
  
  FrameBuffer *texture = new FrameBuffer(0, 0, w, h);
 
//  texture->show();
  texture->label("texture");
//  texture->SetCheckerboard(2, 0xFF000000, 0XFFAAAAFF);
  //texture->SetCheckerboard(2, 0xFF000000, 0xFFFFFFFF);
  texture->LoadTiff("texture.tif");
  tms[1]->texture = texture;
  tms[2]->texture = texture;

  corners[0] = V3(-100.0f, 100.0f, -5.0f);
  corners[1] = V3(-100.0f, -100.0f, -5.0f);
  corners[2] = V3(100.0f, -100.0f, -5.0f);
  corners[3] = V3(100.0f, 100.0f, -5.0f);
  tms[3]->LoadBin("geometry/teapot1K.bin");

  tms[2]->Translate(V3(80.0f, 0, 0.0f));
  tms[3]->Translate(V3(-80.0f, 0,-40.0f));

 billboardppc= new PPC(hfov, w, h);
 //billboardppc->C=tms[3]->GetCenter();
 //billboardppc->c=(tms[0]->GetCenter()-tms[3]->GetCenter())-(billboardppc->a*(0.5*billboardppc->w)+billboardppc->b*(0.5*billboardppc->h));
 billboardppc->PositionAndOrient(tms[3]->GetCenter(),tms[0]->GetCenter(),V3(0,1,0));
 corners[0] =billboardppc->C+billboardppc->c;
 corners[1] =billboardppc->a*billboardppc->w+corners[0];
 corners[2] =billboardppc->b*billboardppc->h+corners[0];
 corners[3] =billboardppc->a*billboardppc->w+billboardppc->b*billboardppc->h+corners[0];
 
 tms[4]->SetAsRectangle(corners);


 FrameBuffer *Billboardtexture = new FrameBuffer(40, 50, billboardppc->w, billboardppc->h);
 
 V3 lightPosition;
 float ambientk = 0.4f;
 Billboardtexture->Set(0xFFFFFFFF);
  Billboardtexture->ClearZB(0.0f);
 Billboardtexture->DrawFilledMode(tms[0],0, billboardppc, lightPosition, ambientk);
 tms[4]->texture =Billboardtexture;

  tms[1]->enabled = false;
  tms[2]->enabled = true;
  tms[3]->enabled = true;
  tms[4]->enabled = false;
  ppc->C = tms[0]->GetCenter() + V3(0.0f, 0.0f, 110.0f);

  fb->show();
  hwfb->show();
  gpufb->show();
 
  // position UI window
//  gui->uiw->position(fb->w+u0 + 2*20, v0);
  gui->uiw->position(u0, fb->h+v0+60);

  Render(ppc, fb);

}

void Scene::RenderHW(bool isgpu) {

  if (isgpu && cgi == NULL) {
    cgi = new CGInterface();
    cgi->PerSessionInit();
    soi = new ShaderOneInterface();
    soi->PerSessionInit(cgi);
  }

  // clear framebuffer
  glClearColor(1.0f, 1.0f, 1.0, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // set view intrinsics
  float near = 10.0f;
  float far = 1000.0f;
  ppc->SetIntrinsicsHW(near, far);
  // set view extrinsics
  ppc->SetExtrinsicsHW();
  int renderMode;
  renderMode=0;// 1 for wireframe mode,0 for filled mode
  // pass all triangle meshes to HW
  for (int tmi = 0; tmi < tmsN; tmi++) {
    if (!(tms[tmi] && tms[tmi]->enabled))
      continue;
	if(tmi==3)
		continue;
	 tms[tmi]->RenderHW(renderMode);

  }
  glEnable(GL_TEXTURE_2D);
	unsigned int billboardtexture;
	glGenTextures(1,&billboardtexture);
	glBindTexture(GL_TEXTURE_2D,billboardtexture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, tms[4]->texture->w,tms[4]->texture->h ,0,GL_RGBA,GL_UNSIGNED_BYTE,tms[4]->texture->pix);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	 textname=billboardtexture;
  if (isgpu) {
    cgi->EnableProfiles();
    soi->PerFrameInit();
    soi->BindPrograms();
  }

  tms[3]->RenderHW(renderMode);
  
   if (isgpu) {
    soi->PerFrameDisable();
    cgi->DisableProfiles();
  }

}

void Scene::RenderFB(FrameBuffer *fbToRender, PPC *fbPPC, PPC *currPPC, 
  FrameBuffer *currFB, float constz) {

  for (int v = 0; v < fbToRender->h; v++) {
    for (int u = 0; u < fbToRender->w; u++) {
      int uv = (fbToRender->h-v-1)*fbToRender->w+u;
      if (fbToRender->zb[uv] == 0.0f)
        continue;
      V3 projP = V3(.5f+(float)u, .5f+(float)v, fbToRender->zb[uv]);
      if (constz > 0.0f) {
        projP[2] = constz;
      }
      V3 P = fbPPC->Unproject(projP);
      V3 projP2;
      if (!currPPC->Project(P, projP2))
        continue;
      V3 currCol;
      currCol.SetFromColor(fbToRender->pix[uv]);
      currFB->SetIfCloser(projP2, currCol);
    }
  }

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

void Scene::SetupStitching() {

  int border = 10;

  im0 = new FrameBuffer(border, 50, 10, 10);
  im0->LoadTiff("mydbg/left.tif");
  im0->label("left");
  im0->show();

  im1 = new FrameBuffer(border*2+im0->w, 50, 10, 10);
  im1->LoadTiff("mydbg/center.tif");
  im1->label("center");
  im1->show();

  im2 = new FrameBuffer(border*3+im0->w*2, 50, 10, 10);
  im2->LoadTiff("mydbg/right.tif");
  im2->label("right");
//  im2->show();

//  canvas = new FrameBuffer(border, im0->h+100, 800, 450);
  canvas = new FrameBuffer(border, 50, 1600, 900);
  canvas->label("canvas");
  canvas->Set(0xFF000000);
  canvas->show();

  fb->hide();
  gui->uiw->hide();

  float hfov = 63.7f;
  ppc0 = new PPC(hfov, im0->w, im0->h);
  ppc1 = new PPC(hfov, im0->w, im0->h);
  ppc2 = new PPC(hfov, im0->w, im0->h);
  ppcc = new PPC(110.0f, canvas->w, canvas->h);


}

// function linked to the DBG GUI button for testing new features
void Scene::DBG() {

 
   
    Fl::check();
	int framesN = 1000;
    for (int fi = 0; fi < framesN; fi++) {
    animationFraction = (float) fi / (float) (framesN-1);
	ppc->C = ppc->C + V3(-0.8f, 0.0f, 0.0f);
	Render(ppc, fb);
    hwfb->redraw();
    gpufb->redraw();
    V3 center = tms[0]->GetCenter();
       Fl::check();
    }


  
  


  return;








  {
    Render(ppc, fb);
//    fb->SetCheckerboard(64, 0xFF000000, 0xFFFFFFFF);
//    fb->redraw();
    int n = 3;
    float **krn = new float*[n];
    for (int i = 0; i < n; i++) {
      krn[i] = new float[n];
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        krn[i][j] = 1.0f;
      }
    }

    krn[0][0] = 0.0f;
    krn[0][1] = -1.0f;
    krn[0][2] = 0.0f;

    krn[1][0] = -1.0f;
    krn[1][1] = 4.0f;
    krn[1][2] = -1.0f;

    krn[2][0] = 0.0f;
    krn[2][1] = -1.0f;
    krn[2][2] = 0.0f;

    im0 = new FrameBuffer(50, 50, 10, 10);
    im0->LoadTiff("mydbg/left.tif");
    im0->label("left");
    im0->show();

    FrameBuffer *cvfb = im0->Convolve(krn, n);
    cvfb->label("convolved im0");
    cvfb->position(40+im0->w, 50);
    cvfb->show();
    return;
  }

  {
    ppc->C = ppc->C + V3(0.0f, 0.0f, -10.0f);
    int stepsN = 100000;
    for (int si = 0; si < stepsN; si++) {
      Render(ppc, fb);
      ppc->C = ppc->C+V3(0.0f, 0.0f, 0.2f);
      Fl::check();
      break;
    }
    return;
  }

  {
    SetupStitching();
    V3 *centerCorrs, *leftCorrs;
    ifstream ifs("mydbg/corrs.txt");
    int corrsN;
    ifs >> corrsN;
    centerCorrs = new V3[corrsN];
    leftCorrs = new V3[corrsN];
    unsigned int colors[6] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF00FFFF, 
      0xFFFF00FF, 0xFFFFFF00};
    for (int ci = 0; ci < corrsN; ci++) {
      int u, v;
      ifs >> u >> v;
      leftCorrs[ci][0] = .5f + (float) u;
      leftCorrs[ci][1] = .5f + (float) v;
      leftCorrs[ci][2] = 1.0f;
      im0->DrawCircle(leftCorrs[ci][0], leftCorrs[ci][1], 7.0f, colors[ci%6]);
      ifs >> u >> v;
      centerCorrs[ci][0] = .5f + (float) u;
      centerCorrs[ci][1] = .5f + (float) v;
      centerCorrs[ci][2] = 1.0f;
      im1->DrawCircle(centerCorrs[ci][0], centerCorrs[ci][1], 7.0f, colors[ci%6]);
    }
    cerr << "INFO: done reading " << corrsN << " correspondences" << endl;
    ifs.close();

    im0->redraw();
    im1->redraw();

    float minError = FLT_MAX;
    float minPan, minTilt, currPan, currTilt, minRoll, currRoll;
    float pan0 = 16.0f, pan1 = 22.0f;
    float tilt0 = 0.0f, tilt1 = 2.0f;
    float roll0 = -3.0f, roll1 = -1.0f;
    int panStepsN = 25, tiltStepsN = 5, rollStepsN = 5;
    int stepi = 0;
    for (int pani = 0; pani < panStepsN; pani++) {
      currPan = pan0 + (pan1-pan0) / (float) (panStepsN-1) *(float)pani;
      for (int tilti = 0; tilti < tiltStepsN; tilti++) {
        currTilt = tilt0 + (tilt1-tilt0) / (float) (tiltStepsN-1) *(float)tilti;
        for (int rolli = 0; rolli < rollStepsN; rolli++) {
          currRoll = roll0 + (roll1-roll0) / (float) (rollStepsN-1) *(float)rolli;
          PPC currppc(*ppc0);
          currppc.Pan(currPan);
          currppc.Tilt(currTilt);
          currppc.Roll(currRoll);
          //        float currError = im1->DiffAtOverlap(im0, &currppc, ppc1);
          float currError = im1->DiffCorrReproj(im0, &currppc, leftCorrs, 
            centerCorrs, ppc1, corrsN);
          if (currError < minError) {
            minPan = currPan;
            minTilt = currTilt;
            minRoll = currRoll;
            minError = currError;
            cerr << endl;
            cerr << "INFO: (pan, tilt, roll), error: ("
              << minPan << ", " << minTilt << ", " << minRoll << "), "
              << sqrtf(minError) << endl;
            canvas->Set(0xFF000000);
            canvas->SetFromImageWithSameEye(im1, ppc1, ppcc);
            canvas->SetFromImageWithSameEye(im0, &currppc, ppcc);
            canvas->redraw();
            Fl::check();
          }
          stepi++;
          float pp = (float) stepi / (float) (rollStepsN * tiltStepsN * panStepsN); 
          pp = 0.01f * (float)((int)(pp*10000.0f));
          cerr << "PROGRESS: " << pp << "%          \r";
        }
      }
    }

    cerr << endl << "DONE" << endl;

  }
  return;

  ppc3 = new PPC(*ppc);
  ppc3->C = ppc3->C + V3(80.0f, 60.0f, 50.0f);
//  ppc3->C = ppc3->C + V3(0.0f, 0.0f, -30.0f);
  fb3 = new FrameBuffer(fb->w+20+20, 50, ppc3->w, ppc3->h);
  fb3->label("Third person FB");
  fb3->Set(0xFF00AAFF);
  fb3->show();
  // position UI window
  gui->uiw->position(fb->w+ 3*20 +fb3->w, 50);


  V3 camTransV(0.0f, 150.0f, 0.0f);
  for (int fi = 0; fi < framesN; fi++) {
    ppc3->PositionAndOrient(ppc3->C, tms[0]->GetCenter(), V3(0.0f, 1.0f, 0.0f));
    Render(ppc, fb);

    fb3->ClearZB(0.0f);
    fb3->Set(0xFFFFFFFF);
    RenderFB(fb, ppc, ppc3, fb3);
    float visF = 30.0f;
    ppc->VisualizeCamera(ppc3, fb3, visF);
    RenderFB(fb, ppc, ppc3, fb3, ppc3->GetF() / visF);
    fb3->DrawWireFrame(tms[0], ppc3); 
    fb3->redraw();
    Fl::check();
    ppc->C = ppc->C + camTransV/(float)framesN;
    ppc->PositionAndOrient(ppc->C, tms[0]->GetCenter(), V3(0.0f, 1.0f, 0.0f));
  }
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
