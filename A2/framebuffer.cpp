#include "framebuffer.h"
#include <iostream>
#include "scene.h"
#include <math.h>


#include "tiffio.h"

using namespace std;

// makes an OpenGL window that supports SW, HW rendering, that can be displayed on screen
//        and that receives UI events, i.e. keyboard, mouse, etc.
FrameBuffer::FrameBuffer(int u0, int v0, 
  int _w, int _h) : Fl_Gl_Window(u0, v0, _w, _h, 0) {

  w = _w;
  h = _h;
  pix = new unsigned int[w*h];
  zb = new float[w*h];

}

// rendering callback; see header file comment
void FrameBuffer::draw() {

	// SW window, just transfer computed pixels from pix to HW for display
	glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);

}


// function called automatically on event within window (callback)
int FrameBuffer::handle(int event)  {  

  switch(event) 
  {   
  case FL_KEYBOARD: {
    KeyboardHandle();
    return 0;
  }
  default:
    break;
  }
  return 0;

}

// keyboard handle
void FrameBuffer::KeyboardHandle() {

  int key = Fl::event_key();
  switch (key) {
    case FL_Left: {
      cerr << "pressed left arrow" << endl;
      break;
    }
    case 'a': {
      cerr << "pressed a" << endl;
	 
      break;
    }
    default:
    cerr << "INFO: do not understand keypress" << endl;
  }
}

// clear to background color
void FrameBuffer::Set(unsigned int color) {

  for (int i = 0; i < w*h; i++) {
    pix[i] = color;
  }

}

// clear to far
void FrameBuffer::ClearZB(float farz) {

  for (int i = 0; i < w*h; i++) {
    zb[i] = farz;
  }

}
// set pixel with coordinates u v to color provided as parameter
void FrameBuffer::SetSafe(int u, int v, unsigned int color) {

  if (u < 0 || u > w-1 || v < 0 || v > h-1)
    return;

  Set(u, v, color);

}

void FrameBuffer::Set(int u, int v, unsigned int color) {

  pix[(h-1-v)*w+u] = color;

}

// set to checkboard
void FrameBuffer::SetCheckerboard(int checkerSize, unsigned int color0, 
  unsigned int color1) {

  for (int v = 0; v < h; v++) {
    for (int u = 0; u < w; u++) {
      int cu = u / checkerSize;
      int cv = v / checkerSize;
      if (((cu + cv) % 2) == 0) {
        Set(u, v, color0);
      }
      else {
        Set(u, v, color1);
      }
    }
  }

}

// draw circle
void FrameBuffer::DrawCircle(float cuf, float cvf, float radius, 
  unsigned int color) {

// axis aligned bounding box (AABB) of circle (it's a square parallel to axes)
    int top = (int) (cvf - radius + 0.5f);
    int bottom = (int) (cvf + radius - 0.5f);
    int left = (int) (cuf - radius + 0.5f);
    int right = (int) (cuf + radius - 0.5f);

// clip AABB with frame
    if (top > h-1 || bottom < 0 || right < 0 || left > w-1)
      return;
    if (left < 0)
      left = 0;
    if (right > w-1)
      right = w-1;
    if (top < 0)
      top = 0;
    if (bottom > h-1)
      bottom = h-1;

    float radius2 = radius*radius;
    for (int v = top; v <= bottom; v++) {
      for (int u = left; u <= right; u++) {
        float uf = .5f + (float)u;
        float vf = .5f + (float)v;
        float d2 = (cvf-vf)*(cvf-vf)+(cuf-uf)*(cuf-uf);
        if (d2 > radius2)
          continue;
        Set(u, v, color);
      }
    }

}

// save as tiff image
void FrameBuffer::SaveAsTiff(char *fname) {

  TIFF* out = TIFFOpen(fname, "w");

  if(out == NULL) {
    cerr << fname << " could not be opened" << endl;
    return;
  }

  cerr << "INFO: yet to be implemented" << endl;
  
  TIFFClose(out);

}
// load/save as text file

    

// draw vertex dots
void FrameBuffer::DrawVertexDots(TMesh *tm, int dotSize, PPC *ppc) {

  for (int vi = 0; vi < tm->vertsN; vi++) {
    V3 projV, projP;
    if (!ppc->Project(tm->verts[vi], projP))
      continue;
    DrawCircle(projP[0], projP[1], dotSize, tm->cols[vi].GetColor());
  }

}

// draw TMesh in wire frame
void FrameBuffer::DrawWireFrame(TMesh *tm, PPC *ppc) {
  for (int tri = 0; tri < tm->trisN; tri++) {
    V3 currvs[3];
    currvs[0] = tm->verts[tm->tris[3*tri+0]];
    currvs[1] = tm->verts[tm->tris[3*tri+1]];
    currvs[2] = tm->verts[tm->tris[3*tri+2]];
    V3 currcols[3];
    currcols[0] = tm->cols[tm->tris[3*tri+0]];
    currcols[1] = tm->cols[tm->tris[3*tri+1]];
    currcols[2] = tm->cols[tm->tris[3*tri+2]];
    for (int ei = 0; ei < 3; ei++) {
      Draw3DSegment(currvs[ei], currcols[ei], 
        currvs[(ei+1)%3], currcols[(ei+1)%3], ppc);
    }
  }
}
//draw TMesh in filled mode with zbuffering
void FrameBuffer::DrawFilledTMesh(TMesh *tm, PPC *ppc){
  for (int tri = 0; tri < tm->trisN; tri++){
	V3 currvs[3];
    currvs[0] = tm->verts[tm->tris[3*tri+0]];
    currvs[1] = tm->verts[tm->tris[3*tri+1]];
    currvs[2] = tm->verts[tm->tris[3*tri+2]];
    V3 currcols[3];
    currcols[0] = tm->cols[tm->tris[3*tri+0]];
    currcols[1] = tm->cols[tm->tris[3*tri+1]];
    currcols[2] = tm->cols[tm->tris[3*tri+2]];
	V3 projC[3];
	if(ppc->Project(currvs[0],projC[0]) && ppc->Project(currvs[1],projC[1]) && ppc->Project(currvs[2],projC[2])){
		
		M33 screenMatrix;
		V3 R(currcols[0][0],currcols[1][0],currcols[2][0]);
		V3 G(currcols[0][1],currcols[1][1],currcols[2][1]);
		V3 B(currcols[0][2],currcols[1][2],currcols[2][2]);
		V3 Z(currvs[0][2],currvs[1][2],currvs[2][2]);
		screenMatrix[0][0]=projC[0][0];
		screenMatrix[0][1]=projC[0][1];
		screenMatrix[0][2]=1;
		screenMatrix[1][0]=projC[1][0];
		screenMatrix[1][1]=projC[1][1];
		screenMatrix[1][2]=1;
		screenMatrix[2][0]=projC[2][0];
		screenMatrix[2][1]=projC[2][1];
		screenMatrix[2][2]=1;
		V3 parameterR = (screenMatrix.Inverted())*R;// RED's abc
	    V3 parameterG = (screenMatrix.Inverted())*G;// GREEN's abc
		V3 parameterB = (screenMatrix.Inverted())*B;// BLUE's abc
		V3 parameterZ = (screenMatrix.Inverted())*Z;// z'abc
		DrawTriangle(projC[0][0],projC[0][1],projC[1][0],projC[1][1],projC[2][0],projC[2][1],parameterR,parameterG,parameterB,parameterZ);
	}
  }
}

// draw triangle
 void FrameBuffer::DrawTriangle(float u1,float v1,float u2,float v2,float u3,float v3,V3 parameterR,V3 parameterG,V3 parameterB,V3 parameterZ){
	float vtop =min(v1,min(v2,v3));
	float vbottom =max(v1,max(v2,v3));
	float uleft =min(u1,min(u2,u3));
	float uright =max(u1,max(u2,u3));
	 // axis aligned bounding box (AABB) of triangle
	int top = (int) (vtop );
	int bottom = (int) ( vbottom);
	int left = (int) (uleft);
	int right = (int) (uright);
	
	// clip AABB with frame
    if (top > h-1 || bottom < 0 || right < 0 || left > w-1)
      return;
    if (left < 0)
      left = 0;
    if (right > w-1)
      right = w-1;
    if (top < 0)
      top = 0;
    if (bottom > h-1)
      bottom = h-1;
	
	
    for (int v = top; v <= bottom; v++) {
      for (int u = left; u <= right; u++) {
		V3 p(u,v,1);
		float Rp=parameterR*p;
		float Gp=parameterG*p;
		float Bp=parameterB*p;
		V3 v3color(Rp,Gp,Bp);
		float z=parameterZ*p;
		p[2]=z;
		
        if (pointInTriangle(u1,v1,u2,v2,u3,v3,u,v))
		
          continue;
        SetIfCloser(p,v3color);
      }
    }
}
 // sets pixel at p[0], p[1] to color c only if p[2] is closer than zb value at 
//        that pixel
void FrameBuffer::SetIfCloser(V3 p, V3 c) {

  if (p[0] < 0.0f || p[0] >= w || p[1] < 0.0f || p[1] >= h)
    return;

  int u = (int) p[0];
  int v = (int) p[1];

  if (zb[(h-1-v)*w+u] >= p[2])
    return; // nothing to draw, already saw a surface closer at that pixel

  zb[(h-1-v)*w+u] = p[2]; // set z at pixel to new closest surface value
  Set(u, v, c.GetColor());

}

 // point in triangle
bool FrameBuffer::pointInTriangle(float u1,float v1,float u2,float v2,float u3,float v3,float u,float v){

	V3 A(u1,v1,0);
	V3 B(u2,v2,0);
	V3 C(u3,v3,0);
	V3 P(u,v,0);
	V3 V0=C-A;
	V3 V1=B-A;
	V3 V2=P-A;

	float dot00= V0*V0;
	float dot01= V0*V1;
	float dot02= V0*V2;
	float dot11= V1*V1;
	float dot12= V1*V2;
	
	float inverDeno =1/(dot00*dot11-dot01*dot01);

	float x=(dot11*dot02-dot01*dot12)*inverDeno;
	if(x<0||x>1){
		return true;
	}
	float y=(dot00*dot12-dot01*dot02)*inverDeno;
	if(y<0||y>1){
		return true;
	}
	return x+y>1;
}
// draw 3D segment
void FrameBuffer::Draw3DSegment(V3 v0, V3 c0, V3 v1, V3 c1, PPC *ppc) {

  V3 projv0, projv1;
  if (!ppc->Project(v0, projv0))
    return;
  if (!ppc->Project(v1, projv1))
    return;
  Draw2DSegment(projv0, c0, projv1, c1);

}

// draw 2D segment
void FrameBuffer::Draw2DSegment(V3 v0, V3 c0, V3 v1, V3 c1) {

  int stepsN;
  float duf = fabsf(v0[0] - v1[0]);
  float dvf = fabsf(v0[1] - v1[1]);
  if (duf > dvf) {
    stepsN = 1 + (int) duf;
  }
  else {
    stepsN = 1 + (int) dvf;
  }

  if (stepsN == 1) {
    SetSafe((int)v0[0], (int)v0[1], c0.GetColor());
    return;
  }

  for (int i = 0; i < stepsN; i++) {
    float frac = (float) i / (float) (stepsN - 1);
    V3 p = v0 + (v1-v0)*frac;
    V3 c = c0 + (c1-c0)*frac;
    SetSafe((int)p[0], (int)p[1], c.GetColor());
  }

}

// draw 3D segment naive
void FrameBuffer::Draw3DSegmentNaive(V3 v0, V3 c0, V3 v1, V3 c1, PPC *ppc) {

  int stepsN = 300;
  for (int i = 0; i < stepsN; i++) {
    float frac = (float)i / (float) (stepsN-1);
    V3 P = v0 + (v1-v0)*frac;
    V3 col = c0 + (c1-c0)*frac;
    V3 projP;
    if (!ppc->Project(P, projP))
      continue;
    DrawCircle(projP[0], projP[1], 1, col.GetColor());
  }

}

// Draw AABB
void FrameBuffer::DrawAABB(AABB aabb, unsigned int colorNear, 
  unsigned int colorFar, PPC *ppc) {

  V3 cnear;
  cnear.SetFromColor(colorNear);
  V3 cfar;
  cfar.SetFromColor(colorFar);

  V3 p0, p1;

  p0 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[1][2]); 
  Draw3DSegment(p0, cnear, p1, cnear, ppc);

  p0 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[1][2]); 
  Draw3DSegment(p0, cnear, p1, cnear, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[1][2]); 
  Draw3DSegment(p0, cnear, p1, cnear, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[1][2]); 
  Draw3DSegment(p0, cnear, p1, cnear, ppc);


  p0 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[0][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cfar, p1, cfar, ppc);

  p0 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[0][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cfar, p1, cfar, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[0][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cfar, p1, cfar, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[0][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cfar, p1, cfar, ppc);


  p0 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cnear, p1, cfar, ppc);

  p0 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cnear, p1, cfar, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cnear, p1, cfar, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cnear, p1, cfar, ppc);

}





#if 0
  4        7

0-------3
| 5     |  6
|       | 
1-------2
#endif
